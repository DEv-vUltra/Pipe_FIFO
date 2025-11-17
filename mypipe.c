#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

/*Tái tạo toán tử  "|" trong Shell bằng pipe

Chương trình chính (tiến trình cha) sẽ tạo ra một pipe và sau đó tạo ra hai tiến trình con.

1. Tiến trình con thứ nhất sẽ thực thi lệnh ls -l.
2. Tiến trình con thứ hai sẽ thực thi lệnh wc -l.

Dữ liệu phải chảy trực tiếp từ con thứ nhất sang con thứ hai thông qua pipe mà
không hiển thị ra màn hình của tiến trình cha.
 */

int main(){
    int pipefd[2]; // pipefd[0] là đầu đọc, pipefd[1] là đầu ghi 
    pid_t pid1, pid2;

    if(pipe(pipefd) == -1){
        perror("pipe");
        exit(1);
    }
    // Tạo tiến trình con 1
    pid1 = fork();
    if(pid1 < 0){
        perror("fork1 failed");
        exit(1);
    }

    if(pid1 == 0){
        
        close(pipefd[0]); // Đóng đầu đọc

        dup2(pipefd[1], STDOUT_FILENO); // Chuyển hướng stdout của nó vào đầu ghi của pipe

        close(pipefd[1]); // Đóng đầu ghi

        execlp("ls", "ls", "-l" , NULL); // Thực thi lệnh ls -l

    }

    // Tạo tiến trình con 2 
    pid2 = fork();
    if (pid2 < 0){
        perror("fork2 failed");
        exit(1);
    }

    if(pid2 == 0){

        close(pipefd[1]); // Đóng đầu ghi

        dup2(pipefd[0], STDIN_FILENO); //Chuyển hướng stdin của nó vào đầu đọc của pipe

        close(pipefd[0]); // Đóng đầu đọc
        
        execlp("wc", "wc", "-l", NULL); // Thực thi lệnh wc -l

    }
    /*Đóng cả 2 đầu pipe để các tiến trình con biết khi nào dữ liệu kết thúc*/
    close(pipefd[0]);
    close(pipefd[1]);

    /*Gọi 2 lần wait để đợi cả 2 tiến trình con hoàn thành */
    wait(NULL);
    wait(NULL);
}