#ifndef DB_PARSER_H
#define DB_PARSER_H

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>

typedef void (*field_cb)(const char *, const char *, void *);

typedef struct db_parser_t db_parser;

struct str_t {
    char *ptr;
    size_t pos, len;
};

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

static inline int str_init(struct str_t *str, size_t len)
{
    *str = (struct str_t){
        .ptr = malloc(len),
        .pos = 0,
        .len = len
    };

    return str->ptr ? 0 : -ENOMEM;
}

static inline void str_resize(struct str_t *str, size_t size)
{
    if (str->len < size) {
        str->len = 1 << (32 - __builtin_clz(size));
        str->ptr = realloc(str->ptr, str->len);
    }
}

static inline void str_free(struct str_t *str)
{
    free(str->ptr);
}

static inline int str_append(struct str_t *str, char c)
{
    str_resize(str, str->pos + 1);

    if (!str->ptr)
        return -ENOMEM;

    str->ptr[str->pos++] = c;
    str->ptr[str->pos] = 0; // FIXME
    return 0;
}

static inline void str_clear(struct str_t *str)
{
    str->pos = 0;
}

static inline int str_set(struct str_t *str, const char *data, size_t len)
{
    str_resize(str, len);

    if (!str->ptr)
        return -ENOMEM;

    *(char *)mempcpy(str->ptr, data, len) = 0;
    return 0;
}

#endif
