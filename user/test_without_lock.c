#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    if (argc < 2){
        fprintf(2, "error: count of arguments uncorrect\n");
        exit(-1);
    }
    int pipe1[2], pipe2[2];
    if (pipe(pipe1) < 0){
        fprintf(2, "error: pipe failed\n");
        exit(-1);
    }
    if (pipe(pipe2) < 0){
        fprintf(2, "error: pipe failed\n");
        exit(-1);
    }
    int mutex = create_mutex();
    if (mutex < 0){
        fprintf(2, "error: mutex failed\n");
        exit(-1);
    }
    int pid = fork();
    if (pid < 0){
        fprintf(2, "error: fork failed\n");
        exit(-1);
    }
    if (pid == 0){
        close(pipe1[1]); close(pipe2[0]);
        char sym; int n; int my_pid = getpid();
        while ((n = read(pipe1[0], &sym, 1)) > 0){
            printf("%d recieved: %c\n", my_pid, sym);
            write(pipe2[1], &sym, 1);
        }
        close(pipe2[1]); close(pipe1[0]);
        free_mutex(mutex);
        exit(0);
    }
    close(pipe1[0]); close(pipe2[1]);
    write(pipe1[1], argv[1], strlen(argv[1]));
    close(pipe1[1]);
    char sym; int n;
    while ((n = read(pipe2[0], &sym, 1)) > 0){
        printf("%d recieved: %c\n", getpid(), sym);
    }
    close(pipe2[0]);
    free_mutex(mutex);
    wait(0);
    exit(0);
}