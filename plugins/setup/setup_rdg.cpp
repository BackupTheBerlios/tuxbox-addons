#ifdef SETUP_RDG

#include <setup.h>
#include <setup_rdg.h>

#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>


#include <lib/base/i18n.h>

#include <lib/dvb/edvb.h>
#include <lib/dvb/eaudio.h>
#include <lib/gui/elabel.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/echeckbox.h>
#include <lib/gui/emessage.h>
#include <lib/gui/eskin.h>
#include <lib/driver/rc.h>
#include <lib/system/econfig.h>
#include <lib/gui/combobox.h>
#include <lib/gui/listbox.h>
#include <lib/gui/enumber.h>
#include <lib/gui/textinput.h>

void
SetupRdg::GetCFG ()
{
  // char section[32] = "";
  // char line[256];
  // char one[64];
  // char two[64];
  // char *ptr;
  // FILE *fp;
}

void
SetupRdg::WriteCFG ()
{
  // FILE *fp;
}

SetupRdg::SetupRdg ():
eWindow (0)
{
  SetupRdg::GetCFG ();

  int fd = eSkin::getActive ()->queryValue ("fontsize", 16);
  int s_y = 130, h_x = 250;
  eLabel *l;

  setText (_("Radegast setup"));
  cmove (ePoint (720 / 2 - h_x, 120));
  cresize (eSize (h_x * 2, 380));

  ok = new eButton (this);
  ok->setText (_("save"));
  ok->setShortcut ("green");
  ok->setShortcutPixmap ("green");
  ok->move (ePoint (10, 270));
  ok->resize (eSize (150, 40));
  ok->setHelpText (_("Save changes and return"));
  ok->loadDeco ();
  CONNECT (ok->selected, SetupRdg::okPressed);

  abort = new eButton (this);
  abort->setShortcut ("red");
  abort->setShortcutPixmap ("red");
  abort->loadDeco ();
  abort->setText (_("abort"));
  abort->move (ePoint (180, 270));
  abort->resize (eSize (150, 40));
  abort->setHelpText (_("Ignore changes and return"));
  CONNECT (abort->selected, SetupRdg::abortPressed);

  defaults = new eButton (this);
  defaults->setShortcut ("blue");
  defaults->setShortcutPixmap ("blue");
  defaults->loadDeco ();
  defaults->setText (_("defaults"));
  defaults->move (ePoint (180 + 170, 270));
  defaults->resize (eSize (150, 40));
  defaults->setHelpText (_("Load good defaults for local CS"));
  CONNECT (defaults->selected, SetupRdg::defaultsPressed);

  statusbar = new eStatusBar (this);
  statusbar->move (ePoint (0, clientrect.height () - 60));
  statusbar->resize (eSize (clientrect.width (), 50));
  statusbar->loadDeco ();
}

void
SetupRdg::okPressed ()
{
  SetupRdg::WriteCFG ();
  close (1);
}

void
SetupRdg::abortPressed ()
{
  close (1);
}

void
SetupRdg::defaultsPressed ()
{
}
#endif //SETUP_RDG
