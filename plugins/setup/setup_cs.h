#ifndef __setup_cs_h
#define __setup_cs_h

#include <lib/gui/ewindow.h>
#include <lib/gui/listbox.h>
#include <lib/gui/statusbar.h>
#include <lib/base/console.h>

class eButton;
class eCheckbox;
class eComboBox;
class eNumber;
class eTextInputField;

#define MAX_IP 10

class SetupScam_cs:public eWindow
{
  eButton *ok, *abort, *defaults;
  eNumber *cs_srvport, *cs_clntport[MAX_IP], *cs_server_ip[MAX_IP];
  eTextInputField *cs_server[MAX_IP], *cs_p_dev;
  eStatusBar *statusbar;
  eCheckbox *cs_au_in, *cs_num;
  eComboBox *ServerN;
  //eListBox < eListBoxEntryText > *ServerN;
private:
  void okPressed ();
  void abortPressed ();
  void defaultsPressed ();
  void GetCFG ();
  void WriteCFG ();
  void NumChanged (int);
  void SelectServerN (eListBoxEntryText * item);
  eString c_server[MAX_IP];
  eString c_port[MAX_IP];
  eString c_mode;
  eString c_serial;
  eString c_au_internet;
  eString s_port;
  eString s_mode;
  eString s_phoenix_dev;
  int v_cs_server_ip[MAX_IP][4];
  int v_cs_au_in;
  int numeric;
  int server;
public:
    SetupScam_cs ();
};

#endif
