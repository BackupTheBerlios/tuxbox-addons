#ifndef __setup_main_h
#define __setup_main_h

#include <setup_window.h>
#include <lib/gui/ewindow.h>
#include <lib/gui/listbox.h>
#include <lib/gui/statusbar.h>
#include <lib/base/console.h>
#include <lib/gui/ebutton.h>

class eButton;

class eMySettings:public eSetupWindow
{
private:
  void my_harddisc_setup ();
  void mount_usb ();
  void umount_usb ();
  void sw_install ();
  void sw_remove ();
  void emu_setup ();
  void extra_setup ();
  void services_setup ();
  void run_plugins ();
public:
    eMySettings ();
};

#endif /* __my_system_settings_h */
