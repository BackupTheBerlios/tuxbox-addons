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

#ifndef __setup_h
#define __setup_h

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

#define MAX_EMU 10
#define MAX_SCRIPTS 8

#ifdef DM7020
#define ENIGMA_BIN "/usr/bin/enigma"
#define PLUGIN_BIN "/usr/lib/tuxbox/plugins/setup7020.so"
#endif

#ifdef DM7000
#define ENIGMA_BIN "/bin/enigma"
#define PLUGIN_BIN "/var/tuxbox/plugins/setup7.so"
#endif

extern char EMU[MAX_EMU][256];
extern char CRDSRV[MAX_EMU][256];
extern char *SCRIPTS[MAX_SCRIPTS];
extern char *Executable;
extern char RUN_MESSAGE[128];
extern char NO_OUTPUT_MESSAGE[128];
extern rc_config *RC;
int pl_exec(char *filename);
char * find_file (const char *file);

#endif
