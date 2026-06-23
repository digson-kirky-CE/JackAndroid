#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fb.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

/* ==================== 帧缓冲绘图函数 ==================== */

static struct fb_info *fbi;

static void draw_pixel(int x, int y, unsigned int color)
{
    if (!fbi || x < 0 || x >= fbi->var.xres || y < 0 || y >= fbi->var.yres)
        return;
    ((unsigned int *)fbi->screen_base)[y * fbi->var.xres + x] = color;
}

static void draw_rect(int x1, int y1, int x2, int y2, unsigned int color)
{
    int x, y;
    for (y = y1; y <= y2; y++)
        for (x = x1; x <= x2; x++)
            draw_pixel(x, y, color);
}

/* 简易 8x16 点阵字符绘制（仅大写字母和数字，用于显示 JackAndroid 等文字） */
static const unsigned char font_8x16[256][8] = {
    /* 此处仅定义需要的字符，完整 ASCII 点阵可嵌入 */
    ['J'] = {0x00, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00},
    ['a'] = {0x00, 0x3C, 0x42, 0x78, 0x44, 0x3A, 0x00, 0x00},
    ['c'] = {0x00, 0x3C, 0x42, 0x40, 0x42, 0x3C, 0x00, 0x00},
    ['k'] = {0x00, 0x42, 0x44, 0x78, 0x44, 0x42, 0x00, 0x00},
    ['n'] = {0x00, 0x42, 0x62, 0x52, 0x4A, 0x46, 0x00, 0x00},
    ['d'] = {0x00, 0x3C, 0x42, 0x42, 0x46, 0x3A, 0x00, 0x00},
    ['r'] = {0x00, 0x42, 0x60, 0x40, 0x42, 0x3C, 0x00, 0x00},
    ['o'] = {0x00, 0x3C, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00},
    ['i'] = {0x00, 0x08, 0x00, 0x38, 0x08, 0x08, 0x3E, 0x00},
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};

static void draw_char(int x, int y, char c, unsigned int color)
{
    int row, col;
    unsigned char ch = (unsigned char)c;
    const unsigned char *glyph = font_8x16[ch];

    if (!glyph) return;

    for (row = 0; row < 8; row++) {
        for (col = 0; col < 8; col++) {
            if (glyph[row] & (1 << (7 - col)))
                draw_pixel(x + col, y + row, color);
        }
    }
}

static void draw_string(int x, int y, const char *str, unsigned int color)
{
    while (*str) {
        draw_char(x, y, *str, color);
        x += 10; /* 字符宽度 + 间距 */
        str++;
    }
}

/* ==================== 刷入画面显示 ==================== */

static void show_flash_screen(void)
{
    int mid_x, mid_y;
    int i;

    if (!fbi) return;

    mid_x = fbi->var.xres / 2;
    mid_y = fbi->var.yres / 2;

    /* 清屏为黑色 */
    draw_rect(0, 0, fbi->var.xres - 1, fbi->var.yres - 1, 0x00000000);
    msleep(200);

    /* 绘制红白条纹背景（波兰国旗样式） */
    draw_rect(0, 0, fbi->var.xres - 1, mid_y - 1, 0xFFFF0000);   /* 上半红色 */
    draw_rect(0, mid_y, fbi->var.xres - 1, fbi->var.yres - 1, 0xFFFFFFFF); /* 下半白色 */
    msleep(300);

    /* 显示黄色大字 "JackAndroid"（居中约 48px 大小） */
    draw_string(mid_x - 160, mid_y - 40, "JackAndroid", 0xFFFFD700);
    msleep(400);

    /* 显示蓝色小字 "Made By Kemiao Kmimage" */
    draw_string(mid_x - 130, mid_y + 15, "Made By Kemiao Kmimage", 0xFF0000FF);
    msleep(600);

    /* 模拟刷入进度闪烁 */
    for (i = 0; i < 3; i++) {
        draw_rect(mid_x - 60, mid_y + 45, mid_x + 60, mid_y + 55, 0xFF00FF00);
        msleep(250);
        draw_rect(mid_x - 60, mid_y + 45, mid_x + 60, mid_y + 55, 0x00000000);
        msleep(150);
    }

    /* 刷入完成，底部红色小字来源信息 */
    draw_string(20, fbi->var.yres - 28, "src:digson-kirky-CE/JackAndroid", 0xFFFF0000);
    msleep(2800);
}

/* ==================== 修改 init 添加 .Mac 服务 ==================== */

static void patch_init_for_mac(void)
{
    struct file *filp;
    loff_t pos = 0;
    mm_segment_t old_fs;
    int ret;

    const char *service_entry =
        "\n# JackAndroid .Mac Service\n"
        "service mac_daemon /system/bin/mac_daemon\n"
        "    class core\n"
        "    user root\n"
        "    group root\n"
        "    oneshot\n"
        "on boot\n"
        "    start mac_daemon\n";

    /* 写入 /system/etc/init/mac_daemon.rc */
    filp = filp_open("/system/etc/init/mac_daemon.rc",
                     O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (IS_ERR(filp)) {
        pr_err("[JackAndroid] Cannot create init rc file (err=%ld)\n",
               PTR_ERR(filp));
        return;
    }

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    ret = filp->f_op->write(filp, service_entry, strlen(service_entry), &pos);
    set_fs(old_fs);
    filp_close(filp, NULL);

    if (ret > 0)
        pr_info("[JackAndroid] .Mac service entry added to init\n");
    else
        pr_err("[JackAndroid] Failed to write init rc file\n");
}

/* ==================== 模块入口/出口 ==================== */

static int __init jack_flash_init(void)
{
    pr_info("[JackAndroid] JackAndroid Level1 starting...\n");

    /* 获取帧缓冲设备 */
    fbi = registered_fb[0];
    if (!fbi) {
        pr_warn("[JackAndroid] No framebuffer device found, skipping display\n");
    }

    /* 显示刷入画面 */
    show_flash_screen();

    /* 修改 init 添加 .Mac 服务 */
    patch_init_for_mac();

    /* 触发重启 */
    pr_info("[JackAndroid] Flash complete! Rebooting in 3 seconds...\n");
    msleep(3000);
    kernel_restart(NULL);

    return 0;
}

static void __exit jack_flash_exit(void)
{
    pr_info("[JackAndroid] Module unloaded.\n");
}

module_init(jack_flash_init);
module_exit(jack_flash_exit);

MODULE_LICENSE("BSD-2-Clause");
MODULE_AUTHOR("Kemiao Kmimage");
MODULE_DESCRIPTION("JackAndroid Level1 - Flash kernel module with boot logo and .Mac init patch");
MODULE_VERSION("1.0");
