/*
 * JackAndroid .Mac Daemon
 * 由 init 在 boot 阶段启动
 * 提供 .Mac 风格服务（邮件/云盘/状态栏）
 * Author: digson-kirky-CE
 * License: BSD-2-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

#define DAEMON_PORT 16888  /* .Mac 风格端口 */
#define BUF_SIZE 4096
#define LOG_TAG "[JackMac]"

/* .Mac 风格命令协议 */
typedef enum {
    CMD_MAIL_CHECK,     /* 检查邮件 */
    CMD_IDISK_LIST,     /* 列出 iDisk 文件 */
    CMD_HOMEPAGE_GET,   /* 获取 HomePage */
    CMD_STATUS_SET,     /* 设置在线状态 */
    CMD_SYNC            /* 同步通讯录 */
} MacCommand;

/* 客户端连接处理 */
static void handle_client(int client_fd) {
    char buf[BUF_SIZE];
    char response[BUF_SIZE];
    time_t now;
    struct tm *tm_info;

    memset(buf, 0, sizeof(buf));
    recv(client_fd, buf, sizeof(buf) - 1, 0);

    time(&now);
    tm_info = localtime(&now);

    /* .Mac 风格响应头 */
    snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Server: JackMac/1.0 (dotMac Style)\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "X-Mac-Style: aqua\r\n"
        "\r\n"
        "<html><body style='font-family: Lucida Grande, sans-serif; background: linear-gradient(#E8F0FE, #FFFFFF);'>"
        "<h1 style='color: #007AFF;'>✦ Jack.Mac ✦</h1>"
        "<p>Welcome to JackAndroid .Mac Services</p>"
        "<p>Time: %s</p>"
        "<p>Request: %s</p>"
        "<p style='color: #999; font-size: 10px;'>src: digson-kirky-CE/JackAndroid</p>"
        "</body></html>\r\n",
        asctime(tm_info), buf);

    send(client_fd, response, strlen(response), 0);
    close(client_fd);
}

/* 守护进程主循环 */
static void run_daemon() {
    int server_fd, client_fd;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    /* 创建 .Mac 风格 socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        fprintf(stderr, "%s Failed to create socket\n", LOG_TAG);
        return;
    }

    /* SO_REUSEADDR 允许快速重启 */
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(DAEMON_PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "%s Bind failed on port %d\n", LOG_TAG, DAEMON_PORT);
        close(server_fd);
        return;
    }

    listen(server_fd, 5);
    printf("%s JackMac daemon started on port %d\n", LOG_TAG, DAEMON_PORT);

    /* 忽略 SIGCHLD 防止僵尸进程 */
    signal(SIGCHLD, SIG_IGN);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&addr, &addr_len);
        if (client_fd < 0) continue;

        /* 每个客户端 fork 子进程处理（UNIX 风格） */
        if (fork() == 0) {
            close(server_fd);
            handle_client(client_fd);
            exit(0);
        }
        close(client_fd);
    }
}

int main(int argc, char *argv[]) {
    printf("%s JackAndroid .Mac Service Starting...\n", LOG_TAG);
    printf("%s Made By Kemiao Kmimage\n", LOG_TAG);
    printf("%s src: digson-kirky-CE/JackAndroid\n", LOG_TAG);

    /* 进入后台运行 */
    if (daemon(0, 0) < 0) {
        fprintf(stderr, "%s daemon() failed\n", LOG_TAG);
        return 1;
    }

    run_daemon();
    return 0;
}
