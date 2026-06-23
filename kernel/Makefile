# JackAndroid Level1 — Kernel Module Makefile
# Author: digson-kirky-CE
# License: BSD-2-Clause

# 交叉编译工具链前缀（根据你的手机架构调整）
CROSS_COMPILE ?= aarch64-linux-gnu-
# 内核源码路径（必须与手机运行内核版本一致）
KDIR ?= /root/kernel

obj-m := hello_jack.o

all:
	@echo "============================================"
	@echo " JackAndroid - by digson-kirky-CE"
	@echo " Level1 Kernel Module Build"
	@echo "============================================"
	@echo ""
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=arm64 CROSS_COMPILE=$(CROSS_COMPILE) modules
	@echo ""
	@echo "============================================"
	@echo " ✅ Build complete!"
	@ls -lh *.ko 2>/dev/null || echo " No .ko generated"
	@echo "============================================"

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=arm64 clean
	@echo " Clean done."

.PHONY: all clean
