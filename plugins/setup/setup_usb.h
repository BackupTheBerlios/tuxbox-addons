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

#ifdef SETUP_USB

#ifndef __setup_usb_h
#define __setup_usb_h

#include <lib/dvb/edvb.h>
#include <lib/gui/listbox.h>
#include <lib/base/console.h>

class eButton;
class eComboBox;
class eStatusBar;

class eMyHarddiskSetup:public eListBoxWindow < eListBoxEntryText >
{
  int nr;
  void selectedHarddisk (eListBoxEntryText * sel);
public:
    eMyHarddiskSetup ();
  int getNr () const
  {
    return nr;
  }
};

class eMyHarddiskMenu:public eWindow
{
  eButton *ext, *format, *bcheck;
  eLabel *status, *model, *capacity, *bus, *lfs;
  eComboBox *fs;
  eStatusBar *sbar;
  int dev;
  bool restartNet;
  int numpart;
  int visible;
  void s_format ();
  void extPressed ();
  void check ();
  void readStatus ();
public:
    eMyHarddiskMenu (int dev);
   ~eMyHarddiskMenu ()
  {
    if (restartNet)
      eDVB::getInstance ()->configureNetwork ();
  }
};

class eMyPartitionCheck:public eWindow
{
  eLabel *lState;
  eButton *bCancel, *bClose;
  int dev;
  void onCancel ();
  void fsckClosed (int);
  int eventHandler (const eWidgetEvent & e);
  void getData (eString);
  eConsoleAppContainer *fsck;
public:
    eMyPartitionCheck (int dev);
};

#endif

#endif //SETUP_USB
