/*
 * JackAndroid .Mac Daemon — iOS 越狱版
 * 由 launchd 启动（com.jackandroid.macdaemon.plist）
 * Author: digson-kirky-CE
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>

#define PORT 16888

static void handle_client(int fd) {
    char buf[1024], resp[2048];
    time_t now = time(NULL);
    recv(fd, buf, sizeof(buf)-1, 0);
    snprintf(resp, sizeof(resp),
        "HTTP/1.1 200 OK\r\n"
        "Server: JackMac/iOS\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<h1>✦ Jack.Mac (iOS)</h1>"
        "<p>Welcome to JackAndroid .Mac Service on XNU</p>"
        "<p>%s</p>"
        "<p style='color:#999;font-size:10px'>src:digson-kirky-CE/JackAndroid</p>",
        ctime(&now));
    send(fd, resp, strlen(resp), 0);
    close(fd);
}

int main(int argc, char *argv[]) {
    int srv, cli;
    struct sockaddr_in sa;
    printf("[JackMac/iOS] .Mac daemon starting...\n");
    printf("[JackMac/iOS] Made By Kemiao Kmimage\n");

    srv = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(PORT);
    bind(srv, (struct sockaddr*)&sa, sizeof(sa));
    listen(srv, 5);
    printf("[JackMac/iOS] Listening on port %d\n", PORT);

    signal(SIGCHLD, SIG_IGN);
    while ((cli = accept(srv, NULL, NULL)) >= 0) {
        if (fork() == 0) { close(srv); handle_client(cli); exit(0); }
        close(cli);
    }
    return 0;
}
