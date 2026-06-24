KDIR ?= /lib/modules/$(shell uname -r)/build
obj-m := AG.o
AG-objs := AG_main.o libag_embedded.o 86box_embedded.o \
           bcachefs_embedded.o glm_speak_embedded.o

all:
	rm AG.o
	@echo "============================================"
	@echo " JackAndroid - by digson-kirky-CE"
	@echo " Building AG.ko (libag + 86box + bcachefs + glm_speak)"
	@echo "============================================"
	gcc -shared -fPIC -Wall -Wl,-soname,libag.so -o libag.so lib/libag.c
	gcc -O2 -static -o 86box_plugin plugins/86box.c
	gcc -O2 -static -o bcachefs_helper plugins/bcachefs_helper.c
	gcc -O2 -static -o glm_speak plugins/glm_speak.c -ldl
	ld -r -b binary -o libag_embedded.o libag.so
	ld -r -b binary -o 86box_embedded.o 86box_plugin
	ld -r -b binary -o bcachefs_embedded.o bcachefs_helper
	ld -r -b binary -o glm_speak_embedded.o glm_speak
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	@ls -lh AG.ko

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f libag.so 86box_plugin bcachefs_helper glm_speak \
	      libag_embedded.o 86box_embedded.o \
	      bcachefs_embedded.o glm_speak_embedded.o

