#ifdef SETUP_USB

#include <setup.h>
#include <setup_usb.h>

#include <enigma.h>
#include <enigma_main.h>
#include <lib/gui/emessage.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/combobox.h>
#include <lib/gui/emessage.h>
#include <lib/gui/statusbar.h>
#include <sys/vfs.h>            // for statfs
#include <unistd.h>
#include <signal.h>

static int
getCapacity (int dev)
{
  FILE *f = fopen (eString ().sprintf ("/proc/scsi/usb-storage-%i/%i", dev, dev).c_str (), "r");
  if (!f)
    return -1;
  int capacity = 128 * 1024 * 1024 / 512;   // Fixed on 128M
  // fscanf(f, "%d", &capacity);
  fclose (f);
  return capacity;
}

static eString
getModel (int dev)
{
  FILE *f = fopen (eString ().sprintf ("/proc/scsi/usb-storage-%i/%i", dev, dev).c_str (), "r");
  if (!f)
    return "";
  fclose (f);
  return "USB disk";
}

static int
freeDiskspace (int dev, eString mp = "")
{
  FILE *f = fopen ("/proc/mounts", "rb");
  if (!f)
    return -1;
  eString path;
  int host = dev / 4;
  int bus = !!(dev & 2);
  int target = !!(dev & 1);
  path.sprintf ("/dev/scsi/host%d/bus%d/target%d/lun0/", host, bus, target);

  while (1)
    {
      char line[1024];
      if (!fgets (line, 1024, f))
        break;
      if (!strncmp (line, path.c_str (), path.size ()))
        {
          eString mountpoint = line;
          mountpoint = mountpoint.mid (mountpoint.find (' ') + 1);
          mountpoint = mountpoint.left (mountpoint.find (' '));
          //eDebug("mountpoint: %s", mountpoint.c_str());
          if (mp && mountpoint != mp)
            return -1;
          struct statfs s;
          int free;
          if (statfs (mountpoint.c_str (), &s) < 0)
            free = -1;
          else
            free = s.f_bfree / 1000 * s.f_bsize / 1000;
          fclose (f);
          return free;
        }
    }
  fclose (f);
  return -1;
}

static int
numPartitions (int dev)
{
  FILE *f = fopen ("/proc/partitions", "rb");
  if (!f)
    return 0;
  eString path;
  int host = dev / 4;
  int bus = !!(dev & 2);
  int target = !!(dev & 1);
  path.sprintf ("ide/host%d/bus%d/target%d/lun0/", host, bus, target);

  int numpart = -1;             // account for "disc"

  while (1)
    {
      char line[1024];
      if (!fgets (line, 1024, f))
        break;
      if (line[1] != ' ')
        continue;
      if (!strncmp (line + 22, path.c_str (), path.size ()))
        numpart++;
    }
  fclose (f);
  return numpart;
}

static eString
getPartFS (int dev, eString mp = "")
{
  FILE *f = fopen ("/proc/mounts", "rb");
  if (!f)
    return "";
  eString path;
  int host = dev / 4;
  int bus = !!(dev & 2);
  int target = !!(dev & 1);
  path.sprintf ("/dev/scsi/host%d/bus%d/target%d/lun0/", host, bus, target);

  while (1)
    {
      char line[1024];
      if (!fgets (line, 1024, f))
        break;

      if (!strncmp (line, path.c_str (), path.size ()))
        {
          eString mountpoint = line;
          mountpoint = mountpoint.mid (mountpoint.find (' ') + 1);
          mountpoint = mountpoint.left (mountpoint.find (' '));
//          eDebug("mountpoint: %s", mountpoint.c_str());
          if (mp && mountpoint != mp)
            continue;

          if (!strncmp (line, path.c_str (), path.size ()))
            {
              eString fs = line;
              fs = fs.mid (fs.find (' ') + 1);
              fs = fs.mid (fs.find (' ') + 1);
              fs = fs.left (fs.find (' '));
              eString mpath = line;
              mpath = mpath.left (mpath.find (' '));
              mpath = mpath.mid (mpath.rfind ('/') + 1);
              fclose (f);
              return fs + ',' + mpath;
            }
        }
    }
  fclose (f);
  return "";
}

eMyHarddiskSetup::eMyHarddiskSetup ():eListBoxWindow < eListBoxEntryText > (_("USB disk Setup"), 5, 420)
{
  nr = 0;

  move (ePoint (150, 136));

  for (int host = 0; host < 1; host++)
    for (int bus = 0; bus < 1; bus++)
      for (int target = 0; target < 1; target++)
        {
          int
            num =
            target +
            bus *
            2 +
            host *
            4;
          //NOTUSED int c = 'a' + num;

          // check for presence
          //NOTUSED char line[1024];
          int
            ok =
            1;
          printf ("Open /proc/scsi/usb-storage-%i/%i\n", num, num);
          FILE *
            f =
            fopen (eString ().sprintf ("/proc/scsi/usb-storage-%i/%i", num, num).c_str (), "r");
          if (!f)
            continue;
          ok = 1;
          fclose (f);

          if (ok)
            {
              printf ("OK\n");
              int
                capacity =
                getCapacity (num);
              if (capacity < 0)
                continue;

              capacity = capacity / 1000 * 512 / 1000;

              eString
                sharddisks;
              sharddisks = getModel (num);
              sharddisks += " (";
              if (capacity)
                sharddisks += eString ().sprintf (", %d MB", capacity);
              sharddisks += ")";

              nr++;

              new
              eListBoxEntryText (&list, sharddisks, (void *) num);
            }
        }

  CONNECT (list.selected, eMyHarddiskSetup::selectedHarddisk);
}

void
eMyHarddiskSetup::selectedHarddisk (eListBoxEntryText * t)
{
  if ((!t) || (((int) t->getKey ()) == -1))
    {
      close (0);
      return;
    }
  int dev = (int) t->getKey ();

  eMyHarddiskMenu menu (dev);

  hide ();
  menu.show ();
  menu.exec ();
  menu.hide ();
  show ();
}

void
eMyHarddiskMenu::check ()
{
  hide ();
  eMyPartitionCheck check (dev);
  check.show ();
  check.exec ();
  check.hide ();
  show ();
  restartNet = true;
}

void
eMyHarddiskMenu::extPressed ()
{
  if (visible)
    {
      gPixmap *pm = eSkin::getActive ()->queryImage ("arrow_down");
      if (pm)
        ext->setPixmap (pm);
      fs->hide ();
      sbar->hide ();
      resize (getSize () - eSize (0, 45));
      sbar->move (sbar->getPosition () - ePoint (0, 45));
      sbar->show ();
      eZap::getInstance ()->getDesktop (eZap::desktopFB)->invalidate (eRect (getAbsolutePosition () + ePoint (0, height ()), eSize (width (), 45)));
      visible = 0;
    }
  else
    {
      gPixmap *pm = eSkin::getActive ()->queryImage ("arrow_up");
      if (pm)
        ext->setPixmap (pm);
      sbar->hide ();
      sbar->move (sbar->getPosition () + ePoint (0, 45));
      resize (getSize () + eSize (0, 45));
      sbar->show ();
      fs->show ();
      visible = 1;
    }
}

void
eMyHarddiskMenu::s_format ()
{
  hide ();
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
      if (numpart)
        {
          eMessageBox msg (_("There's data on this USB disk.\n"
                             "You will lose that data. Proceed?"), _("formatting USB disk..."), eMessageBox::btYes | eMessageBox::btNo, eMessageBox::btNo);
          msg.show ();
          int res = msg.exec ();
          msg.hide ();
          if (res != eMessageBox::btYes)
            break;
        }
      int host = dev / 4;
      int bus = !!(dev & 2);
      int target = !!(dev & 1);

// kill samba server... (exporting /mnt/usb)
      system ("killall -9 smbd");
      restartNet = true;

      system (eString ().sprintf ("/bin/umount /dev/scsi/host%d/bus%d/target%d/lun0/part*", host, bus, target).c_str ());

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

      if (!fs->getCurrent ()->getKey ())    // reiserfs
        {
          ::sync ();
          if (system (eString ().sprintf ("/sbin/mkreiserfs -f -f /dev/scsi/host%d/bus%d/target%d/lun0/part1", host, bus, target).c_str ()) >> 8)
            goto err;
          ::sync ();
          if (system (eString ().sprintf ("/bin/mount -t reiserfs /dev/scsi/host%d/bus%d/target%d/lun0/part1 /mnt/usb", host, bus, target).c_str ()) >> 8)
            goto err;
          ::sync ();
          goto noerr;
        }
      else                      // ext3
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
        eZapMain::getInstance ()->clearRecordings ();
        eMessageBox msg (_("successfully formatted your disk!"), _("formatting USB disk..."), eMessageBox::btOK | eMessageBox::iconInfo);
        msg.show ();
        msg.exec ();
        msg.hide ();
      }
      readStatus ();
    }
  while (0);
  show ();
}

void
eMyHarddiskMenu::readStatus ()
{
  bus->setText ("");

  eString mod = getModel (dev);
  setText (mod);
  model->setText (mod);
  int cap = getCapacity (dev) / 1000 * 512 / 1000;

  if (cap != -1)
    capacity->setText (eString ().sprintf ("%d MB", cap));

  numpart = numPartitions (dev);
  int fds;

  if (numpart == -1)
    status->setText (_("(error reading information)"));
  else if (!numpart)
    status->setText (_("uninitialized - format it to use!"));
  else if ((fds = freeDiskspace (dev)) != -1)
    status->setText (eString ().sprintf (_("in use, %d MB free"), fds));
  else
    status->setText (_("initialized, but unknown filesystem"));
}

eMyHarddiskMenu::eMyHarddiskMenu (int dev):
dev (dev),
restartNet (false)
{
  visible = 0;
  status = new eLabel (this);
  status->setName ("status");
  model = new eLabel (this);
  model->setName ("model");
  capacity = new eLabel (this);
  capacity->setName ("capacity");
  bus = new eLabel (this);
  bus->setName ("bus");

  format = new eButton (this);
  format->setName ("format");
  bcheck = new eButton (this);
  bcheck->setName ("check");
  ext = new eButton (this);
  ext->setName ("ext");

  fs = new eComboBox (this, 2);
  fs->setName ("fs");
  fs->hide ();

  sbar = new eStatusBar (this);
  sbar->setName ("statusbar");

  new eListBoxEntryText (*fs, ("ext3"), (void *) 1);
  new eListBoxEntryText (*fs, ("reiserfs"), (void *) 0);
  fs->setCurrent ((void *) 1);

  if (eSkin::getActive ()->build (this, "eHarddiskMenu"))
    eFatal ("skin load of \"eHarddiskMenu\" failed");

  gPixmap *pm = eSkin::getActive ()->queryImage ("arrow_down");
  if (pm)
    {
      eSize s = ext->getSize ();
      ext->setPixmap (pm);
      ext->setPixmapPosition (ePoint (s.width () / 2 - pm->x / 2, s.height () / 2 - pm->y / 2));
    }

  readStatus ();

  CONNECT (ext->selected, eMyHarddiskMenu::extPressed);
  CONNECT (format->selected, eMyHarddiskMenu::s_format);
  CONNECT (bcheck->selected, eMyHarddiskMenu::check);
}

eMyPartitionCheck::eMyPartitionCheck (int dev):
eWindow (1),
dev (dev),
fsck (0)
{
  lState = new eLabel (this);
  lState->setName ("state");
  bClose = new eButton (this);
  bClose->setName ("close");
  CONNECT (bClose->selected, eMyPartitionCheck::accept);
  if (eSkin::getActive ()->build (this, "ePartitionCheck"))
    eFatal ("skin load of \"ePartitionCheck\" failed");
  bClose->hide ();
}

int
eMyPartitionCheck::eventHandler (const eWidgetEvent & e)
{
  switch (e.type)
    {
    case eWidgetEvent::execBegin:
      {
        eString fs = getPartFS (dev, "/mnt/usb"), part = fs.mid (fs.find (",") + 1);

        fs = fs.left (fs.find (","));

        eDebug ("part = %s, fs = %s", part.c_str (), fs.c_str ());

        int host = dev / 4;
        int bus = !!(dev & 2);
        int target = !!(dev & 1);

        // kill samba server... (exporting /mnt/usb)
        system ("killall -9 smbd");

        if (system ("/bin/umount /mnt/usb") >> 8)
          {
            eMessageBox msg (_("could not unmount the filesystem... "), _("check filesystem..."), eMessageBox::btOK | eMessageBox::iconError);
            close (-1);
          }
        if (fs == "ext3")
          {
            eWindow::globalCancel (eWindow::OFF);
            fsck =
              new eConsoleAppContainer (eString ().sprintf ("/sbin/fsck.ext3 -f /dev/scsi/host%d/bus%d/target%d/lun0/%s", host, bus, target, part.c_str ()));

            if (!fsck->running ())
              {
                eMessageBox msg (_("sorry, couldn't find fsck.ext3 utility to check the ext3 filesystem."),
                                 _("check filesystem..."), eMessageBox::btOK | eMessageBox::iconError);
                msg.show ();
                msg.exec ();
                msg.hide ();
                close (-1);
              }
            else
              {
                eDebug ("fsck.ext3 opened");
                CONNECT (fsck->dataAvail, eMyPartitionCheck::getData);
                CONNECT (fsck->appClosed, eMyPartitionCheck::fsckClosed);
              }
          }
        else if (fs == "reiserfs")
          {
            eWindow::globalCancel (eWindow::OFF);
            fsck =
              new eConsoleAppContainer (eString ().
                                        sprintf ("/sbin/reiserfsck --fix-fixable /dev/scsi/host%d/bus%d/target%d/lun0/%s", host, bus, target, part.c_str ()));

            if (!fsck->running ())
              {
                eMessageBox msg (_("sorry, couldn't find reiserfsck utility to check the reiserfs filesystem."),
                                 _("check filesystem..."), eMessageBox::btOK | eMessageBox::iconError);
                msg.show ();
                msg.exec ();
                msg.hide ();
                close (-1);
              }
            else
              {
                eDebug ("reiserfsck opened");
                CONNECT (fsck->dataAvail, eMyPartitionCheck::getData);
                CONNECT (fsck->appClosed, eMyPartitionCheck::fsckClosed);
                fsck->write ("Yes\n", 4);
              }
          }
        else
          {
            eMessageBox msg (_("not supported filesystem for check."), _("check filesystem..."), eMessageBox::btOK | eMessageBox::iconError);
            msg.show ();
            msg.exec ();
            msg.hide ();
            close (-1);
          }
      }
      break;

    case eWidgetEvent::execDone:
      eWindow::globalCancel (eWindow::ON);
      if (fsck)
        delete fsck;
      break;

    default:
      return eWindow::eventHandler (e);
    }
  return 1;
}

void
eMyPartitionCheck::onCancel ()
{
  if (fsck)
    fsck->kill ();
}

void
eMyPartitionCheck::fsckClosed (int state)
{
  int host = dev / 4;
  int bus = !!(dev & 2);
  int target = !!(dev & 1);

  if (system (eString ().sprintf ("/bin/mount /dev/scsi/host%d/bus%d/target%d/lun0/part1 /mnt/usb", host, bus, target).c_str ()) >> 8)
    eDebug ("mount m/usb after check failed");

  if (fsck)
    {
      delete fsck;
      fsck = 0;
    }

  bClose->show ();
}

void
eMyPartitionCheck::getData (eString str)
{
  str.removeChars ('\x8');
  if (str.find ("<y>") != eString::npos)
    fsck->write ("y", 1);
  else if (str.find ("[N/Yes]") != eString::npos)
    fsck->write ("Yes", 3);

  lState->setText (str);
}

#endif // SETUP_USB
