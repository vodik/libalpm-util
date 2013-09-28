VERSION = alpha# $(shell git describe --tags)

CFLAGS := -fPIC -std=c99 \
	-Wall -Wextra -pedantic \
	-D_GNU_SOURCE \
	-DREPOSE_VERSION=\"${VERSION}\" \
	${CFLAGS}

LDFLAGS = -shared -DPIC

all: alpm-util.so
alpm-util.so: db/db_parser.o
	${CC} ${LDFLAGS} -o $@ $?

db/db_parser.c: db/db_parser.rl
	ragel -G2 $< -o $@

install: alpm-util
	# install -Dm755 repose ${DESTDIR}${PREFIX}/bin/repose
	# install -Dm644 _repose ${DESTDIR}${PREFIX}/share/zsh/site-functions/_repose
	# install -Dm644 repose.1 $(DESTDIR)/usr/share/man/man1/repose.1

clean:
	# ${RM} repose *.o alpm/*.o

.PHONY: clean install uninstall
