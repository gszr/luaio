CVS=/usr/src

KMOD= luasocket
SRCS= luasocket.c

CPPFLAGS+=	-I../io/include \
			-I${CVS}/external/mit/lua/dist/src \
			-I${CVS}/sys/sys

.include <bsd.kmodule.mk>
