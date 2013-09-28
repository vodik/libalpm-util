#ifndef DB
#define DB

#include "db_parser.h"

typedef void (*entry_cb)(const char *, const char *, void *data);

int alpm_parse_file_db(int fd, entry_cb on_entry, field_cb on_field, void *data);
int alpm_parse_directory_db(int fd, entry_cb on_entry, field_cb on_field, void *data);
int alpm_parse_db(const char *path, entry_cb on_entry, field_cb on_field, void *data);

#endif
