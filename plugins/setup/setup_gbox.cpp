#ifdef SETUP_GBOX

#include <setup.h>
#include <setup_gbox.h>

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
SetupGbox::GetCFG ()
{
  FILE *file;
  char line[256];
  char c;
  int val;
  file = fopen ("/var/keys/gbox_cfg", "r");
  if (file)
    {
      while (fgets (line, 256, file) != NULL)
        {
          if (sscanf (line, "%c: { %02d }", &c, &val) == 2)
            {
              switch (c)
                {
                case 'G':
                  v_GboxG = val;
                  break;
                case 'C':
                  v_GboxC = val;
                  break;
                case 'M':
                  v_GboxM = val;
                  break;
                case 'A':
                  v_GboxA = val;
                  break;
                case 'U':
                  v_GboxU = val - 1;
                  break;
                case 'H':
                  v_GboxH = val;
                  break;
                case 'T':
                  v_GboxT = val;
                  break;
                }
            }
        }
      fclose (file);
    }
}

void
SetupGbox::WriteCFG ()
{
  FILE *file1, *file2;
  char line[256];
  char c;
  int val;
  int ok = 0;
  file1 = fopen ("/var/keys/gbox_cfg", "r");
  if (file1)
    {
      file2 = fopen ("/var/keys/gbox_cfg.new", "w");
      if (file2)
        {
          while (fgets (line, 256, file1) != NULL)
            {
              if (sscanf (line, "%c: { %02d }", &c, &val) == 2)
                {
                  switch (c)
                    {
                    case 'G':
                      fprintf (file2, "%c: { %02d }\n", c, v_GboxG);
                      break;
                    case 'C':
                      fprintf (file2, "%c: { %02d }\n", c, v_GboxC);
                      break;
                    case 'M':
                      fprintf (file2, "%c: { %02d }\n", c, v_GboxM);
                      break;
                    case 'A':
                      fprintf (file2, "%c: { %02d }\n", c, v_GboxA);
                      break;
                    case 'U':
                      fprintf (file2, "%c: { %02d }\n", c, v_GboxU + 1);
                      break;
                    case 'H':
                      fprintf (file2, "%c: { %02d }\n", c, v_GboxH);
                      break;
                    case 'T':
                      fprintf (file2, "%c: { %02d }\n", c, v_GboxT);
                      break;
                    default:
                      fprintf (file2, line);
                      break;
                    }
                }
              else
                fprintf (file2, line);
            }
          if (fclose (file2) == 0)
            ok = 1;
        }
      fclose (file1);
      if (ok == 1)
        rename ("/var/keys/gbox_cfg.new", "/var/keys/gbox_cfg");
    }
}

SetupGbox::SetupGbox ():
eWindow (0)
{
  SetupGbox::GetCFG ();

  int fd = eSkin::getActive ()->queryValue ("fontsize", 16);
  int h_x = 300, l_w = 110;
  eLabel *l;

  setText (_("GBOX-Setup"));
  cmove (ePoint (720 / 2 - h_x, 120));
  cresize (eSize (h_x * 2, 380));

  l = new eLabel (this);
  l->move (ePoint (10, 10));
  l->resize (eSize (l_w, fd + 4));
  l->setText (_("Work Mode"));
  GboxG = new eComboBox (this, 4, l);
  GboxG->move (ePoint (l_w + 10, 10));
  GboxG->resize (eSize (150, 35));
  GboxG->setHelpText (_("Select working mode"));
  GboxG->loadDeco ();
  new eListBoxEntryText (*GboxG, eString ().sprintf ("pure Emu"), (void *) 0);
  new eListBoxEntryText (*GboxG, eString ().sprintf ("pure SoftCam"), (void *) 1);
  new eListBoxEntryText (*GboxG, eString ().sprintf ("pure Net"), (void *) 2);
  new eListBoxEntryText (*GboxG, eString ().sprintf ("Mixed"), (void *) 3);
  GboxG->setCurrent (v_GboxG, false);

  l = new eLabel (this);
  l->move (ePoint (h_x, 10));
  l->resize (eSize (l_w, fd + 4));
  l->setText (_("Up. Mode:"));
  GboxU = new eComboBox (this, 2, l);
  GboxU->move (ePoint (h_x + l_w, 10));
  GboxU->resize (eSize (150, 35));
  GboxU->setHelpText (_("Select Key Update mode"));
  GboxU->loadDeco ();
  new eListBoxEntryText (*GboxU, eString ().sprintf ("only new keys"), (void *) 0);
  new eListBoxEntryText (*GboxU, eString ().sprintf ("all keys"), (void *) 1);
  GboxU->setCurrent (v_GboxU, false);

  l = new eLabel (this);
  l->move (ePoint (10, 50));
  l->resize (eSize (l_w, fd + 4));
  l->setText (_("Disp ECM:"));
  GboxC = new eComboBox (this, 3, l);
  GboxC->move (ePoint (l_w + 10, 50));
  GboxC->resize (eSize (150, 35));
  GboxC->setHelpText (_("Display ECM messages on Konsole?"));
  GboxC->loadDeco ();
  new eListBoxEntryText (*GboxC, eString ().sprintf ("None"), (void *) 0);
  new eListBoxEntryText (*GboxC, eString ().sprintf ("only valid"), (void *) 1);
  new eListBoxEntryText (*GboxC, eString ().sprintf ("valid and bad"), (void *) 2);
  GboxC->setCurrent (v_GboxC, false);

  l = new eLabel (this);
  l->move (ePoint (h_x, 50));
  l->resize (eSize (l_w, fd + 4));
  l->setText (_("K. Folder:"));
  GboxT = new eComboBox (this, 2, l);
  GboxT->move (ePoint (h_x + l_w, 50));
  GboxT->resize (eSize (150, 35));
  GboxT->setHelpText (_("Select folder for Keys"));
  GboxT->loadDeco ();
  new eListBoxEntryText (*GboxT, eString ().sprintf ("/var/keys"), (void *) 0);
  new eListBoxEntryText (*GboxT, eString ().sprintf ("/var/tmp"), (void *) 1);
  GboxT->setCurrent (v_GboxT, false);

  l = new eLabel (this);
  l->move (ePoint (10, 90));
  l->resize (eSize (l_w, fd + 4));
  l->setText (_("Disp EMM:"));
  GboxM = new eComboBox (this, 3, l);
  GboxM->move (ePoint (l_w + 10, 90));
  GboxM->resize (eSize (150, 35));
  GboxM->setHelpText (_("Display EMM messages on Konsole"));
  GboxM->loadDeco ();
  new eListBoxEntryText (*GboxM, eString ().sprintf ("None"), (void *) 0);
  new eListBoxEntryText (*GboxM, eString ().sprintf ("only valid"), (void *) 1);
  new eListBoxEntryText (*GboxM, eString ().sprintf ("valid and bad"), (void *) 2);
  GboxM->setCurrent (v_GboxM, false);

  l = new eLabel (this);
  l->move (ePoint (10, 130));
  l->resize (eSize (l_w, fd + 4));
  l->setText (_("AU:"));
  GboxA = new eComboBox (this, 3, l);
  GboxA->move (ePoint (l_w + 10, 130));
  GboxA->resize (eSize (150, 35));
  GboxA->setHelpText (_("Select AU mode"));
  GboxA->loadDeco ();
  new eListBoxEntryText (*GboxA, eString ().sprintf ("Off"), (void *) 0);
  new eListBoxEntryText (*GboxA, eString ().sprintf ("On"), (void *) 1);
  new eListBoxEntryText (*GboxA, eString ().sprintf ("Auto"), (void *) 2);
  GboxA->setCurrent (v_GboxA, false);

  ok = new eButton (this);
  ok->setText (_("save"));
  ok->setShortcut ("green");
  ok->setShortcutPixmap ("green");
  ok->move (ePoint (10, 270));
  ok->resize (eSize (150, 40));
  ok->setHelpText (_("Save changes and return"));
  ok->loadDeco ();
  CONNECT (ok->selected, SetupGbox::okPressed);

  abort = new eButton (this);
  abort->setShortcut ("red");
  abort->setShortcutPixmap ("red");
  abort->loadDeco ();
  abort->setText (_("abort"));
  abort->move (ePoint (180, 270));
  abort->resize (eSize (150, 40));
  abort->setHelpText (_("Ignore changes and return"));
  CONNECT (abort->selected, SetupGbox::abortPressed);

  defaults = new eButton (this);
  defaults->setShortcut ("blue");
  defaults->setShortcutPixmap ("blue");
  defaults->loadDeco ();
  defaults->setText (_("defaults"));
  defaults->move (ePoint (180 + 170, 270));
  defaults->resize (eSize (150, 40));
  defaults->setHelpText (_("Load good defaults for local CS"));
  CONNECT (defaults->selected, SetupGbox::defaultsPressed);

  statusbar = new eStatusBar (this);
  statusbar->move (ePoint (0, clientrect.height () - 60));
  statusbar->resize (eSize (clientrect.width (), 50));
  statusbar->loadDeco ();
}

void
SetupGbox::okPressed ()
{
  v_GboxG = (int) GboxG->getCurrent ()->getKey ();
  v_GboxC = (int) GboxC->getCurrent ()->getKey ();
  v_GboxM = (int) GboxM->getCurrent ()->getKey ();
  v_GboxA = (int) GboxA->getCurrent ()->getKey ();
  v_GboxU = (int) GboxU->getCurrent ()->getKey ();
  // v_GboxH = (int)GboxH->getCurrent()->getKey();
  v_GboxT = (int) GboxT->getCurrent ()->getKey ();
  SetupGbox::WriteCFG ();
  close (1);
}

void
SetupGbox::abortPressed ()
{
  close (1);
}

void
SetupGbox::defaultsPressed ()
{
  GboxG->setCurrent (0, false);
  GboxC->setCurrent (1, false);
  GboxM->setCurrent (1, false);
  GboxA->setCurrent (2, false);
  GboxU->setCurrent (1 - 1, false);
  // GboxH->setCurrent (1, false);
  GboxT->setCurrent (1, false);
}

#endif // SETUP_GBOX
