/*
 * AG.c — JackAndroid AeroGlass Kernel Module (Debian)
 * Embeds:
 *   - libag.so        → /usr/.ja/lib/libag.so
 *   - 86box           → /usr/.ja/bin/86box
 *   - bcachefs_helper → /usr/.ja/bin/bcachefs_helper
 *   - glm_speak       → /usr/.ja/bin/glm_speak
 *
 * On insmod: create dirs → write binaries → printk banner
 *
 * Author : digson-kirky-CE
 * License: BSD-2-Clause
 * Version: 1.0
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

/* ===== 嵌入二进制符号（由 ld -r -b binary 生成）===== */
extern unsigned char _binary_libag_so_start[];
extern unsigned char _binary_libag_so_end[];

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

/* ===== 安装工作 ===== */
static struct work_struct ag_work;

static void ag_install_work(struct work_struct *work)
{
    char *envp[] = {"HOME=/root",
                    "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL};
    char *argv[] = {"/bin/sh", "-c",
                     "mkdir -p /usr/.ja/lib /usr/.ja/bin", NULL};

    /* 建目录 */
    call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);

    /* 写二进制 */
    write_binary("/usr/.ja/lib/libag.so",
                 _binary_libag_so_start, _binary_libag_so_end);
    write_binary("/usr/.ja/bin/86box",
                 _binary_86box_plugin_start, _binary_86box_plugin_end);
    write_binary("/usr/.ja/bin/bcachefs_helper",
                 _binary_bcachefs_helper_start, _binary_bcachefs_helper_end);
    write_binary("/usr/.ja/bin/glm_speak",
                 _binary_glm_speak_start, _binary_glm_speak_end);

    pr_info("[AG] ============================================\n");
    pr_info("[AG]  JackAndroid AeroGrap Install — COMPLETE\n");
    pr_info("[AG]  libag.so  → /usr/.ja/lib/\n");
    pr_info("[AG]  86box    → /usr/.ja/bin/\n");
    pr_info("[AG]  bcachefs → /usr/.ja/bin/\n");
    pr_info("[AG]  glm_speak→ /usr/.ja/bin/\n");
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
MODULE_DESCRIPTION("JackAndroid AeroGlass — embeds libag.so + 86box + bcachefs_helper + glm_speak");
MODULE_VERSION("1.0");

