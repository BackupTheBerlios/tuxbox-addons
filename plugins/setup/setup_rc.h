#ifndef __setup_rc_h
#define __setup_rc_h

class rc_config
{
public:
  int StartServer;
  int StartDhcp;
  int StartSamba;
  int StartCron;
  int Enabled;
  int v_SelectedEmu;
  int no_emu;
  int sleep;
  char var_on[16];
  void read ();
  void write ();
};

#endif
