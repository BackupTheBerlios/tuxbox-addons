#ifndef __setup_extra_h
#define __setup_extra_h

#include <lib/gui/ewindow.h>
#include <lib/gui/listbox.h>
#include <lib/gui/statusbar.h>
#include <lib/base/console.h>

class eButton;
class eComboBox;
class eNumber;

class ExtraSetup:public eWindow
{
  eButton *ok, *abort;
  eComboBox *var_on;
  eNumber *sleep;
  eLabel *l;
  eStatusBar *statusbar;
  void okPressed ();
  void abortPressed ();
private:
public:
    ExtraSetup ();
};

#endif
