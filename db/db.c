#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>
#include <dirent.h>
#include <archive.h>
#include <archive_entry.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "db_parser.h"

typedef void (*entry_cb)(const char *, const char *, void *data);

/* {{{ WHOLESALE ALPM CRAP */
static int _alpm_splitname(const char *target, struct str_t *name, struct str_t *version)
{
    /* the format of a db entry is as follows:
     * package-version-rel/
     * package-version-rel/desc (we ignore the filename portion)
     * package name can contain hyphens, so parse from the back- go bact
     * two hyphens and we have split the version from the name.
     */
    const char *pkgver, *end;

    if(target == NULL) {
        return -1;
    }

    /* remove anything trailing a '/' */
    /* for(end = target; *end && *end != '/'; end++); */
    end = strchrnul(target, '/');

    /* do the magic parsing- find the beginning of the version string
     * by doing two iterations of same loop to lop off two hyphens */
    /* for(pkgver = end - 1; *pkgver && *pkgver != '-'; pkgver--); */
    pkgver = memrchr(target, '-', end    - target);
    pkgver = memrchr(target, '-', pkgver - target);
    if (!pkgver)
        return -1;

    /* for(pkgver = pkgver - 1; *pkgver && *pkgver != '-'; pkgver--); */
    /* if(*pkgver != '-' || pkgver == target) { */
    /*     return -1; */
    /* } */

    /* copy into fields and return */
    str_set(version, pkgver + 1, end - pkgver - 1);
    str_set(name,    target,     pkgver - target);

    return 0;
}
/* }}} */

static int alpm_parse_archive_entry(struct archive *archive, db_parser *parser)
{
    char *block;
    size_t block_size;
    int64_t offset;

    while (1) {
        int ret = archive_read_data_block(archive, (const void **)&block,
                                          &block_size, &offset);

        if (ret == ARCHIVE_EOF) {
            break;
        } else if (ret != ARCHIVE_OK) {
            errx(EXIT_FAILURE, "failed to read header: %s", archive_error_string(archive));
        }

        int rc = db_parser_feed(parser, block, block_size);
        if (rc < 0)
            errx(1, "error parsing db entry: %s", strerror(-rc));
    }

    return 0;
}

int alpm_parse_file_db(int fd, entry_cb on_entry, field_cb on_field, void *data)
{
    struct archive *archive = NULL;
    struct stat st;
    char *memblock = MAP_FAILED;
    int rc = 0;

    fstat(fd, &st);

    if (st.st_size == 0)
        return 0;

    memblock = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED | MAP_POPULATE, fd, 0);
    if (memblock == MAP_FAILED)
        err(EXIT_FAILURE, "failed to mmap database");

    archive = archive_read_new();

    archive_read_support_filter_all(archive);
    archive_read_support_format_all(archive);

    int r = archive_read_open_memory(archive, memblock, st.st_size);
    if (r != ARCHIVE_OK) {
        warnx("file is not an archive");
        rc = -1;
        goto cleanup;
    }

    db_parser parser;
    db_parser_init(&parser, on_field, data);

    struct str_t pkgname, pkgver;
    str_init(&pkgname, 512);
    str_init(&pkgver,  512);

    for (;;) {
        struct archive_entry *entry;

        r = archive_read_next_header(archive, &entry);
        if (r == ARCHIVE_EOF) {
            break;
        } else if (r != ARCHIVE_OK) {
            errx(EXIT_FAILURE, "failed to read header: %s", archive_error_string(archive));
        }

        const mode_t mode = archive_entry_mode(entry);
        const char *name = archive_entry_pathname(entry);
        if (S_ISDIR(mode))
            continue;

        /* char *pkgname = NULL, *pkgver = NULL; */
        if (_alpm_splitname(name, &pkgname, &pkgver) < 0)
            errx(1, "failed to splitname");

        on_entry(pkgname.ptr, pkgver.ptr, data);
        /* printf("ENTRY: %s\n", name); */

        alpm_parse_archive_entry(archive, &parser);
        printf("\n");
    }

cleanup:
    /* close(fd); */

    if (memblock != MAP_FAILED)
        munmap(memblock, st.st_size);

    if (archive) {
        archive_read_close(archive);
        archive_read_free(archive);
    }

    return rc;

}

/* {{{ FILE */
static int alpm_parse_file_entry(int fd, db_parser *parser)
{
    struct stat st;
    char *memblock = MAP_FAILED;
    int rc = 0;

    fstat(fd, &st);
    if (st.st_size == 0)
        return 0;

    memblock = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED | MAP_POPULATE, fd, 0);
    if (memblock == MAP_FAILED)
        err(EXIT_FAILURE, "failed to mmap database");

    rc = db_parser_feed(parser, memblock, st.st_size);
    if (rc < 0)
        errx(1, "error parsing db entry: %s", strerror(-rc));

    if (memblock != MAP_FAILED)
        munmap(memblock, st.st_size);

    return rc;
}

int alpm_parse_directory_db(int fd, entry_cb on_entry, field_cb on_field, void *data)
{
    DIR *dir = fdopendir(fd);

    db_parser parser;
    db_parser_init(&parser, on_field, data);

    struct str_t pkgname, pkgver;
    str_init(&pkgname, 512);
    str_init(&pkgver,  512);

    for (;;) {
        const struct dirent *dp = readdir(dir);
        if (!dp)
            break;

        if (dp->d_type & DT_DIR) {
            if (*dp->d_name == '.')
                continue;

            if (_alpm_splitname(dp->d_name, &pkgname, &pkgver) < 0)
                errx(1, "failed to splitname");

            int fd2 = openat(fd, dp->d_name, O_RDONLY);
            DIR *dir2 = fdopendir(fd2);

            for (;;) {
                const struct dirent *dp2 = readdir(dir2);
                if (!dp2)
                    break;

                if (dp2->d_type & DT_DIR)
                    continue;
                if (*dp->d_name == '.')
                    continue;
                if (strcmp(dp2->d_name, "mtree") == 0)
                    continue;
                if (strcmp(dp2->d_name, "install") == 0)
                    continue;

                int fd3 = openat(fd2, dp2->d_name, O_RDONLY);
                if (fd3 < 0)
                    errx(1, "failed to open %s", dp2->d_name);

                on_entry(pkgname.ptr, pkgver.ptr, data);
                alpm_parse_file_entry(fd3, &parser);
                close(fd3);
            }

            closedir(dir2);
        }
    }

    closedir(dir);
    return 0;
}
/* }}} */

int alpm_parse_db(const char *path, entry_cb on_entry, field_cb on_field, void *data)
{
    int ret = -1;
    struct stat st;

    int fd = open(path, O_RDONLY);
    fstat(fd, &st);

    if (S_ISREG(st.st_mode))
        ret = alpm_parse_file_db(fd, on_entry, on_field, data);
    else if (S_ISDIR(st.st_mode))
        ret = alpm_parse_directory_db(fd, on_entry, on_field, data);

    close(fd);
    return ret;
}
