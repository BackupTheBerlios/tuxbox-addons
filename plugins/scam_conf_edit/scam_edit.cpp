/*
 * scam edit made by The_Hydra (I think)
 *
 */

#include <plugin.h>
#include <stdio.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <lib/gui/statusbar.h>
#include <lib/gui/ewindow.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/emessage.h>
#include <lib/gui/elabel.h>
#include <lib/gui/enumber.h>
#include <lib/gui/listbox.h>
// #include <lib/gdi/font.h>
#include <lib/system/init_num.h>
#include <lib/gui/textinput.h>
#include <lib/gui/echeckbox.h>
#include <lib/dvb/decoder.h>
#include "scam_edit.h"

extern "C" int plugin_exec (PluginParam * par);

gFont eKeyEntry::fontKey;
gFont eKeyEntry::fontDesc;

int
plugin_exec (PluginParam * par)
{
  eKeyDatabase::getInstance ()->loadFile (eKey::IRDETO, "/var/keys/irdeto");
  eKeyDatabase::getInstance ()->loadFile (eKey::NAGRAVISION, "/var/keys/nagra");
  eKeyDatabase::getInstance ()->loadFile (eKey::NAGRAVISION_BOXKEY, "/var/keys/nagra_boxkey");
  eKeyDatabase::getInstance ()->loadFile (eKey::NAGRAVISION, "/var/keys/AutoRoll.key");
  eKeyDatabase::getInstance ()->loadFile (eKey::SECA, "/var/keys/seca");
  eKeyDatabase::getInstance ()->loadFile (eKey::VIACCESS, "/var/keys/via");
  eKeyDatabase::getInstance ()->loadFile (eKey::ECM_SERVER, "/var/scam/ca_servers");
  eKeyDatabase::getInstance ()->loadFile (eKey::LOCAL_ECM_SERVER, "/var/scam/local_ca_server");
  eKeyDatabase::getInstance ()->loadFile (eKey::CW_SERVER, "/var/scam/cw_servers");
  eKeyEditor::run ();

  struct stat
    buf;
  if (stat ("/var/keys", &buf)) // /var/keys dont exist... do create
    mkdir ("/var/keys", 777);
  if (stat ("/var/scam", &buf)) // /var/scam dont exist... do create
    mkdir ("/var/scam", 777);

  eKeyDatabase::getInstance ()->saveFile (eKey::IRDETO, "/var/keys/irdeto");
  eKeyDatabase::getInstance ()->saveFile (eKey::NAGRAVISION, "/var/keys/nagra");
  eKeyDatabase::getInstance ()->saveFile (eKey::NAGRAVISION_BOXKEY, "/var/keys/nagra_boxkey");
  eKeyDatabase::getInstance ()->saveFile (eKey::SECA, "/var/keys/seca");
  eKeyDatabase::getInstance ()->saveFile (eKey::VIACCESS, "/var/keys/via");
  eKeyDatabase::getInstance ()->saveFile (eKey::ECM_SERVER, "/var/scam/ca_servers");
  eKeyDatabase::getInstance ()->saveFile (eKey::LOCAL_ECM_SERVER, "/var/scam/local_ca_server");
  eKeyDatabase::getInstance ()->saveFile (eKey::CW_SERVER, "/var/scam/cw_servers");

  if (eKeyDatabase::getInstance ()->modified.size ())
    {
      system ("killall -HUP scam"); // any change.. inform scam
      system ("killall -HUP dccamd");
    }
#if 0                           // NEVER KILL AND RESTART SCAM
  {
    std::set < int >&
      modified = eKeyDatabase::getInstance ()->modified;
    int
      changed = 0;
    for (std::set < int >::iterator it (modified.begin ()); it != modified.end (); ++it)
      {
        changed |= *it;
      }
    if (changed &= ~(eKey::ECM_SERVER | eKey::LOCAL_ECM_SERVER))
      {
        // must restart camd.. sighup only working for ca_servers and ca_local_server at moment
        signal (SIGCHLD, SIG_IGN);
        eDebug ("restart scam... hope it is in PATH");
        system ("killall -9 scam");
        system ("killall -9 dccamd");
        if (fork () == 0)
          {
            for (unsigned int i = 3; i < 90; ++i)
              close (i);
            eDebug ("restart scam");
            execlp ("scam", "scam", NULL);
            _exit (0);
          }
      }
    else                        // changes in
      {
        eDebug ("send sighup to scam.. for reread ca_server config's");
        signal (SIGCHLD, SIG_IGN);
        system ("killall -HUP scam");
        system ("killall -HUP dccamd");
      }
  }
#endif
  return 0;
}

eIrdetoKey *
eIrdetoKey::parse (eString keystring)
{
  switch (keystring[0])
    {
    case 'I':                  // Irdeto PK
      {
        eIrdetoPK *key = new eIrdetoPK;
        key->system = IRDETO;
        key->type = PK;
        int temp_key[8];
        int used = -1;
        // we can not use the return value here because of %n :(

        if (!sscanf (keystring.c_str (), "I: { %x { %x %x %x %x %x %x %x %x } }%n",
                     &key->keyid, temp_key + 0, temp_key + 1, temp_key + 2, temp_key + 3, temp_key + 4, temp_key + 5, temp_key + 6, temp_key + 7, &used))
          {
            sscanf (keystring.c_str (), "I: { %x { %x %x %x %x %x %x %x %x }}%n",
                    &key->keyid, temp_key + 0, temp_key + 1, temp_key + 2, temp_key + 3, temp_key + 4, temp_key + 5, temp_key + 6, temp_key + 7, &used);
          }

        for (int i = 0; i < 8; ++i)
          key->key[i] = temp_key[i];

        if (used == -1)
          {
            printf ("parse error!\n");
            delete key;
            return 0;
          }
        while (keystring[used] == ' ')
          used++;
        key->comment[1] = keystring.mid (used);
        return key;
      }
    case 'M':                  // Irdeto PMK
      {
        eIrdetoPMK *key = new eIrdetoPMK;
        key->system = IRDETO;
        key->type = PMK;
        int temp_key[8];
        int used = -1;

        int temp_provid[3];
        // we can not use the return value here because of %n :(

        if (!sscanf (keystring.c_str (), "M: { %02x %02x %02x { %x %x %x %x %x %x %x %x } }%n",
                     temp_provid, temp_provid + 1, temp_provid + 2,
                     temp_key + 0, temp_key + 1, temp_key + 2, temp_key + 3, temp_key + 4, temp_key + 5, temp_key + 6, temp_key + 7, &used))
          {
            sscanf (keystring.c_str (), "M: { %02x %02x %02x { %x %x %x %x %x %x %x %x }}%n",
                    temp_provid, temp_provid + 1, temp_provid + 2,
                    temp_key + 0, temp_key + 1, temp_key + 2, temp_key + 3, temp_key + 4, temp_key + 5, temp_key + 6, temp_key + 7, &used);
          }

        key->providerid = temp_provid[0] << 16;
        key->providerid |= temp_provid[1] << 8;
        key->providerid |= temp_provid[2];

        for (int i = 0; i < 8; ++i)
          key->key[i] = temp_key[i];

        if (used == -1)
          {
            printf ("parse error!\n");
            delete key;
            return 0;
          }
        while (keystring[used] == ' ')
          used++;
        key->comment[1] = keystring.mid (used);
        return key;
      }
    case 'H':                  // Irdeto HMK
      {
        eIrdetoHMK *key = new eIrdetoHMK;
        key->system = IRDETO;
        key->type = HMK;
        int temp_key[10];
        int temp_hs[3];
        int used = -1;

        // we can not use the return value here because of %n :(
        if (!sscanf (keystring.c_str (), "H: { %02x %02x %02x { %x %x %x %x %x %x %x %x %x %x } }%n",
                     temp_hs, temp_hs + 1, temp_hs + 2,
                     temp_key + 0, temp_key + 1, temp_key + 2, temp_key + 3,
                     temp_key + 4, temp_key + 5, temp_key + 6, temp_key + 7, temp_key + 8, temp_key + 9, &used))
          {
            sscanf (keystring.c_str (), "H: { %02x %02x %02x { %x %x %x %x %x %x %x %x %x %x }}%n",
                    temp_hs, temp_hs + 1, temp_hs + 2,
                    temp_key + 0, temp_key + 1, temp_key + 2, temp_key + 3,
                    temp_key + 4, temp_key + 5, temp_key + 6, temp_key + 7, temp_key + 8, temp_key + 9, &used);
          }

        key->hexserial = temp_hs[0] << 16;
        key->hexserial |= temp_hs[1] << 8;
        key->hexserial |= temp_hs[2];

        for (int i = 0; i < 10; ++i)
          key->key[i] = temp_key[i];

        if (used == -1)
          {
            printf ("parse error!\n");
            delete key;
            return 0;
          }
        while (keystring[used] == ' ')
          used++;
        key->comment[1] = keystring.mid (used);
        return key;
      }
    default:
      return 0;
    }
}

eNagraKey *
eNagraKey::parse (eString keystring)
{
  switch (keystring[0])
    {
    case 'N':                  // opkey
      {
        eNagraOpKey *key = new eNagraOpKey;
        key->system = NAGRAVISION;
        key->type = Op;
        key->sysid = -1;
        int temp_key[8];
        int used = -1;
        int tmp_sysid[2];
        // we can not use the return value here because of %n :(
        if (sscanf (keystring.c_str (), "N: { %x %x %x { %x %x %x %x %x %x %x %x } }%n",
                    tmp_sysid + 0, tmp_sysid + 1,
                    &key->nr, temp_key + 0, temp_key + 1, temp_key + 2, temp_key + 3, temp_key + 4, temp_key + 5, temp_key + 6, temp_key + 7, &used) < 11)
          {
            if (!sscanf (keystring.c_str (), "N: { %x %x %x { %x %x %x %x %x %x %x %x }}%n",
                         tmp_sysid + 0, tmp_sysid + 1,
                         &key->nr, temp_key + 0, temp_key + 1, temp_key + 2, temp_key + 3, temp_key + 4, temp_key + 5, temp_key + 6, temp_key + 7, &used) < 11)
              {
                if (!sscanf (keystring.c_str (), "N: { %x %x { %x %x %x %x %x %x %x %x } }%n",
                             &key->sysid,
                             &key->nr,
                             temp_key + 0, temp_key + 1, temp_key + 2, temp_key + 3, temp_key + 4, temp_key + 5, temp_key + 6, temp_key + 7, &used) < 10)
                  {
                    sscanf (keystring.c_str (), "N: { %x %x { %x %x %x %x %x %x %x %x }}%n",
                            &key->sysid,
                            &key->nr, temp_key + 0, temp_key + 1, temp_key + 2, temp_key + 3, temp_key + 4, temp_key + 5, temp_key + 6, temp_key + 7, &used);
                  }
              }
          }

        if (key->sysid == -1)
          {
            key->sysid = tmp_sysid[0] << 8;
            key->sysid |= tmp_sysid[1];
          }

        for (int i = 0; i < 8; ++i)
          key->key[i] = temp_key[i];

        if (used == -1)
          {
            printf ("parse error!\n");
            delete key;
            return 0;
          }
        while (keystring[used] == ' ')
          used++;
        key->comment[1] = keystring.mid (used);
        return key;
      }
    default:
      return 0;
    }
  return 0;
}

eNagraBoxKey *
eNagraBoxKey::parse (eString keystring)
{
  switch (keystring[0])
    {
    case 'B':                  // nagra box key
      {
        eDebug ("boxkey");
        eNagraBoxKey *key = new eNagraBoxKey;
        key->system = NAGRAVISION_BOXKEY;
        int temp_key[8];
        int used = -1;
        int temp_serial[4];
        // we can not use the return value here because of %n :(

        if (!sscanf (keystring.c_str (), "B: { %x %x %x %x { %x %x %x %x %x %x %x %x } }%n",
                     &temp_serial[0], &temp_serial[1], &temp_serial[2], &temp_serial[3],
                     temp_key + 0, temp_key + 1, temp_key + 2, temp_key + 3, temp_key + 4, temp_key + 5, temp_key + 6, temp_key + 7, &used))
          {
            sscanf (keystring.c_str (), "B: { %x %x %x %x { %x %x %x %x %x %x %x %x }}%n",
                    &temp_serial[0], &temp_serial[1], &temp_serial[2], &temp_serial[3],
                    temp_key + 0, temp_key + 1, temp_key + 2, temp_key + 3, temp_key + 4, temp_key + 5, temp_key + 6, temp_key + 7, &used);
          }

        key->serial = ((temp_serial[0] << 24) | (temp_serial[1] << 16) | (temp_serial[2] << 8) | (temp_serial[3]));

        for (int i = 0; i < 8; ++i)
          key->key[i] = temp_key[i];

        if (used == -1)
          {
            printf ("parse error!\n");
            delete key;
            return 0;
          }
        while (keystring[used] == ' ')
          used++;
        key->comment[1] = keystring.mid (used);
        return key;
      }
    default:
      return 0;
    }
  return 0;
}

eSecaKey *
eSecaKey::parse (eString keystring)
{
  switch (keystring[0])
    {
    case 'S':                  // Seca Op-key
      {
        eSecaOpKey *key = new eSecaOpKey;
        key->system = SECA;
        key->type = Op;
        int temp_key[8];
        int used = -1;
        int temp_prov[2];
        // we can not use the return value here because of %n :(

        if (!sscanf (keystring.c_str (), "S: { %x %x %x { %x %x %x %x %x %x %x %x } }%n",
                     &temp_prov[0], &temp_prov[1],
                     &key->nr, temp_key + 0, temp_key + 1, temp_key + 2, temp_key + 3, temp_key + 4, temp_key + 5, temp_key + 6, temp_key + 7, &used))
          {
            sscanf (keystring.c_str (), "S: { %x %x %x { %x %x %x %x %x %x %x %x }}%n",
                    &temp_prov[0], &temp_prov[1],
                    &key->nr, temp_key + 0, temp_key + 1, temp_key + 2, temp_key + 3, temp_key + 4, temp_key + 5, temp_key + 6, temp_key + 7, &used);
          }

        key->provider = (temp_prov[0] << 8) | temp_prov[1];

        for (int i = 0; i < 8; ++i)
          key->key[i] = temp_key[i];

        if (used == -1)
          {
            printf ("parse error!\n");
            delete key;
            return 0;
          }
        while (keystring[used] == ' ')
          used++;
        key->comment[1] = keystring.mid (used);
        return key;
      }
    case 'M':                  // Seca Masterkey
      {
        eSecaMasterKey *key = new eSecaMasterKey;
        key->system = SECA;
        key->type = Master;
        int temp_key[8];
        int temp_prov[2], temp_ppua[4];
        int used = -1;

        // we can not use the return value here because of %n :(
        if (!sscanf (keystring.c_str (), "M: { %x %x %x %x %x %x %x { %x %x %x %x %x %x %x %x } }%n",
                     &temp_prov[0], &temp_prov[1],
                     &temp_ppua[0], &temp_ppua[1], &temp_ppua[2], &temp_ppua[3],
                     &key->nr, temp_key + 0, temp_key + 1, temp_key + 2, temp_key + 3, temp_key + 4, temp_key + 5, temp_key + 6, temp_key + 7, &used))
          {
            sscanf (keystring.c_str (), "M: { %x %x %x %x %x %x %x { %x %x %x %x %x %x %x %x }}%n",
                    &temp_prov[0], &temp_prov[1],
                    &temp_ppua[0], &temp_ppua[1], &temp_ppua[2], &temp_ppua[3],
                    &key->nr, temp_key + 0, temp_key + 1, temp_key + 2, temp_key + 3, temp_key + 4, temp_key + 5, temp_key + 6, temp_key + 7, &used);
          }

        key->provider = (temp_prov[0] << 8) | temp_prov[1];
        key->ppua = (((((temp_ppua[0] << 8) | temp_ppua[1]) << 8) | temp_ppua[2]) << 8) | temp_ppua[3];

        for (int i = 0; i < 8; ++i)
          key->key[i] = temp_key[i];

        if (used == -1)
          {
            printf ("parse error!\n");
            delete key;
            return 0;
          }
        while (keystring[used] == ' ')
          used++;
        key->comment[1] = keystring.mid (used);
        return key;
      }
    default:
      return 0;
    }
}

eViaccessKey *
eViaccessKey::parse (eString keystring)
{
  switch (keystring[0])
    {
    case 'V':                  // Viaccess Op-key
      {
        eViaccessOpKey *key = new eViaccessOpKey;
        key->system = VIACCESS;
        key->type = Op;
        int temp_key[8];
        int used = -1;
        int temp_prov[3];
        // we can not use the return value here because of %n :(
        if (!sscanf (keystring.c_str (), "V: { %x %x %x %x { %x %x %x %x %x %x %x %x } }%n",
                     &temp_prov[0], &temp_prov[1], &temp_prov[2],
                     &key->nr, temp_key + 0, temp_key + 1, temp_key + 2, temp_key + 3, temp_key + 4, temp_key + 5, temp_key + 6, temp_key + 7, &used))
          {
            sscanf (keystring.c_str (), "V: { %x %x %x %x { %x %x %x %x %x %x %x %x }}%n",
                    &temp_prov[0], &temp_prov[1], &temp_prov[2],
                    &key->nr, temp_key + 0, temp_key + 1, temp_key + 2, temp_key + 3, temp_key + 4, temp_key + 5, temp_key + 6, temp_key + 7, &used);
          }

        key->provider = (((temp_prov[0] << 8) | temp_prov[1]) << 8) | temp_prov[2];

        for (int i = 0; i < 8; ++i)
          key->key[i] = temp_key[i];

        if (used == -1)
          {
            printf ("parse error!\n");
            delete key;
            return 0;
          }
        while (keystring[used] == ' ')
          used++;
        key->comment[1] = keystring.mid (used);
        return key;
      }
    case 'M':                  // Viaccess Masterkey
      {
        eViaccessMasterKey *key = new eViaccessMasterKey;
        key->system = VIACCESS;
        key->type = Master;
        int temp_key[8];
        int temp_prov[4], temp_ppua[5];
        int used = -1;

        // we can not use the return value here because of %n :(
        if (!sscanf (keystring.c_str (), "M: { %x %x %x %x %x %x %x %x %x { %x %x %x %x %x %x %x %x } }%n",
                     &temp_prov[0], &temp_prov[1], &temp_prov[2],
                     &temp_ppua[0], &temp_ppua[1], &temp_ppua[2], &temp_ppua[3], &temp_ppua[4],
                     &key->nr, temp_key + 0, temp_key + 1, temp_key + 2, temp_key + 3, temp_key + 4, temp_key + 5, temp_key + 6, temp_key + 7, &used))
          {
            sscanf (keystring.c_str (), "M: { %x %x %x %x %x %x %x %x %x { %x %x %x %x %x %x %x %x }}%n",
                    &temp_prov[0], &temp_prov[1], &temp_prov[2],
                    &temp_ppua[0], &temp_ppua[1], &temp_ppua[2], &temp_ppua[3], &temp_ppua[4],
                    &key->nr, temp_key + 0, temp_key + 1, temp_key + 2, temp_key + 3, temp_key + 4, temp_key + 5, temp_key + 6, temp_key + 7, &used);
          }

        key->provider = (((temp_prov[0] << 8) | temp_prov[1]) << 8) | temp_prov[2];
//      key->ppua = (((((temp_ppua[0] << 8) | temp_ppua[1]) << 8 ) | temp_ppua[2]) << 8)  | temp_ppua[3];

        for (int i = 0; i < 5; ++i)
          key->ua[i] = temp_ppua[i];

        for (int i = 0; i < 8; ++i)
          key->key[i] = temp_key[i];

        if (used == -1)
          {
            printf ("parse error!\n");
            delete key;
            return 0;
          }
        while (keystring[used] == ' ')
          used++;
        key->comment[1] = keystring.mid (used);
        return key;
      }
    default:
      return 0;
    }
}

eKeyDatabase::eKeyDatabase ()
{
  keys.setAutoDelete (true);
  instance = this;
}

eKeyDatabase::~eKeyDatabase ()
{
  instance = 0;
}

void
eKeyDatabase::loadKey (int system, eString comment, eString key)
{
  eKey *keyp = 0;
  switch (system)
    {
    case eKey::IRDETO:
      keyp = eIrdetoKey::parse (key);
      break;
    case eKey::NAGRAVISION:
      keyp = eNagraKey::parse (key);
      break;
    case eKey::NAGRAVISION_BOXKEY:
      keyp = eNagraBoxKey::parse (key);
      break;
    case eKey::SECA:
      keyp = eSecaKey::parse (key);
      break;
    case eKey::VIACCESS:
      keyp = eViaccessKey::parse (key);
      break;
    case eKey::ECM_SERVER:
      keyp = eECMServerEntry::parse (key);
      break;
    case eKey::LOCAL_ECM_SERVER:
      keyp = eLocalECMServerEntry::parse (key);
      break;
    case eKey::CW_SERVER:
      keyp = eCWServerEntry::parse (key);
      break;
    default:
      eDebug ("unsupported system '%d'", system);
    }
  if (!keyp)
    {
      eDebug ("failed to parse: '%s'", key.c_str ());
      return;
    }
  keyp->comment[0] = comment;
  keyp->modified = 0;
  keys.push_back (keyp);
}

int
eKeyDatabase::loadFile (int ca, const char *filename)
{
  FILE *f = fopen (filename, "rt");
  if (!f)
    return -1;

  eString comment = "";

  while (1)
    {
      char line[256];
      if (!fgets (line, 256, f))
        break;
      if ((*line && *line != '#') || (ca & (eKey::ECM_SERVER | eKey::LOCAL_ECM_SERVER | eKey::CW_SERVER)))
        {
          line[strlen (line) - 1] = 0;
          loadKey (ca, comment, line);
          comment = "";
        }
      else
        comment += line;
    }
  fclose (f);
  return 0;
}

int
eKeyDatabase::saveFile (int ca, const char *filename, const char *mask)
{
  int modified = 0;
  for (ePtrList < eKey >::iterator i (keys.begin ()); i != keys.end (); ++i)
    {
      if (i->system != ca)
        continue;
      if (mask && !strstr (i->getFileText ().c_str (), mask))
        continue;
      if (i->modified)
        setModified (ca);
    }

  if (this->modified.find (ca) != this->modified.end ())
    modified++;

  if (!modified)
    {
      eDebug ("%s didn't change!", filename);
      return 0;
    }

  eDebug ("%d modifications in %s", modified, filename);

  FILE *f = fopen (filename, "wt");
  if (!f)
    return -1;

  for (ePtrList < eKey >::iterator i (keys.begin ()); i != keys.end (); ++i)
    {
      if (i->system != ca)
        continue;
      if (mask && !strstr (i->getFileText ().c_str (), mask))
        continue;
      fputs (i->comment[0].c_str (), f);
      fputs (i->getFileText ().c_str (), f);
      if (i->system != eKey::ECM_SERVER && i->system != eKey::LOCAL_ECM_SERVER && i->system != eKey::CW_SERVER)
        {
          fputs (" ", f);
          fputs (i->comment[1].c_str (), f);
        }
      fputs ("\n", f);
    }

  fclose (f);
  return 0;
}

eKeyDatabase *
  eKeyDatabase::instance;

class eKeyEdit:
public eWindow
{
  eTextInputField *
    comment;
  std::list < eEditableEntry > &entries;
  eString & cmt;
  eButton *
    save;
  void
  do_save ()
  {
    for (std::list < eEditableEntry >::iterator i (entries.begin ()); i != entries.end (); ++i)
      {
        eString number = i->field->getText ();
        if (i->ptr_int)
          sscanf (number.c_str (), "%x", i->ptr_int);
        else if (i->ptr_u8)
          {
            for (int a = 0; a < i->size / 2; ++a)
              {
                int
                  tmp;
                sscanf (number.c_str () + a * 2, "%02x", &tmp);
                i->ptr_u8[a] = tmp;
              }
          }
      }
    cmt = comment->getText ();
    close (0);
  }
public:
  eKeyEdit (eString & cmt, std::list < eEditableEntry > &entries, int system):
  eWindow (),
  entries (entries),
  cmt (cmt)
  {
    int
      x = 150;
    cmove (ePoint (100, 160));
    cresize (eSize (450, 400));
    eString text = "edit ";
    switch (system)
      {
      case eKey::IRDETO:
        text += "irdeto";
        break;
      case eKey::NAGRAVISION:
        text += "nagravision";
        break;
      case eKey::NAGRAVISION_BOXKEY:
        text += "nagravision box";
        break;
      case eKey::SECA:
        text += "seca";
        break;
      case eKey::VIACCESS:
        text += "viaccess";
        break;
      }
    text += " key";
    setText (text);

    save = new eButton (this);
    save->setText (_("save"));
    save->setShortcut ("green");
    save->setShortcutPixmap ("green");

    eLabel *
      l;

    l = new eLabel (this), l->move (ePoint (10, 10));
    l->resize (eSize (x, 30));
    l->setText (_("Comment"));

    comment = new eTextInputField (this);
    comment->move (ePoint (x + 10, 10));
    comment->resize (eSize (clientrect.width () - x - 20, 35));
    comment->loadDeco ();
    comment->setText (cmt);
    comment->setMaxChars (100);

    CONNECT (save->selected, eKeyEdit::do_save);

    eWidget *
      first = 0;

    int
      y = 50;

    for (std::list < eEditableEntry >::iterator i (entries.begin ()); i != entries.end (); ++i)
      {
        i->label = new eLabel (this);
        i->label->setText (i->desc);
        i->label->move (ePoint (10, y));
        i->label->resize (eSize (x, 30));

        i->field = new eTextInputField (this);
        i->field->move (ePoint (x + 10, y));
        i->field->resize (eSize (clientrect.width () - x - 20, 35));
        i->field->setUseableChars ("fedcba9876543210" + 16 - i->base);
        i->field->loadDeco ();

        if (!first)
          first = i->field;

        eString number;
        eString format;
        if (i->ptr_u8)
          for (int a = 0; a < i->size / 2; ++a)
            {
              eString c;
              c.sprintf ("%02x", i->ptr_u8[a]);
              number += c;
            }
        else if (i->ptr_int)
          number.sprintf ("%0*x", i->size, *i->ptr_int);

        i->field->setMaxChars (i->size);
        i->field->setText (number);
        y += 40;
      }

    save->move (ePoint (clientrect.width () - 150, y));
    save->resize (eSize (140, 35));
    save->loadDeco ();

    if (first)
      setFocus (first);

    cresize (eSize (450, y + 45));
  }

};

class eKeyCreator:
public eWindow
{
  int
    ca_system;
  eListBox < eListBoxEntryText > *list;

  void
  selected (eListBoxEntryText * l)
  {
    key = 0;
    if (!l)
      {
        close (-1);
        return;
      }

    const char *
      k = (const char *) l->getKey ();

    switch (ca_system)
      {
      case eKey::IRDETO:
        key = eIrdetoKey::parse (k);
        break;
      case eKey::NAGRAVISION:
        key = eNagraKey::parse (k);
        break;
      case eKey::NAGRAVISION_BOXKEY:
        key = eNagraBoxKey::parse (k);
        break;
      case eKey::SECA:
        key = eSecaKey::parse (k);
        break;
      case eKey::VIACCESS:
        key = eViaccessKey::parse (k);
        break;
      }

    close (0);
  }
  eKey *&
    key;
public:
  eKeyCreator (eKey * &key, int ca_system):
  eWindow (),
  ca_system (ca_system),
  key (key)
  {
    cmove (ePoint (200, 100));
    cresize (eSize (300, 300));
    list = new eListBox < eListBoxEntryText > (this);
    list->move (ePoint (10, 10));
    list->resize (eSize (clientrect.width () - 20, clientrect.height () - 20));
    list->loadDeco ();
    CONNECT (list->selected, eKeyCreator::selected);

    switch (ca_system)
      {
      case eKey::IRDETO:
        new eListBoxEntryText (list, "PK", (void *) "I: { 00 { 00 00 00 00 00 00 00 00 } }");
        new eListBoxEntryText (list, "PMK", (void *) "M: { 00 00 00 { 00 00 00 00 00 00 00 00 } }");
        new eListBoxEntryText (list, "HMK", (void *) "H: { 00 00 00 { 00 00 00 00 00 00 00 00 00 00 } }");
        break;
      case eKey::NAGRAVISION:
        new eListBoxEntryText (list, "OpKey", (void *) "N: { 00 00 00 { 00 00 00 00 00 00 00 00 } }");
        break;
      case eKey::NAGRAVISION_BOXKEY:
        new eListBoxEntryText (list, "BoxKey", (void *) "B: { 00 00 00 00 { 00 00 00 00 00 00 00 00 }}");
        break;
      case eKey::SECA:
        new eListBoxEntryText (list, "OpKey", (void *) "S: { 00 00 00 { 00 00 00 00 00 00 00 00 }}");
        new eListBoxEntryText (list, "MasterKey", (void *) "M: { 00 00 00 00 00 00 00 { 00 00 00 00 00 00 00 00 }}");
        break;
      case eKey::VIACCESS:
        new eListBoxEntryText (list, "OpKey", (void *) "V: { 00 00 00 00 { 00 00 00 00 00 00 00 00 }}");
        new eListBoxEntryText (list, "MasterKey", (void *) "M: { 00 00 00 00 00 00 00 00 00 { 00 00 00 00 00 00 00 00 }}");
        break;
      }
  }
};

class eSystemChooser:
public eListBoxWindow < eListBoxEntryText >
{
  void
  choosedSystem (eListBoxEntryText * system)
  {
    if (!system)
      {
        close (-1);
      }
    else if ((int) system->getKey () == eKey::LOCAL_ECM_SERVER)
      {
        hide ();
        eLocaleServerEditWindow s;
        s.show ();
        s.exec ();
        s.hide ();
        show ();
      }
    else if ((int) system->getKey () == eKey::CW_SERVER)
      {
        hide ();
        eCWServerEditWindow s;
        s.show ();
        s.exec ();
        s.hide ();
        show ();
      }
    else
      {
        hide ();
        eKeyEditor kedit ((int) system->getKey ());
        kedit.show ();
        kedit.exec ();
        kedit.hide ();
        show ();
      }
  }
public:
eSystemChooser ():eListBoxWindow < eListBoxEntryText > ("Scam Edit", 9, 400, true)
  {
    cmove (ePoint (180, 150));
    new eListBoxEntryText (&list, "Remote ECM Server Settings", (void *) eKey::ECM_SERVER, 0, "press ok to open remote ecm server list (cardsharing)");
    new eListBoxEntryText (&list, "Local ECM Server Settings", (void *) eKey::LOCAL_ECM_SERVER, 0, "press ok to open local ecm server setting");
    new eListBoxEntryText (&list, "CW Server Settings", (void *) eKey::CW_SERVER, 0, "press ok to open cw server setting");
    new eListBoxEntrySeparator ((eListBox < eListBoxEntry > *) & list, eSkin::getActive ()->queryImage ("listbox.separator"), 0, true);
    new eListBoxEntryText (&list, "Irdeto / Betacrypt Keys", (void *) eKey::IRDETO, 0, "press ok to open irdeto/betacrypt key editor");
    new eListBoxEntryText (&list, "Seca Keys", (void *) eKey::SECA, 0, "press ok to open seca key editor");
    new eListBoxEntryText (&list, "Nagra Keys", (void *) eKey::NAGRAVISION, 0, "press ok to open nagra key editor");
    new eListBoxEntryText (&list, "Nagra Boxkey", (void *) eKey::NAGRAVISION_BOXKEY, 0, "press ok to open nagra boxkey editor");
    new eListBoxEntryText (&list, "Viaccess Keys", (void *) eKey::VIACCESS, 0, "press ok to open viaccess key editor");
    CONNECT (list.selected, eSystemChooser::choosedSystem);
  }
};

eKeyEditor::eKeyEditor (int system):
eWindow ()
{
  cmove (ePoint (60, 100));
  cresize (eSize (600, 380));
  if (system == eKey::ECM_SERVER)
    setText ("ECM server list");
  else
    setText ("Key list");

  listbox = new eListBox < eKeyEntry > (this);
  listbox->move (ePoint (10, 10));
  listbox->resize (eSize (clientrect.width () - 20, clientrect.height () - 70));
  listbox->loadDeco ();

  eButton *bt;

  bt = new eButton (this);
  bt->move (ePoint (10, clientrect.height () - 50));
  bt->resize (eSize (200, 35));
  bt->loadDeco ();
  bt->setText (_("New"));
  bt->setShortcut ("green");
  bt->setShortcutPixmap ("green");
  CONNECT (bt->selected, eKeyEditor::add);

  bt = new eButton (this);
  bt->move (ePoint (250, clientrect.height () - 50));
  bt->resize (eSize (200, 35));
  bt->loadDeco ();
  bt->setText (_("Remove"));
  bt->setShortcut ("red");
  bt->setShortcutPixmap ("red");
  CONNECT (bt->selected, eKeyEditor::remove);

  CONNECT (listbox->selected, eKeyEditor::selected);

  fillList (eKeyDatabase::getInstance (), current_ca = system);
}

void
eKeyEditor::selected (eKeyEntry * key)
{
  if (!key)
    {
      close (0);
      return;
    }
  hide ();

  if (current_ca == eKey::ECM_SERVER)
    {
      eECMServerEditWindow wnd ((eECMServerEntry *) key->getKey ());
      wnd.show ();
      wnd.exec ();
      wnd.hide ();
      show ();
      return;
    }

  std::list < eEditableEntry > entries;
  key->getKey ()->getEntries (entries);
  if (entries.empty ())
    {
      eMessageBox msg ("Sorry, this key can't be edited.", "sorry");
      msg.show ();
      msg.exec ();
      msg.hide ();
    }
  else
    {
      eKeyEdit ke (key->getKey ()->comment[1], entries, key->getKey ()->system);
      ke.show ();
      if (!ke.exec ())
        key->getKey ()->modified = 1;
      ke.hide ();
    }
  show ();
}

void
eKeyEditor::add ()
{
  eKey *key = 0;

  hide ();

  if (current_ca == eKey::ECM_SERVER)
    {
      key = eECMServerEntry::parse (eString ("#new;test.new.net;1234;secret;xxxx"));
      eECMServerEditWindow wnd ((eECMServerEntry *) key);
      wnd.show ();
      if (wnd.exec ())
        {
          delete key;
          key = 0;
        }
      wnd.hide ();
    }
  else
    {
      std::list < eEditableEntry > entries;
      hide ();
      {
        eKeyCreator kc (key, current_ca);
        kc.show ();
        kc.exec ();
        kc.hide ();
      }
      if (!key)
        {
          show ();
          return;
        }
      key->getEntries (entries);
      eKeyEdit ke (key->comment[1], entries, current_ca);
      ke.show ();
      if (ke.exec ())
        {
          if (key)
            {
              delete key;
              key = 0;
            }
        }
      ke.hide ();
    }
  if (key)
    {
      listbox->beginAtomic ();
      new eKeyEntry (listbox, key);
      listbox->moveSelection (listbox->dirLast);
      listbox->endAtomic ();
      key->modified = 1;
      eKeyDatabase::getInstance ()->keys.push_back (key);
    }
  show ();
}

void
eKeyEditor::remove ()
{
  hide ();
  int res;
  eMessageBox msg (current_ca == eKey::ECM_SERVER ? _("Really remove Server?") : _("Really remove key?"),
                   current_ca == eKey::ECM_SERVER ? _("Remove Server") : _("Remove Key"), eMessageBox::btYes | eMessageBox::btNo);
  msg.show ();
  res = msg.exec ();
  msg.hide ();
  if (res == eMessageBox::btYes)
    {
      eKeyEntry *current = listbox->getCurrent ();
      if (current)
        {
          eKeyDatabase::getInstance ()->setModified (current_ca);
          eKeyDatabase::getInstance ()->keys.remove (current->getKey ());
          listbox->remove (current);
        }
    }
  show ();
}

eKeyEditor::~eKeyEditor ()
{
}

void
eKeyEditor::fillList (eKeyDatabase * db, int ca)
{
  listbox->beginAtomic ();
  listbox->clearList ();
  for (ePtrList < eKey >::iterator i (db->keys.begin ()); i != db->keys.end (); ++i)
    if ((ca == -1) || (i->system == ca))
      new eKeyEntry (listbox, i);
  listbox->endAtomic ();
}

eECMServerEditWindow::eECMServerEditWindow (eECMServerEntry * entry):curEntry (entry), password (entry->password)
{
  cmove (ePoint (120, 105));
  cresize (eSize (500, 395));
  setText ("ECM server edit");

  int
    x = 150;

  eLabel *
    l = new eLabel (this);
  l->move (ePoint (10, 10));
  l->resize (eSize (x, 30));
  l->setText ("Comment");

  comment = new eTextInputField (this);
  comment->move (ePoint (x + 10, 10));
  comment->resize (eSize (clientrect.width () - x - 20, 35));
  comment->loadDeco ();
  comment->setHelpText ("here you can enter any text as comment");
  comment->setText (entry->comment[1]);

  l = new eLabel (this);
  l->move (ePoint (10, 55));
  l->resize (eSize (x, 30));
  l->setText ("Server URL");

  url = new eTextInputField (this);
  url->move (ePoint (x + 10, 55));
  url->resize (eSize (clientrect.width () - x - 105, 35));
  url->loadDeco ();
  url->setUseableChars ("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.1234567890@");
  url->setHelpText ("enter url or ip of the server");
  url->setText (entry->url);

  port = new eTextInputField (this);
  port->move (ePoint (clientrect.width () - 85, 55));
  port->resize (eSize (75, 35));
  port->setHelpText ("enter the port number of the server");
  port->loadDeco ();
  port->setUseableChars ("1234567890");
  port->setMaxChars (5);
  port->setText (entry->port);

  l = new eLabel (this);
  l->move (ePoint (10, 100));
  l->resize (eSize (x, 30));
  l->setText ("Password");

  pass = new eTextInputField (this);
  pass->move (ePoint (x + 10, 100));
  pass->resize (eSize (clientrect.width () - x - 20, 35));
  pass->setHelpText ("enter server password");
  pass->setText ("******");
  pass->loadDeco ();
  CONNECT (pass->selected, eECMServerEditWindow::passwordSelected);

  l = new eLabel (this);
  l->move (ePoint (10, 145));
  l->resize (eSize (x, 30));
  l->setText ("CAIDs");

  caid = new eTextInputField (this);
  caid->move (ePoint (x + 10, 145));
  caid->resize (eSize (65, 35));
  caid->loadDeco ();
  caid->setUseableChars ("x1234567890abcdef");
  caid->setMaxChars (4);
  caid->setHelpText ("enter a new caid here and press the yellow button");

  active = new eCheckbox (this);
  active->move (ePoint (10, 180));
  active->resize (eSize (140, 30));
  active->setHelpText ("set this check to use this server");
  active->loadDeco ();
  active->setText ("use server");
  active->setCheck (entry->activated);

  add = new eButton (this, 0, 0);
  add->move (ePoint (5, 210));
  add->resize (eSize (150, 30));
  add->setText ("add");
  add->setShortcut ("yellow");
  add->setShortcutPixmap ("yellow");
  // add->setAlign(eTextPara::dirLeft);
  CONNECT (add->selected, eECMServerEditWindow::addPressed);

  remove = new eButton (this, 0, 0);
  remove->move (ePoint (5, 245));
  remove->resize (eSize (150, 30));
  remove->setText ("remove");
  remove->setShortcut ("red");
  remove->setShortcutPixmap ("red");
  // remove->setAlign(eTextPara::dirLeft);
  CONNECT (remove->selected, eECMServerEditWindow::removePressed);

  caids = new eListBox < eListBoxEntryText > (this);
  caids->move (ePoint (clientrect.width () - 255, 145));
  caids->resize (eSize (245, 185));
  caids->setColumns (3);
  caids->loadDeco ();
  caids->setFlags (eListBoxBase::flagLostFocusOnFirst | eListBoxBase::flagLostFocusOnLast);
  caids->setHelpText ("red - remove selected entry, left - go to caid input field, right - go to 'use server' field");
  for (std::list < eString >::iterator it (entry->caids.begin ()); it != entry->caids.end (); ++it)
    {
      new eListBoxEntryText (caids, *it);
    }
  caids->sort ();

  save = new eButton (this);
  save->move (ePoint (10, 285));
  save->resize (eSize (200, 40));
  save->setText ("save");
  save->loadDeco ();
  save->setShortcut ("green");
  save->setShortcutPixmap ("green");
  save->setHelpText ("press ok to save changes and close window");
  CONNECT (save->selected, eECMServerEditWindow::savePressed);

  eStatusBar *
    status = new eStatusBar (this);
  status->move (ePoint (0, clientrect.height () - 50));
  status->resize (eSize (clientrect.width (), 50));
  status->loadDeco ();
}

void
eECMServerEditWindow::passwordSelected ()
{
  if (pass->inEditMode ())
    pass->setText (password);
  else
    {
      password = pass->getText ();
      pass->setText ("******");
    }
}

void
eECMServerEditWindow::addPressed ()
{
  eString str = caid->getText ();
  if (str)
    {
      caids->beginAtomic ();
      new eListBoxEntryText (caids, str);
      caids->sort ();
      caids->endAtomic ();
    }
}

void
eECMServerEditWindow::removePressed ()
{
  if (caids->getCurrent ())
    caids->remove (caids->getCurrent ());
}

struct saveCAID:public
  std::unary_function <
eListBoxEntryText &, void >
{
  std::list <
  eString > &
    list;

  saveCAID (std::list < eString > &list):
  list (list)
  {
  }

  bool
  operator  () (eListBoxEntryText & s)
  {
    list.push_back (s.getText ());
    return 0;
  }
};

void
eECMServerEditWindow::savePressed ()
{
  curEntry->caids.clear ();
  caids->forEachEntry (saveCAID (curEntry->caids));
  curEntry->port = port->getText ();
  curEntry->url = url->getText ();
  curEntry->password = password;
  curEntry->comment[1] = comment->getText ();
  curEntry->modified = 1;
  curEntry->activated = active->isChecked ();
  close (0);
}

eECMServerEntry *
eECMServerEntry::parse (eString string)
{
  eECMServerEntry *e = new eECMServerEntry ();
  e->system = ECM_SERVER;

  if (string[0] == '#')
    {
      e->activated = 0;
      string.erase (0, 1);
    }
  else
    e->activated = 1;

// parse comment
  unsigned int pos = string.find (';');
  if (pos != eString::npos)
    e->comment[1] = string.left (pos);

// parse url
  unsigned int oldpos = pos + 1;
  pos = string.find (';', oldpos);
  if (pos != eString::npos)
    e->url = string.mid (oldpos, pos - oldpos);

// parse port
  oldpos = pos + 1;
  pos = string.find (';', oldpos);
  if (pos != eString::npos)
    e->port = string.mid (oldpos, pos - oldpos);

// parse password
  oldpos = pos + 1;
  pos = string.find (';', oldpos);
  if (pos != eString::npos)
    e->password = string.mid (oldpos, pos - oldpos);

// parse all caids
  do
    {
      oldpos = pos + 1;
      pos = string.find (';', oldpos);
      eString str;
      if (pos != eString::npos) // more than one caids left..
        str += string.mid (oldpos, pos - oldpos);
      else
        str += string.mid (oldpos, string.length () - oldpos);
      if (str)
        e->caids.push_back (str);
    }
  while (pos != eString::npos);

  if (e->url && e->password && e->port)
    return e;
  else
    delete e;
  return 0;
}

eLocalECMServerEntry *
eLocalECMServerEntry::parse (eString string)
{
  eLocalECMServerEntry *e = new eLocalECMServerEntry ();
  e->system = eKey::LOCAL_ECM_SERVER;

  if (string[0] == '#')
    {
      e->activated = 0;
      string.erase (0, 1);
    }
  else
    e->activated = 1;

// parse ip / iface..
  unsigned int pos = string.find (';');
  if (pos != eString::npos)
    e->ip = string.left (pos);

// parse port
  unsigned int oldpos = pos + 1;
  pos = string.find (';', oldpos);
  if (pos != eString::npos)
    e->port = string.mid (oldpos, pos - oldpos);

// parse pasword
  oldpos = pos + 1;
  pos = string.find (';', oldpos);
  if (pos != eString::npos)
    e->pass = string.mid (oldpos, pos - oldpos);
  else
    e->pass = string.mid (oldpos, string.length () - oldpos);

  return e;
}

eCWServerEntry *
eCWServerEntry::parse (eString string)
{
  eCWServerEntry *e = new eCWServerEntry ();
  e->system = eKey::CW_SERVER;

  if (string[0] == '#')
    {
      e->activated = 0;
      string.erase (0, 1);
    }
  else
    e->activated = 1;

// parse ip / iface..
  unsigned int pos = string.find (';');
  if (pos != eString::npos)
    e->url = string.left (pos);
  else
    e->url = string.left (string.length ());

  return e;
}


    // thanks to the autoinit-stuff, our keydatabase exists all the time 
    // when the plugin is loaded.
eAutoInitP0 < eKeyDatabase > init_eKeyDatabase (eAutoInitNumbers::dvb + 2, "key database");

int
eKeyEditor::run ()
{
  eSystemChooser sys;
  sys.show ();
  sys.exec ();
  sys.hide ();
  return 0;
}

void
eLocaleServerEditWindow::savePressed ()
{
  curentry->port = port->getText ();
  curentry->pass = password;
  curentry->activated = active->isChecked ();
  curentry->modified = 1;
  close (0);
}

void
eLocaleServerEditWindow::passwordSelected ()
{
  if (pass->inEditMode ())
    pass->setText (password);
  else
    {
      password = pass->getText ();
      pass->setText ("******");
    }
}

eLocaleServerEditWindow::eLocaleServerEditWindow ():curentry (0)
{
  ePtrList < eKey > &keys = eKeyDatabase::getInstance ()->keys;

  for (ePtrList < eKey >::iterator it (keys.begin ()); it != keys.end (); ++it)
    if (it->system == eKey::LOCAL_ECM_SERVER)
      {
        curentry = (eLocalECMServerEntry *) * it;
        break;
      }

  if (!curentry)                //create new ..
    {
      curentry = eLocalECMServerEntry::parse (eString ("#0.0.0.0;1234;Secret"));
      if (curentry)
        keys.push_back (curentry);
    }

  password = curentry->pass;

  cmove (ePoint (135, 140));
  cresize (eSize (450, 260));
  setText ("Local server settings");

  int
    x = 150;

  eLabel *
    l = new eLabel (this);
  l->move (ePoint (10, 10));
  l->resize (eSize (x, 30));
  l->setText ("Password");

  pass = new eTextInputField (this);
  pass->move (ePoint (x + 10, 10));
  pass->resize (eSize (clientrect.width () - x - 20, 35));
  pass->setHelpText ("enter server password");
  pass->setText ("******");
  pass->loadDeco ();
  CONNECT (pass->selected, eLocaleServerEditWindow::passwordSelected);

  l = new eLabel (this);
  l->move (ePoint (10, 55));
  l->resize (eSize (x, 30));
  l->setText ("Server Port");

  port = new eTextInputField (this);
  port->move (ePoint (x + 10, 55));
  port->resize (eSize (75, 35));
  port->setHelpText ("enter the port number of the server");
  port->loadDeco ();
  port->setUseableChars ("1234567890");
  port->setMaxChars (5);
  port->setText (curentry->port);

  active = new eCheckbox (this);
  active->move (ePoint (10, 100));
  active->resize (eSize (250, 30));
  active->setHelpText ("set this check when you will let other people use your Smartcard(s) (Card Sharing)");
  active->loadDeco ();
  active->setText ("enable server");
  active->setCheck (curentry->activated);

  save = new eButton (this);
  save->move (ePoint (10, 150));
  save->resize (eSize (200, 40));
  save->setText ("save");
  save->loadDeco ();
  save->setShortcut ("green");
  save->setShortcutPixmap ("green");
  save->setHelpText ("press ok to save changes and close window");
  CONNECT (save->selected, eLocaleServerEditWindow::savePressed);

  eStatusBar *
    status = new eStatusBar (this);
  status->move (ePoint (0, clientrect.height () - 50));
  status->resize (eSize (clientrect.width (), 50));
  status->loadDeco ();
}


void
eCWServerEditWindow::savePressed ()
{
  curentry->url = url->getText ();
  curentry->activated = active->isChecked ();
  curentry->modified = 1;
  close (0);
}

eCWServerEditWindow::eCWServerEditWindow ():curentry (0)
{
  ePtrList < eKey > &keys = eKeyDatabase::getInstance ()->keys;

  for (ePtrList < eKey >::iterator it (keys.begin ()); it != keys.end (); ++it)
    if (it->system == eKey::CW_SERVER)
      {
        curentry = (eCWServerEntry *) * it;
        break;
      }

  if (!curentry)                //create new ..
    {
      curentry = eCWServerEntry::parse (eString ("#secret.cwserver.org"));
      if (curentry)
        keys.push_back (curentry);
    }

  cmove (ePoint (135, 140));
  cresize (eSize (450, 200));
  setText ("CW server settings");

  int
    x = 150;

  eLabel *
    l = new eLabel (this);
  l->move (ePoint (10, 10));
  l->resize (eSize (x, 30));
  l->setText ("URL");

  url = new eTextInputField (this);
  url->move (ePoint (x + 10, 10));
  url->resize (eSize (clientrect.width () - x - 20, 35));
  url->setText (curentry->url);
  url->setHelpText ("enter server url");
  url->loadDeco ();

  active = new eCheckbox (this);
  active->move (ePoint (10, 55));
  active->resize (eSize (250, 30));
  active->setHelpText ("set this check when you will let other people use your Smartcard(s) (Card Sharing)");
  active->loadDeco ();
  active->setText ("enable server");
  active->setCheck (curentry->activated);

  save = new eButton (this);
  save->move (ePoint (10, 100));
  save->resize (eSize (200, 40));
  save->setText ("save");
  save->loadDeco ();
  save->setShortcut ("green");
  save->setShortcutPixmap ("green");
  save->setHelpText ("press ok to save changes and close window");
  CONNECT (save->selected, eCWServerEditWindow::savePressed);

  eStatusBar *
    status = new eStatusBar (this);
  status->move (ePoint (0, clientrect.height () - 50));
  status->resize (eSize (clientrect.width (), 50));
  status->loadDeco ();
}
