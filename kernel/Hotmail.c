/*
 * Hotmail.ko — JackAndroid .Mac Hotmail Plugin (Android)
 * On insmod: shutdown → display "Hotmail for .Mac" → copy binary to /system/.Mac/bin/
 *
 * Author: digson-kirky-CE
 * License: BSD-2-Clause
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fb.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/workqueue.h>
#include <linux/kmod.h>

static struct fb_info *fbi;

/* 帧缓冲绘图 */
static void draw_pixel(int x, int y, unsigned int c)
{
    if (!fbi || x < 0 || y < 0 || x >= fbi->var.xres || y >= fbi->var.yres)
        return;
    ((unsigned int *)fbi->screen_base)[y * fbi->var.xres + x] = c;
}

static void draw_rect(int x1, int y1, int x2, int y2, unsigned int c)
{
    int x, y;
    for (y = y1; y <= y2; y++)
        for (x = x1; x <= x2; x++)
            draw_pixel(x, y, c);
}

/* 简易字符绘制（8x8 点阵简化版） */
static void draw_string(int x, int y, const char *s, unsigned int c)
{
    /* 此处省略完整字库，实际应包含 ASCII 8x8 点阵 */
    /* 生产代码应包含 font_8x8 数组 */
    pr_info("[Hotmail] DISPLAY: %s\n", s);
}

static void show_hotmail_screen(void)
{
    int mx, my;

    if (!fbi) return;

    mx = fbi->var.xres / 2;
    my = fbi->var.yres / 2;

    /* 清屏为黑色 */
    draw_rect(0, 0, fbi->var.xres - 1, fbi->var.yres - 1, 0x00000000);
    msleep(300);

    /* 黄色大字 "Hotmail for .Mac" */
    draw_string(mx - 160, my - 20, "Hotmail for .Mac", 0xFFFFD700);
    msleep(2000);
}

static struct work_struct hotmail_work;

static void hotmail_install_work(struct work_struct *work)
{
    char *envp[] = {"HOME=/root", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL};
    char *argv[] = {"/bin/sh", "-c", NULL, NULL};
    char cmd[256];
    int ret;

    /* 显示画面 */
    show_hotmail_screen();

    /* 创建 /system/.Mac/bin/ 并复制二进制 */
    snprintf(cmd, sizeof(cmd),
             "mkdir -p /system/.Mac/bin && "
             "cp /data/local/tmp/hotmail_bin /system/.Mac/bin/hotmail && "
             "chmod 755 /system/.Mac/bin/hotmail");

    argv[2] = cmd;
    ret = call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
    if (ret < 0)
        pr_err("[Hotmail] Failed to install binary (err=%d)\n", ret);
    else
        pr_info("[Hotmail] Binary installed to /system/.Mac/bin/hotmail\n");

    /* 触发重启 */
    pr_info("[Hotmail] Install complete. Rebooting...\n");
    msleep(3000);
    kernel_restart(NULL);
}

static int __init hotmail_init(void)
{
    fbi = registered_fb[0];

    pr_info("[Hotmail] ============================================\n");
    pr_info("[Hotmail] JackAndroid - by digson-kirky-CE\n");
    pr_info("[Hotmail] Hotmail for .Mac\n");
    pr_info("[Hotmail] ============================================\n");

    INIT_WORK(&hotmail_work, hotmail_install_work);
    schedule_work(&hotmail_work);

    return 0;
}

static void __exit hotmail_exit(void)
{
    pr_info("[Hotmail] Module unloaded\n");
}

module_init(hotmail_init);
module_exit(hotmail_exit);

MODULE_LICENSE("BSD-2-Clause");
MODULE_AUTHOR("Kemiao Kmimage");
MODULE_DESCRIPTION("JackAndroid .Mac Hotmail Plugin — shutdown + display + install");
MODULE_VERSION("1.0");

