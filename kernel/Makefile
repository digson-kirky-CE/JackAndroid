# JackAndroid Kernel Modules Makefile — Level1
# Targets:
#   hello_jack.ko — JackAndroid startup identifier
#   Hotmail.ko    — Hotmail account stub (Android)
#   W7.ko         — JAD-7 WiFi 7 shim (non-WiFi7 platforms)
#   MA-debian.ko  — Monster Audio MH Series driver stub (Debian only)
#
# Usage:
#   Debian x86_64/ARM64: make
#   Android ARM64 cross: make KDIR=/path/to/android_kernel ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
#   Alpine i586 non-PAE: make KDIR=/path/to/alpine_i586_kernel ARCH=i386 CROSS_COMPILE=i586-alpine-linux-musl-

KDIR ?= /lib/modules/$(shell uname -r)/build
ARCH ?= $(shell uname -m | sed 's/x86_64/amd64/;s/aarch64/arm64/')
CROSS_COMPILE ?=

obj-m := hello_jack.o
obj-m += Hotmail.o
obj-m += W7.o
obj-m += MA-debian.o

all:
	@echo "============================================"
	@echo " JackAndroid - by digson-kirky-CE"
	@echo " Building Level1 kernel modules for $(ARCH)"
	@echo " Targets: hello_jack Hotmail W7 MA-debian"
	@echo "============================================"
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules
	@echo ""
	@echo " Built modules:"
	@ls -lh *.ko 2>/dev/null || echo " (none)"
	@echo "============================================"

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f *.o *.ko *.mod.c Module.symvers modules.order

.PHONY: all clean

