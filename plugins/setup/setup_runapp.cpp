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

#include <setup.h>
#include <setup_emu.h>
#include <setup_runapp.h>

#define DEBUG 1

#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>


#include <lib/base/i18n.h>

//#include <lib/gui/ewindow.h>
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

RunApp::RunApp ():
eWindow (0)
{
  //setText(_("Running script"));
  printf ( "%s is running, please wait\n", Executable );
  setText (eString ().sprintf ("%s is running, please wait", Executable));
  cmove (ePoint (50, 100));
  cresize (eSize (630, 400));

  bClose = new eButton (this);
  bClose->setText (_("Close"));
  bClose->setShortcut ("green");
  bClose->setShortcutPixmap ("green");
  bClose->move (ePoint (440, 10));
  bClose->resize (eSize (170, 40));
  bClose->loadDeco ();
  bClose->hide ();
  CONNECT (bClose->selected, RunApp::accept);

/*
	bCancel=new eButton(this);
	bCancel->loadDeco();
	bCancel->setText(_("abort"));
	bCancel->move(ePoint(210, 220));
	bCancel->resize(eSize(170, 40));
	bCancel->setHelpText(_("ignore changes and return"));
	CONNECT( bCancel->selected, RunApp::onCancel );
*/

  lState = new eLabel (this);
  lState->setName ("state");
  lState->move (ePoint (0, 0));
  lState->resize (eSize (630, 400));
}

int
RunApp::eventHandler (const eWidgetEvent & e)
{
  switch (e.type)
    {
    case eWidgetEvent::execBegin:
      {
        app = NULL;
        output = 0;
        // app = new eConsoleAppContainer (eString ().sprintf (Executable));
        app = new eConsoleAppContainer (Executable);
        bClose->hide ();

        if (!app->running ())
          {
            eMessageBox msg (_("sorry, start script."), _("sorry, start script."), eMessageBox::btOK | eMessageBox::iconError);
            msg.show ();
            msg.exec ();
            msg.hide ();
            close (-1);
          }
        else
          {
            CONNECT (app->dataAvail, RunApp::getData);
            CONNECT (app->appClosed, RunApp::appClosed);
          }
      }
      break;

    case eWidgetEvent::execDone:
      if (app)
        delete app;
      break;

    default:
      return eWindow::eventHandler (e);
    }
  return 1;
}

void
RunApp::getData (eString str)
{
  char *begin, *ptr;
  int c;

  // printf( "RunApp::getData\n");
  eString buf;
  buf = lState->getText () + str;
  begin = (char *) buf.c_str ();
  ptr = begin + strlen (begin);
  c = 0;
  while (ptr > begin && c <= 17)
    {
      ptr--;
      if (ptr[0] == '\n')
        c++;
    }
  if (ptr[0] == '\n')
    ptr++;
  lState->setText (ptr);
  output = 1;
}

void
RunApp::onCancel ()
{
  if (app)
    app->kill ();
  close (1);
}

void
RunApp::appClosed (int i)
{
  // printf( "RunApp::appClosed i=%i\n", i);
  if (!output)
    lState->setText (NO_OUTPUT_MESSAGE);
  if (i != 0)
    lState->setText (_("An error occured during exucution"));
  bClose->show ();
}
