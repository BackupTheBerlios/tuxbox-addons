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

#ifndef __setup_runapp_h
#define __setup_runapp_h

#include <lib/gui/ewindow.h>
#include <lib/gui/listbox.h>
#include <lib/gui/statusbar.h>
#include <lib/base/console.h>

class eButton;
class eCheckbox;
class eComboBox;
class eNumber;
class eTextInputField;
class rc_config;

class RunApp:public eWindow
{
  int output;
  eConsoleAppContainer *app;
  eLabel *lState;
  eButton *bCancel, *bClose;
  int eventHandler (const eWidgetEvent & e);
  void onCancel ();
  void getData (eString);
  void appClosed (int);
public:
    RunApp ();
};

#endif
