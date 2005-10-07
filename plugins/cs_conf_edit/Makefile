CDK_SRC  ?= $(HOME)/tuxbox/dreambox
CDKROOT  ?= /net/tuxbox/dreambox/cdkroot
CROSS     = powerpc-tuxbox-linux-gnu-
CXX       = $(CROSS)g++
STRIP     = $(CROSS)strip
DBG       = -DDEBUG

DEFINES  = $(DBG)
OBJECTS  = cs_edit.o
PLUGIN    = cs_conf_edit

INCLUDES = -I$(CDK_SRC)/apps/tuxbox/enigma/include -I$(CDKROOT)/include -I$(CDKROOT)/include/sigc++-1.2 \
	   -I$(CDKROOT)/lib/sigc++-1.2/include -I$(CDK_SRC)/apps/tuxbox/enigma -I$(CDK_SRC)/apps/misc/libs/libxmltree \
	   -I$(CDK_SRC)/apps/tuxbox/plugins/include -I$(CDK_SRC)/apps/tuxbox/enigma/src -I.

CFLAGS   = $(DEFINES) $(INCLUDES) -fno-rtti -fno-exceptions -Wall -O2 -mcpu=405 -mmultiple -mstring -g -ggdb3 -pipe

all: $(PLUGIN).so

$(PLUGIN).so: $(OBJECTS)
	$(CXX) -shared -Wall -Os -mcpu=405 -mmultiple -mstring -g -ggdb3 -pipe   -o $(PLUGIN).so  $(OBJECTS)
	$(STRIP) --remove-section=.note --remove-section=.comment $(PLUGIN).so

.cpp.o:
	$(CXX) $(CFLAGS) -c $<

clean:
	rm -rf *.so *.o *.ipk

install: all
	cp $(PLUGIN).* $(HOME)/DM7020/ipkg/ronaldd_cs/usr/lib/tuxbox/plugins
	cp $(PLUGIN).* /net/tuxbox/emu_root/lib/tuxbox/plugins
	#ncftpput -u root -p dreambox dm7020 /usr/lib/tuxbox/plugins $(PLUGIN).so $(PLUGIN).cfg
