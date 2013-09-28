#include <stdlib.h>
#include <stdio.h>
#include <err.h>

#include <db/db.h>

static void on_db_entry(const char *pkgname, const char *pkgver, void __attribute__((unused)) *data)
{
    printf("> ENTRY: %s-%s\n", pkgname, pkgver);
}

static void on_db_entry_field(const char *title, const char *value, void __attribute__((unused)) *data)
{
    printf("%s = %s\n", title, value);
}

int main(int argc, char *argv[])
{
    int i;

    if (argc == 1)
        errx(1, "not enough arguments");

    for (i = 1; i < argc; ++i)
        alpm_parse_db(argv[i], on_db_entry, on_db_entry_field, NULL);
}
