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

#ifdef SETUP_INST

#include "setup.h"
#include "setup_install.h"
#include "setup_emu.h"
#include "setup_runapp.h"
#include "setup_usb.h"

#include <lib/gui/emessage.h>
#include <lib/gui/listbox.h>
#include <enigma_plugins.h>

char exe[256];
int dlOk;

InstallSw::InstallSw ():eWindow (0)
{
  printf ("InstallSw::InstallSw()\n");

  setText (_("TEST"));
  cmove (ePoint (100, 100));
  cresize (eSize (520, 400));

  dummyb = new eListBox < eListBoxEntryText > (this);
  dummyb->setText (_("TEST"));
  dummyb->move (ePoint (10, 10));
  dummyb->resize (eSize (500, 300));
  dummyb->loadDeco ();

  abort = new eButton (this);
  abort->setText (_("Cancel"));
  abort->setShortcut ("red");
  abort->setShortcutPixmap ("red");
  abort->move (ePoint (20, 320));
  abort->resize (eSize (170, 40));
  abort->setHelpText (_("Exit installer"));
  abort->loadDeco ();
  CONNECT (abort->selected, InstallSw::abortPressed);

  dl = new eButton (this);
  dl->setText (_("Get SW-List"));
  dl->setShortcut ("blue");
  dl->setShortcutPixmap ("blue");
  dl->move (ePoint (200, 320));
  dl->resize (eSize (170, 40));
  dl->setHelpText (_("Dowmload software list from internet"));
  dl->loadDeco ();
  CONNECT (dl->selected, InstallSw::dlPressed);

  statusbar = new eStatusBar (this);
  statusbar->move (ePoint (0, clientrect.height () - 30));
  statusbar->resize (eSize (clientrect.width (), 30));
  statusbar->loadDeco ();
}

RemoveSw::RemoveSw ():eWindow (0)
{
  printf ("RemoveSw::RemoveSw()\n");
  FILE *
    F;
  char
    line[256];
  int
    entry =
    0;

  setText (_("TEST"));
  cmove (ePoint (100, 100));
  cresize (eSize (520, 400));

  abort = new eButton (this);
  abort->setText (_("Cancel"));
  abort->setShortcut ("red");
  abort->setShortcutPixmap ("red");
  abort->move (ePoint (20, 320));
  abort->resize (eSize (170, 40));
  abort->setHelpText (_("Exit installer"));
  abort->loadDeco ();
  CONNECT (abort->selected, RemoveSw::abortPressed);

  listb = new eListBox < eListBoxEntryText > (this);
  listb->setText (_("TEST"));
  listb->move (ePoint (10, 10));
  listb->resize (eSize (500, 300));
  listb->loadDeco ();
  listb->show ();

  system ("ls /var/tuxbox/installer >/var/tmp/installed");

  F = fopen ("/var/tmp/installed", "r");
  if (F)
    {
      while (fgets (line, 256, F) != NULL)
        {
          new
          eListBoxEntryText (listb, _(line), (void *) entry);
        }
      fclose (F);
    }
  CONNECT (listb->selected, RemoveSw::do_remove);

  statusbar = new eStatusBar (this);
  statusbar->move (ePoint (0, clientrect.height () - 30));
  statusbar->resize (eSize (clientrect.width (), 30));
  statusbar->loadDeco ();
}

InstallSw::~InstallSw ()
{
  printf ("InstallSw::~InstallSw()\n");
}

void
InstallSw::abortPressed ()
{
  printf ("InstallSw::abortPressed()\n");
  close (1);
}

RemoveSw::~RemoveSw ()
{
  printf ("RemoveSw::~RemoveSw()\n");
}

void
RemoveSw::abortPressed ()
{
  printf ("RemoveSw::abortPressed()\n");
  close (1);
}

void
InstallSw::dlPressed ()
{
  printf ("InstallSw::dlPressed()\n");
  FILE *F;
  char line[256], one[128], two[128], three[128];
  int entry = 0;
  // eListBoxEntryText * entrys[25];
  dl->hide ();

  listb = new eListBox < eListBoxEntryText > (this);
  listb->setText (_("TEST"));
  listb->move (ePoint (10, 10));
  listb->resize (eSize (500, 300));
  listb->loadDeco ();

  system ("/bin/install.sh get_packagelist");

  F = fopen ("/var/tmp/package_list.txt", "r");
  if (F)
    {
      while (fgets (line, 256, F) != NULL)
        {
          if (sscanf (line, "%s %s %s", one, two, three) == 3)
            {
              sprintf (line, "%s %s", one, two);
              printf ("qq\n");
              //entrys[entry] = new eListBoxEntryText (listb, _(line), (void *) entry);
              new eListBoxEntryText (listb, _(line), (void *) entry);
            }
        }
      fclose (F);
    }
  dummyb->hide ();
  listb->show ();
  CONNECT (listb->selected, InstallSw::do_install);
}

dlWindow::dlWindow ():eWindow (0)
{
  setText (_("Downloading software, please wait..."));
  cmove (ePoint (150, 200));
  cresize (eSize (350, 150));

  ok = new eButton (this);
  ok->setText (_("Install"));
  ok->setShortcut ("green");
  ok->setShortcutPixmap ("green");
  ok->move (ePoint (20, 100));
  ok->resize (eSize (140, 40));
  ok->setHelpText (_("Install the software"));
  ok->loadDeco ();
  ok->hide ();
  CONNECT (ok->selected, dlWindow::okPressed);

  abort = new eButton (this);
  abort->setText (_("Cancel"));
  abort->setShortcut ("red");
  abort->setShortcutPixmap ("red");
  abort->move (ePoint (170, 100));
  abort->resize (eSize (140, 40));
  abort->setHelpText (_("Cancel download"));
  abort->loadDeco ();
  CONNECT (abort->selected, dlWindow::abortPressed);

  // system (exe);
  app = NULL;
  app = new eConsoleAppContainer (eString ().sprintf (exe));
  if (!app->running ())
    {
      eMessageBox msg (_("sorry, cannot find script."), _("sorry, cannot find script."), eMessageBox::btOK | eMessageBox::iconError);
      msg.show ();
      msg.exec ();
      msg.hide ();
      close (-1);
    }
  else
    {
      // CONNECT( app->dataAvail, dlWindow::getData );
      CONNECT (app->appClosed, dlWindow::appClosed);
    }
}

void
dlWindow::appClosed (int i)
{
  ok->show ();
}

void
dlWindow::abortPressed ()
{
  printf ("InstallSw::abortPressed()\n");
  system ("killall wget");
  dlOk = 0;
  close (1);
}

void
dlWindow::okPressed ()
{
  printf ("InstallSw::okPressed()\n");
  dlOk = 1;
  close (1);
}

void
InstallSw::do_install (eListBoxEntryText * item)
{
  printf ("InstallSw::do_install()\n");
  if (item)
    {
      eString abc = item->getText ();
      sprintf (exe, "/bin/install.sh download %s", abc.c_str ());
      dlWindow dl;
      dl.show ();
      dl.exec ();
      dl.hide ();
      if (dlOk)
        {
          sprintf (exe, "/bin/install.sh install %s", abc.c_str ());
          Executable = exe;
          printf ("E: %s\n", Executable);
          strcpy (RUN_MESSAGE, "Installing software, please wait this may take a minute");
          hide ();
          RunApp run;
          run.show ();
          run.exec ();
          run.hide ();
          show ();
        }
      else
        {
          sprintf (exe, "/bin/install.sh cleanup");
          system (exe);
        }
    }
}

void
RemoveSw::do_remove (eListBoxEntryText * item)
{
  printf ("RemoveSw::do_remove()\n");
  if (item)
    {
      char exe[256];
      eString abc = item->getText ();
      sprintf (exe, "/bin/install.sh remove %s", abc.c_str ());
      //sprintf(exe, "/bin/install.sh install var_gbox -" );
      Executable = exe;
      printf ("E: %s\n", Executable);
      strcpy (RUN_MESSAGE, "Removing software, please wait");
      hide ();
      RunApp run;
      run.show ();
      run.exec ();
      run.hide ();
      show ();
      listb->remove (item, false);
    }
}
#endif // SETUP_INST
