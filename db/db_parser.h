#ifndef DB_PARSER_H
#define DB_PARSER_H

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>

#include "../str.h"

typedef void (*field_cb)(const char *, const char *, void *);

typedef struct db_parser_t db_parser;

struct db_parser_t {
    int cs;
    struct str_t title;
    struct str_t value;
    field_cb cb;
    void *data;
};

void db_parser_init(db_parser *pr, field_cb cb, void *data);
int db_parser_feed(db_parser *pr, const char *entry, size_t len);
void db_parser_free(db_parser *pr);

#endif
