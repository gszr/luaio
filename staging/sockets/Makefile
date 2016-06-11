CVS=/usr/src

KMOD= luasockets
SRCS= luasockets.c

CPPFLAGS+=	-I${CVS}/external/mit/lua/dist/src \
			-I${CVS}/sys/sys \
			-I/vagrant/netbsd/work/src/sys/modules/lua/include

.include <bsd.kmodule.mk>
