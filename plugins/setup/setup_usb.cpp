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
#include "setup_usb.h"

#include <lib/gui/emessage.h>
#include <lib/gui/listbox.h>
#include <enigma_plugins.h>
#include <plugin.h>

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

UsbSetup::UsbSetup ():eSetupWindow ("Usb Setup", 10, 350)
{
  move (ePoint (180, 100));
  int
    entry = 0;

  CONNECT ((new eListBoxEntryMenu (&list, _("Format USB device"), eString ().sprintf ("(%d) %s", ++entry, _("Format USB device"))))->selected,
           UsbSetup::format_usb);

#ifdef MOUNT_UNMOUNT
  CONNECT ((new eListBoxEntryMenu (&list, _("Mount USB stick"), eString ().sprintf ("(%d) %s", ++entry, _("Mount USB stick"))))->selected, UsbSetup::mount_usb);
  CONNECT ((new eListBoxEntryMenu (&list, _("UN-Mount USB stick"), eString ().sprintf ("(%d) %s", ++entry, _("UN-Mount USB stick"))))->selected,
           UsbSetup::umount_usb);
#endif
}

void
UsbSetup::format_usb ()
{
  int host = 0, bus = 0, target = 0;
  do
    {
      {
        eMessageBox msg (_("Are you SURE that you want to format this USB disk?\n"),
                         _("formatting USB disk..."), eMessageBox::btYes | eMessageBox::btCancel, eMessageBox::btCancel);
        msg.show ();
        int res = msg.exec ();
        msg.hide ();
        if (res != eMessageBox::btYes)
          break;
      }
// kill samba server... (exporting /mnt/usb)
      system ("killall -9 smbd");

      system (eString ().sprintf ("/bin/umount /dev/scsi/host%d/bus%d/target%d/lun0/part*", host, bus, target).c_str ());

      if (usb_mounted ())
        {
          eMessageBox msg (_("sorry, Cannot format a mounted USB device."), _("formatting USB disk..."), eMessageBox::btOK | eMessageBox::iconError);
          msg.show ();
          msg.exec ();
          msg.hide ();
          break;
        }

      eMessageBox msg (_("please wait while initializing USB disk.\nThis might take some minutes.\n"), _("formatting USB disk..."), 0);
      msg.show ();

      FILE *f = popen (eString ().sprintf ("/sbin/sfdisk -f /dev/scsi/host%d/bus%d/target%d/lun0/disc", host, bus, target).c_str (), "w");
      if (!f)
        {
          eMessageBox msg (_("sorry, couldn't find sfdisk utility to partition USB disk."),
                           _("formatting USB disk..."), eMessageBox::btOK | eMessageBox::iconError);
          msg.show ();
          msg.exec ();
          msg.hide ();
          break;
        }
      fprintf (f, "0,\n;\n;\n;\ny\n");
      fclose (f);
      {
        ::sync ();
        if (system (eString ().sprintf ("/sbin/mkfs.ext3 -b 4096 /dev/scsi/host%d/bus%d/target%d/lun0/part1", host, bus, target).c_str ()) >> 8)
          goto err;
        ::sync ();
        if (system (eString ().sprintf ("/bin/mount -t ext3 /dev/scsi/host%d/bus%d/target%d/lun0/part1 /mnt/usb", host, bus, target).c_str ()) >> 8)
          goto err;
        ::sync ();
        goto noerr;
      }
    err:
      {
        eMessageBox msg (_("creating filesystem failed."), _("formatting USB disk..."), eMessageBox::btOK | eMessageBox::iconError);
        msg.show ();
        msg.exec ();
        msg.hide ();
        break;
      }
    noerr:
      {
        eMessageBox msg (_("successfully formatted your USB disk!"), _("formatting USB disk..."), eMessageBox::btOK | eMessageBox::iconInfo);
        msg.show ();
        msg.exec ();
        msg.hide ();
      }
    }
  while (0);
}

#ifdef MOUNT_UNMOUNT
void
UsbSetup::mount_usb ()
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
UsbSetup::umount_usb ()
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
