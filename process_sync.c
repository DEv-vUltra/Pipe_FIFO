#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];
    int N = 3;  // số tiến trình con
    pid_t pid;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    for (int i = 0; i < N; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {  // tiến trình con
            // close(pipefd[1]);  // đóng đầu ghi, chỉ đọc
            char buf;
            read(pipefd[0], &buf, 1);  // chặn cho đến khi nhận token
            printf("Process con %d bat dau chay.\n", i + 1);
            // giả lập công việc
            sleep(1);
            printf("Process con %d ket thuc.\n", i + 1  );

            // trả token cho tiến trình tiếp theo
            write(pipefd[1], "x", 1);
            
            close(pipefd[1]);
            close(pipefd[0]);
            exit(0);
        }
        // tiến trình cha tiếp tục tạo con
    }

    // Cha: gửi token đầu tiên để tiến trình con đầu tiên chạy
    write(pipefd[1], "x", 1);

    // chờ tất cả tiến trình con
    for (int i = 0; i < N; i++) {
        wait(NULL);
    }

    close(pipefd[1]);
    return 0;
}
