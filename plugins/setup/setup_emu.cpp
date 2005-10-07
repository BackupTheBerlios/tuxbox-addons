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

#ifdef SETUP_EMU

#include <dlfcn.h>

#include "setup.h"
#include "setup_emu.h"
#include "setup_runapp.h"
#include "setup_main.h"
#include "setup_cs.h"
#include "setup_gbox.h"
#include "setup_rdg.h"
#include "setup_rc.h"

#define TITLE "Emu setup"

#include<plugin.h>

#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>


#include <lib/base/i18n.h>

//#include <lib/gui/ewindow.h>
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


eZapEmuSetup::eZapEmuSetup ():
eWindow (0)
{
  int i;

#ifdef DEBUG
  printf ("eZapEmuSetup::eZapEmuSetup()\n");
#endif

  Serial = NULL;

  RC->read ();

  setText (_(TITLE));
  cmove (ePoint (170, 150));
  cresize (eSize (390, 350));

  int fd = eSkin::getActive ()->queryValue ("fontsize", 16);

#ifdef OLD_CODE
  RC->Enabled = eZapEmuSetup::EmuEnabled ();
  RC->StartServer = eZapEmuSetup::EmuStartServer ();
  RC->StartDhcp = eZapEmuSetup::EmuStartDhcp ();
  RC->v_SelectedEmu = eZapEmuSetup::WichEmu ();
#endif

  eLabel *l = new eLabel (this);
  l->setText (_("Select Emu:"));
  l->move (ePoint (10, 10));
  l->resize (eSize (200, fd + 4));

  eListBoxEntryText *entrys[10];
  SelectedEmu = new eListBox < eListBoxEntryText > (this, l);
  SelectedEmu->loadDeco ();
  SelectedEmu->setFlags (eListBox < eListBoxEntryText >::flagNoUpDownMovement);
  SelectedEmu->move (ePoint (160, 10));
  SelectedEmu->resize (eSize (200, 35));
  for (i = 0; i < RC->no_emu; i++)
    {
      // printf ("R:(%s) i=%i\n", EMU[i], i);
      entrys[i] = new eListBoxEntryText (SelectedEmu, EMU[i], (void *) i);
    }
  SelectedEmu->setCurrent (entrys[RC->v_SelectedEmu]);
  SelectedEmu->setHelpText (_("choose emu ( left, right )"));
  CONNECT (SelectedEmu->selected, eZapEmuSetup::EmuSetup);

  eLabel *l2 = new eLabel (this);
  l2->setText (_("Select Server:"));
  l2->move (ePoint (10, 50));
  l2->resize (eSize (200, fd + 4));

  eListBoxEntryText *entrys2[10];
  SelectedCrdsrv = new eListBox < eListBoxEntryText > (this, l);
  SelectedCrdsrv->loadDeco ();
  SelectedCrdsrv->setFlags (eListBox < eListBoxEntryText >::flagNoUpDownMovement);
  SelectedCrdsrv->move (ePoint (160, 50));
  SelectedCrdsrv->resize (eSize (200, 35));
  for (i = 0; i < RC->no_crdsrv; i++)
    {
      // printf ("R:(%s) i=%i\n", CRDSRV[i], i);
      entrys2[i] = new eListBoxEntryText (SelectedCrdsrv, CRDSRV[i], (void *) i);
    }
  SelectedCrdsrv->setCurrent (entrys2[RC->v_SelectedCrdsrv]);
  SelectedCrdsrv->setHelpText (_("choose card server ( left, right )"));
  CONNECT (SelectedCrdsrv->selected, eZapEmuSetup::EmuSetup);


#ifdef NOT_ANY_MORE
  eEmuEnabled = new eCheckbox (this, Enabled, 1);
  eEmuEnabled->setText (_("Emu"));
  eEmuEnabled->move (ePoint (10, 50));
  eEmuEnabled->resize (eSize (fd + 4 + 150, fd + 4));
  eEmuEnabled->setHelpText (_("enable/disable emu"));
  CONNECT (eEmuEnabled->checked, eZapEmuSetup::EmuEnabledChanged);
#endif

  keyupdate = new eButton (this);
  keyupdate->setShortcut ("blue");
  keyupdate->setShortcutPixmap ("blue");
  keyupdate->setText (_("Key setting"));
  keyupdate->move (ePoint (20, 90));
  keyupdate->resize (eSize (170, 40));
  keyupdate->setHelpText (_("Launch the key update and convert plugin"));
  keyupdate->loadDeco ();
  CONNECT (keyupdate->selected, eZapEmuSetup::keyupdatePressed);

#ifdef NOT_ANY_MORE
  softcam2all = new eButton (this);
  softcam2all->setText (_("Soft -> all"));
  softcam2all->setShortcut ("yellow");
  softcam2all->setShortcutPixmap ("yellow");
  softcam2all->move (ePoint (20, 100));
  softcam2all->resize (eSize (170, 40));
  softcam2all->setHelpText (_("Convert SoftCam.Key to all other key formats"));
  softcam2all->loadDeco ();
  CONNECT (softcam2all->selected, eZapEmuSetup::softcam2allPressed);
#endif

  v_CardInfo = CardInfo ();
  if (v_CardInfo == 0)
    {
      cardinfo = new eButton (this);
      cardinfo->setShortcut ("yellow");
      cardinfo->setShortcutPixmap ("yellow");
      cardinfo->setText (_("Card info"));
      cardinfo->move (ePoint (210, 90));
      cardinfo->resize (eSize (170, 40));
      cardinfo->setHelpText (_("Show some info of seca card"));
      cardinfo->loadDeco ();
      CONNECT (cardinfo->selected, eZapEmuSetup::cardinfoPressed);
    }

#ifdef NOT_ANY_MORE
  extraset = new eButton (this);
  extraset->setShortcut ("1");
  extraset->setShortcutPixmap ("1");
  extraset->setText (_("Extra Setup"));
  extraset->move (ePoint (20, 220));
  extraset->resize (eSize (170, 40));
  extraset->setHelpText (_("Enter the extra setup menu"));
  extraset->loadDeco ();
  CONNECT (extraset->selected, eZapEmuSetup::ExtraSettings);

  v_IpInfo = IpInfo ();
  if (v_IpInfo == 0)
    {
      ipinfo = new eButton (this);
      ipinfo->setShortcut ("2");
      ipinfo->setShortcutPixmap ("2");
      ipinfo->setText (_("DHCP info"));
      ipinfo->move (ePoint (210, 220));
      ipinfo->resize (eSize (170, 40));
      ipinfo->setHelpText (_("Show some info of seca card"));
      ipinfo->loadDeco ();
      CONNECT (ipinfo->selected, eZapEmuSetup::ipinfoPressed);
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
  CONNECT (ok->selected, eZapEmuSetup::okPressed);

  abort = new eButton (this);
  abort->setShortcut ("red");
  abort->setShortcutPixmap ("red");
  abort->loadDeco ();
  abort->setText (_("abort"));
  abort->move (ePoint (210, 270));
  abort->resize (eSize (170, 40));
  abort->setHelpText (_("ignore changes and return"));
  CONNECT (abort->selected, eZapEmuSetup::abortPressed);

  statusbar = new eStatusBar (this);
  statusbar->move (ePoint (0, clientrect.height () - 30));
  statusbar->resize (eSize (clientrect.width (), 30));
  statusbar->loadDeco ();
}

int eZapEmuSetup::CardInfo ()
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

void
eZapEmuSetup::cardinfoPressed ()
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

eZapEmuSetup::~eZapEmuSetup ()
{
}

void
eZapEmuSetup::EmuEnabledChanged (int i)
{
  RC->Enabled = i;
}

void
eZapEmuSetup::okPressed ()
{
#ifdef DEBUG
  if (RC->no_emu > 0)
    printf ("okPressed: i=%i v=%s v2=%i\n", Enabled, EMU[RC->v_SelectedEmu], RC->v_SelectedEmu);
#endif
  if (RC->no_emu > 0)
    RC->v_SelectedEmu = (int) SelectedEmu->getCurrent ()->getKey ();
  if (RC->no_crdsrv > 0)
    RC->v_SelectedCrdsrv = (int) SelectedCrdsrv->getCurrent ()->getKey ();
  if (Serial != NULL)
    v_Serial = (int) Serial->getCurrent ()->getKey ();
  RC->write ();
  system ("touch /var/tmp/.emu_restart ; killall sleep");
  close (0);
#ifdef DEBUG
  printf ("END okPressed\n");
#endif
}

void
eZapEmuSetup::abortPressed ()
{
#ifdef DEBUG
  printf ("abortPressed\n");
#endif
  close (0);
}

void
eZapEmuSetup::ExtraSettings ()
{
  eMySettings set;
  set.show ();
  set.exec ();
  set.hide ();
}

void
eZapEmuSetup::softcam2allPressed ()
{
  // printf ("softcam2allPressed\n");
  Executable = (char *) SCRIPTS[0];
  hide ();
  RunApp run;
  run.show ();
  run.exec ();
  run.hide ();
  show ();
}

void
eZapEmuSetup::keyupdatePressed ()
{
  // printf ("keyupdatePressed\n");
  if ( !pl_exec("key_setting.so") )
    {
      eMessageBox msg (_("Cannot find the 'key_setting' plugin"), _("Cannot find plugin"), eMessageBox::btOK | eMessageBox::iconError);
      msg.show ();
      msg.exec ();
      msg.hide ();
    }
}

char *
find_file (const char *file)
{
  struct stat st;
  static char filename[256];

  printf ("FINDFILE: file=(%s)\n", file);

  sprintf ( filename, "/usr/lib/tuxbox/plugins/%s", file);
  if (stat (filename, &st) == 0)
    return filename;
  sprintf ( filename, "/var/tuxbox/plugins/%s", file);
  if (stat (filename, &st) == 0)
    return filename;
  sprintf ( filename, "/lib/tuxbox/plugins/%s", file);
  if (stat (filename, &st) == 0)
    return filename;
  sprintf ( filename, "/var/bin/%s", file);
  if (stat (filename, &st) == 0)
    return filename;
  sprintf ( filename, "/usr/bin/%s", file);
  if (stat (filename, &st) == 0)
    return filename;
  sprintf ( filename, "/bin/%s", file);
  if (stat (filename, &st) == 0)
    return filename;

  printf ("NOT FOUND\n");
  strcpy (filename, file);
  return filename;
}

void
eZapEmuSetup::EmuSetup (eListBoxEntryText * item)
{
  char tmp[128];
  char *ptr;

  if (!item)
    return;
  eString str = item->getText ();

  sprintf (tmp, "%s_conf_edit.so", str.c_str ());
  ptr = strstr (tmp, "_cs_conf_edit.so");
  if (ptr)
    strcpy (tmp, "cs_conf_edit.so" );
  // strcpy (filename, find_file(tmp) );

  if ( !pl_exec(tmp) )
    {
      // eMessageBox msg (_(str.c_str ()), _(str.c_str ()), eMessageBox::btOK | eMessageBox::iconError);
      eMessageBox msg (_("No setup avaiable for this emu"), _("No setup avaiable for this emu"), eMessageBox::btOK | eMessageBox::iconError);
      msg.show ();
      msg.exec ();
      msg.hide ();
    }
}

#endif
