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

#define TEMPFILE "/tmp/TempFile"

void
feed2dir (char *src)
{
  FILE *F, *F2 = NULL;
  char line[2048], package[128], section[128], cmd[128];
  char dir[128], file[128];
  char *ptr, *two;
  int len;
  struct stat s;

  unlink (TEMPFILE);
  F = fopen (src, "r");
  if (F)
    {
      while (fgets (line, sizeof (line), F) != NULL)
        {
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
                  sprintf (dir, "/tmp/feeds/%s/%s", basename (src), section);
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
  system ("rm -rf /tmp/feeds");
  feed2dir ("/usr/lib/ipkg/lists/official");
  feed2dir ("/usr/lib/ipkg/lists/ronaldd");
}
