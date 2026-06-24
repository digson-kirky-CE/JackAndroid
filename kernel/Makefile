KDIR ?= /path/to/your/kernel/source
obj-m := Hotmail.o

all:
	@echo "============================================"
	@echo " JackAndroid - by digson-kirky-CE"
	@echo " Building Hotmail.ko + hotmail_bin"
	@echo "============================================"
	# 编译内核模块
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=arm64 CROSS_COMPILE=$(CROSS_COMPILE) modules
	# 编译用户态二进制
	$(CROSS_COMPILE)gcc -static -O2 -o hotmail_bin hotmail_bin.c
	@ls -lh Hotmail.ko hotmail_bin

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f hotmail_bin

