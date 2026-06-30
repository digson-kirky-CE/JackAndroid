
# 通用 libag.so
gcc -shared -fPIC -Wall -Wl,-soname,libag.so -o libag.so lib/libag.c

# LX2LS 特化版（仅 LineShine 需要）
aarch64-linux-gnu-gcc -shared -fPIC -O3 -march=armv9-a+sve2 \
    -Wl,-soname,libag.so -o libag_lx2ls.so lib/lx2ls/libag_lx2ls.c

# 插件
gcc -O2 -static -o 86box_plugin plugins/86box.c
gcc -O2 -static -o bcachefs_helper plugins/bcachefs_helper.c
gcc -O2 -static -o glm_speak plugins/glm_speak.c -ldl

# 转二进制对象
ld -r -b binary -o libag_embedded.o libag.so
ld -r -b binary -o libag_lx2ls_embedded.o libag_lx2ls.so
ld -r -b binary -o 86box_embedded.o 86box_plugin
ld -r -b binary -o bcachefs_embedded.o bcachefs_helper
ld -r -b binary -o glm_speak_embedded.o glm_speak
# AG_main.c 是主源码（前面给的 AG.c 改名为 AG_main.c）
make -C $KDIR M=$(pwd) ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE modules

