#include <dlfcn.h>
#include <unistd.h>
#include <stdio.h>

#include<plugin.h>
#include<setup.h>

int pl_exec(char *so)
{
  int ok = 0;
  void *handle;
  char filename[256];
  char *error;
  int (*do_plugin_exec) (PluginParam * par);

  strcpy (filename, find_file (so) );

  ok = 1;
  if (ok == 1)
    {
      handle = dlopen (filename, RTLD_GLOBAL | RTLD_NOW);
      if (!handle)
        {
          fprintf (stderr, "%s\n", dlerror ());
          ok = 0;
        }
      dlerror ();               /* Clear any existing error */
      *(void **) (&do_plugin_exec) = dlsym (handle, "plugin_exec");
      if ((error = dlerror ()) != NULL)
        {
          fprintf (stderr, "%s\n", error);
          ok = 0;
        }
      if (ok == 1)
        {
          (*do_plugin_exec) (NULL);
          dlclose (handle);
        }
    }

  return ok;
}
