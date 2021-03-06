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

#ifndef __setup_rc_h
#define __setup_rc_h

#ifdef DM7020
#define RC_CONFIG "/etc/rc.config"
#else
#define RC_CONFIG "/var/etc/rc.config"
#endif

class rc_config
{
public:
  int StartServer;
  int StartDhcp;
  int StartSamba;
  int StartCron;
  int Enabled;
  int v_SelectedEmu;
  int v_SelectedCrdsrv;
  int no_emu;
  int no_crdsrv;
  int sleep;
  char var_on[16];
  char swap_on[16];
  char swap_size[16];
  //char crdsrv[32];
  void read ();
  void write ();
};

#endif
