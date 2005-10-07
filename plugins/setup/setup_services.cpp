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

#ifdef SETUP_SERVICES

#include "setup.h"
#include "setup_services.h"
#include "setup_rc.h"
#include "setup_runapp.h"

#define TITLE "Services Setup"

#include<plugin.h>

#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>


#include <lib/base/i18n.h>

//#include <lib/gui/ewindow.h>
//#include <lib/dvb/edvb.h>
//#include <lib/dvb/eaudio.h>
//#include <lib/gui/elabel.h>
//#include <lib/gui/ebutton.h>
#include <lib/gui/echeckbox.h>
//#include <lib/gui/emessage.h>
//#include <lib/gui/eskin.h>
//#include <lib/driver/rc.h>
//#include <lib/system/econfig.h>
//#include <lib/gui/combobox.h>
//#include <lib/gui/listbox.h>
//#include <lib/gui/enumber.h>
//#include <lib/gui/textinput.h>


ServicesSetup::ServicesSetup ():
eWindow (0)
{
  RC->read ();

  setText (_("Services Setup"));
  cmove (ePoint (170, 150));
  cresize (eSize (390, 350));

  Serial = NULL;

  eStartServer = new eCheckbox (this, RC->StartServer, 1);
  eStartServer->setText (_("Start CS Server"));
  eStartServer->move (ePoint (10, 10));
  eStartServer->resize (eSize (170, 35));
  eStartServer->setHelpText (_("start/do not start Card Sharing Server"));
  CONNECT (eStartServer->checked, ServicesSetup::EmuStartServerChanged);

#if 0
  eStartDhcp = new eCheckbox (this, RC->StartDhcp, 1);
  eStartDhcp->setText (_("Use DHCP"));
  eStartDhcp->move (ePoint (10, 50));
  eStartDhcp->resize (eSize (170, 35));
  eStartDhcp->setHelpText (_("start/do not start DHCP Client daemon"));
  CONNECT (eStartDhcp->checked, ServicesSetup::EmuStartDhcpChanged);
#endif

  eStartCron = new eCheckbox (this, RC->StartCron, 1);
  eStartCron->setText (_("Start crond"));
  eStartCron->move (ePoint (10, 50));
  eStartCron->resize (eSize (170, 35));
  eStartCron->setHelpText (_("start/do not start crond the time daemon"));
  CONNECT (eStartCron->checked, ServicesSetup::EmuStartCronChanged);

  eStartSamba = new eCheckbox (this, RC->StartSamba, 1);
  eStartSamba->setText (_("Start Samba"));
  eStartSamba->move (ePoint (10, 90));
  eStartSamba->resize (eSize (170, 35));
  eStartSamba->setHelpText (_("start/do not start the samba daemons"));
  CONNECT (eStartSamba->checked, ServicesSetup::EmuStartSambaChanged);

#ifdef NOT_ANY_MORE
  v_CardInfo = CardInfo ();
  if (v_CardInfo == 0)
    {
      cardinfo = new eButton (this);
      cardinfo->setShortcut ("yellow");
      cardinfo->setShortcutPixmap ("yellow");
      cardinfo->setText (_("Card info"));
      cardinfo->move (ePoint (205, 10));
      cardinfo->resize (eSize (170, 35));
      cardinfo->setHelpText (_("Show some info of seca card"));
      cardinfo->loadDeco ();
      CONNECT (cardinfo->selected, ServicesSetup::cardinfoPressed);
    }
#endif

#if 0
  v_IpInfo = IpInfo ();
  if (v_IpInfo == 0)
    {
      ipinfo = new eButton (this);
      ipinfo->setShortcut ("blue");
      ipinfo->setShortcutPixmap ("blue");
      ipinfo->setText (_("DHCP info"));
      ipinfo->move (ePoint (205, 50));
      ipinfo->resize (eSize (170, 35));
      ipinfo->setHelpText (_("Show some info of seca card"));
      ipinfo->loadDeco ();
      CONNECT (ipinfo->selected, ServicesSetup::ipinfoPressed);
    }
#endif

  ok = new eButton (this);
  ok->setText (_("save"));
  ok->setShortcut ("green");
  ok->setShortcutPixmap ("green");
  ok->move (ePoint (20, 270));
  ok->resize (eSize (170, 40));
  ok->setHelpText (_("save changes and return"));
  ok->loadDeco ();
  CONNECT (ok->selected, ServicesSetup::okPressed);

  abort = new eButton (this);
  abort->setShortcut ("red");
  abort->setShortcutPixmap ("red");
  abort->loadDeco ();
  abort->setText (_("abort"));
  abort->move (ePoint (210, 270));
  abort->resize (eSize (170, 40));
  abort->setHelpText (_("ignore changes and return"));
  CONNECT (abort->selected, ServicesSetup::abortPressed);

  statusbar = new eStatusBar (this);
  statusbar->move (ePoint (0, clientrect.height () - 30));
  statusbar->resize (eSize (clientrect.width (), 30));
  statusbar->loadDeco ();
}

void
ServicesSetup::EmuStartServerChanged (int i)
{
  RC->StartServer = i;
}

void
ServicesSetup::EmuStartDhcpChanged (int i)
{
  RC->StartDhcp = i;
}

void
ServicesSetup::EmuStartCronChanged (int i)
{
  RC->StartCron = i;
}

void
ServicesSetup::EmuStartSambaChanged (int i)
{
  RC->StartSamba = i;
}

void
ServicesSetup::okPressed ()
{
#ifdef DEBUG
  printf ("okPressed\n");
#endif
  RC->write ();
  system ("killall sleep");
  close (0);
}

void
ServicesSetup::abortPressed ()
{
#ifdef DEBUG
  printf ("abortPressed\n");
#endif
  close (0);
}

#ifdef NOT_ANY_MORE
void
ServicesSetup::cardinfoPressed ()
{
  // printf ("keyupdatePressed\n");
  Executable = (char *) SCRIPTS[2];
  hide ();
  RunApp run;
  run.show ();
  run.exec ();
  run.hide ();
  show ();
}
#endif

void
ServicesSetup::ipinfoPressed ()
{
  // printf ("keyupdatePressed\n");
  Executable = (char *) SCRIPTS[3];
  hide ();
  RunApp run;
  run.show ();
  run.exec ();
  run.hide ();
  show ();
}

#ifdef NOT_ANY_MORE
int
ServicesSetup::CardInfo ()
{
  FILE *file;
  file = fopen ("/var/tmp/cardinfo", "r");
  if (file)
    {
      return 0;
      fclose (file);
    }
  else
    {
      return -1;
    }
}
#endif

int
ServicesSetup::IpInfo ()
{
  FILE *file;
  file = fopen ("/var/tmp/ipinfo", "r");
  if (file)
    {
      return 0;
      fclose (file);
    }
  else
    {
      return -1;
    }
}
#endif
