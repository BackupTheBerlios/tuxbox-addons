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
#include <lib/gui/echeckbox.h>

#include <dirent.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#ifdef SETUP_IPKG

char exe[256];
char dir[256];
int InFileBrowser;
int qwerty;

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
  if (stat ("/etc/ipkg/official-updated-ronaldd-feed.conf", &st) != 0)
    {
      CONNECT ((new
                eListBoxEntryMenu (&list, _("Upgrade to Ronaldd image"),
                                   eString ().sprintf ("(%d) %s", ++entry, _("Select to upgrade official images to ronaldd images"))))->selected,
               ipkgSetup::ipkg2ronaldd);
    }
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
  struct stat st1, st2;
  int rv;
  strcpy (dir, "/tmp/feeds");
  hide ();
  rv = stat (dir, &st1);
  stat ("/usr/lib/ipkg/lists/official", &st2);
  if (st1.st_mtime < st2.st_mtime || rv != 0)
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

void
ipkgSetup::ipkg2ronaldd ()
{
  int res;
  eMessageBox
    msg (_
         ("This wil upgrade some original packages (enigma and others) to changed versions, for better emu support\n\nIf you want to go back to an original image, you need to flash the original .nfi file"),
         _("Upgrade to Ronaldd image?"), eMessageBox::btYes | eMessageBox::btCancel, eMessageBox::btCancel);
  msg.show ();
  res = msg.exec ();
  msg.hide ();
  if (res == eMessageBox::btYes)
    {
      strcpy (exe, "mk_ronaldd_image.sh");
      Executable = exe;
      hide ();
      RunApp run;
      run.show ();
      run.exec ();
      run.hide ();
      show ();
    }
}


void
get_package_details (char *file, char *version, char *section)
{
  FILE *F;
  char line[2048], *ptr;

  if (version)
    strcpy (version, "?");

  F = fopen (file, "r");
  if (F)
    {
      while (fgets (line, sizeof (line), F) != NULL)
        {
          if ((ptr = strchr (line, '\n')) != NULL)
            {
              *ptr = '\0';
            }
          if (section && strncmp (line, "Section: ", 9) == 0)
            {
              if ((ptr = strchr (line, ' ')) != NULL)
                {
                  ptr++;
                  strcpy (section, ptr);
                }
            }
          if (version && strncmp (line, "Version: ", 9) == 0)
            {
              if ((ptr = strchr (line, ' ')) != NULL)
                {
                  ptr++;
                  strcpy (version, ptr);
                }
            }
        }
    }
}

ipkgInstRem::ipkgInstRem ():
eSetupWindow (_("Package Manager"), 10, 450)
{
  struct dirent **namelist;
  int n, i;
  char name[128], file[320], version[64];
  struct stat st;

  move (ePoint (130, 100));

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
              get_package_details (file, version, NULL);
              sprintf (name, "%s - %s", namelist[i]->d_name, version);
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
  char file[320], package[256], ipkg_cmd[64], section[128];
  struct stat st;
  int res, i;

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
      strcpy (package, tmp.c_str ());
      for (i = 0; i < strlen (package); i++)
        if (package[i] == ' ' && package[i + 1] == '-' && package[i + 2] == ' ')
          package[i] = '\0';
      sprintf (file, "/usr/lib/ipkg/info/%s.control", package);
      if (stat (file, &st) == 0)
        {
          qwerty = 1;
          ipkgConfirm confirm;
          confirm.show ();
          confirm.exec ();
          confirm.hide ();
          res = confirm.do_remove;
          strcpy (ipkg_cmd, "remove");
          if (confirm.do_install && confirm.do_remove)
            {
              strcpy (ipkg_cmd, "-force-reinstall install");
              get_package_details (file, NULL, section);
              if ((strcmp (package, "setup-plugin") == 0) || (strcmp (section, "settings") == 0) || confirm.v_force_overwrite)
                {
                  sprintf (exe, "%s %s", "-force-overwrite", ipkg_cmd);
                  strcpy (ipkg_cmd, exe);
                }
            }
          sprintf (exe, "ipkg %s %s", ipkg_cmd, package);
        }
      else
        {
          qwerty = 0;
          ipkgConfirm confirm;
          confirm.show ();
          confirm.exec ();
          confirm.hide ();
          res = confirm.do_install;
          sprintf (file, "%s/%s", dir, package);
          get_package_details (file, NULL, section);
          strcpy (ipkg_cmd, "install");
          if ((strcmp (package, "setup-plugin") == 0) || (strcmp (section, "settings") == 0) || confirm.v_force_overwrite)
            strcpy (ipkg_cmd, "-force-overwrite install");
          sprintf (exe, "ipkg %s %s", ipkg_cmd, package);
        }
      if (res)
        {
          if ((qwerty == 0) && (strcmp (section, "settings") == 0) && (stat ("/etc/.settings_package", &st) == 0))
            {
              eMessageBox msg (_("please wait removing old settings package.\n"), _("please wait..."), 0);
              msg.show ();
              system ("ipkg remove `cat /etc/.settings_package`");
              msg.hide ();
            }
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

ipkgConfirm::ipkgConfirm ():
eWindow (0)
{
  printf ("ipkgConfirm::ipkgConfirm action=%d\n", qwerty);
  do_install = 0;
  do_remove = 0;
  v_force_overwrite = 0;

  cmove (ePoint (170, 150));
  cresize (eSize (380, 330));

  force_overwrite = new eCheckbox (this, v_force_overwrite, 1);
  force_overwrite->setText (_("Force overwrite"));
  force_overwrite->move (ePoint (10, 10));
  force_overwrite->resize (eSize (180, 35));
  force_overwrite->setHelpText (_("Select to overwrite existing files that are also in package."));
  CONNECT (force_overwrite->checked, ipkgConfirm::force_overwriteChanged);

  if (qwerty == 0)
    {
      setText (_("Install package?"));
      install = new eButton (this);
      install->setText ("install");
      install->setShortcut ("green");
      install->setShortcutPixmap ("green");
      install->move (ePoint (10, 230));
      install->resize (eSize (170, 40));
      install->setHelpText (_("Install package"));
      install->loadDeco ();
      CONNECT (install->selected, ipkgConfirm::installPressed);
    }
  else
    {
      setText (_("Remove package?"));
      reinstall = new eButton (this);
      reinstall->setShortcut ("yellow");
      reinstall->setShortcutPixmap ("yellow");
      reinstall->loadDeco ();
      reinstall->setText ("reinstall");
      reinstall->move (ePoint (10, 180));
      reinstall->resize (eSize (170, 40));
      reinstall->setHelpText (_("Re-Install package"));
      CONNECT (reinstall->selected, ipkgConfirm::reinstallPressed);

      remove = new eButton (this);
      remove->setShortcut ("green");
      remove->setShortcutPixmap ("green");
      remove->loadDeco ();
      remove->setText ("remove");
      remove->move (ePoint (10, 230));
      remove->resize (eSize (170, 40));
      remove->setHelpText (_("Remove package"));
      CONNECT (remove->selected, ipkgConfirm::removedPressed);
    }
  abort = new eButton (this);
  abort->setShortcut ("red");
  abort->setShortcutPixmap ("red");
  abort->loadDeco ();
  abort->setText (_("cancel"));
  abort->move (ePoint (200, 230));
  abort->resize (eSize (170, 40));
  abort->setHelpText (_("Do nothing"));
  CONNECT (abort->selected, ipkgConfirm::abortPressed);

  statusbar = new eStatusBar (this);
  statusbar->move (ePoint (0, clientrect.height () - 50));
  statusbar->resize (eSize (clientrect.width (), 50));
  statusbar->loadDeco ();
}

void
ipkgConfirm::installPressed ()
{
  do_install = 1;
  do_remove = 0;
  close (0);
}

void
ipkgConfirm::reinstallPressed ()
{
  do_install = 1;
  do_remove = 1;
  close (0);
}

void
ipkgConfirm::removedPressed ()
{
  do_install = 0;
  do_remove = 1;
  close (0);
}

void
ipkgConfirm::abortPressed ()
{
  close (0);
}

void
ipkgConfirm::force_overwriteChanged (int i)
{
  v_force_overwrite = i;
}

#endif // SETUP_IPKG
