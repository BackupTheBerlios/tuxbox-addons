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
