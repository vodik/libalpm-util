A toy library to experiment with providing a simple interface for
certain functionality in libalpm since certain components like parsers
are heavily coupled together.

## DB Parsing

Both a raw parser for pacman's database format and an iterator to step
through database entries are provided. Currently done through callbacks
but this might change. Three main functions:

```c
int alpm_parse_file_db(int fd, entry_cb on_entry, field_cb on_field, void *data);
int alpm_parse_directory_db(int fd, entry_cb on_entry, field_cb on_field, void *data);
int alpm_parse_db(const char *path, entry_cb on_entry, field_cb on_field, void *data);
```

`alpm_parse_db` will check if `path` is a file or a directory and call
either `alpm_parse_file_db` (for archives) or `alpm_parse_directory_db`
(for uncompressed ones, like `/var/lib/pacman/local`).

## Package Parsing

TODO
