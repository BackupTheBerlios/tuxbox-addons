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

extern char EMU[MAX_EMU][256];
extern char *SCRIPTS[MAX_SCRIPTS];
extern char *Executable;
extern char RUN_MESSAGE[128];
extern rc_config *RC;

#endif
