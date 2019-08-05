/*
 * simplecgen.c: generates html files using the simplectemplate library
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

#include <libgen.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>

#include "simplecgen.h"
#include "utils.h"

int main(int argc, char **argv)
{
  if (argc > 1)
  {
    if (strcmp (argv[1], "--version") == 0)
    {
      printf ("%s\n", VERSION);
      return 0;
    }
    else
    {
      printf ("unsupported command line arguments given\n");
      return 1;
    }
  }

  char cfg_file[PATH_MAX + 1];
  // Not configurable yet. Look for the config in the directory where
  // simplecgen is run from.
  sprintf (cfg_file, "./%s", CONFIG_FILE);

  struct_cfg cfgopts;

#ifdef DEBUG
  PRINT_DEBUG ("config file = %s\n", cfg_file);
#endif

  /* As more config options are added, it will be easier to pass
   * a single structure, as opposed to all the different config
   * variables
   */
  parse_config  (cfg_file, &cfgopts);

#ifdef DEBUG
  PRINT_DEBUG ("after parsing, site_title is '%s'\n", cfgopts.site_title);
  PRINT_DEBUG ("after parsing, site_description is '%s'\n", cfgopts.site_description);
  PRINT_DEBUG ("after parsing, repo_URL is '%s'\n", cfgopts.repo_URL);
#endif

  FILE *tail_fp;
  if ((tail_fp = fopen ("templates/tail.html", "r")) == NULL)
  {
    perror ("Error opening file");
    return errno;
  }

  fseek (tail_fp, 0, SEEK_END);
  size_t tail_size = ftell (tail_fp);
  rewind (tail_fp);

  char output_tail[tail_size + 1];
  fread (output_tail, tail_size, 1, tail_fp);

  if (fclose (tail_fp) == EOF)
  {
    perror ("Error closing file");
    return errno;
  }

  struct dirent *dir_entry;
  DIR *infiles_dir;
  /* FIXME: need a check to make sure the directory and file exists
   * add more flexibility so the user can change this (hint: config file)
   */
  if ((infiles_dir = opendir ("infiles")) == NULL)
  {
    perror ("Error opening directory");
    return errno;
  }

  while ((dir_entry = readdir (infiles_dir)) != NULL)
  {
    /* Only read .sct files */
    if (strstr (dir_entry->d_name, ".sct") == NULL)
      continue;

    bufchk (dir_entry->d_name, LEN_MAX_FILENAME);
    char input_file[LEN_MAX_FILENAME + sizeof "infiles/"];
    snprintf (input_file, sizeof input_file, "infiles/%s", dir_entry->d_name);

    char infile_URL[LEN_MAX_FILENAME + sizeof cfgopts.repo_URL];
    snprintf (infile_URL, sizeof infile_URL, "%s/%s", cfgopts.repo_URL, dir_entry->d_name);

    printf ("processing %s\n", input_file);
    st_page st_page_props;
    process_sct (input_file, &cfgopts, &st_page_props);

    const char *data[] = {
      "title", st_page_props.title,
      "sub_title", st_page_props.sub_title,
      "body", st_page_props.body,
      "infile_URL", infile_URL
    };

    char layout_template[LEN_MAX_FILENAME] = "";
    sprintf (layout_template, "templates/%s.html", st_page_props.layout);

    FILE *fp_layout;
    if ((fp_layout = fopen (layout_template, "r")) == NULL)
    {
      printf ("  :Error: layout: %s not found at %d\n", layout_template, __LINE__);
      return 1;
    }

    const int data_size = (ARRAY_SIZE(data) / 2);

    /* FIXME: need a check to make sure the directory and file exists
     * add more flexibility so the user can change this (hint: config file)
     */
    char *output_head = render_template_file2 ("templates/head.html", data_size, data);
    if (output_head == NULL)
    {
      fprintf (stderr, "NULL returned while rendering template %s\n", "templates/head.html");
      exit (EXIT_FAILURE);
    }

    char *output_layout = render_template_file2 (layout_template, data_size, data);
    if (output_layout == NULL)
    {
      fprintf (stderr, "NULL returned while rendering template %s\n", layout_template);
      exit (EXIT_FAILURE);
    }

    int res_access = access ("_web_root", F_OK);
    if (res_access)
      mkdir ("_web_root", S_IRWXU);

    char dest_file[sizeof "_web_root/" + LEN_MAX_FILENAME + sizeof ".html"];
    snprintf (dest_file, sizeof dest_file, "_web_root/%s.html", st_page_props.sct_basename);

    FILE *fp;
    if ((fp = fopen (dest_file, "w")) == NULL)
    {
      perror ("Error opening");
      free (st_page_props.contents);
      return errno;
    }

    fwrite (output_head, strlen (output_head), 1, fp);
    fwrite (output_layout, strlen (output_layout), 1, fp);
    fwrite (output_tail, strlen (output_tail), 1, fp);

    if (fclose (fp) == EOF)
    {
      perror ("Error while closing file\n");
    }

    free (st_page_props.contents);
    free (output_head);
    free (output_layout);
  }

  if (closedir (infiles_dir) != 0)
  {
    perror ("Error closing directory:");
    return errno;
  }

  return 0;
}

void
process_sct (char *input_file, struct_cfg *cfgopts, st_page *st_page_props)
{
  FILE *fp = fopen (input_file, "r");
  if (fp == NULL)
  {
    perror ("Error opening");
    exit (errno);
  }

  /* get the title line */
  /* declared statically because 'title' points to it outside of this function */
  static char title_line[LEN_MAX_LINE + 1];
  if (fgets (title_line, LEN_MAX_LINE, fp) == NULL)
  {
    perror ("Error getting line");
    fclose (fp);
    exit (EXIT_FAILURE);
  }

  st_page_props->title = strchr (title_line, ':');

  if (st_page_props->title == NULL)
  {
    printf ("%s has the wrong format\n", input_file);
    exit (EXIT_FAILURE);
  }

  st_page_props->title++;
  st_page_props->title = del_char_shift_left (' ', st_page_props->title);
  trim (st_page_props->title);

  /* get the layout line */
  /* declared statically because 'layout' points to it outside of this function */
  static char layout_line[LEN_MAX_LINE + 1];

  if (fgets (layout_line, LEN_MAX_LINE, fp) == NULL)
  {
    perror ("Error getting line");
    fclose (fp);
    exit (EXIT_FAILURE);
  }

  st_page_props->layout = strchr (layout_line, ':');

  if (st_page_props->layout == NULL)
  {
    printf ("%s has the wrong format\n", input_file);
    exit (EXIT_FAILURE);
  }

#ifdef DEBUG
PRINT_DEBUG ("Layout: '%s'\n", st_page_props->layout);
#endif

  st_page_props->layout++;
  st_page_props->layout = del_char_shift_left (' ', st_page_props->layout);

#ifdef DEBUG
PRINT_DEBUG ("Layout: '%s'\n", st_page_props->layout);
#endif

  trim (st_page_props->layout);

#ifdef DEBUG
PRINT_DEBUG ("Layout: '%s'\n", st_page_props->layout);
#endif

  /* Go back to the beginning of the file */
  if (fseek (fp, 0, SEEK_END) != 0)
  {
    perror ("Error while seeking file");
    exit (errno);
  }

  size_t len = ftell (fp);

  if ((st_page_props->contents = calloc (len + 1, 1)) == NULL)
  {
    printf ("Unable to allocate memory\n");
    exit (EXIT_FAILURE);
  }

  rewind (fp);
  fread (st_page_props->contents, len, 1, fp);

  if (fclose (fp) == EOF)
  {
    perror ("Error  closing");
    free (st_page_props->contents);
    exit (errno);
  }

  /* find the first ocurrence of "-" */
  // char *body;
  st_page_props->body = strchr (st_page_props->contents, '-');

  if (st_page_props->body == NULL)
  {
    printf ("%s has the wrong format\n", input_file);
    fclose (fp);
    free (st_page_props->contents);
    exit (EXIT_FAILURE);
  }

  while (*st_page_props->body == '-' || *st_page_props->body == '\n')
    st_page_props->body = del_char_shift_left (*st_page_props->body, st_page_props->body);

  len = strlen (st_page_props->body) - 1;

  /* truncate anything, especially newlines */
  while (st_page_props->body[len] != '>')
  {
    st_page_props->body[len] = '\0';
    len--;
  }

  /* truncate the .sct extension */
  input_file[strlen (input_file) - 4] = '\0';
  st_page_props->sct_basename = basename (input_file);

  if (strcmp (st_page_props->sct_basename, "index") != 0)
    strcpy (st_page_props->sub_title, cfgopts->site_title);
  else
    strcpy (st_page_props->sub_title, cfgopts->site_description);

#ifdef DEBUG
PRINT_DEBUG ("basename is '%s' at L%d\n", st_page_props->sct_basename, __LINE__);
PRINT_DEBUG ("sub_title is '%s' at L%d\n", st_page_props->sub_title, __LINE__);
#endif
}
