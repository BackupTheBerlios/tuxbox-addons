#ifndef __setup_services_h
#define __setup_services_h

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

#endif
