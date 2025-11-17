#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define MAIN_FIFO "/tmp/main_fifo"

int main() {
    int id;
    printf("Nhập Client ID: ");
    scanf("%d", &id);
    getchar(); // bỏ newline

    // FIFO riêng để server trả lời
    char client_fifo[64];
    sprintf(client_fifo, "/tmp/client_%d_fifo", id);
    mkfifo(client_fifo, 0666);

    // mở fifo chung để gửi yêu cầu
    int mainfd = open(MAIN_FIFO, O_WRONLY);

    // gửi yêu cầu
    char msg[128];
    printf("Nhập tin nhắn gửi server: ");
    fgets(msg, sizeof(msg), stdin);

    char fullmsg[256];
    sprintf(fullmsg, "%d:%s", id, msg);
    write(mainfd, fullmsg, strlen(fullmsg));
    close(mainfd);

    // server phản hồi
    int cfd = open(client_fifo, O_RDONLY);
    char buffer[256];
    read(cfd, buffer, sizeof(buffer));

    printf("Client %d nhận: %s\n", id, buffer);

    close(cfd);
    unlink(client_fifo);
    return 0;
}
