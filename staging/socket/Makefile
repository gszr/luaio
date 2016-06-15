CVS=/usr/src

KMOD= luasocket
SRCS= luasocket.c

CPPFLAGS+=	-I${CVS}/external/mit/lua/dist/src \
			-I${CVS}/sys/sys \
			-I/vagrant/netbsd/work/src/sys/modules/lua/include

.include <bsd.kmodule.mk>
