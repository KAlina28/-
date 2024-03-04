#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc, char* argv[]){
    if (argc < 2){
        fprintf(2, "ERROR: uncorrect count of argument\n");
        exit(-1);
    }
    int fd[2];
    if (pipe(fd) == -1){
        fprintf(2, "ERROR: could not create a pipe\n");
        exit(-1);
    }
    int parent_id = fork();
    int status;
    if (parent_id < 0){
        fprintf(2, "ERROR: problem with create child process");
        exit(-1);
    }else if (parent_id == 0){
        close(fd[1]);
        close(0);
        dup(fd[0]);
        close(fd[0]);
        char *argv[] = {"/wc", 0};
        exec("/wc", argv);
        fprintf(2, "ERROR: wc is failed\n");
        exit(-1);
    }else if (parent_id > 0){
        close(fd[0]);
        int size_;
        for (int i = 1; i < argc; ++i){
            size_ = strlen(argv[i]);
            if (write(fd[1], argv[i], size_) == -1){
                fprintf(2, "ERROR: could not write to pipe\n");
                exit(-1);
            }
            if (write(fd[1], "\n", 1) == -1){
                fprintf(2, "ERROR: could not write to pipe\n");
                exit(-1);
            }
        }
        close(fd[1]);
        wait(&status);
    }
    exit(0);
}