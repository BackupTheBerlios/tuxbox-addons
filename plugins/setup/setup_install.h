#ifndef __setup_install_h
#define __setup_install_h

#include <setup_window.h>
#include <lib/gui/ewindow.h>
#include <lib/gui/listbox.h>
#include <lib/gui/statusbar.h>
#include <lib/base/console.h>
#include <lib/gui/ebutton.h>

class eButton;

class InstallSw:public eWindow
{
  eButton *abort, *dl;
    eListBox < eListBoxEntryText > *listb, *dummyb;
  eStatusBar *statusbar;
private:
  void abortPressed ();
  void dlPressed ();
  void do_install (eListBoxEntryText * item);
public:
    InstallSw ();
   ~InstallSw ();
};

class RemoveSw:public eWindow
{
  eButton *abort, *dl;
    eListBox < eListBoxEntryText > *listb;
  eStatusBar *statusbar;
private:
  void abortPressed ();
  void dlPressed ();
  void do_remove (eListBoxEntryText * item);
public:
    RemoveSw ();
   ~RemoveSw ();
};

class dlWindow:public eWindow
{
  eButton *abort, *ok;
  eConsoleAppContainer *app;
private:
  void abortPressed ();
  void okPressed ();
  void appClosed (int);
public:
    dlWindow ();
};

#endif
