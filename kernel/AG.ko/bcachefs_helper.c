/*
 * bcachefs_helper — Bcachefs Support Helper for JackAndroid
 * Embedded inside AG.ko, extracted to /usr/.ja/bin/bcachefs_helper
 * Checks Bcachefs support; if missing, creates Bcachefs on spare disk
 *
 * Author: digson-kirky-CE
 * License: BSD-2-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define BCFS_MODULE "bcachefs"
#define SPARE_DISK  "/dev/sdb"      /* 备用硬盘设备 */
#define MOUNT_POINT "/mnt/bcachefs"

static int check_bcachefs_support(void)
{
    FILE *f;
    char line[256];
    int found = 0;

    /* 检查 /proc/filesystems 是否包含 bcachefs */
    f = fopen("/proc/filesystems", "r");
    if (!f) return 0;

    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, "bcachefs")) {
            found = 1;
            break;
        }
    }
    fclose(f);

    /* 也检查内核模块是否加载 */
    if (!found) {
        f = fopen("/proc/modules", "r");
        if (f) {
            while (fgets(line, sizeof(line), f)) {
                if (strstr(line, "bcachefs")) {
                    found = 1;
                    break;
                }
            }
            fclose(f);
        }
    }

    return found;
}

static int create_bcachefs_partition(void)
{
    char cmd[512];
    int ret;

    printf("[bcachefs] Bcachefs not supported on current system.\n");
    printf("[bcachefs] Attempting to create Bcachefs on %s...\n", SPARE_DISK);

    /* 检查备用硬盘是否存在 */
    struct stat st;
    if (stat(SPARE_DISK, &st) != 0) {
        printf("[bcachefs] Error: %s not found. Please attach a spare disk.\n", SPARE_DISK);
        return -1;
    }

    /* 创建挂载点 */
    mkdir(MOUNT_POINT, 0755);

    /* 格式化分区为 Bcachefs */
    snprintf(cmd, sizeof(cmd),
             "mkfs.bcachefs %s", SPARE_DISK);
    ret = system(cmd);
    if (ret != 0) {
        printf("[bcachefs] Failed to format %s as Bcachefs\n", SPARE_DISK);
        return -1;
    }

    /* 挂载 */
    snprintf(cmd, sizeof(cmd),
             "mount -t bcachefs %s %s", SPARE_DISK, MOUNT_POINT);
    ret = system(cmd);
    if (ret != 0) {
        printf("[bcachefs] Failed to mount Bcachefs at %s\n", MOUNT_POINT);
        return -1;
    }

    printf("[bcachefs] Bcachefs created and mounted at %s\n", MOUNT_POINT);
    return 0;
}

int main(void)
{
    printf("JackAndroid Bcachefs Support Helper\n");
    printf(" by digson-kirky-CE\n");
    printf(" src: digson-kirky-CE/JackAndroid\n\n");

    if (check_bcachefs_support()) {
        printf("[bcachefs] Bcachefs is supported on this system.\n");
        printf("[bcachefs] Ready for file manager development.\n");
        return 0;
    }

    printf("[bcachefs] Bcachefs not detected.\n");
    return create_bcachefs_partition();
}

