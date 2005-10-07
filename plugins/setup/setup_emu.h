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

#ifndef __setup_emu_h
#define __setup_emu_h

#include <lib/gui/ewindow.h>
#include <lib/gui/listbox.h>
#include <lib/gui/statusbar.h>
#include <lib/base/console.h>

class eButton;
class eCheckbox;
class eComboBox;
class eNumber;
class eTextInputField;
class rc_config;

#define MAX_EMU 10
#define MAX_SCRIPTS 8

extern char EMU[MAX_EMU][256];
extern char CRDSRV[MAX_EMU][256];
extern char *SCRIPTS[MAX_SCRIPTS];
extern char *Executable;
extern char RUN_MESSAGE[128];
extern rc_config *RC;
// extern char *find_file (char *file);

class ServicesSetup:public eWindow
{
  eButton *ok, *abort, *softcam2all, *extraset, *load_draaibaar, *load_astra, *keyupdate, *cardinfo, *ipinfo;
  eCheckbox *eEmuEnabled, *eStartServer, *eStartDhcp, *eStartCron, *eStartSamba;
    eListBox < eListBoxEntryText > *Serial;
  eStatusBar *statusbar;
private:
  void EmuStartServerChanged (int i);
  void EmuStartDhcpChanged (int i);
  void EmuStartCronChanged (int i);
  void EmuStartSambaChanged (int i);
  void okPressed ();
  void abortPressed ();
#ifdef NOT_ANY_MORE
  int v_CardInfo;
  int CardInfo ();
  void cardinfoPressed ();
#endif
  int v_IpInfo;
  int IpInfo ();
  void ipinfoPressed ();
public:
    ServicesSetup ();
};

class eZapEmuSetup:public eWindow
{
  eButton *ok, *abort, *softcam2all, *extraset, *load_draaibaar, *load_astra, *keyupdate, *cardinfo, *ipinfo;
  eCheckbox *eEmuEnabled, *eStartServer, *eStartDhcp;
  eStatusBar *statusbar;
    eListBox < eListBoxEntryText > *SelectedEmu;
    eListBox < eListBoxEntryText > *SelectedCrdsrv;
    eListBox < eListBoxEntryText > *Serial;
  eConsoleAppContainer *app;
  eLabel *lState;
  eButton *bCancel, *bClose;
private:
  int v_CardInfo;
  int CardInfo ();
  void cardinfoPressed ();
  void EmuEnabledChanged (int i);
  void softcam2allPressed ();
  void keyupdatePressed ();
  void load_astraPressed ();
  void load_draaibaarPressed ();
  void okPressed ();
  void abortPressed ();
  void ExtraSettings ();
  void ReadRC ();
  void WriteRC ();
  void EmuSetup (eListBoxEntryText * item);
  int EmuEnabled ();
  int EmuStartServer ();
  int EmuStartDhcp ();
  int WichEmu ();
  int WhichSerial ();
  int Enabled;
  unsigned int v_Serial;
public:
    eZapEmuSetup ();
   ~eZapEmuSetup ();
};

#endif
#endif
