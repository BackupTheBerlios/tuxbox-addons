#ifndef __ucas_h
#define __ucas_h

class eLabel;
class eCheckbox;
class eTextInputField;

struct eEditableEntry
{
  __u8 *ptr_u8;
  unsigned int *ptr_int;

  int base;
  int size;

    std::string desc;

  eLabel *label;
  eTextInputField *field;

    eEditableEntry (__u8 * ptr_u8, int base, int size, const char *desc):ptr_u8 (ptr_u8), ptr_int (0), base (base), size (size), desc (desc)
  {
  }

  eEditableEntry (unsigned int *ptr_int, int base, int size, const char *desc):ptr_u8 (0), ptr_int (ptr_int), base (base), size (size), desc (desc)
  {
  }
};

struct eKey
{
  enum
  {
    IRDETO = 1, SECA = 2, NAGRAVISION = 4,
    NAGRAVISION_BOXKEY = 8, VIACCESS = 16, CONAX = 32,
    ECM_SERVER = 64, LOCAL_ECM_SERVER = 128, CW_SERVER = 256
  };

  int system;
  int modified;
  eString comment[2];

  virtual eString getText () = 0;
  virtual eString getFileText () = 0;
    virtual ~ eKey ()
  {
  }

  virtual void getEntries (std::list < eEditableEntry > &entries)
  {
  }
};

struct eECMServerEntry:eKey
{
  eString url, port, password;
  bool activated;
    std::list < eString > caids;
  static eECMServerEntry *parse (eString data);
  eString getText ()
  {
    eString ret = url;
      ret += ' ';
      ret += port;
      ret += ' ';
      ret += "*******";
      ret += ' ';
    for (std::list < eString >::iterator it (caids.begin ()); it != caids.end ();)
      {
        ret += *it;
        ++it;
        if (it != caids.end ())
          ret += ' ';
      }
    return ret;
  }
  eString getFileText ()
  {
    eString ret = activated ? "" : "#";
    ret += comment[1];
    ret += ';';
    ret += url;
    ret += ';';
    ret += port;
    ret += ';';
    ret += password;
    ret += ';';
    for (std::list < eString >::iterator it (caids.begin ()); it != caids.end ();)
      {
        ret += *it;
        ++it;
        if (it != caids.end ())
          ret += ';';
      }
    return ret;
  }
};

struct eLocalECMServerEntry:eKey
{
  eString ip, pass, port;
  bool activated;
  static eLocalECMServerEntry *parse (eString data);
  eString getText ()
  {
    return "";
  }
  eString getFileText ()
  {
    eString ret = activated ? "" : "#";
    ret += ip;
    ret += ';';
    ret += port;
    ret += ';';
    ret += pass;
    return ret;
  }
};

struct eCWServerEntry:eKey
{
  eString url;
  bool activated;
  static eCWServerEntry *parse (eString data);
  eString getText ()
  {
    return "";
  }
  eString getFileText ()
  {
    eString ret = activated ? "" : "#";
    ret += url;
    return ret;
  }
};

struct eIrdetoKey:eKey
{
  enum
  { PK, PMK, HMK };
  int type;
  static eIrdetoKey *parse (eString key);
};

struct eIrdetoPK:eIrdetoKey
{
  unsigned int keyid;
  __u8 key[8];
  eString getText ()
  {
    return eString ().sprintf ("PK, keyid=%02x {%02x %02x %02x %02x %02x %02x %02x %02x}",
                               keyid, key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]);
  }
  eString getFileText ()
  {
    return eString ().sprintf ("I: { %02x { %02x %02x %02x %02x %02x %02x %02x %02x } }",
                               keyid, key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]);
  }
  virtual void getEntries (std::list < eEditableEntry > &entries)
  {
    entries.push_back (eEditableEntry (&keyid, 16, 1, "Keyid"));
    entries.push_back (eEditableEntry (key, 16, 16, "PK"));
  }
};

struct eIrdetoPMK:eIrdetoKey
{
  unsigned int providerid;
  __u8 key[8];
  eString getText ()
  {
    return eString ().sprintf ("PMK, provid=%06x {%02x %02x %02x %02x %02x %02x %02x %02x}",
                               providerid, key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]);
  }
  eString getFileText ()
  {
    return eString ().sprintf ("M: { %02x %02x %02x { %02x %02x %02x %02x %02x %02x %02x %02x } }",
                               (providerid >> 16) & 0xFF, (providerid >> 8) & 0xFF, providerid & 0xFF,
                               key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]);
  }
  virtual void getEntries (std::list < eEditableEntry > &entries)
  {
    entries.push_back (eEditableEntry (&providerid, 16, 6, "ProviderID"));
    entries.push_back (eEditableEntry (key, 16, 16, "PMK"));
  }
};

struct eIrdetoHMK:public eIrdetoKey
{
  unsigned int hexserial;
  __u8 key[10];
  eString getText ()
  {
    return eString ().sprintf ("HMK, serial=%06x {%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x}",
                               hexserial, key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7], key[8], key[9]);
  }
  eString getFileText ()
  {
    return eString ().sprintf ("H: { %02x %02x %02x { %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x } }",
                               (hexserial >> 16) & 0xFF, (hexserial >> 8) & 0xFF, hexserial & 0xFF,
                               key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7], key[8], key[9]);
  }
  virtual void getEntries (std::list < eEditableEntry > &entries)
  {
    entries.push_back (eEditableEntry (&hexserial, 16, 6, "Hexserial"));
    entries.push_back (eEditableEntry (key, 16, 20, "HMK"));
  }
};

struct eNagraBoxKey:eKey
{
  unsigned int serial;
  static eNagraBoxKey *parse (eString key);
  __u8 key[8];
  eString getText ()
  {
    return eString ().sprintf ("BoxKey Serial %08x {%02x %02x %02x %02x %02x %02x %02x %02x}",
                               serial, key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]);
  }
  eString getFileText ()
  {
    return eString ().sprintf ("B: { %02x %02x %02x %02x { %02x %02x %02x %02x %02x %02x %02x %02x } }",
                               serial >> 24, (serial >> 16) & 0xFF, (serial >> 8) & 0xFF, serial & 0xFF,
                               key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]);
  }
  virtual void getEntries (std::list < eEditableEntry > &entries)
  {
    entries.push_back (eEditableEntry (&serial, 16, 8, "Serial"));
    entries.push_back (eEditableEntry (key, 16, 16, "Key"));
  }
};

struct eNagraKey:eKey
{
  enum
  { Op, AutoE1, AutoN1, AutoN2, AutoVer };
  int type;
  unsigned int sysid;
  static eNagraKey *parse (eString key);
};

struct eNagraAutorollRSA:public eNagraKey
{
  __u8 key[64];
  eString getText ()
  {
    return eString ().sprintf ("sysid=%04x %s RSA {%02x %02x %02x %02x [...] %02x %02x %02x %02x}",
                               sysid, (type == AutoE1) ? "E1" : (type == AutoN1) ? "N1" :
                               (type == AutoN2) ? "N2" : "??", key[0], key[1], key[2], key[3], key[60], key[61], key[62], key[63]);
  }
  eString getFileText ()
  {
    return "";
  }
  virtual void getEntries (std::list < eEditableEntry > &entries)
  {
  }
};

struct eNagraVerifyKey:public eNagraKey
{
  __u8 key[8];
  eString getText ()
  {
    return eString ().sprintf ("sysid=%04x V {%02x %02x %02x %02x %02x %02x %02x %02x}", sysid, key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]);
  }
  eString getFileText ()
  {
    return "";
  }
  virtual void getEntries (std::list < eEditableEntry > &entries)
  {
    entries.push_back (eEditableEntry (&sysid, 16, 4, "SystemID"));
    entries.push_back (eEditableEntry (key, 16, 16, "VerifyKey"));
  }
};

struct eNagraOpKey:public eNagraKey
{
  unsigned int nr;
  __u8 key[8];
  eString getText ()
  {
    return eString ().sprintf ("Nagra sysid=%04x %02x {%02x %02x %02x %02x %02x %02x %02x %02x}",
                               sysid, nr, key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]);
  }
  eString getFileText ()
  {
    return eString ().sprintf ("N: { %02x %02x %02x { %02x %02x %02x %02x %02x %02x %02x %02x } }",
                               sysid >> 8, sysid & 0xFF, nr, key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]);
  }
  virtual void getEntries (std::list < eEditableEntry > &entries)
  {
    entries.push_back (eEditableEntry (&sysid, 16, 4, "SystemID"));
    entries.push_back (eEditableEntry (&nr, 16, 1, "KeyNr"));
    entries.push_back (eEditableEntry (key, 16, 16, "OpKey"));
  }
};

struct eSecaKey:public eKey
{
  enum
  { Master, Op };
  int type;
  unsigned int provider;
  static eSecaKey *parse (eString key);
};

struct eSecaMasterKey:public eSecaKey
{
  unsigned int ppua;
  unsigned int nr;
  __u8 key[8];
  eString getText ()
  {
    return eString ().sprintf ("provider=%04x ppua=%08x %x {%02x %02x %02x %02x %02x %02x %02x %02x}",
                               provider, ppua, nr, key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]);
  }
  eString getFileText ()
  {
    return eString ().sprintf ("M: { %02x %02x %02x %02x %02x %02x %02x { %02x %02x %02x %02x %02x %02x %02x %02x } }",
                               provider >> 8, provider & 0xFF,
                               (ppua >> 24) & 0xFF, (ppua >> 16) & 0xFF,
                               (ppua >> 8) & 0xFF, (ppua) & 0xFF, nr, key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]);
  }
  virtual void getEntries (std::list < eEditableEntry > &entries)
  {
    entries.push_back (eEditableEntry (&provider, 16, 4, "SystemID"));
    entries.push_back (eEditableEntry (&ppua, 16, 8, "PPUA"));
    entries.push_back (eEditableEntry (key, 16, 16, "MasterKey"));
  }
};

struct eSecaOpKey:public eSecaKey
{
  unsigned int nr;
  __u8 key[8];
  eString getText ()
  {
    return eString ().sprintf ("provider=%04x %x {%02x %02x %02x %02x %02x %02x %02x %02x}",
                               provider, nr, key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]);
  }
  eString getFileText ()
  {
    return eString ().sprintf ("S: { %02x %02x %02x { %02x %02x %02x %02x %02x %02x %02x %02x } }",
                               provider >> 8, provider & 0xFF, nr, key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]);
  }
  virtual void getEntries (std::list < eEditableEntry > &entries)
  {
    entries.push_back (eEditableEntry (&provider, 16, 4, "SystemID"));
    entries.push_back (eEditableEntry (&nr, 16, 1, "KeyNr"));
    entries.push_back (eEditableEntry (key, 16, 16, "OpKey"));
  }
};

struct eViaccessKey:public eKey
{
  enum
  { Master, Op };
  int type;
  unsigned int provider;
  static eViaccessKey *parse (eString key);
};

struct eViaccessMasterKey:public eViaccessKey
{
//  unsigned int ppua;
  __u8 ua[5];
  unsigned int nr;
  __u8 key[8];
  eString getText ()
  {
    return eString ().sprintf ("provider=%06x ua=%02x%02x%02x%02x%02x %x {%02x %02x %02x %02x %02x %02x %02x %02x}",
                               provider, ua[0], ua[1], ua[2], ua[3], ua[4], nr, key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]);
  }
  eString getFileText ()
  {
    return eString ().sprintf ("M: { %02x %02x %02x %02x %02x %02x %02x %02x %02x { %02x %02x %02x %02x %02x %02x %02x %02x } }",
                               provider >> 16, (provider >> 8) & 0xFF, provider & 0xFF,
//          (ppua >> 24) & 0xFF, (ppua >> 16) & 0xFF, 
//          (ppua >> 8) & 0xFF, (ppua) & 0xFF, 
                               ua[0], ua[1], ua[2], ua[3], ua[4], nr, key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]);
  }
  virtual void getEntries (std::list < eEditableEntry > &entries)
  {
    entries.push_back (eEditableEntry (&provider, 16, 6, "SystemID"));
    entries.push_back (eEditableEntry (ua, 16, 10, "UA"));
    entries.push_back (eEditableEntry (key, 16, 16, "MasterKey"));
  }
};

struct eViaccessOpKey:public eViaccessKey
{
  unsigned int nr;
  __u8 key[8];
  eString getText ()
  {
    return eString ().sprintf ("provider=%06x %x {%02x %02x %02x %02x %02x %02x %02x %02x}",
                               provider, nr, key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]);
  }
  eString getFileText ()
  {
    return eString ().sprintf ("V: { %02x %02x %02x %02x { %02x %02x %02x %02x %02x %02x %02x %02x } }",
                               provider >> 16, (provider >> 8) & 0xFF, provider & 0xFF, nr, key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]);
  }
  virtual void getEntries (std::list < eEditableEntry > &entries)
  {
    entries.push_back (eEditableEntry (&provider, 16, 6, "SystemID"));
    entries.push_back (eEditableEntry (&nr, 16, 1, "KeyNr"));
    entries.push_back (eEditableEntry (key, 16, 16, "OpKey"));
  }
};

class eKeyEntry:public eListBoxEntry
{
  friend class eListBox < eKeyEntry >;

  eString description;

  static gFont fontDesc, fontKey;
  int yOffs;
  eKey *key;
public:
    eKeyEntry (eListBox < eKeyEntry > *lb, eKey * key):eListBoxEntry ((eListBox < eListBoxEntry > *)lb), key (key)
  {
    if (!fontDesc.pointSize)
      fontDesc = eSkin::getActive ()->queryFont ("eListBox.EntryText.normal");
    if (!fontKey.pointSize)
      {
        fontKey = eSkin::getActive ()->queryFont ("epg.description");
        fontKey.pointSize += 4;
      }
  }
  bool operator== (const eKey * o_key) const
  {
    return o_key == key;
  }
  eKey *getKey ()
  {
    return key;
  }
protected:
  static int getEntryHeight ()
  {
    return 50;
  }

  const eString & redraw (gPainter * rc, const eRect & rect, gColor coActiveB, gColor coActiveF, gColor coNormalB, gColor coNormalF, int state)
  {
    drawEntryRect (rc, rect, coActiveB, coActiveF, coNormalB, coNormalF, state);
    eRect descr, keyr;

    descr = keyr = rect;
    descr.setHeight (30);
    keyr.setY (keyr.top () + 25);
    keyr.setHeight (20);

    rc->setFont (fontKey);
    rc->renderText (keyr, key->getText ());

    rc->setFont (fontDesc);
    rc->renderText (descr, key->comment[1]);

    return description;
  }
};

class eKeyDatabase
{
  static eKeyDatabase *instance;
public:
    ePtrList < eKey > keys;
    std::set < int >modified;
  static eKeyDatabase *getInstance ()
  {
    return instance;
  }
  eKeyDatabase ();
  ~eKeyDatabase ();
  void loadKey (int ca, eString, eString);
  int loadFile (int ca, const char *filename);

  int saveFile (int ca, const char *filename, const char *mask = 0);

  void setModified (int ca_system)
  {
    modified.insert (ca_system);
  }
};

class eKeyEditor:public eWindow
{
  eListBox < eKeyEntry > *listbox;

  int current_ca;
  void fillList (eKeyDatabase * db, int ca);
  void selected (eKeyEntry * key);
  void add ();
  void remove ();
public:
    eKeyEditor (int system);
   ~eKeyEditor ();
  static int run ();
};

class eECMServerEditWindow:public eWindow
{
  eECMServerEntry *curEntry;
  eTextInputField *comment, *url, *port, *pass, *caid;
    eListBox < eListBoxEntryText > *caids;
  eButton *add, *remove, *save;
  eCheckbox *active;
  eString password;
  void addPressed ();
  void removePressed ();
  void savePressed ();
  void passwordSelected ();
public:
    eECMServerEditWindow (eECMServerEntry *);
};

class eLocaleServerEditWindow:public eWindow
{
  eLocalECMServerEntry *curentry;
  eTextInputField *pass, *port;
  eCheckbox *active;
  eButton *save;
  eString password;
  void savePressed ();
  void passwordSelected ();
public:
    eLocaleServerEditWindow ();
};

class eCWServerEditWindow:public eWindow
{
  eCWServerEntry *curentry;
  eTextInputField *url;
  eCheckbox *active;
  eButton *save;
  void savePressed ();
public:
    eCWServerEditWindow ();
};

#endif
