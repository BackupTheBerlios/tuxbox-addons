#ifndef __setup_tpl_h
#define __setup_tpl_h

#include <lib/gui/ewindow.h>
#include <lib/gui/listbox.h>
#include <lib/gui/statusbar.h>
#include <lib/base/console.h>

class eButton;
class eCheckbox;
class eComboBox;
class eNumber;
class eTextInputField;

class SetupRdg:public eWindow
{
  eButton *ok, *abort, *defaults;
  eStatusBar *statusbar;
private:
  void okPressed ();
  void abortPressed ();
  void defaultsPressed ();
  void GetCFG ();
  void WriteCFG ();
public:
    SetupRdg ();
};

#endif
