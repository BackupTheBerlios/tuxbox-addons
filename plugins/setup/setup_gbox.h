#ifndef __setup_gbox_h
#define __setup_gbox_h

#include <lib/gui/ewindow.h>
#include <lib/gui/listbox.h>
#include <lib/gui/statusbar.h>
#include <lib/base/console.h>

class eButton;
class eCheckbox;
class eComboBox;
class eNumber;
class eTextInputField;

class SetupGbox:public eWindow
{
  eButton *ok, *abort, *defaults;
  eStatusBar *statusbar;
  eComboBox *GboxG, *GboxC, *GboxM, *GboxA, *GboxU, *GboxH, *GboxT;
  void GboxAChanged (eListBoxEntryText *);
private:
  void okPressed ();
  void abortPressed ();
  void defaultsPressed ();
  void GetCFG ();
  void WriteCFG ();
  int v_GboxG;
  int v_GboxC;
  int v_GboxM;
  int v_GboxA;
  int v_GboxU;
  int v_GboxH;
  int v_GboxT;
public:
    SetupGbox ();
};

#endif
