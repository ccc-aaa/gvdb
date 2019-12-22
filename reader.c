#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "gvdb-reader.h"

int main(int argc, char *argv[])
{
    struct stat statbuf;
    GvdbTable *table;
    GBytes *bytes;
    GError *error;
    gchar **names;
    gsize length, n;
    size_t flen;
    int fd, i;

    for (i = 1; i < argc; i++) {
        if ((fd = open(argv[i], O_RDONLY)) == -1) {
            perror(argv[i]);
            continue;
        }

        if (fstat(fd, &statbuf) == -1) {
            perror(argv[i]);
            close(fd);
            continue;
        }

        if ((bytes =
             (GBytes *) mmap(NULL, (size_t) statbuf.st_size, PROT_READ,
                             MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
            perror(argv[i]);
            close(fd);
            continue;
        }

        puts(argv[i]);

        if ((table =
             gvdb_table_new_from_bytes(bytes, TRUE, &error)) == NULL) {
            munmap(bytes, (size_t) statbuf.st_size);
            close(fd);
            continue;
        }

        if ((names = gvdb_table_get_names(table, &length)) == NULL) {
            g_free(table);
            munmap(bytes, (size_t) statbuf.st_size);
            close(fd);
            continue;
        }

        for (n = 0; n < length; n++) {
            printf("\t%s\n", names[n]);
        }

        puts("");

        g_free(names);
        g_free(table);
        munmap(bytes, (size_t) statbuf.st_size);
        close(fd);
    }

    return 0;
}
