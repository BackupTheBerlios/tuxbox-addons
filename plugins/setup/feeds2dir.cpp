/*
 * Ronald's setup plugin for dreambox
 * Copyright (c) 2004 Ronaldd <Ronaldd@sat4all.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#define TEMPFILE "/tmp/TempFile"

void
feed2dir (char *src)
{
  FILE *F, *F2 = NULL;
  char line[2048], package[512], section[512], cmd[512];
  char dir[512], file[512], feed[128];
  char *ptr, *two;
  int len;
  struct stat s;

  printf ("feed2dir: %s\n", src);

  if (strncmp (basename (src), "official-updated-ronaldd", 24) == 0)
    strcpy (feed, "official");
  else
    strcpy (feed, basename (src));

  unlink (TEMPFILE);
  F = fopen (src, "r");
  if (F)
    {
      while (fgets (line, sizeof (line), F) != NULL)
        {
          if (strlen (line) > 2000)
            printf ("LONG: %s\n", line);
          if ((ptr = strchr (line, '\n')) != NULL)
            {
              *ptr = '\0';
            }
          if (!F2)
            F2 = fopen (TEMPFILE, "w");
          if (F2)
            {
              len = strlen (line);
              if (strlen (line) == 0 && strlen (package) > 0)
                {
                  fclose (F2);
                  F2 = NULL;
                  sprintf (dir, "/tmp/feeds/%s/%s", feed, section);
                  if (stat (dir, &s) < 0)
                    {
                      sprintf (cmd, "mkdir -p %s", dir);
                      system (cmd);
                    }
                  sprintf (file, "%s/%s", dir, package);
                  rename (TEMPFILE, file);
                  package[0] = '\0';
                  section[0] = '\0';
                }
              else
                {
                  fprintf (F2, "%s\n", line);
                  if ((ptr = strchr (line, ' ')) != NULL)
                    {
                      *ptr = '\0';
                      two = ptr + 1;
                      if (strcmp (line, "Section:") == 0)
                        {
                          strcpy (section, two);
                          // printf ("Section: %s\n", section);
                        }
                      if (strcmp (line, "Package:") == 0)
                        {
                          strcpy (package, two);
                          // printf ("Package: %s\n", package);
                        }
                    }
                }
            }
        }
    }
  else
    {
      printf ("Cannot open file\n");
    }
  if (F)
    fclose (F);
}

void
feeds2dir ()
{
  FILE *F;
  char list[128], *ptr;
  char line[128];

  system ("rm -rf /tmp/feeds");
  system ("grep -h ^src /etc/ipkg/*.conf |awk '{print $2}' >/tmp/lists");   // Not nice
  F = fopen ("/tmp/lists", "r");
  if (F)
    {
      while (fgets (line, sizeof (line), F) != NULL)
        {
          if ((ptr = strchr (line, '\n')) != NULL)
            {
              *ptr = '\0';
            }
          sprintf (list, "/usr/lib/ipkg/lists/%s", line);
          // if ( strncmp ( "official-updated-ronaldd", line, 24) != 0 )
          feed2dir (list);
        }
      fclose (F);
    }
  unlink ("/tmp/lists");
}
