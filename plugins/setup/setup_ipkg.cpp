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
#include "setup_ipkg.h"
#include "setup_emu.h"
#include "setup_runapp.h"
#include "setup_usb.h"
#include "setup_install.h"
#include "setup_extra.h"
#include "setup_rc.h"
#include "setup_restore.h"
#include "feeds2dir.h"

#include <lib/gui/emessage.h>
#include <lib/gui/listbox.h>
#include <lib/system/econfig.h>
#include <enigma_plugins.h>
#include <lib/base/console.h>
#include <plugin.h>

#include <dirent.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#ifdef SETUP_IPKG

char exe[256];
char dir[256];
int InFileBrowser;

ipkgSetup::ipkgSetup ():
eSetupWindow (_("Package Manager"), 10, 350)
{
  move (ePoint (180, 100));
  int entry = 0;
  FILE *F;
  struct stat st;
  if (stat ("/etc/ipkg/ronaldd-feed.conf", &st) != 0)
    {
      F = fopen ("/etc/ipkg/ronaldd-feed.conf", "w");
      if (F)
        {
          fprintf (F, "src/gz ronaldd http://ronaldd.irde.to/ipkg/feed\n");
          fclose (F);
        }
    }
  CONNECT ((new
            eListBoxEntryMenu (&list, _("Update package database"),
                               eString ().sprintf ("(%d) %s", ++entry, _("Run ipkg update, to update package database"))))->selected, ipkgSetup::ipkg_update);
  CONNECT ((new
            eListBoxEntryMenu (&list, _("Upgrade all packages"),
                               eString ().sprintf ("(%d) %s", ++entry, _("Run ipkg upgrade, to upgrade all packages"))))->selected, ipkgSetup::ipkg_upgrade);
  CONNECT ((new
            eListBoxEntryMenu (&list, _("Install/remove packages"),
                               eString ().sprintf ("(%d) %s", ++entry, _("Install/remove packages"))))->selected, ipkgSetup::ipkg_inst_rem);
}

void
ipkgSetup::ipkg_update ()
{
  sprintf (exe, "ipkg update");
  Executable = exe;
  printf ("E: %s\n", Executable);
  strcpy (RUN_MESSAGE, "");
  hide ();
  RunApp run;
  run.show ();
  run.exec ();
  run.hide ();
  eMessageBox msg (_("please wait building package list.\n"), _("please wait..."), 0);
  msg.show ();
  feeds2dir ();
  msg.hide ();
  show ();
}

void
ipkgSetup::ipkg_upgrade ()
{
  sprintf (exe, "ipkg upgrade");
  Executable = exe;
  printf ("E: %s\n", Executable);
  strcpy (RUN_MESSAGE, "");
  strcpy (NO_OUTPUT_MESSAGE, "No package needed upgrade");
  hide ();
  RunApp run;
  run.show ();
  run.exec ();
  run.hide ();
  show ();
}

void
ipkgSetup::ipkg_inst_rem ()
{
  struct stat st;
  strcpy (dir, "/tmp/feeds");
  hide ();
  if (stat (dir, &st) != 0)
    {
      eMessageBox msg (_("please wait building package list.\n"), _("please wait..."), 0);
      msg.show ();
      feeds2dir ();
      msg.hide ();
    }
  InFileBrowser = 1;
  while (InFileBrowser)
    {
      ipkgInstRem win;
      InFileBrowser = 0;
      win.show ();
      win.exec ();
      win.hide ();
    }
  show ();
}


ipkgInstRem::ipkgInstRem ():
eSetupWindow (_("Package Manager"), 10, 350)
{
  struct dirent **namelist;
  int n, i;
  char name[128], file[320];
  struct stat st;

  move (ePoint (180, 100));

  listbox = new eListBox < eListBoxEntryText > (this);
  listbox->setText (_("Remove Software"));
  listbox->move (ePoint (10, 10));
  listbox->resize (eSize (clientrect.width () - 20, clientrect.height () - 60));
  listbox->loadDeco ();
  // listbox->show ();

  printf ("scandir: %s", dir);
  n = scandir (dir, &namelist, 0, alphasort);
  if (n < 0)
    perror ("scandir");
  else
    {
      eConfig::getInstance ()->setKey ("/temp/true", 1);
      eConfig::getInstance ()->setKey ("/temp/false", 0);
      for (i = 0; i < n; i++)
        {
          if (strcmp (namelist[i]->d_name, ".") == 0)
            continue;
          if (strcmp (namelist[i]->d_name, "..") == 0 && strcmp (dir, "/tmp/feeds") == 0)
            continue;
          printf ("%s\n", namelist[i]->d_name);
          sprintf (file, "%s/%s", dir, namelist[i]->d_name);
          stat (file, &st);
          if (st.st_mode & S_IFDIR)
            {
              sprintf (name, "[%s]", namelist[i]->d_name);
              new eListBoxEntryText (listbox, name, namelist[i]->d_name);
            }
          else
            {
              sprintf (name, "%s", namelist[i]->d_name);
              sprintf (file, "/usr/lib/ipkg/info/%s.control", namelist[i]->d_name);
              if (stat (file, &st) == 0)
                {
                  new eListBoxEntryCheck ((eListBox < eListBoxEntry > *)listbox, name, "/temp/true", _("bla"));
                }
              else
                {
                  new eListBoxEntryCheck ((eListBox < eListBoxEntry > *)listbox, name, "/temp/false", _("bla"));
                }
            }
          free (namelist[i]);
        }
      free (namelist);
      CONNECT (listbox->selected, ipkgInstRem::okPressed);
      setFocus (listbox);
    }
}

void
ipkgInstRem::okPressed (eListBoxEntryText * item)
{
  eString tmp;
  char file[320], ipkg_cmd[64];
  struct stat st;
  int res;
  tmp = item->getText ();
  if (tmp.c_str ()[0] == '[')
    {
      if (tmp == "[..]")
        {
          sprintf (dir, "%s", dirname (dir));
        }
      else
        {
          sprintf (dir, "%s/%s", dir, tmp.c_str () + 1);    // + 1 = skip [
          dir[strlen (dir) - 1] = '\0';
        }
    }
  else
    {
      sprintf (file, "/usr/lib/ipkg/info/%s.control", tmp.c_str ());
      if (stat (file, &st) == 0)
        {
          eMessageBox msg (_("Are you SURE to remove this package?\n"),
                           _("Are you SURE to remove this package?"), eMessageBox::btYes | eMessageBox::btCancel, eMessageBox::btCancel);
          msg.show ();
          res = msg.exec ();
          msg.hide ();
          sprintf (exe, "ipkg remove %s", tmp.c_str ());
        }
      else
        {
          eMessageBox msg (_("Are you SURE to install this package?\n"),
                           _("Are you SURE to install this package?"), eMessageBox::btYes | eMessageBox::btCancel, eMessageBox::btCancel);
          msg.show ();
          res = msg.exec ();
          msg.hide ();
          if (strcmp (tmp.c_str (), "setup-plugin") == 0)
            strcpy (ipkg_cmd, "-force-overwrite install");
          else
            strcpy (ipkg_cmd, "install");
          sprintf (exe, "ipkg %s %s", ipkg_cmd, tmp.c_str ());
        }
      if (res == eMessageBox::btYes)
        {
          Executable = exe;
          printf ("E: %s\n", Executable);
          strcpy (RUN_MESSAGE, "");
          hide ();
          RunApp run;
          run.show ();
          run.exec ();
          run.hide ();
          show ();
        }
    }
  InFileBrowser = 1;
  close (0);
}

#endif // SETUP_IPKG
