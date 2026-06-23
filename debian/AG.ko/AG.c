#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/workqueue.h>
#include <linux/kmod.h>

static struct work_struct ag_work;

static void ag_install_work(struct work_struct *work)
{
    char *envp[] = {"HOME=/root", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL};
    char *argv[] = {"/bin/bash", "/usr/.ja/lib/install_libs.sh", NULL};
    int ret;

    pr_info("[JackAndroid] JackAndroid AeroGrap Install\n");

    ret = call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
    if (ret < 0)
        pr_err("[JackAndroid] Failed to integrate lib (err=%d)\n", ret);
    else
        pr_info("[JackAndroid] Lib integrated to /usr/.ja/lib\n");
}

static int __init ag_init(void)
{
    pr_info("[JackAndroid] JackAndroid AeroGrap Install\n");

    INIT_WORK(&ag_work, ag_install_work);
    schedule_work(&ag_work);

    return 0;
}

static void __exit ag_exit(void)
{
    pr_info("[JackAndroid] AG.ko unloaded\n");
}

module_init(ag_init);
module_exit(ag_exit);

MODULE_LICENSE("BSD-2-Clause");
MODULE_AUTHOR("Kemiao Kmimage");
MODULE_DESCRIPTION("JackAndroid AeroGlass Kernel Module — Debian");
MODULE_VERSION("1.0");

