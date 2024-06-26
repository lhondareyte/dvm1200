#
#  $Id$
# 
#CFLAGS	= -O2 -Wall -D  __DEBUG_DVM__
CFLAGS	= -O2 -Wall -pedantic -Werror 
LDFLAGS = -s
PRG	= dvm1200
SOURCES	= decode.c dvm1200.c
OBJECTS	= decode.o dvm1200.o
BINDIR  = $(DESTDIR)$(PREFIX)/bin
MANDIR  = $(DESTDIR)$(PREFIX)/share/man/man1

all: $(PRG)

$(PRG): $(OBJECTS)
	@printf "Linking   $@:"
	@$(CC) $(LDFLAGS) $(CFLAGS) $(OBJECTS) -o $@
	@printf "\tdone.\n"

clean:
	@rm -f  $(OBJECTS) $(PRG) *~ *gz

man:
	@gzip -cn $(PRG).1 > $(PRG).1.gz

install: man
	install -m 755 $(PRG) $(BINDIR)
	install -m 644 $(PRG).1.gz $(MANDIR)
.c.o:
	@printf "Compiling $<:"
	@$(CC) $(CFLAGS) -Os -c $< -o $@
	@printf "\tdone.\n"
