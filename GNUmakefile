#
#  $Id$
# 
CC      = gcc
CFLAGS	= -O2 -Wall 

HEADERS       := $(wildcard *.h)
SOURCES       := $(wildcard *.c)
ASMSRCS       := $(wildcard *.s)
OBJECTS       := $(patsubst %.c,%.o,$(SOURCES))
OBJECTS       += $(patsubst %.s,%.o,$(ASMSRCS))

OSNAME        = $(shell uname)
ARCH          = $(shell uname -m)
GIT_VERSION   = $(shell git describe --tags)
GIT_YEAR      = $(shell git log -1 --format=%cd | cut -d' ' -f5)
CFLAGS	      += -DVERSION=\"$(GIT_VERSION)\"
CFLAGS	      += -DYEAR=\"$(GIT_YEAR)\"

PRG=dvm1200

$(PRG): $(OBJECTS) GNUmakefile
	@printf "Linking   $@:"
	@$(CC) $(LDFLAGS) $(CFLAGS) $(OBJECTS) -o $@
	@printf "\tdone.\n"

mrproper: clean
	@rm -f $(PRG)

clean:
	@rm -f  $(OBJECTS) *~
	@chmod 644 $(SOURCES) $(HEADERS) $(ASMSRCS)
.c.o:
	@printf "Compiling $<:"
	@$(CC) $(CFLAGS) -Os -c $< -o $@
	@printf "\tdone.\n"
