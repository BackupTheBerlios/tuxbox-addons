#ifdef SETUP_EXTRA

#include "setup.h"
#include "setup_extra.h"
#include "setup_rc.h"

#define TITLE "Extra Setup"

#include<plugin.h>

#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include <lib/base/i18n.h>

#include <lib/gui/echeckbox.h>
#include <lib/gui/enumber.h>
#include <lib/gui/combobox.h>


ExtraSetup::ExtraSetup ():
eWindow (0)
{
  // int fd = eSkin::getActive ()->queryValue ("fontsize", 16);

  RC->read ();

  setText (TITLE);
  cmove (ePoint (170, 150));
  cresize (eSize (390, 350));

  l = new eLabel (this);
  l->setText ("Sleep:");
  l->move (ePoint (10, 10));
  l->resize (eSize (150, 35));

  sleep = new eNumber (this, 1, 0, 9, 2, &RC->sleep, 0, l);
  sleep->move (ePoint (130, 10));
  sleep->resize (eSize (100, 35));
  sleep->setFlags (eNumber::flagDrawPoints);
  sleep->setHelpText (_("Number of seconds to wait before mounting HDD and USB"));
  sleep->loadDeco ();

  l = new eLabel (this);
  l->move (ePoint (10, 50));
  l->resize (eSize (120, 25));
  l->setText (_("Settings on:"));
  var_on = new eComboBox (this, 3, l);
  var_on->move (ePoint (130, 50));
  var_on->resize (eSize (100, 35));
  var_on->setHelpText (_("Where to store settings: FLASH, USB or HDD"));
  var_on->loadDeco ();
  new eListBoxEntryText (*var_on, eString ().sprintf ("flash"), (void *) 0);
  new eListBoxEntryText (*var_on, eString ().sprintf ("usb"), (void *) 1);
  new eListBoxEntryText (*var_on, eString ().sprintf ("hdd"), (void *) 2);
  if (strcmp (RC->var_on, "usb") == 0)
    var_on->setCurrent (1, false);
  else if (strcmp (RC->var_on, "hdd") == 0)
    var_on->setCurrent (2, false);
  else
    var_on->setCurrent (0, false);

  ok = new eButton (this);
  ok->setText (_("save"));
  ok->setShortcut ("green");
  ok->setShortcutPixmap ("green");
  ok->move (ePoint (20, 260));
  ok->resize (eSize (170, 40));
  ok->setHelpText (_("save changes and return"));
  ok->loadDeco ();
  CONNECT (ok->selected, ExtraSetup::okPressed);

  abort = new eButton (this);
  abort->setShortcut ("red");
  abort->setShortcutPixmap ("red");
  abort->loadDeco ();
  abort->setText (_("abort"));
  abort->move (ePoint (210, 260));
  abort->resize (eSize (170, 40));
  abort->setHelpText (_("ignore changes and return"));
  CONNECT (abort->selected, ExtraSetup::abortPressed);

  statusbar = new eStatusBar (this);
  statusbar->move (ePoint (0, clientrect.height () - 50));
  statusbar->resize (eSize (clientrect.width (), 50));
  statusbar->loadDeco ();
}

void
ExtraSetup::okPressed ()
{
#ifdef DEBUG
  printf ("okPressed\n");
#endif
  RC->sleep = sleep->getNumber ();
  // strcpy ( RC->var_on, ((eString*) var_on->getCurrent()->getKey())->c_str() );
  switch ((int) var_on->getCurrent ()->getKey ())
    {
    case 1:
      strcpy (RC->var_on, "usb");
      break;
    case 2:
      strcpy (RC->var_on, "hdd");
      break;
    default:
      strcpy (RC->var_on, "flash");
      break;
    }
  RC->write ();
  system ("killall sleep");
  close (0);
}

void
ExtraSetup::abortPressed ()
{
#ifdef DEBUG
  printf ("abortPressed\n");
#endif
  close (0);
}
#endif
