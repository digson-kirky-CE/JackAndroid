KDIR ?= /lib/modules/$(shell uname -r)/build
obj-m   := AG.o
AG-objs := AG_main.o \
           libag_embedded.o \
           libag_lx2ls_embedded.o \
           86box_embedded.o \
           bcachefs_embedded.o \
           glm_speak_embedded.o


all:
	@echo "============================================"
	@echo " JackAndroid - by digson-kirky-CE"
	@echo " Building AG.ko (libag + 86box + bcachefs + glm_speak)"
	@echo "============================================"
	bash buildmake.sh

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f libag.so 86box_plugin bcachefs_helper glm_speak \
	      libag_embedded.o 86box_embedded.o \
	      bcachefs_embedded.o glm_speak_embedded.o

