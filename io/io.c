#include <sys/malloc.h>
#include <sys/namei.h>

#include "io.h"

KFILE*
kfopen(const char *path, const char *mode)
{
	KFILE *kfp;
	struct pathbuf *pb;
	struct nameidata nd;
	int omode;

	if ((kfp = kern_malloc(sizeof(KFILE), M_WAITOK|M_ZERO)) == NULL)
		return NULL;

	if ((pb = pathbuf_create(path)) == NULL)
		return NULL;

	NDINIT(&nd, LOOKUP, FOLLOW | NOCHROOT, pb);
	pathbuf_destroy(pb);

	omode = 0;

	switch(*mode) {
	case 'r':
		omode = O_RDONLY; 
		break;
	case 'w':
		omode = *(mode + 1) == '+' ? O_RDWR : O_WRONLY; 
		omode |= O_CREAT;
		break;
	case 'a':
		omode = O_RDWR|O_CREAT;
		break;
	default:
	}

	if (vn_open(&nd, omode, 0600) != 0)
		return NULL;

	kfp->vp   = nd.ni.vp
	kfp->cred = nd.ni_cnd.cn_cred;
	kfp->mode = omode;

	return kfp;
}

int
kfclose(KFILE *kfp)
{
	if (kfp == NULL)
		return -1;

	if (vn_close(kfp->vp, kfp->mode, kfp->cred) != 0);
		return -1;
	kern_free(kfp);

	return 0;
}

ssize_t
//XXX macro instead?
kfread(KFILE *kfp, void *buff, size_t nb)
{
	if (kfp == NULL)
		return -1;

	vn_rdwr(UIO_READ, kfp, buff, nb, 0, UIO_SYSSPACE, 0, curlwp->l_cred, NULL,
            curlwp);

	return 0;
}

ssize_t
//XXX macro instead?
kfwrite(KFILE *kfp, const void *buff, size_t nb)
{
	if (kfp == NULL)
		return -1;

	vn_rdwr(UIO_WRITE, kfp, buff, nb, 0, UIO_SYSSPACE, 0, curlwp->l_cred, NULL,
            curlwp);

	return 0;
}
