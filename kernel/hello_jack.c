#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fb.h>
#include <linux/delay.h>
#include <linux/sched.h>

/* 简易像素绘图函数 */
static void draw_pixel(struct fb_info *fbi, int x, int y, unsigned int color)
{
    if (x >= 0 && x < fbi->var.xres && y >= 0 && y < fbi->var.yres)
        ((unsigned int *)fbi->screen_base)[y * fbi->var.xres + x] = color;
}

static void draw_rect(struct fb_info *fbi, int x1, int y1, int x2, int y2, unsigned int color)
{
    int x, y;
    for (y = y1; y <= y2; y++)
        for (x = x1; x <= x2; x++)
            draw_pixel(fbi, x, y, color);
}

/* 简易字符绘制（8x16 像素）*/
static void draw_char(struct fb_info *fbi, int x, int y, char c, unsigned int color)
{
    /* 此处为简化版，实际需嵌入完整 ASCII 点阵字库 */
    /* 生产代码应包含 font_8x16 数组 */
    printk(KERN_DEBUG "[JackAndroid] Drawing char '%c' at (%d,%d)\n", c, x, y);
}

static void draw_string(struct fb_info *fbi, int x, int y, const char *str, unsigned int color)
{
    while (*str) {
        draw_char(fbi, x, y, *str, color);
        x += 10; /* 字符宽度 + 间距 */
        str++;
    }
}

static int __init jack_flash_init(void)
{
    struct fb_info *fbi;
    int mid_x, mid_y;
    int i;

    fbi = registered_fb[0];
    if (!fbi) {
        pr_err("[JackAndroid] No framebuffer!\n");
        return -ENODEV;
    }

    mid_x = fbi->var.xres / 2;
    mid_y = fbi->var.yres / 2;

    /* === 第一步：清屏为黑色 === */
    draw_rect(fbi, 0, 0, fbi->var.xres - 1, fbi->var.yres - 1, 0x00000000);
    msleep(500);

    /* === 第二步：绘制红白条纹背景（波兰国旗样式） === */
    /* 上半部分红色 */
    draw_rect(fbi, 0, 0, fbi->var.xres - 1, mid_y - 1, 0xFFFF0000);
    /* 下半部分白色 */
    draw_rect(fbi, 0, mid_y, fbi->var.xres - 1, fbi->var.yres - 1, 0xFFFFFFFF);
    msleep(300);

    /* === 第三步：显示黄色大字 "JackAndroid" === */
    /* 居中显示，字号约为 48px（此处用近似位置） */
    draw_string(fbi, mid_x - 180, mid_y - 60, "JackAndroid", 0xFFFFD700); /* 金黄色 */
    msleep(500);

    /* === 第四步：显示蓝色小字 "Made By Kemiao Kmimage" === */
    draw_string(fbi, mid_x - 120, mid_y + 30, "Made By Kemiao Kmimage", 0xFF0000FF);
    msleep(800);

    /* === 第五步：模拟刷入进度（闪烁效果） === */
    for (i = 0; i < 3; i++) {
        draw_rect(fbi, mid_x - 50, mid_y + 70, mid_x + 50, mid_y + 80, 0xFF00FF00);
        msleep(200);
        draw_rect(fbi, mid_x - 50, mid_y + 70, mid_x + 50, mid_y + 80, 0x00000000);
        msleep(200);
    }

    /* === 第六步：刷入完成，显示红色来源信息 === */
    /* 屏幕底部红色小字：src:digson-kirky-CE/JackAndroid */
    draw_string(fbi, 20, fbi->var.yres - 25, "src:digson-kirky-CE/JackAndroid", 0xFFFF0000);
    msleep(3000); /* 停留 3 秒后重启 */

    /* === 第七步：触发重启 === */
    pr_info("[JackAndroid] Flash complete! Rebooting in 3 seconds...\n");
    msleep(1000);
    
    /* 调用内核重启函数 */
    kernel_restart(NULL);

    return 0;
}

static void __exit jack_flash_exit(void)
{
    pr_info("[JackAndroid] Module exited.\n");
}

module_init(jack_flash_init);
module_exit(jack_flash_exit);

MODULE_LICENSE("BSD-2-Clause");
MODULE_AUTHOR("Kemiao Kmimage");
MODULE_DESCRIPTION("JackAndroid Level1 flash kernel module with boot animation");
MODULE_VERSION("1.0");
