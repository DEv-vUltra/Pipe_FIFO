#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/select.h>

#define MAIN_FIFO "/tmp/main_fifo"

int main() {
    int fd;
    char buffer[256];

    // Tạo FIFO chung
    mkfifo(MAIN_FIFO, 0666);

    // Mở FIFO ở dạng NON-BLOCKING
    fd = open(MAIN_FIFO, O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("open main fifo");
        exit(1);
    }

    printf("Server non-blocking đang chạy...\n");

    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);

        struct timeval tv;
        tv.tv_sec = 1;     // timeout 1 giây
        tv.tv_usec = 0;

        // select() sẽ không block toàn bộ server
        int ret = select(fd + 1, &readfds, NULL, NULL, &tv);

        if (ret < 0) {
            perror("select");
            continue;
        } 
        else if (ret == 0) {
            // Timeout → không có dữ liệu → làm việc khác
            // printf("Server: không có request...\n");
            continue;
        }

        if (FD_ISSET(fd, &readfds)) {
            memset(buffer, 0, sizeof(buffer));
            int n = read(fd, buffer, sizeof(buffer));

            if (n > 0) {
                printf("Server nhận: %s\n", buffer);

                // Parse message: "ID: message"
                int id;
                char msg[200];
                sscanf(buffer, "%d:%199[^\n]", &id, msg);

                // FIFO riêng của client
                char client_fifo[64];
                sprintf(client_fifo, "/tmp/client_%d_fifo", id);

                // Server gửi phản hồi
                int cfd = open(client_fifo, O_WRONLY | O_NONBLOCK);
                if (cfd >= 0) {
                    char reply[256];
                    sprintf(reply, "Server trả lời client %d: %s", id, msg);
                    write(cfd, reply, strlen(reply));
                    close(cfd);
                } else {
                    printf("Không mở được FIFO của client %d\n", id);
                }
            } else {
                // Do NON-BLOCKING, đọc 0 byte nghĩa là không ai đang giữ FIFO
                // ta cần reopen để tránh EOF "ảo"
                close(fd);
                fd = open(MAIN_FIFO, O_RDONLY | O_NONBLOCK);
            }
        }
    }

    close(fd);
    unlink(MAIN_FIFO);
    return 0;
}
