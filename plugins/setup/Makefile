CDK_SRC ?= $(HOME)/tuxbox/dreambox
CDKROOT ?= /net/tuxbox/dreambox/cdkroot
CROSS    = powerpc-tuxbox-linux-gnu-
CXX      = $(CROSS)g++
STRIP    = $(CROSS)strip
DBG      = -DDEBUG

ifeq ($b,5)
BOX = DM5600
DEFINES  = $(DBG) -DSETUP_EMU -DSETUP_CS -DSETUP_GBOX -DDM5600
OBJECTS  = setup_5600.o setup_usb.o setup_main.o setup_cs.o setup_gbox.o setup_rdg.o
else
BOX = DM7000
DEFINES  = $(DBG) -DSETUP_EMU -DSETUP_EXTRA -DSETUP_USB -DSETUP_INST -DSETUP_SERVICES -DSETUP_GBOX -DSETUP_CS -DSETUP_RESTORE \
		-DDM7000 -DMOUNT_UNMOUNT #-DSETUP_RDG
OBJECTS  = setup_runapp.o setup_extra.o setup_rc.o setup_services.o setup_emu.o setup_usb.o setup_main.o \
	   setup_install.o setup_cs.o setup_gbox.o setup_rdg.o setup_restore.o
endif

INCLUDES = -I$(CDK_SRC)/apps/tuxbox/enigma/include -I$(CDKROOT)/include -I$(CDKROOT)/include/sigc++-1.2 \
	   -I$(CDKROOT)/lib/sigc++-1.2/include -I$(CDK_SRC)/apps/tuxbox/enigma -I$(CDK_SRC)/apps/misc/libs/libxmltree \
	   -I$(CDK_SRC)/apps/tuxbox/plugins/include -I$(CDK_SRC)/apps/tuxbox/enigma/src -I.

CFLAGS   = $(DEFINES) $(INCLUDES) -fno-rtti -fno-exceptions -Wall -O2 -mcpu=405 -msoft-float -mmultiple -mstring -g -ggdb3 -pipe


ifeq ($(BOX),DM7000)
all: setup7.so
else
all: setup5.so
endif

setup7.so: .7 $(OBJECTS)
	$(CXX) -shared -Wall -Os -mcpu=405 -msoft-float -mmultiple -mstring -g -ggdb3 -pipe   -o setup7.so  $(OBJECTS)
	$(STRIP) --remove-section=.note --remove-section=.comment setup7.so

setup5.so: .5 $(OBJECTS)
	$(CXX) -shared -Wall -Os -mcpu=405 -msoft-float -mmultiple -mstring -g -ggdb3 -pipe   -o setup5.so  $(OBJECTS)
	$(STRIP) --remove-section=.note --remove-section=.comment setup5.so

.7:
	$(MAKE) clean
	rm -f .5
	touch .7

.5:
	$(MAKE) clean
	rm -f .7
	touch .5

.cpp.o:
	$(CXX) $(CFLAGS) -c $<

clean:
	rm -rf *.so *.o

install: all
	cp setup?.cfg setup?.so /net/tuxbox/plugins/
ifeq ($(BOX),DM7000)
	cp setup7.so setup7.cfg /net/tuxbox/tftpboot/image/lib/tuxbox/plugins/
	cp setup7.so setup7.cfg /net/tuxbox/emu_root$(KERNEL)/lib/tuxbox/plugins/
else
	cp setup5.so setup5.cfg /net/tuxbox/emu_dm5600_root/lib/tuxbox/plugins/
	cp setup5.so setup5.cfg /net/tuxbox/dm5600/tmp/matrix_031127/lib/tuxbox/plugins/
endif
