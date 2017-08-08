CC    = cc
PROG=	dvm1200
SRCS!=	ls *.c

OSNAME        != uname
ARCH          != uname -m
GIT_VERSION   != git describe --tags
GIT_YEAR      != git log -1 --format=%cd | cut -d' ' -f5
#CFLAGS	      += -D__DEBUG_DVM__
#CFLAGS	      += -DVERSION=\"$(GIT_VERSION)\"
#CFLAGS	      += -DYEAR=\"$(GIT_YEAR)\"

.include <bsd.prog.mk>
