# Debian / x86_64 原生编译
cd kernel
make
sudo insmod hello_jack.ko
sudo insmod Hotmail.ko hotmail_user=user@hotmail.com hotmail_pass=pass
sudo insmod W7.ko

# Android ARM64 交叉编译
# make KDIR=/path/to/android_kernel ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-

# 清理
# make clean

