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
#include "setup_restore.h"

#define TITLE "Restore Settings"

#include<plugin.h>

#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>


#include <lib/base/i18n.h>
#include <enigma.h>

//#include <lib/gui/ewindow.h>
//#include <lib/dvb/edvb.h>
#include <lib/dvb/eaudio.h>
//#include <lib/gui/elabel.h>
//#include <lib/gui/ebutton.h>
#include <lib/gui/echeckbox.h>
//#include <lib/gui/emessage.h>
//#include <lib/gui/eskin.h>
//#include <lib/driver/rc.h>
#include <lib/system/econfig.h>
//#include <lib/gui/combobox.h>
//#include <lib/gui/listbox.h>
//#include <lib/gui/enumber.h>
//#include <lib/gui/textinput.h>

int v_DVB, v_AV, v_NET;

RestoreSetup::RestoreSetup ():
eWindow (0)
{
  setText (_("Restore Settings"));
  cmove (ePoint (170, 150));
  cresize (eSize (390, 350));

  v_DVB = 1; v_AV = 1; v_NET = 0;

  DVB = new eCheckbox (this, v_DVB, 1);
  DVB->setText (_("LNB/rotor set."));
  DVB->move (ePoint (10, 10));
  DVB->resize (eSize (170, 35));
  DVB->setHelpText (_("Restore LNB/rotor settings"));
  CONNECT (DVB->checked, RestoreSetup::RestoreDVBchanged);

  AV = new eCheckbox (this, v_AV, 1);
  AV->setText (_("AV set."));
  AV->move (ePoint (10, 50));
  AV->resize (eSize (170, 35));
  AV->setHelpText (_("Restore AV settings"));
  CONNECT (AV->checked, RestoreSetup::RestoreAVchanged);

  NET = new eCheckbox (this, v_NET, 1);
  NET->setText (_("Network set."));
  NET->move (ePoint (10, 90));
  NET->resize (eSize (170, 35));
  NET->setHelpText (_("Restore network settings"));
  CONNECT (NET->checked, RestoreSetup::RestoreNETchanged);

  ok = new eButton (this);
  ok->setText (_("Restore"));
  ok->setShortcut ("green");
  ok->setShortcutPixmap ("green");
  ok->move (ePoint (20, 270));
  ok->resize (eSize (170, 40));
  ok->setHelpText (_("Restore settings and return"));
  ok->loadDeco ();
  CONNECT (ok->selected, RestoreSetup::okPressed);

  abort = new eButton (this);
  abort->setShortcut ("red");
  abort->setShortcutPixmap ("red");
  abort->loadDeco ();
  abort->setText (_("abort"));
  abort->move (ePoint (210, 270));
  abort->resize (eSize (170, 40));
  abort->setHelpText (_("ignore changes and return"));
  CONNECT (abort->selected, RestoreSetup::abortPressed);

  statusbar = new eStatusBar (this);
  statusbar->move (ePoint (0, clientrect.height () - 30));
  statusbar->resize (eSize (clientrect.width (), 30));
  statusbar->loadDeco ();
}

void
RestoreSetup::RestoreDVBchanged (int i)
{
  v_DVB = i;
}

void
RestoreSetup::RestoreAVchanged (int i)
{
  v_AV = i;
}

void
RestoreSetup::RestoreNETchanged (int i)
{
  v_NET = i;
}

void delete_lnbs()
{
  int lnb, sat ;
  char key[256];

  for (lnb=0; lnb<16; ++lnb)
  {
    sprintf ( key, "/elitedvb/DVB/config/lnbs/%d/lofH", lnb ); eConfig::getInstance()->delKey(key);
    sprintf ( key, "/elitedvb/DVB/config/lnbs/%d/lofL", lnb ); eConfig::getInstance()->delKey(key);
    sprintf ( key, "/elitedvb/DVB/config/lnbs/%d/lofThreshold", lnb ); eConfig::getInstance()->delKey(key);
    sprintf ( key, "/elitedvb/DVB/config/lnbs/%d/IncreasedVoltage", lnb ); eConfig::getInstance()->delKey(key);
    sprintf ( key, "/elitedvb/DVB/config/lnbs/%d/DiSEqCMode", lnb ); eConfig::getInstance()->delKey(key);
    sprintf ( key, "/elitedvb/DVB/config/lnbs/%d/MiniDiSEqCParam", lnb ); eConfig::getInstance()->delKey(key);
    sprintf ( key, "/elitedvb/DVB/config/lnbs/%d/DiSEqCParam", lnb ); eConfig::getInstance()->delKey(key);
    sprintf ( key, "/elitedvb/DVB/config/lnbs/%d/DiSEqCRepeats", lnb ); eConfig::getInstance()->delKey(key);
    sprintf ( key, "/elitedvb/DVB/config/lnbs/%d/FastDiSEqC", lnb ); eConfig::getInstance()->delKey(key);
    sprintf ( key, "/elitedvb/DVB/config/lnbs/%d/SeqRepeat", lnb ); eConfig::getInstance()->delKey(key);
    sprintf ( key, "/elitedvb/DVB/config/lnbs/%d/SwapCmds", lnb ); eConfig::getInstance()->delKey(key);
    sprintf ( key, "/elitedvb/DVB/config/lnbs/%d/uncommitted_cmd", lnb ); eConfig::getInstance()->delKey(key);
    sprintf ( key, "/elitedvb/DVB/config/lnbs/%d/useGotoXX", lnb ); eConfig::getInstance()->delKey(key);
    sprintf ( key, "/elitedvb/DVB/config/lnbs/%d/useRotorInPower", lnb ); eConfig::getInstance()->delKey(key);
    sprintf ( key, "/elitedvb/DVB/config/lnbs/%d/DegPerSec", lnb ); eConfig::getInstance()->delKey(key);
    sprintf ( key, "/elitedvb/DVB/config/lnbs/%d/gotoXXLaDirection", lnb ); eConfig::getInstance()->delKey(key);
    sprintf ( key, "/elitedvb/DVB/config/lnbs/%d/gotoXXLoDirection", lnb ); eConfig::getInstance()->delKey(key);
    sprintf ( key, "/elitedvb/DVB/config/lnbs/%d/gotoXXLatitude", lnb ); eConfig::getInstance()->delKey(key);
    sprintf ( key, "/elitedvb/DVB/config/lnbs/%d/gotoXXLongitude", lnb ); eConfig::getInstance()->delKey(key);
    for (sat=0; sat<64; ++sat)
    {
      sprintf ( key, "/elitedvb/DVB/config/lnbs/%d/satellites/%d/description", lnb, sat ); eConfig::getInstance()->delKey(key);
    }
  }
  eConfig::getInstance ()->flush ();
}

void restore_settings()
{
  FILE *F, *W;
  char type, line[512], *ptr, *v_ptr;
  eString key, value;
  int nr, do_restore;

#ifdef DEBUG
  printf ("okPressed\n");
#endif
  F = fopen ("/var/tuxbox/config/enigma/config.save", "r");
  W = fopen ("/var/tuxbox/config/enigma/config.restore", "w");
  if (F && W)
    {
      while ( fgets (line, 512, F) != NULL)
        {
                      ptr = strstr (line, "\n");
                      if (ptr)
                        *ptr = '\0';
          nr = sscanf ( line, "%c:", &type );
          if (nr == 1)
            {
              printf ("L1: (%c) (%s)\n", type, line);
              ptr = strstr (line, "=");
              if (ptr)
                {
                  *ptr = '\0';
                  ptr++;
                  key = line + 2;
                  value = ptr; v_ptr = ptr;
                  // printf ("L2: (%c) (%s) (%s)\n", type, key, value);
                  do_restore = 0;
                  if (strncmp ("/elitedvb/audio", key.c_str (), 15) == 0 && v_AV)
                    do_restore = 1;
                  if (strncmp ("/elitedvb/video", key.c_str (), 15) == 0 && v_AV)
                    do_restore = 1;
                  if (strncmp ("/elitedvb/DVB", key.c_str (), 13) == 0 && v_DVB)
                  {
                    delete_lnbs();
                    do_restore = 1;
                  }
                  if (strncmp ("/elitedvb/network", key.c_str (), 17) == 0 && v_NET)
                    do_restore = 1;
                  if (!do_restore)
                    continue;
                  fprintf ( W, "%c:%s=%s\n", type, key.c_str(), v_ptr );
                  printf  ( "%c:%s=%s\n", type, key.c_str(), v_ptr );
#if 0
                  switch (type)
                    {
                    case 'd':
                      double v_d;
                      sscanf (v_ptr, "%lf", &v_d);
                      ptr = strstr (line, "=");
                      if (ptr)
                        *ptr = '.';
#ifdef DEBUG
                      printf ("d: (%s) (%lf)\n", key.c_str (), v_d);
#endif
                      eConfig::getInstance ()->setKey (key.c_str (), v_d);
                      break;
                    case 'u':
                      unsigned int v_u;
                      sscanf (v_ptr, "%x", &v_u);
#ifdef DEBUG
                      printf ("u: (%s) (%x)\n", key.c_str (), v_u);
#endif
                      eConfig::getInstance ()->setKey (key.c_str (), v_u);
                      break;
                    case 'i':
                      int v_i;
                      sscanf (v_ptr, "%x", &v_i);
#ifdef DEBUG
                      printf ("i: (%s) (%x)\n", key.c_str (), v_i);
#endif
                      eConfig::getInstance ()->setKey (key.c_str (), v_i);
                      break;
                    default:
#ifdef DEBUG
                      printf ("%c: (%s) (%s)\n", type, key.c_str (), v_ptr);
#endif
                      eConfig::getInstance ()->setKey (key.c_str (), value);
                    }
#endif
                }
            }
        }
      //eAudio::getInstance ()->saveSettings ();
      eConfig::getInstance ()->flush ();

    }
      if(W)
      fclose (W);
      if(F)
      fclose (F);
}

void
RestoreSetup::okPressed ()
{
  WaitWindow Wait;
  Wait.show ();
  Wait.exec ();
  Wait.hide ();
  close (0);
}

void
RestoreSetup::abortPressed ()
{
#ifdef DEBUG
  printf ("abortPressed\n");
#endif
  close (0);
}

WaitWindow::WaitWindow ():eWindow (0)
{
  setText (_("Restoring Settings, please wait..."));
  cmove (ePoint (150, 200));
  cresize (eSize (350, 150));

  ok = new eButton (this);
  ok->setText (_("Reboot"));
  ok->setShortcut ("green");
  ok->setShortcutPixmap ("green");
  ok->move (ePoint (20, 100));
  ok->resize (eSize (140, 40));
  ok->setHelpText (_("Install the software"));
  ok->loadDeco ();
  ok->hide ();
  CONNECT (ok->selected, WaitWindow::okPressed);

  abort = new eButton (this);
  abort->setText (_("Shutdown"));
  abort->setShortcut ("red");
  abort->setShortcutPixmap ("red");
  abort->move (ePoint (180, 100));
  abort->resize (eSize (140, 40));
  abort->setHelpText (_("Install the software"));
  abort->loadDeco ();
  abort->hide ();
  CONNECT (abort->selected, WaitWindow::abortPressed);
}

void
WaitWindow::okPressed ()
{
  eZap::getInstance()->quit(1);
}

void
WaitWindow::abortPressed ()
{
  eZap::getInstance()->quit();
}

int
WaitWindow::eventHandler (const eWidgetEvent & e)
{
  switch (e.type)
    {
    case eWidgetEvent::execBegin:
      restore_settings();
      ok->show();
      abort->show();
      break;

    default:
      return eWindow::eventHandler (e);
    }
  return 1;
}

#endif
