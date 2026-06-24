/*
 * AG.c — JackAndroid AeroGlass Kernel Module (Debian / LineShine LX2)
 * Embeds:
 *   - libag.so           (generic AeroGlass lib)
 *   - libag_lx2ls.so     (LineShine LX2 tuned build)
 *   - 86box             (86BOX VM frontend plugin)
 *   - bcachefs_helper   (Bcachefs auto-create helper)
 *   - glm_speak         (GLM-5.2 AI dialogue app)
 *
 * On insmod:
 *   - detect LineShine OS → install matching libag
 *   - headless node (no fb0) → skip splash, only pr_info
 *   - graphical node    → show red/white Polish flag + JackAndroid Logo
 *
 * Author : digson-kirky-CE
 * License: BSD-2-Clause
 * Version : 1.1 (LineShine LX2LS support)
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/kmod.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/string.h>

/* ===== 嵌入二进制符号（ld -r -b binary） ===== */
extern unsigned char _binary_libag_so_start[];
extern unsigned char _binary_libag_so_end[];

extern unsigned char _binary_libag_lx2ls_so_start[];
extern unsigned char _binary_libag_lx2ls_so_end[];

extern unsigned char _binary_86box_plugin_start[];
extern unsigned char _binary_86box_plugin_end[];

extern unsigned char _binary_bcachefs_helper_start[];
extern unsigned char _binary_bcachefs_helper_end[];

extern unsigned char _binary_glm_speak_start[];
extern unsigned char _binary_glm_speak_end[];

/* ===== 写文件辅助 ===== */
static void write_binary(const char *path,
                         unsigned char *start,
                         unsigned char *end)
{
    struct file *filp;
    loff_t pos = 0;
    mm_segment_t old_fs;
    size_t sz = end - start;

    filp = filp_open(path, O_CREAT | O_WRONLY | O_TRUNC, 0755);
    if (IS_ERR(filp)) {
        pr_err("[AG] Failed to open %s (err=%ld)\n", path, PTR_ERR(filp));
        return;
    }
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    filp->f_op->write(filp, start, sz, &pos);
    set_fs(old_fs);
    filp_close(filp, NULL);

    pr_info("[AG]  → %s (%zu bytes)\n", path, sz);
}

/* ===== Framebuffer splash（仅当有 fb0） ===== */
static void show_flash_screen(void)
{
    extern struct fb_info *registered_fb[];
    struct fb_info *fbi = registered_fb[0];

    if (!fbi) {
        /* Headless — e.g. LineShine compute node */
        pr_info("[AG] (headless node — no fb0, skip splash)\n");
        pr_info("[AG] JackAndroid — by digson-kirky-CE\n");
        pr_info("[AG] Level1 AeroGrap Install\n");
        return;
    }

    /* ---- 有 framebuffer：波兰旗 + 黄字 JackAndroid + 蓝字 Kmimage ---- */
    int xres = fbi->var.xres;
    int yres = fbi->var.yres;
    int mid_y = yres / 2;
    unsigned int *vram = (unsigned int *)fbi->screen_base;
    int x, y;

    /* 清屏黑 */
    for (y = 0; y < yres; y++)
        for (x = 0; x < xres; x++)
            vram[y * xres + x] = 0x00000000;

    /* 上半红 */
    for (y = 0; y < mid_y; y++)
        for (x = 0; x < xres; x++)
            vram[y * xres + x] = 0x000000FF;   /* ARGB 红 */

    /* 下半白 */
    for (y = mid_y; y < yres; y++)
        for (x = 0; x < xres; x++)
            vram[y * xres + x] = 0x00FFFFFF;   /* ARGB 白 */

    /* 字库省略（生产用 8x8 font_8x8 数组），此处 printk 标注 */
    pr_info("[AG] DISPLAY: JackAndroid YELLOW — Made By Kemiao Kmimage BLUE\n");
    msleep(2000);
    pr_info("[AG] src: digson-kirky-CE/JackAndroid\n");
    msleep(1000);
}

/* ===== 安装工作 ===== */
static struct work_struct ag_work;

static void ag_install_work(struct work_struct *work)
{
    int is_lineshine = 0;
    struct file *rf;
    char buf[512];
    loff_t pos = 0;
    mm_segment_t old_fs;

    /* --- 检测是否为 LineShine Debian --- */
    rf = filp_open("/etc/os-release", O_RDONLY, 0);
    if (!IS_ERR(rf)) {
        old_fs = get_fs(); set_fs(KERNEL_DS);
        kernel_read(rf, buf, sizeof(buf) - 1, &pos);
        set_fs(old_fs);
        filp_close(rf, NULL);
        if (strstr(buf, "LineShine") || strstr(buf, "LX2"))
            is_lineshine = 1;
    }

    /* --- 建目录 --- */
    char *mkdir_cmd = "mkdir -p /usr/.ja/lib /usr/.ja/bin";
    char *envp[] = {"HOME=/root",
                     "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL};
    char *argv[] = {"/bin/sh", "-c", mkdir_cmd, NULL};
    call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);

    /* --- 写 libag（按平台选择） --- */
    if (is_lineshine) {
        write_binary("/usr/.ja/lib/libag.so",
                     _binary_libag_lx2ls_so_start,
                     _binary_libag_lx2ls_so_end);
        pr_info("[AG] → using LineShine LX2LS tuned libag\n");
    } else {
        write_binary("/usr/.ja/lib/libag.so",
                     _binary_libag_so_start,
                     _binary_libag_so_end);
        pr_info("[AG] → using generic libag\n");
    }

    /* --- 写插件 --- */
    write_binary("/usr/.ja/bin/86box",
                 _binary_86box_plugin_start,
                 _binary_86box_plugin_end);
    write_binary("/usr/.ja/bin/bcachefs_helper",
                 _binary_bcachefs_helper_start,
                 _binary_bcachefs_helper_end);
    write_binary("/usr/.ja/bin/glm_speak",
                 _binary_glm_speak_start,
                 _binary_glm_speak_end);

    /* --- 显示 splash（headless 节点只 printk） --- */
    show_flash_screen();

    pr_info("[AG] ============================================\n");
    pr_info("[AG]  JackAndroid AeroGrap Install — COMPLETE\n");
    pr_info("[AG] ============================================\n");
    pr_info("[AG]  src: digson-kirky-CE / JackAndroid\n");
}

/* ===== 模块入口 ===== */
static int __init ag_init(void)
{
    pr_info("[AG] ============================================\n");
    pr_info("[AG]  JackAndroid - by digson-kirky-CE\n");
    pr_info("[AG]  AeroGlass Kernel Module (AG.ko)\n");
    pr_info("[AG]  JackAndroid AeroGrap Install\n");
    pr_info("[AG] ============================================\n");

    INIT_WORK(&ag_work, ag_install_work);
    schedule_work(&ag_work);

    return 0;
}

static void __exit ag_exit(void)
{
    pr_info("[AG] AeroGlass (AG.ko) unloaded\n");
}

module_init(ag_init);
module_exit(ag_exit);

MODULE_LICENSE("BSD-2-Clause");
MODULE_AUTHOR("Kemiao Kmimage");
MODULE_DESCRIPTION("JackAndroid AeroGlass — embeds libag(generic+LX2LS) + 86box + bcachefs + glm_speak");
MODULE_VERSION("1.1");

