#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <err.h>

#include <db/db_parser.h>

static const char *db_entry =
    "%FILENAME%\n"
    "repose-git-1.50.g15432df-1-x86_64.pkg.tar.xz\n"
    "\n"
    "%NAME%\n"
    "repose-git\n"
    "\n"
    "%VERSION%\n"
    "1.50.g15432df-1\n"
    "\n"
    "%DESC%\n"
    "A archlinux repo building tool\n"
    "\n"
    "%CSIZE%\n"
    "19360\n"
    "\n"
    "%ISIZE%\n"
    "74752\n"
    "\n"
    "%MD5SUM%\n"
    "342dc59fd475d7abd3ba8c42a4416ee5\n"
    "\n"
    "%SHA256SUM%\n"
    "6cff3e2ea169ca99ba0918211598fe1ddd9d8d6a7b2f70105d11b5d710341058\n"
    "\n"
    "%PGPSIG%\n"
    "iQEcBAABAgAGBQJSFBDAAAoJEPbPpz3CDWxP8sMIAJ2p1c7bC6sFbB9VRZKp5RjsrAnMXXsKZItdapCKY2wsCGAj9NylSztnsRZsRx0KBxBxChV8rc9sN+yBqt/Gy3Jfd7Jlemoll32913gL4gP8i4C7Mq2o0ZWTdWpXEVVSBudQjc8pc9baqhE3vID2LaDYUOizLBAFTM4bUtQhgxYZ3av0KiwOWroxcNbeFc9Ldkhx/cxDzhuvkU+Zbilz60SlzJJR9YGpXdwmnb2EsJzFhO18bpuIKUFGTmxyCygRbiHp77N/TGywJEVT1TDbypbWJBKqnOo6lZPpbMp6thCvINDXtpx3AqOqalW85ewAsQ0sqc/BbAMkEQig5EzJOcg=\n"
    "\n"
    "%URL%\n"
    "http://github.com/vodik/repose\n"
    "\n"
    "%LICENSE%\n"
    "GPL\n"
    "\n"
    "%ARCH%\n"
    "x86_64\n"
    "\n"
    "%BUILDDATE%\n"
    "1377046720\n"
    "\n"
    "%PACKAGER%\n"
    "Simon Gomizelj <simongmzlj@gmail.com>\n"
    "\n";

void on_db_entry_field(const char *title, const char *value, void __attribute__((unused)) *data)
{
    printf("%s = %s\n", title, value);
}

int main(void)
{
    db_parser parser;
    db_parser_init(&parser, on_db_entry_field, NULL);

    size_t len = strlen(db_entry);
    size_t i, block = 512;
    const char *p = db_entry;

    for (i = 0; i < len; i += block) {
        size_t pass = (i + block > len) ? (len - i) : block;

        if (db_parser_feed(&parser, p, pass) < 0)
            errx(1, "error parsing db entry");

        p += pass;
    }

    db_parser_free(&parser);
}
