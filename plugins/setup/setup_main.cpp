/*
 * Ronald's setup plugin for dreambox
 * Copyright (c) 2004 Ronaldd <Ronaldd@sat4all.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "setup.h"
#include "setup_main.h"
#include "setup_emu.h"
#include "setup_runapp.h"
#include "setup_usb.h"
#include "setup_install.h"
#include "setup_extra.h"
#include "setup_rc.h"
#include "setup_restore.h"

#define TITLE "Setup plugin (v0.17)"

#include <lib/gui/emessage.h>
#include <lib/gui/listbox.h>
#include <enigma_plugins.h>
#include <plugin.h>

eListBox < eListBoxEntryText > *listb;

char EMU[MAX_EMU][256] = { "dr.matrix", "mgcamd" };
char *SCRIPTS[MAX_SCRIPTS] =
  { "/bin/keyconvert.sh 2>&1", "/bin/keyupdate.sh 2>&1", "cat /var/tmp/cardinfo", "cat /var/tmp/ipinfo", "/bin/load_d.sh", "/bin/load_a.sh" };
char *Executable;
char RUN_MESSAGE[128] = "Script is running, please wait!";
rc_config *RC = new rc_config;

extern "C" int plugin_exec (PluginParam * par);

int
plugin_exec (PluginParam * par)
{
  eMySettings setup;
  setup.show ();
  setup.exec ();
  setup.hide ();
  //show();
  return 0;
}

eMySettings::eMySettings ():eSetupWindow (_(TITLE), 10, 350)
{
  move (ePoint (180, 100));
  int
    entry = 0;

#ifdef SETUP_EMU
  CONNECT ((new eListBoxEntryMenu (&list, _("EMU Setup"), eString ().sprintf ("(%d) %s", ++entry, _("open EMU setup"))))->selected, eMySettings::emu_setup);
#endif
#ifdef SETUP_EXTRA
  CONNECT ((new eListBoxEntryMenu (&list, _("Extra Setup"), eString ().sprintf ("(%d) %s", ++entry, _("open Extra setup"))))->selected,
           eMySettings::extra_setup);
#endif

#ifdef SETUP_SERVICES
  CONNECT ((new eListBoxEntryMenu (&list, _("Services to run"), eString ().sprintf ("(%d) %s", ++entry, _("open Services setup"))))->selected,
           eMySettings::services_setup);
#endif

#ifdef SETUP_USB
  CONNECT ((new eListBoxEntryMenu (&list, _("USB disk Setup"), eString ().sprintf ("(%d) %s", ++entry, _("open USB disk setup"))))->selected,
           eMySettings::my_harddisc_setup);
#endif

#ifdef SETUP_INST
  CONNECT ((new eListBoxEntryMenu (&list, _("Install software (needs internet)"), eString ().sprintf ("(%d) %s", ++entry, _("Install software"))))->selected,
           eMySettings::sw_install);
  CONNECT ((new eListBoxEntryMenu (&list, _("Remove installed software"), eString ().sprintf ("(%d) %s", ++entry, _("Remove installed software"))))->selected,
           eMySettings::sw_remove);
#endif

#ifdef SETUP_RESTORE
  FILE *
    F = fopen ("/var/tuxbox/config/enigma/config.save", "r");
  if (F)
    {
      CONNECT ((new
                eListBoxEntryMenu (&list, _("Restore Settings (Not ready)"),
                                   eString ().sprintf ("(%d) %s", ++entry, _("Open Restore Settings window, for testing puposes only"))))->selected,
               eMySettings::restore_setup);
      fclose (F);
    }
#endif

#ifdef MOUNT_UNMOUNT
  CONNECT ((new eListBoxEntryMenu (&list, _("Mount USB stick"), eString ().sprintf ("(%d) %s", ++entry, _("Mount USB stick"))))->selected,
           eMySettings::mount_usb);
  CONNECT ((new eListBoxEntryMenu (&list, _("UN-Mount USB stick"), eString ().sprintf ("(%d) %s", ++entry, _("UN-Mount USB stick"))))->selected,
           eMySettings::umount_usb);
#endif

  new
  eListBoxEntrySeparator ((eListBox < eListBoxEntry > *) & list, eSkin::getActive ()->queryImage ("listbox.separator"), 0, true);

  CONNECT ((new eListBoxEntryMenu (&list, _("Plugins"), eString ().sprintf ("(%d) %s", ++entry, _("Run the normale plugins"))))->selected,
           eMySettings::run_plugins);
}

void
eMySettings::run_plugins ()
{
  printf ("eMySettings::emu_setup()\n");
  hide ();
  eZapPlugins plugins (2);
  plugins.exec ();
  show ();
}

#ifdef SETUP_EMU
void
eMySettings::emu_setup ()
{
  printf ("eMySettings::emu_setup()\n");
  hide ();
  eZapEmuSetup setup;
  setup.show ();
  setup.exec ();
  setup.hide ();
  show ();
}
#endif // SETUP_EMU

#ifdef SETUP_EXTRA
void
eMySettings::extra_setup ()
{
  printf ("eMySettings::extra_setup()\n");
  hide ();
  ExtraSetup setup;
  setup.show ();
  setup.exec ();
  setup.hide ();
  show ();
}
#endif // SETUP_EXTRA

#ifdef SETUP_SERVICES
void
eMySettings::services_setup ()
{
  printf ("eMySettings::services_setup()\n");
  hide ();
  ServicesSetup setup;
  setup.show ();
  setup.exec ();
  setup.hide ();
  show ();
}
#endif // SETUP_SERVICES

#ifdef SETUP_USB
void
eMySettings::my_harddisc_setup ()
{
  printf ("eMySettings::my_harddisc_setup()\n");
  hide ();
  eMyHarddiskSetup setup;
  setup.show ();
  setup.exec ();
  setup.hide ();
  show ();
}
#endif // SETUP_USB

#ifdef SETUP_RESTORE
void
eMySettings::restore_setup ()
{
  printf ("eMySettings::restore_setup()\n");
  hide ();
  RestoreSetup setup;
  setup.show ();
  setup.exec ();
  setup.hide ();
  show ();
}
#endif // SETUP_RESTORE

#ifdef SETUP_INST
void
eMySettings::sw_install ()
{
  printf ("eMySettings::sw_install()\n");
  InstallSw inst;
  inst.show ();
  inst.exec ();
  inst.hide ();
  printf ("end eMySettings::sw_install()\n");
}

void
eMySettings::sw_remove ()
{
  printf ("eMySettings::sw_remove()\n");
  RemoveSw rem;
  rem.show ();
  rem.exec ();
  rem.hide ();
}
#endif // SETUP_INST

#ifdef MOUNT_UNMOUNT
int
usb_mounted ()
{
  int ret;
  FILE *F;
  F = fopen ("/mnt/usb/a", "w");
  if (F)
    {
      fclose (F);
      ret = 1;
    }
  else
    ret = 0;
  return ret;
}

void
eMySettings::mount_usb ()
{
  eMessageBox *msg;
  if (usb_mounted ())
    msg = new eMessageBox (_("USB Stick is already mounted"), _("USB Stick is already mounted"), eMessageBox::btOK | eMessageBox::iconWarning);
  else
    {
      system ("mount /dev/scsi/host0/bus0/target0/lun0/part1 /mnt/usb");
      if (usb_mounted ())
        msg = new eMessageBox (_("USB Stick successfuly mounted"), _("USB Stick successfuly mounted"), eMessageBox::btOK | eMessageBox::iconInfo);
      else
        msg =
          new eMessageBox (_("USB Stick NOT MOUNTED, unknown error"), _("USB Stick NOT MOUNTED, unknown error"), eMessageBox::btOK | eMessageBox::iconError);
    }
  msg->show ();
  msg->exec ();
  msg->hide ();
}

void
eMySettings::umount_usb ()
{
  eMessageBox *msg;
  if (!usb_mounted ())
    msg = new eMessageBox (_("USB Stick is NOT MOUNTED"), _("USB Stick is NOT MOUNTED"), eMessageBox::btOK | eMessageBox::iconWarning);
  else
    {
      system ("umount /mnt/usb");
      if (usb_mounted ())
        msg =
          new eMessageBox (_("USB Stick IS STILL MOUNTED, unknown error"), _("USB Stick IS STILL MOUNTED, unknown error"),
                           eMessageBox::btOK | eMessageBox::iconError);
      else
        msg = new eMessageBox (_("USB Stick successfuly un-mounted"), _("USB Stick successfuly un-mounted"), eMessageBox::btOK | eMessageBox::iconInfo);
    }
  msg->show ();
  msg->exec ();
  msg->hide ();
}
#endif // MOUNT_UNMOUNT
