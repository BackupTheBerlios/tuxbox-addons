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
#include "setup_ipkg.h"
#include "setup_runapp.h"
#include "setup_usb.h"
#include "setup_install.h"
#include "setup_extra.h"
#include "setup_rc.h"
#include "setup_restore.h"
#include "enigma.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#ifdef DM7020
#define TITLE "Setup plugin (v0.11)"
#else
#define TITLE "Setup plugin (v0.20)"
#endif

#include <lib/gui/emessage.h>
#include <lib/gui/listbox.h>
#include <enigma_plugins.h>
#include <plugin.h>

eListBox < eListBoxEntryText > *listb;

char EMU[MAX_EMU][256] = { "scam" };
char CRDSRV[MAX_EMU][256] = { "auto" };
char *SCRIPTS[MAX_SCRIPTS] =
  { "/bin/keyconvert.sh 2>&1", "/bin/keyupdate.sh 2>&1", "cat /var/tmp/cardinfo", "cat /var/tmp/ipinfo", "/bin/load_d.sh", "/bin/load_a.sh" };
char *Executable;
char RUN_MESSAGE[128] = "Script is running, please wait!";
char NO_OUTPUT_MESSAGE[128] = "Success!";
rc_config *RC = new rc_config;
time_t time_stamp;
time_t time_stamp_enigma;

extern "C" int plugin_exec (PluginParam * par);

int
plugin_exec (PluginParam * par)
{
  char cmd[64];
  sprintf ( cmd, "sh %s", RC_CONFIG );
  if (system ( cmd ) )
  {
    printf ( "Config error, reset\n" );
    unlink ( RC_CONFIG );
    FILE *fp = fopen ( RC_CONFIG, "w" );
    if (fp)
      fclose (fp);
  }
  eMySettings setup;
  setup.show ();
  setup.exec ();
  setup.hide ();
  //show();
  return 0;
}

eMySettings::eMySettings ():eSetupWindow (_(TITLE), 10, 350)
{
  int
    entry = 0;
  FILE *
    F;
  struct stat
    st;

  move (ePoint (180, 100));

  if (stat ("/usr/bin/enigma", &st) == 0)
    time_stamp_enigma = st.st_mtime;
  if (stat ("/usr/lib/tuxbox/plugins/setup7020.so", &st) == 0)
    time_stamp = st.st_mtime;


#ifdef SETUP_EMU
  CONNECT ((new eListBoxEntryMenu (&list, _("EMU Setup"), eString ().sprintf ("(%d) %s", ++entry, _("open EMU setup"))))->selected, eMySettings::emu_setup);
#endif

#ifdef SETUP_IPKG
  CONNECT ((new eListBoxEntryMenu (&list, _("Package Manager"), eString ().sprintf ("(%d) %s", ++entry, _("Run Package Manager"))))->selected,
           eMySettings::ipkg_setup);
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
  if ((F = fopen ("/dev/scsi/host0/bus0/target0/lun0/disc", "r")) != NULL)
    {
      fclose (F);
      CONNECT ((new eListBoxEntryMenu (&list, _("USB disk Setup"), eString ().sprintf ("(%d) %s", ++entry, _("open USB disk setup"))))->selected,
               eMySettings::usb_setup);
    }
#endif

#ifdef SETUP_INST
  CONNECT ((new eListBoxEntryMenu (&list, _("Install software (needs internet)"), eString ().sprintf ("(%d) %s", ++entry, _("Install software"))))->selected,
           eMySettings::sw_install);
  CONNECT ((new eListBoxEntryMenu (&list, _("Remove installed software"), eString ().sprintf ("(%d) %s", ++entry, _("Remove installed software"))))->selected,
           eMySettings::sw_remove);
#endif

#ifdef SETUP_RESTORE
  F = fopen ("/var/tuxbox/config/enigma/config.save", "r");
  if (F)
    {
      CONNECT ((new
                eListBoxEntryMenu (&list, _("Restore Settings"),
                                   eString ().sprintf ("(%d) %s", ++entry, _("Open Restore Settings window, for testing puposes only"))))->selected,
               eMySettings::restore_setup);
      fclose (F);
    }
#endif

  new eListBoxEntrySeparator ((eListBox < eListBoxEntry > *) & list, eSkin::getActive ()->queryImage ("listbox.separator"), 0, true);

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
/*
  plugins.execPluginByName("newcamd_conf_edit.cfg");
*/
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

#ifdef SETUP_IPKG
void
eMySettings::ipkg_setup ()
{
  int res;
  struct stat st;
  char exe[256];
  FILE *F;

  hide ();
  printf ("eMySettings::ipkg_setup()\n");
  if (stat ("/etc/ipkg/ronaldd-feed.conf", &st) != 0)
    {
      F = fopen ("/etc/ipkg/ronaldd-feed.conf", "w");
      if (F)
        {
          fprintf (F, "src/gz ronaldd http://ronaldd.irde.to/ipkg/feed\n");
          fclose (F);
        }
    }
  if (stat ("/usr/lib/ipkg/info/setup-plugin.control", &st) != 0)
    {
      eMessageBox msg (_("Setup plugin is not installed as a package, this is needed for package manager.\nInstall now?"),
                       _("Install Setup plugin as package?"), eMessageBox::btYes | eMessageBox::btCancel, eMessageBox::btCancel);
      msg.show ();
      res = msg.exec ();
      msg.hide ();
      if (res == eMessageBox::btYes)
        {
          system ("echo true >/etc/init.d/modutils.sh ; chmod 755 /etc/init.d/modutils.sh");    // HACK for error in dream package
          sprintf (exe, "sh -c \"ipkg update ; ipkg install -force-overwrite setup-plugin\"");
          Executable = exe;
          strcpy (RUN_MESSAGE, "");
          RunApp run;
          run.show ();
          run.exec ();
          run.hide ();
          unlink ("/etc/init.d/modutils.sh");   // HACK for error in dream package
        }
    }

  if (stat ("/usr/bin/enigma", &st) == 0)
    if (time_stamp_enigma != st.st_mtime)
      {
        eMessageBox *msg;
        msg = new eMessageBox (_("Restarting enigma"), _("enigma is updated"), eMessageBox::btOK | eMessageBox::iconInfo);
        msg->show ();
        msg->exec ();
        msg->hide ();
        eZap::getInstance ()->quit (1);
      }
  if (stat ("/usr/lib/tuxbox/plugins/setup7020.so", &st) == 0)
    if (time_stamp != st.st_mtime)
      {
        eMessageBox *msg;
        msg = new eMessageBox (_("Leaving Setup plugin because it is updated"), _("Setup plugin is updated"), eMessageBox::btOK | eMessageBox::iconInfo);
        msg->show ();
        msg->exec ();
        msg->hide ();
        close (0);
        return;
      }
  if (stat ("/usr/lib/ipkg/info/setup-plugin.control", &st) == 0)
    {
      ipkgSetup setup;
      setup.show ();
      setup.exec ();
      setup.hide ();
    }

  show ();
}
#endif // SETUP_IPKG

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
eMySettings::usb_setup ()
{
  printf ("eMySettings::usb_setup()\n");
  hide ();
  UsbSetup setup;
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
