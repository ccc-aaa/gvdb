/*
 * Copyright © 2019 Chris Barts
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Chris Barts <chbarts@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include "gvdb-reader.h"

static void recurse_table(GvdbTable * table, int depth)
{
    GvdbTable *rtable;
    GVariant *var;
    GString *val;
    gchar **names;
    gsize length, n;

    if ((names = gvdb_table_get_names(table, &length)) == NULL) {
        return;
    }

    for (n = 0; n < length; n++) {
        printf("%*c%s\n", depth, ' ', names[n]);
        if (gvdb_table_has_value(table, names[n]) == TRUE) {
            var = gvdb_table_get_value(table, names[n]);
            val = g_variant_print_string(var, NULL, TRUE);
            printf("%*c%s\n", depth + 4, ' ', val->str);
            g_string_free(val, TRUE);
            g_variant_unref(var);
        } else if ((rtable = gvdb_table_get_table(table, names[n])) !=
                   NULL) {
            recurse_table(rtable, depth + 4);
            gvdb_table_free(rtable);
        }
    }

    g_free(names);
}

int main(int argc, char *argv[])
{
    GvdbTable *table, *rtable;
    GVariant *var;
    GError *error = NULL;
    GString *val;
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
            printf("%*c%s\n", 4, ' ', names[n]);
            if (gvdb_table_has_value(table, names[n]) == TRUE) {
                var = gvdb_table_get_value(table, names[n]);
                val = g_variant_print_string(var, NULL, TRUE);
                printf("%*c%s\n", 8, ' ', val->str);
                g_string_free(val, TRUE);
                g_variant_unref(var);
            } else if ((rtable = gvdb_table_get_table(table, names[n])) !=
                       NULL) {
                recurse_table(rtable, 8);
                gvdb_table_free(rtable);
            }
        }

        puts("");

        g_free(names);
        gvdb_table_free(table);
    }

    return 0;
}
