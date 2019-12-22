#include <stdio.h>
#include <stdlib.h>
#include "gvdb-reader.h"

int main(int argc, char *argv[])
{
    GvdbTable *table;
    GError *error = NULL;
    gchar **names;
    gsize length, n;
    int i;

    for (i = 1; i < argc; i++) {
        if ((table = gvdb_table_new(argv[i], TRUE, &error)) == NULL) {
            fprintf(stderr, "Can't read database from %s: %s\n", argv[i],
                    error->message);
            g_error_free(error);
            error = NULL;
            continue;
        }

        puts(argv[i]);

        if ((names = gvdb_table_get_names(table, &length)) == NULL) {
            g_free(table);
            continue;
        }

        for (n = 0; n < length; n++) {
            printf("\t%s\n", names[n]);
        }

        puts("");

        g_free(names);
        g_free(table);
    }

    return 0;
}
