CDK_SRC  ?= $(HOME)/tuxbox/dreambox
CDKROOT  ?= /net/tuxbox/dreambox/cdkroot
CROSS     = powerpc-tuxbox-linux-gnu-
CXX       = $(CROSS)g++
STRIP     = $(CROSS)strip
DBG       = -DDEBUG
b        ?= DM7020

ifeq ($(b), DM5600)
BOX = DM5600
DEFINES  = $(DBG) -DSETUP_EMU -DSETUP_CS -DSETUP_GBOX -DDM5600
OBJECTS  = setup_5600.o setup_usb.o setup_main.o setup_cs.o setup_gbox.o setup_rdg.o
PLUGIN    = setup5
endif

ifeq ($(b), DM7000)
BOX = DM7000
# $(DBG) -DSETUP_GBOX
DEFINES  = $(DBG) -DSETUP_EMU -DSETUP_EXTRA -DSETUP_USB -DSETUP_INST -DSETUP_SERVICES -DSETUP_CS -DSETUP_GBOX -DSETUP_RESTORE \
		-DDM7000 -DMOUNT_UNMOUNT #-DSETUP_RDG
OBJECTS  = setup_runapp.o setup_extra.o setup_rc.o setup_services.o setup_emu.o setup_usb.o setup_main.o \
	   setup_install.o setup_cs.o setup_gbox.o setup_rdg.o setup_restore.o
PLUGIN    = setup7
endif

ifeq ($(b), DM7020)
BOX = DM7020
DEFINES  = $(DBG) -DDM7020 -DSETUP_EMU -DSETUP_USB -DSETUP_CS -DSETUP_GBOX -DMOUNT_UNMOUNT -DSETUP_IPKG #-DSETUP_RDG
OBJECTS  = setup_runapp.o setup_extra.o setup_rc.o setup_services.o setup_emu.o setup_usb.o setup_main.o \
	   setup_install.o setup_cs.o setup_gbox.o setup_rdg.o setup_restore.o setup_ipkg.o feeds2dir.o
PLUGIN    = setup7020
PACKAGE = package feeds2dir
F2D_O = feeds2dir.o feeds2dir_main.o
endif

INCLUDES = -I$(CDK_SRC)/apps/tuxbox/enigma/include -I$(CDKROOT)/include -I$(CDKROOT)/include/sigc++-1.2 \
	   -I$(CDKROOT)/lib/sigc++-1.2/include -I$(CDK_SRC)/apps/tuxbox/enigma -I$(CDK_SRC)/apps/misc/libs/libxmltree \
	   -I$(CDK_SRC)/apps/tuxbox/plugins/include -I$(CDK_SRC)/apps/tuxbox/enigma/src -I.

CFLAGS   = $(DEFINES) $(INCLUDES) -fno-rtti -fno-exceptions -Wall -O2 -mcpu=405 -mmultiple -mstring -g -ggdb3 -pipe
#CFLAGS   = $(DEFINES) $(INCLUDES) -Wall -O2 -mcpu=405

feeds2dir: $(F2D_O)
	$(CXX) -Wall -o feeds2dir $(F2D_O)

all: $(PLUGIN).so $(PACKAGE)

$(PLUGIN).so: $(OBJECTS)
	$(CXX) -shared -Wall -Os -mcpu=405 -mmultiple -mstring -g -ggdb3 -pipe   -o $(PLUGIN).so  $(OBJECTS)
	#$(CXX) -shared -Wall -o $(PLUGIN).so $(OBJECTS)
	$(STRIP) --remove-section=.note --remove-section=.comment $(PLUGIN).so

package:
	rm -rf tmp_root
	mkdir -p tmp_root/usr/lib/tuxbox/plugins
	mkdir -p tmp_root/CONTROL
	mkdir -p tmp_root/usr/bin
	cp $(PLUGIN).* tmp_root/usr/lib/tuxbox/plugins
	cp files/control tmp_root/CONTROL
	cp files/postinst tmp_root/CONTROL
	cp files/mk_emulist.sh tmp_root/usr/bin
	cp files/reset_emu.sh tmp_root/usr/bin
	cp files/mk_ronaldd_image.sh tmp_root/usr/bin
	ipkg-build -o 0 -g 0 tmp_root

.cpp.o:
	$(CXX) $(CFLAGS) -c $<

clean:
	rm -rf *.so *.o *.ipk

install: all
	cp setup*.cfg setup*.so /net/tuxbox/plugins/
ifeq ($(BOX),DM7000)
	cp setup7.so setup7.cfg /net/tuxbox/tftpboot/image/lib/tuxbox/plugins/
	cp setup7.so setup7.cfg /net/tuxbox/emu_root$(KERNEL)/lib/tuxbox/plugins/
endif
ifeq ($(BOX),DM7020)
	ncftpput -u root -p dreambox dm7020 /usr/lib/tuxbox/plugins $(PLUGIN).so $(PLUGIN).cfg
	#ncftpput -u root -p dreambox dm7020 /tmp setup*.ipk
	cp setup*ipk /local/web/dreambox/root/ipkg/feed
	rm -f $(PLUGIN).zip
	zip $(PLUGIN).zip $(PLUGIN).so $(PLUGIN).cfg
endif
ifeq ($(BOX),DM5600)
	cp setup5.so setup5.cfg /net/tuxbox/emu_dm5600_root/lib/tuxbox/plugins/
	cp setup5.so setup5.cfg /net/tuxbox/dm5600/tmp/matrix_031127/lib/tuxbox/plugins/
endif
