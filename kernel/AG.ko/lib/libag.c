/*
 * libag.c — JackAndroid AeroGlass Support Library
 * Compiled into libag.so, installed to /usr/.ja/lib by AG.ko
 *
 * Author : digson-kirky-CE
 * License: BSD-2-Clause
 * Version: 1.0
 */

#include <stdio.h>

#define AG_TAG  "[libag] JackAndroid AeroGlass 1.0 (by digson-kirky-CE)"

__attribute__((visibility("default")))
int ag_init(void)
{
    fprintf(stderr, AG_TAG " loaded\n");
    return 0;
}

__attribute__((visibility("default")))
const char *ag_version(void)
{
    return "JackAndroid AeroGlass 1.0 (by digson-kirky-CE)";
}

__attribute__((visibility("default")))
int ag_probe(void)
{
    /* 返回非 0 表示 libag 存在 */
    return 1;
}

/* 编译：
   gcc -shared -fPIC -Wall -Wl,-soname,libag.so \
       -o libag.so libag.c
*/

