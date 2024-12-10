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

#define MAXDEPTH 10

static void recurse_table(GvdbTable * table, int depth, int indent, const gchar* prefix);
void help(const char* name);

int main(int argc, char *argv[])
{
    GError *error = NULL;
    GvdbTable *table;
    int pretty = 0;
    int arg_start = 1;

    if(argc <= 1) {
      help(argv[0]);
      return 1;
    }

    for(int i = 1; i < argc; i++) {
      if(strncmp(argv[i], "--help", 6) == 0 || strncmp(argv[i], "-h", 2) == 0) {
        help(argv[0]);
        return 0;
      } else if(strncmp(argv[i], "--pretty", 8) == 0 || strncmp(argv[i], "-p", 2) == 0) {
        pretty = 1;
        arg_start = i+1;
      } else if(strncmp(argv[i], "--", 2) == 0) {
        arg_start = i+1;
        break;
      }
    }

    for (int i = arg_start; i < argc; i++) {
        if ((table = gvdb_table_new(argv[i], TRUE, &error)) == NULL) {
            fprintf(stderr, "Can't read database from %s: %s\n", argv[i],
                    error->message);
            g_error_free(error);
            error = NULL;
            continue;
        }

        if(pretty == 1) {
          puts(argv[i]);
          recurse_table(table, 1, 2, NULL);
          puts("");
        } else {
          recurse_table(table, 1, 0, NULL);
        }

        gvdb_table_free(table);
    }

    return 0;
}

void help(const char* name) {
  printf("Usage: %s dbpath [dbpath...]\n", name);
}

static void recurse_table(GvdbTable * table, int depth, int indent, const gchar* prefix)
{
    GvdbTable *rtable;
    GVariant *var;
    GString *val;
    gchar **names;
    gsize length;

    if ((names = gvdb_table_get_names(table, &length)) == NULL) {
        return;
    }

    if (depth >= MAXDEPTH) {
      printf("max recursion reached.");
      return;
    }

    for (gsize n = 0; n < length; n++) {
        if (gvdb_table_has_value(table, names[n]) == TRUE) {
            if(indent > 0) {
              printf("%*c", depth*indent, ' ');
            } else if(prefix != NULL) {
              printf("%s ", prefix);
            }
            printf("%s:", names[n]);
            var = gvdb_table_get_value(table, names[n]);
            val = g_variant_print_string(var, NULL, FALSE);
            printf("\t%s\n", val->str);
            g_string_free(val, TRUE);
            g_variant_unref(var);
        } else if ((rtable = gvdb_table_get_table(table, names[n])) !=
                   NULL) {
          if(indent == 0) {
            gsize limit = strlen(names[n]);
            gchar* buffer = NULL;

            if(prefix != NULL) {
              limit += strlen(prefix);
              buffer = g_new0(gchar, limit+2);
              snprintf(buffer, limit, "%s %s", prefix, names[n]);
            } else {
              buffer = g_new0(gchar, limit+1);
              strncpy(buffer, names[n], limit+1);
            }
            recurse_table(rtable, depth+1, indent, buffer);
            free(buffer);
          } else {
            printf("%s:\n", names[n]);
            recurse_table(rtable, depth+1, indent, NULL);
          }
          gvdb_table_free(rtable);
        }
    }

    for (gsize n = 0; n < length; n++) { g_free(names[n]); }
    g_free(names);
}

