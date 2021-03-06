#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#ifdef DM7020
#define INIT_D "/etc/init.d"
#endif

#ifdef DM7000
#define INIT_D "/var/etc/init.d"
#endif

int port = 80;

int
echo_system (char *cmd)
{
  printf ("+ %s\n", cmd);
  return system (cmd);
}

void
wget (const char *url)
{
  char cmd[512];
  sprintf (cmd, "wget -O /dev/null -q --header 'Authorization: Basic cm9vdDpkcmVhbWJveA==' 'http://127.0.0.1:%d%s'", port, url);
  printf ("+ %s\n", cmd);
  system (cmd);
}

char
message (const char *mes, int duration)
{
  char url[512];
  sprintf (url, "/cgi-bin/xmessage?timeout=%d&caption=Emu+Message&body=%s&type=1", duration, mes);
  wget (url);
}

void
rezap ()
{
  char cmd[512];
  sprintf (cmd,
           "sh -c \"SERVICE=`wget --header 'Authorization: Basic cm9vdDpkcmVhbWJveA==' -O - 'http://127.0.0.1:%d/cgi-bin/currentService'` ; wget --header 'Authorization: Basic cm9vdDpkcmVhbWJveA==' -q -O /dev/null http://127.0.0.1:%d/cgi-bin/zapTo?path=\\$SERVICE\"",
           port, port);
  printf ("+ %s\n", cmd);
  system (cmd);
}

void
restart_emu ()
{
  char line[512], cmd[128], crdsrv[32], emu[32];
  char *ptr;
  FILE *F;
  F = fopen ("/etc/rc.config", "r");
  if (F)
    {
      emu[0] = '\0';
      crdsrv[0] = '\0';
      while (fgets (line, 256, F) != NULL)
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
          if ((ptr = strchr (line, '=')) != NULL)
            {
              *ptr = '\0';
              if (strcmp (line, "EMU") == 0)
                {
                  strcpy (emu, ptr + 1);
                }
              if (strcmp (line, "CRDSRV") == 0)
                {
                  strcpy (crdsrv, ptr + 1);
                }
            }
        }
      if (emu[0])
        {
          if ((strcmp (crdsrv, "auto") == 0) || (crdsrv[0] == '\0'))
            {
              strcpy (crdsrv, emu);
              if (strcmp (crdsrv, "mgcam") == 0)
                strcpy (crdsrv, "newcs");
            }
          printf ("Stop\n");
          message ("Stopping+cam+daemons", 2);
          system ("sh -c \"ls " INIT_D "/emu_* |while read x ; do sh \\$x stop ; done\"");
          system ("sh -c \"ls " INIT_D "/crdsrv* |while read x ; do sh \\$x stop ; done\"");
          unlink ("/var/tmp/camd.socket");
          sleep (4);
          unlink ("/etc/rcS.d/S40emu");
          sprintf (cmd, "ln -s ../init.d/emu_%s /etc/rcS.d/S40emu", emu);
          echo_system (cmd);
          unlink ("/etc/rcS.d/S40crdsrv");
          if ( strcmp (crdsrv, "none") != 0 )
          {
            sprintf (cmd, "ln -s ../init.d/crdsrv_%s /etc/rcS.d/S40crdsrv", crdsrv);
            echo_system (cmd);
            sprintf (line, "Starting+server:+%s", crdsrv);
            message (line, 2);
            system ("sh -c \"/etc/rcS.d/S40crdsrv start\"");
          }
          sprintf (line, "Starting:+%s", emu);
          message (line, 2);
          system ("sh -c \"/etc/rcS.d/S40emu start\"");
          sleep (5);
          rezap ();
        }
    }
}

int
main (int argc, char **argv)
{
  struct stat st, prev_st;
  int rv;
  pid_t pid, sid;

  printf ("emu daemon started\n");

  if (fork () == 0)
    {
      sid = setsid ();
      if (sid < 0)
        {
          printf ("setsid() error\n");
          exit (1);
        }
      if ((chdir ("/")) < 0)
        {
          printf ("chdir() error\n");
          exit (1);
        }
      if (argc == 1)
        {
          close (STDIN_FILENO);
          close (STDOUT_FILENO);
          close (STDERR_FILENO);
        }
      message ("emud+started", 2);

      stat ("/etc/rc.config", &prev_st);
      while (1)
        {
          usleep (100000);      // 0.1 sec
          rv = stat ("/etc/rc.config", &st);
          if (rv == 0 && st.st_mtime != prev_st.st_mtime)
            {
              prev_st = st;
              printf ("CHAMGE\n");
              restart_emu ();
            }
        }
    }
}
