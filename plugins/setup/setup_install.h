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

#ifndef __setup_install_h
#define __setup_install_h

#include <setup_window.h>
#include <lib/gui/ewindow.h>
#include <lib/gui/listbox.h>
#include <lib/gui/statusbar.h>
#include <lib/base/console.h>
#include <lib/gui/ebutton.h>

class eButton;

class InstallSw:public eWindow
{
  eButton *abort, *dl;
    eListBox < eListBoxEntryText > *listb, *dummyb;
  eStatusBar *statusbar;
private:
  void abortPressed ();
  void dlPressed ();
  void do_install (eListBoxEntryText * item);
public:
    InstallSw ();
   ~InstallSw ();
};

class RemoveSw:public eWindow
{
  eButton *abort, *dl;
    eListBox < eListBoxEntryText > *listb;
  eStatusBar *statusbar;
private:
  void abortPressed ();
  void dlPressed ();
  void do_remove (eListBoxEntryText * item);
public:
    RemoveSw ();
   ~RemoveSw ();
};

class dlWindow:public eWindow
{
  eButton *abort, *ok;
  eConsoleAppContainer *app;
private:
  void abortPressed ();
  void okPressed ();
  void appClosed (int);
public:
    dlWindow ();
};

#endif
