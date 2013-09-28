#ifndef STR_H
#define STR_H

#include <stdlib.h>
#include <string.h>

struct str_t {
    char *ptr;
    size_t pos, len;
};

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

    *(char *)mempcpy(str->ptr, data, len) = 0; // FIXME
    return 0;
}

#endif
