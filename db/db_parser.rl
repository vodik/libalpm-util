#include "db_parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <err.h>

%%{
    machine alpm_db;

    action new { str_clear(&pr->title); }
    action title {
        rc = str_append(&pr->title, fc);
        if (rc < 0)
            return rc;
    }

    action value {
        rc = str_append(&pr->value, fc);
        if (rc < 0)
            return rc;
    }

    action accept {
        pr->cb(pr->title.ptr, pr->value.ptr, pr->data);
        str_clear(&pr->value);
    }

    title = '%'    >new   [^%]+  $title '%\n';
    value = [^%\n] >value [^\n]* $value '\n' @accept;

    main := ( title | value | '\n' )*;
}%%

%%write data;

void db_parser_init(db_parser *pr, field_cb cb, void *data)
{
    *pr = (db_parser){
        .cb   = cb,
        .data = data
    };

    str_init(&pr->title, 1024);
    str_init(&pr->value, 1024);

    %%access pr->;
    %%write init;
}

int db_parser_feed(db_parser *pr, const char *entry, size_t len)
{
    int rc = 0;
    const char *p = entry;
    const char *pe = entry + len;

    %%access pr->;
    %%write exec;

    if (pr->cs == alpm_db_error) {
        return -EINVAL;
    }

    return rc;
}

void db_parser_free(db_parser *pr)
{
    str_free(&pr->title);
    str_free(&pr->value);
}
