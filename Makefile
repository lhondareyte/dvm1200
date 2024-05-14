#
#  $Id$
# 
#CFLAGS	= -O2 -Wall -D  __DEBUG_DVM__
CFLAGS	= -O2 -Wall -pedantic -Werror
PRG	= dvm1200
SOURCES	= decode.c dvm1200.c
OBJECTS	= decode.o dvm1200.o

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
	install -m 755 -o root -g wheel $(PRG) /usr/local/bin
	install -m 644 -o root -g wheel $(PRG).1.gz /usr/local/man/man1
.c.o:
	@printf "Compiling $<:"
	@$(CC) $(CFLAGS) -Os -c $< -o $@
	@printf "\tdone.\n"
