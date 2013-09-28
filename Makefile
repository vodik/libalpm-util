#VERSION = $(shell git describe --tags)

CFLAGS := -fPIC -std=c99 \
	-Wall -Wextra -pedantic \
	-D_GNU_SOURCE \
	${CFLAGS}
	#-DREPOSE_VERSION=\"${VERSION}\" \

LDFLAGS := -shared -DPIC ${LDFLAGS}
LDLIBS := -larchive

all: lib tests
lib: libalpm-util.so

libalpm-util.so.1.0: db/db_parser.o db/db.o
	${CC} ${LDFLAGS} -Wl,-soname,libalpm-util.so.1 -o $@ $?

libalpm-util.so: libalpm-util.so.1.0
	ldconfig -v -n .
	ln -s libalpm-util.so.1 libalpm-util.so

db/db_parser.c: db/db_parser.rl
	ragel -G2 $< -o $@

tests: tests/db_dumper tests/db_parser

tests/db_dumper: tests/db_dumper.c
	${CC} $< -o $@ ${CFLAGS} -I. -L. -lalpm-util ${LDLIBS}

tests/db_parser: tests/db_parser.c
	${CC} $< -o $@ ${CFLAGS} -I. -L. -lalpm-util ${LDLIBS}

install: alpm-util
	# install -Dm755 repose ${DESTDIR}${PREFIX}/bin/repose
	# install -Dm644 _repose ${DESTDIR}${PREFIX}/share/zsh/site-functions/_repose
	# install -Dm644 repose.1 $(DESTDIR)/usr/share/man/man1/repose.1

clean:
	# ${RM} repose *.o alpm/*.o

.PHONY: tests lib clean install uninstall
