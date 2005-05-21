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

// #include <setup.h>
#include <cs_edit.h>

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
#include <plugin.h>

extern "C" int plugin_exec (PluginParam * par);
extern char *find_file(const char *);

int
plugin_exec (PluginParam * par)
{
  find_file ( "/bin" );
  SetupScam_cs setup;
  setup.show ();
  setup.exec ();
  setup.hide ();
  return 0;
}

void
SetupScam_cs::GetCFG ()
{
  printf ("SetupScam_cs::GetCFG()\n");
  char section[32] = "";
  char line[256];
  char one[64];
  char two[64];
  char three[64];
  char *ptr;
  int i, n;
  FILE *fp;

  c_server[1] = "localhost";
  for (i = 2; i < MAX_IP; i++)
    c_server[i] = "";
  c_port[1] = "1500";
  c_mode = "3";
  c_serial = "00000000";
  c_au_internet = "false";
  s_port = "1500";
  s_mode = "4";
#ifdef DM5600
  s_phoenix_dev = "/dev/tts/p";
#else
  s_phoenix_dev = "/dev/notused";
#endif
  v_cs_server_ip[1][0] = 127;
  v_cs_server_ip[1][1] = 0;
  v_cs_server_ip[1][2] = 0;
  v_cs_server_ip[1][3] = 1;
  for (i = 2; i < MAX_IP; i++)
    {
      v_cs_server_ip[i][0] = 0;
      v_cs_server_ip[i][1] = 0;
      v_cs_server_ip[i][2] = 0;
      v_cs_server_ip[i][3] = 0;
      c_port[i] = "1500";
    }
  numeric = 1;

  fp = fopen ("/var/etc/romu.cfg", "r");
  if (fp)
    {
      while (fgets (line, 256, fp) != NULL)
        {
          if ((ptr = strchr (line, '#')) != NULL)
            {
              *ptr = '\0';
            }
          if ((ptr = strchr (line, '\n')) != NULL)
            {
              *ptr = '\0';
            }
          if (strlen (line) == 0)
            {
              continue;
            }
          //printf ("S: %s L: %s\n", section, line);
          if (sscanf (line, "[%s]", section) == 1)
            continue;
          if (sscanf (line, "%s = %s", one, two) == 2)
            {
              if (strncmp (section, "server", 6) == 0)
                {
                  // printf ("Server: O: (%s) T: (%s)\n", one, two);
                  if (strcasecmp (one, "port") == 0)
                    s_port = two;
                  if (strcasecmp (one, "phoenix_dev") == 0)
                    s_phoenix_dev = two;
                  if (strcasecmp (one, "mode") == 0)
                    s_mode = two;
                }
              if (strncmp (section, "client", 6) == 0)
                {
                  // printf ("Client: O: (%s) T: (%s)\n", one, two);
                  if (strcasecmp (one, "serial") == 0)
                    c_serial = two;
                  if (strcasecmp (one, "port") == 0)
                    c_port[1] = two;
                  if (strcasecmp (one, "au_internet") == 0)
                    c_au_internet = two;
                  if (sscanf (one, "server_%d", &n) == 1)
                    {
                      printf ("N %d\n", n);
                      if ((ptr = strchr (two, ':')) != NULL)
                        *ptr = ' ';
                      sscanf (two, "%s %s", one, three);
                      c_server[n] = one;
                      c_port[n] = three;
                      printf ("S %d = %s\n", n, c_server[n].c_str ());
                      for (int i = 0; i < 3; i++)
                        if ((ptr = strchr (one, '.')) != NULL)
                          *ptr = ' ';
                      if (sscanf (one, "%d %d %d %d", &v_cs_server_ip[n][0], &v_cs_server_ip[n][1], &v_cs_server_ip[n][2], &v_cs_server_ip[n][3]) != 4)
                        numeric = 0;
                    }
                  if (strcasecmp (one, "server") == 0)
                    {
                      c_server[1] = two;
                      for (int i = 0; i < 3; i++)
                        if ((ptr = strchr (two, '.')) != NULL)
                          *ptr = ' ';
                      if (sscanf (two, "%d %d %d %d", &v_cs_server_ip[1][0], &v_cs_server_ip[1][1], &v_cs_server_ip[1][2], &v_cs_server_ip[1][3]) != 4)
                        numeric = 0;
                    }
                  if (strcasecmp (one, "mode") == 0)
                    c_mode = two;
                }
            }
        }
      fclose (fp);
    }
  printf ("end SetupScam_cs::GetCFG()\n");
}

void
SetupScam_cs::WriteCFG ()
{
  FILE *fp;
  fp = fopen ("/var/etc/romu.cfg", "w");
  if (fp)
    {
      fprintf (fp, "[client]\n");
      //fprintf (fp, "server = %s\n", c_server.c_str ());
      for (int i = 1; i < MAX_IP; i++)
        if (c_server[i] != "" && c_server[i] != "0.0.0.0")
          fprintf (fp, "server_%d = %s:%s\n", i, c_server[i].c_str (), c_port[i].c_str ());
      //fprintf (fp, "port = %s\n", c_port.c_str ());
      fprintf (fp, "mode = %s\n", c_mode.c_str ());
      if (c_serial.c_str () != "00000000")
        fprintf (fp, "serial = %s\n", c_serial.c_str ());
      fprintf (fp, "au_internet = %s\n", c_au_internet.c_str ());
      fprintf (fp, "\n[server]\n");
      fprintf (fp, "port = %s\n", s_port.c_str ());
      fprintf (fp, "mode = %s\n", s_mode.c_str ());
      fprintf (fp, "phoenix_dev = %s\n", s_phoenix_dev.c_str ());
      fclose (fp);
    }
}

SetupScam_cs::SetupScam_cs ():
eWindow (0)
{
  SetupScam_cs::GetCFG ();

  int fd = eSkin::getActive ()->queryValue ("fontsize", 16);
  int nsrvport, nclntport;
  int s_y = 130, h_x = 250;
  char name[15];
  server = 1;
  //struct in_addr sinet_address;
  eLabel *l;

  setText (_("CS-Setup"));
  cmove (ePoint (720 / 2 - h_x, 120));
  cresize (eSize (h_x * 2, 380));

  l = new eLabel (this);
  l->setText ("Client");
  l->move (ePoint (10, 10));
  l->resize (eSize (150, fd + 4));

  //l = new eLabel (this);
  //l->setText ("Server:");
  //l->move (ePoint (20, 50));
  //l->resize (eSize (150, fd + 4));

  cs_num = new eCheckbox (this, numeric, 1);
  cs_num->setText (_("Numeric Addresses"));
  cs_num->move (ePoint (h_x * 2 - 210, 10));
  cs_num->resize (eSize (210, fd + 4));
  cs_num->setHelpText (_("Use IP adresses instead of hostnames"));
  CONNECT (cs_num->checked, SetupScam_cs::NumChanged);

  ServerN = new eComboBox (this, 4, l);
  //ServerN = new eListBox < eListBoxEntryText > (this);
  ServerN->move (ePoint (20, 50));
  ServerN->resize (eSize (110, fd + 4));
  ServerN->setHelpText (_("Select Server"));
  ServerN->setFlags (eListBox < eListBoxEntryText >::flagNoUpDownMovement);
  ServerN->loadDeco ();
  for (int i = 1; i < MAX_IP; i++)
    {
      sprintf (name, "server %d", i);
      new eListBoxEntryText (*ServerN, eString ().sprintf (name), (void *) i);
      //new eListBoxEntryText (ServerN, eString ().sprintf (name), (void *) i);
    }
  ServerN->setCurrent (0, false);
  //ServerN->show();
  CONNECT (ServerN->selchanged, SetupScam_cs::SelectServerN);

  for (int i = 1; i < MAX_IP; i++)
    {
      printf ("Q1: %d\n", i);
      cs_server_ip[i] = new eNumber (this, 4, 0, 255, 3, v_cs_server_ip[i], 0, l);
      cs_server_ip[i]->move (ePoint (130, 50));
      cs_server_ip[i]->resize (eSize (200, fd + 10));
      cs_server_ip[i]->setFlags (eNumber::flagDrawPoints);
      cs_server_ip[i]->setHelpText (_("Enter IP Address of CS-Server"));
      cs_server_ip[i]->loadDeco ();
      cs_server_ip[i]->hide ();
    }

  for (int i = 1; i < MAX_IP; i++)
    {
      printf ("Q2: %d\n", i);
      cs_server[i] = new eTextInputField (this);
      cs_server[i]->move (ePoint (130, 50));
      cs_server[i]->resize (eSize (200, fd + 10));
      cs_server[i]->setHelpText (_("Enter hostname of CS-server"));
      cs_server[i]->setUseableChars ("01234567890abcdefghijklmnopqrstuvwxyz._-");
      cs_server[i]->setMaxChars (64);
      cs_server[i]->loadDeco ();
      cs_server[i]->setText (c_server[i]);
      cs_server[i]->hide ();
    }

  l = new eLabel (this);
  l->setText ("Port:");
  l->move (ePoint (20, 80));
  l->resize (eSize (150, fd + 4));

  for (int i = 1; i < MAX_IP; i++)
    {
      sscanf (c_port[i].c_str (), "%i", &nclntport);
      cs_clntport[i] = new eNumber (this, 1, 0, 9999, 4, &nclntport, 0, l);
      cs_clntport[i]->move (ePoint (130, 80));
      cs_clntport[i]->resize (eSize (100, fd + 10));
      cs_clntport[i]->setFlags (eNumber::flagDrawPoints);
      cs_clntport[i]->setHelpText (_("Enter port number, the server is listening on"));
      cs_clntport[i]->loadDeco ();
      cs_clntport[i]->hide ();
    }

  if (c_au_internet == "true")
    v_cs_au_in = 1;
  else
    v_cs_au_in = 0;
  cs_au_in = new eCheckbox (this, v_cs_au_in, 1);
  cs_au_in->setText (_("AU int."));
  cs_au_in->move (ePoint (h_x, 80));
  cs_au_in->resize (eSize (fd + 4 + 150, fd + 4));
  cs_au_in->setHelpText (_("AutoUpdate CS-Server over internet"));
  //CONNECT (cs_au_in->checked, eZapEmuSetup::EmuEnabledChanged);


// SERVER
  l = new eLabel (this);
  l->setText ("Local Server");
  l->move (ePoint (10, s_y));
  l->resize (eSize (150, fd + 4));

  l = new eLabel (this);
  l->setText ("Port:");
  l->move (ePoint (20, s_y + 40));
  l->resize (eSize (150, fd + 4));

  sscanf (s_port.c_str (), "%i", &nsrvport);
  cs_srvport = new eNumber (this, 1, 0, 9999, 4, &nsrvport, 0, l);
  cs_srvport->move (ePoint (130, s_y + 40));
  cs_srvport->resize (eSize (100, fd + 10));
  cs_srvport->setFlags (eNumber::flagDrawPoints);
  cs_srvport->setHelpText (_("Enter port number, the server must listen on"));
  cs_srvport->loadDeco ();

  l = new eLabel (this);
  l->setText ("Phoenix:");
  l->move (ePoint (20, s_y + 70));
  l->resize (eSize (150, fd + 4));

  cs_p_dev = new eTextInputField (this);
  cs_p_dev->move (ePoint (130, s_y + 70));
  cs_p_dev->resize (eSize (200, fd + 10));
  cs_p_dev->setHelpText (_("Devicename for optional phoenix device"));
  //cs_p_dev->setUseableChars("01234567890abcdefABCDEF:");
  cs_p_dev->setMaxChars (32);
  cs_p_dev->loadDeco ();
  cs_p_dev->setText (s_phoenix_dev);

  ok = new eButton (this);
  ok->setText (_("save"));
  ok->setShortcut ("green");
  ok->setShortcutPixmap ("green");
  ok->move (ePoint (10, 270));
  ok->resize (eSize (150, 40));
  ok->setHelpText (_("Save changes and return"));
  ok->loadDeco ();
  CONNECT (ok->selected, SetupScam_cs::okPressed);

  abort = new eButton (this);
  abort->setShortcut ("red");
  abort->setShortcutPixmap ("red");
  abort->loadDeco ();
  abort->setText (_("abort"));
  abort->move (ePoint (180, 270));
  abort->resize (eSize (150, 40));
  abort->setHelpText (_("Ignore changes and return"));
  CONNECT (abort->selected, SetupScam_cs::abortPressed);

  defaults = new eButton (this);
  defaults->setShortcut ("blue");
  defaults->setShortcutPixmap ("blue");
  defaults->loadDeco ();
  defaults->setText (_("defaults"));
  defaults->move (ePoint (180 + 170, 270));
  defaults->resize (eSize (150, 40));
  defaults->setHelpText (_("Load good defaults for local CS"));
  CONNECT (defaults->selected, SetupScam_cs::defaultsPressed);

  statusbar = new eStatusBar (this);
  statusbar->move (ePoint (0, clientrect.height () - 60));
  statusbar->resize (eSize (clientrect.width (), 50));
  statusbar->loadDeco ();

  cs_clntport[server]->show ();
  if (numeric)
    {
      cs_server[server]->hide ();
      cs_server_ip[server]->show ();
    }
  else
    {
      cs_server[server]->show ();
      cs_server_ip[server]->hide ();
    }
}

void
SetupScam_cs::SelectServerN (eListBoxEntryText * item)
{
  int i;
  for (i = 1; i < MAX_IP; i++)
    {
      cs_server[i]->hide ();
      cs_server_ip[i]->hide ();
      cs_clntport[i]->hide ();
    }
  server = (int) ServerN->getCurrent ()->getKey ();
  printf ("QQQ: %d\n", server);
  cs_clntport[server]->show ();
  if (numeric)
    {
      cs_server[server]->hide ();
      cs_server_ip[server]->show ();
    }
  else
    {
      cs_server[server]->show ();
      cs_server_ip[server]->hide ();
    }
}

void
SetupScam_cs::okPressed ()
{
  char buf[20];
  for (int i = 1; i < MAX_IP; i++)
    {
      sprintf (buf, "%d", cs_clntport[i]->getNumber ());
      c_port[i] = buf;
    }
  c_mode = "3";
  if (cs_au_in->isChecked ())
    c_au_internet = "true";
  else
    c_au_internet = "false";
  sprintf (buf, "%d", cs_srvport->getNumber ());
  s_port = buf;
  s_mode = "4";
  s_phoenix_dev = cs_p_dev->getText ();

  if (numeric)
    {
      for (int j = 1; j < MAX_IP; j++)
        {
          for (int i = 0; i < 4; i++)
            v_cs_server_ip[j][i] = cs_server_ip[j]->getNumber (i);
          c_server[j] = eString ().sprintf ("%d.%d.%d.%d", v_cs_server_ip[j][0], v_cs_server_ip[j][1], v_cs_server_ip[j][2], v_cs_server_ip[j][3]);
        }
    }
  else
    {
      for (int j = 1; j < MAX_IP; j++)
        c_server[j] = cs_server[j]->getText ();
    }

  SetupScam_cs::WriteCFG ();
  close (1);
}

void
SetupScam_cs::abortPressed ()
{
  close (1);
}

void
SetupScam_cs::defaultsPressed ()
{
  cs_server[1]->setText ("localhost");
  cs_clntport[1]->setNumber (1500);
  cs_srvport->setNumber (1500);
  cs_au_in->setCheck (0);
#ifdef DM5600
  cs_p_dev->setText ("/dev/tts/p");
#else
  cs_p_dev->setText ("/dev/notused");
#endif
  cs_server_ip[1]->setNumber (0, 127);
  cs_server_ip[1]->setNumber (1, 0);
  cs_server_ip[1]->setNumber (2, 0);
  cs_server_ip[1]->setNumber (3, 1);
  for (int i = 2; i < MAX_IP; i++)
    {
      cs_server_ip[i]->setNumber (0, 0);
      cs_server_ip[i]->setNumber (1, 0);
      cs_server_ip[i]->setNumber (2, 0);
      cs_server_ip[i]->setNumber (3, 0);
      cs_server[i]->setText ("");
      cs_clntport[i]->setNumber (1500);
    }
}

void
SetupScam_cs::NumChanged (int i)
{
  if (cs_num->isChecked ())
    numeric = 1;
  else
    numeric = 0;
  printf ("Q:%d", server);
  if (numeric)
    {
      printf ("Q0:\n");
      cs_server[server]->hide ();
      printf ("Q0:\n");
      cs_server_ip[server]->show ();
      printf ("Q0:\n");
    }
  else
    {
      printf ("Q00:\n");
      cs_server[server]->show ();
      printf ("Q00:\n");
      cs_server_ip[server]->hide ();
      printf ("Q00:\n");
    }
}
