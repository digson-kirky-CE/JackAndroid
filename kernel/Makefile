# JackAndroid Kernel Modules Makefile
# Targets: hello_jack.ko, Hotmail.ko, W7.ko
# Usage:
#   Android: make KDIR=/path/to/android_kernel ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
#   Debian:  make (uses current kernel headers)

KDIR ?= /lib/modules/$(shell uname -r)/build
ARCH ?= $(shell uname -m | sed 's/x86_64/amd64/;s/aarch64/arm64/')
CROSS_COMPILE ?=

obj-m := hello_jack.o
obj-m += Hotmail.o
obj-m += W7.o

all:
	@echo "============================================"
	@echo " JackAndroid - by digson-kirky-CE"
	@echo " Building kernel modules for $(ARCH)"
	@echo "============================================"
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules
	@echo ""
	@echo " Built modules:"
	@ls -lh *.ko 2>/dev/null || echo " (none)"

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f *.o *.ko *.mod.c Module.symvers modules.order

.PHONY: all clean

