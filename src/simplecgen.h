/*
 * simplecgen.h: generates html files using the simplectemplate library
 *
 * Copyright (C) 2017-2019  Andy Alt (andy400-dev@yahoo.com)
 *
 * This file is part of simplecgen <https://github.com/theimpossibleastronaut/simplecgen>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#ifndef _INC_SIMPLECGEN_H
#define _INC_SIMPLECGEN_H

#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "template_functions.h"

#ifndef PATH_MAX
#   define PATH_MAX 1024
#endif

#ifndef VERSION
  #define VERSION "unversioned"
#endif

#ifdef DEBUG
#   define PRINT_DEBUG printf ("[DEBUG]:"); printf
#endif

#define LEN_MAX_LINE 161

#ifndef PATH_MAX
# #define PATH_MAX 256
#endif

#define LEN_MAX_FILENAME (PATH_MAX + 1)

#define CONFIG_FILE "simplecgen.conf"

typedef unsigned short int ushort;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

/* This enum list will get used more later
 */
enum {
  EXIT_BUF_ERR = 100,
  ERROR_CONFIG_OPEN,
  ERROR_CONFIG_CLOSE,
  ERROR_CONFIG_LINE
};

typedef struct cfg {
  char site_title[LEN_MAX_LINE];
  char site_description[LEN_MAX_LINE];
  char repo_URL[LEN_MAX_LINE];
} struct_cfg;

typedef struct st_page st_page;

struct st_page {
  char *layout;
  char *title;
  char *body;

  /* sub_title will appear after 'page title | ' in the title bar */
  char sub_title[LEN_MAX_LINE];

  char *sct_basename;

  char *contents;
};

void
process_sct (char *input_file, struct_cfg *cfgopts, st_page *st_page_props);

#endif
