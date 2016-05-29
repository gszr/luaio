
CVS=/usr/src
.include "${CVS}/sys/modules/lua/Makefile.inc"

KMOD= luasockets
SRCS= luasockets.c

CPPFLAGS+= -I${CVS}/external/mit/lua/dist/src \
           -I${CVS}/sys/modules/lua/libs \
           -I${CVS}/sys/sys

.include <bsd.kmodule.mk>
