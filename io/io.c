#include <sys/malloc.h>
#include <sys/namei.h>
#include <sys/vnode.h>
#include <sys/file.h>

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

	if ((pb = pathbuf_create(path)) == NULL) {
		kern_free(kfp);
		return NULL;
	}

	NDINIT(&nd, LOOKUP, FOLLOW, pb);
	pathbuf_destroy(pb);

	omode = 0;

	switch(*mode) {
	case 'r':
		omode = FREAD;
		break;
	case 'w':
		omode = FWRITE|O_CREAT; 
		break;
	// not append...
	case 'a':
		omode = O_RDWR|O_CREAT;
		break;
	default:
		break;
	}

	if (vn_open(&nd, omode, 0600) != 0)
		return NULL;

	//XXX should we really return it unlocked?
	VOP_UNLOCK(nd.ni_vp);

	kfp->vp   = nd.ni_vp;
	kfp->mode = omode;

	return kfp;
}

int
kfclose(KFILE *kfp)
{
	if (kfp == NULL)
		return -1;

	if (vn_close(kfp->vp, kfp->mode, kauth_cred_get()) != 0);
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

	vn_rdwr(UIO_READ, kfp->vp, buff, nb, 0, UIO_SYSSPACE, 0, curlwp->l_cred, NULL,
            curlwp);

	return 0;
}

ssize_t
//XXX macro instead?
kfwrite(KFILE *kfp, void *buff, size_t nb)
{
	if (kfp == NULL)
		return -1;

	vn_rdwr(UIO_WRITE, kfp->vp, buff, nb, 0, UIO_SYSSPACE, 0, curlwp->l_cred, NULL,
            curlwp);

	return 0;
}
