#ifndef __setup_runapp_h
#define __setup_runapp_h

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

class RunApp:public eWindow
{
  eConsoleAppContainer *app;
  eLabel *lState;
  eButton *bCancel, *bClose;
  int eventHandler (const eWidgetEvent & e);
  void onCancel ();
  void getData (eString);
  void appClosed (int);
public:
    RunApp ();
};

#endif
