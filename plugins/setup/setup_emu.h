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
extern char *SCRIPTS[MAX_SCRIPTS];
extern char *Executable;
extern char RUN_MESSAGE[128];
extern rc_config *RC;

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
  int v_CardInfo;
  int CardInfo ();
  void cardinfoPressed ();
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
    eListBox < eListBoxEntryText > *Serial;
  eConsoleAppContainer *app;
  eLabel *lState;
  eButton *bCancel, *bClose;
private:
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
