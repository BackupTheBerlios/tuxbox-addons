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

#include "setup.h"
#include "setup_rc.h"

#define DEBUG 1

#include<plugin.h>

#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>


#include <lib/base/i18n.h>

#include <lib/dvb/edvb.h>
#include <lib/dvb/eaudio.h>
#include <lib/gui/elabel.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/echeckbox.h>
#include <lib/gui/emessage.h>
#include <lib/gui/eskin.h>
#include <lib/driver/rc.h>
#include <lib/system/econfig.h>
#include <lib/gui/combobox.h>
#include <lib/gui/listbox.h>
#include <lib/gui/enumber.h>
#include <lib/gui/textinput.h>

void
rc_config::read ()
{
  FILE *file;
  char buf[256];
  char *ptr;
  char left[32], right[64], line[128];
  int i;
#ifdef DEBUG
  printf ("ReadRC()\n");
#endif

  system ("mk_emulist.sh");
  file = fopen ("/var/tmp/emulist", "r");
  if (file)
    {
      i = 0;
      while (fscanf (file, "%s\n", buf) != EOF)
        {
          // printf( "Q: (%s)\n", buf );
          strcpy (EMU[i], buf);
          i++;
        }
      fclose (file);
      RC->no_emu = i;
    }
  else
    {
      RC->no_emu = 0;
    }

  StartDhcp = 0;
  StartServer = 0;
  StartCron = 0;
  StartSamba = 0;
  Enabled = 0;
  v_SelectedEmu = 0;
  strcpy (RC->var_on, "flash");
  strcpy (RC->swap_on, "no");
  sleep = 0;

  file = fopen (RC_CONFIG, "r");
  if (file)
    {
      while (fgets (line, 256, file) != NULL)
        {
          if ((ptr = strstr (line, "=")))
            *ptr = ' ';
          if (sscanf (line, "%s %s", left, right) == 2)
            {
#ifdef DEBUG
              printf ("RC_CONFIG: %s=%s\n", left, right);
#endif

              if (strcmp (left, "SWAP_ON") == 0)
                strcpy (RC->swap_on, right);

              if (strcmp (left, "SWAP_SIZE") == 0)
                strcpy (RC->swap_size, right);

              if (strcmp (left, "VAR_ON") == 0)
                strcpy (RC->var_on, right);

              if (strcmp (left, "SLEEP") == 0)
                sscanf (right, "%d", &RC->sleep);

              if (strcmp (left, "CS_SERVER") == 0)
                if (strcmp (right, "enabled") == 0)
                  RC->StartServer = 1;

              if (strcmp (left, "SAMBA") == 0)
                if (strcmp (right, "enabled") == 0)
                  RC->StartSamba = 1;

              if (strcmp (left, "CRON") == 0)
                if (strcmp (right, "enabled") == 0)
                  RC->StartCron = 1;

              if (strcmp (left, "DHCP") == 0)
                if (strcmp (right, "enabled") == 0)
                  RC->StartDhcp = 1;

              if (strcmp (left, "EMU") == 0)
                for (i = 0; i < RC->no_emu; i++)
                  {
                    // printf ("right=(%s) EMU[i]=(%s) i=%i\n", right, EMU[i], i );
                    if (strcmp (right, EMU[i]) == 0)
                      {
                        RC->Enabled = 1;
                        RC->v_SelectedEmu = i;
                      }
                  }

            }
        }
      fclose (file);
    }
#ifdef DEBUG
  printf ("StartServer=%d StartSamba=%d StartCron=%d StartDhcp=%d no_emu=%d\n", RC->StartServer, RC->StartSamba, RC->StartCron, RC->StartDhcp, RC->no_emu);
  printf ("RC_CONFIG2: EMU=%s\n", EMU[RC->v_SelectedEmu]);
  printf ("RC_CONFIG2: CS_SERVER=%s\n", RC->StartServer ? "enabled" : "disabled");
  printf ("RC_CONFIG2: SAMBA=%s\n", RC->StartSamba ? "enabled" : "disabled");
  printf ("RC_CONFIG2: CRON=%s\n", RC->StartCron ? "enabled" : "disabled");
  printf ("RC_CONFIG2: DHCP=%s\n", RC->StartDhcp ? "enabled" : "disabled");
#endif
}

void
rc_config::write ()
{
  FILE *in, *out;
#ifdef DEBUG
  printf ("WriteRC(): StartServer=%d StartSamba=%d StartCron=%d StartDhcp=%d\n", RC->StartServer, RC->StartSamba, RC->StartCron, RC->StartDhcp);
#endif
  in = fopen (RC_CONFIG, "r");
  out = fopen (RC_CONFIG ".new", "w");
  char *ptr;
  char left[32], right[64], line[128], line2[128];
  int done = 0;

  if (in && out)
    {
      while (fgets (line, 256, in) != NULL)
        {
          strcpy (line2, line);
          if ((ptr = strstr (line, "=")))
            *ptr = ' ';
          if (sscanf (line, "%s %s", left, right) == 2)
            {
#ifdef DEBUG
              printf ("RC_CONFIG (write): %s=%s\n", left, right);
#endif

              if (strcmp (left, "CS_SERVER") == 0)
                {
                  fprintf (out, "CS_SERVER=%s\n", RC->StartServer ? "enabled" : "disabled");
                  done |= 1;
                }

              else if (strcmp (left, "SAMBA") == 0)
                {
                  fprintf (out, "SAMBA=%s\n", RC->StartSamba ? "enabled" : "disabled");
                  done |= 2;
                }

              else if (strcmp (left, "CRON") == 0)
                {
                  fprintf (out, "CRON=%s\n", RC->StartCron ? "enabled" : "disabled");
                  done |= 4;
                }

              else if (strcmp (left, "DHCP") == 0)
                {
                  fprintf (out, "DHCP=%s\n", RC->StartDhcp ? "enabled" : "disabled");
                  done |= 8;
                }

              else if (strcmp (left, "EMU") == 0)
                {
                  fprintf (out, "EMU=%s\n", EMU[RC->v_SelectedEmu]);
                  done |= 16;
                }

              else if (strcmp (left, "SLEEP") == 0)
                {
                  fprintf (out, "SLEEP=%d\n", RC->sleep);
                  done |= 32;
                }

              else if (strcmp (left, "VAR_ON") == 0)
                {
                  fprintf (out, "VAR_ON=%s\n", RC->var_on);
                  done |= 64;
                }
              else if (strcmp (left, "SWAP_ON") == 0)
                {
                  fprintf (out, "SWAP_ON=%s\n", RC->swap_on);
                  done |= 128;
                }
              else if (strcmp (left, "SWAP_SIZE") == 0)
                {
                  fprintf (out, "SWAP_SIZE=%s\n", RC->swap_size);
                  done |= 256;
                }

              else
                fprintf (out, "%s", line2);
            }
          else
            fprintf (out, "%s", line2);
        }
      if ((done & 1) == 0)
        fprintf (out, "CS_SERVER=%s\n", RC->StartServer ? "enabled" : "disabled");
      if ((done & 2) == 0)
        fprintf (out, "SAMBA=%s\n", RC->StartSamba ? "enabled" : "disabled");
      if ((done & 4) == 0)
        fprintf (out, "CRON=%s\n", RC->StartCron ? "enabled" : "disabled");
      if ((done & 8) == 0)
        fprintf (out, "DHCP=%s\n", RC->StartDhcp ? "enabled" : "disabled");
      if ((done & 16) == 0)
        fprintf (out, "EMU=%s\n", EMU[RC->v_SelectedEmu]);
      if ((done & 32) == 0)
        fprintf (out, "SLEEP=%d\n", RC->sleep);
      if ((done & 64) == 0)
        fprintf (out, "VAR_ON=%s\n", RC->var_on);
      if ((done & 128) == 0)
        fprintf (out, "SWAP_ON=%s\n", RC->swap_on);
      if ((done & 256) == 0)
        fprintf (out, "SWAP_SIZE=%s\n", RC->swap_size);

      if (fclose (in) == 0)
        in = NULL;
      if (fclose (out) == 0)
        out = NULL;
      rename (RC_CONFIG ".new", RC_CONFIG);
    }
  if (in)
    fclose (in);
  if (out)
    fclose (out);
  // sprintf (line, "ln -sf ../init.d/emu_%s /etc/rcS.d/S40emu", EMU[RC->v_SelectedEmu]); system (line);
  // system ("killall sleep");
#ifdef DEBUG
  printf ("End WriteRC()\n");
#endif
}
