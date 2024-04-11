#include "kernel/types.h"
#include "kernel.stat.h"
#include "user/user.h"
#include "kernek/param.h"

int main(int argc, char *argv[]){
    if (argc < 2){
        fprintf(2, "error");
        exit(-1);
    }
    if (argc == 2 && strcmp(argv[1], "st") == 0){
        int pid = fork();
        int s;
        if (pid == 0){
            exec("vm", argv);
            exit(0);
        }else{
            wait(&s);
            char *buf = (char *) malloc(sizeof (char) * BUFFER_SIZE);
            if (dmesg(buf, BUFFER_SIZE) < 0){
                fprintf(2, "error");
                exit(1);
            }
            printf("%s", buf);
            free(buf);
        }
    }
    char *buf = (char *) malloc(sizeof (char) * BUFFER_SIZE);
    if (!buf){
        fprintf(2, "error");
        exit(1);
    }
    if (dmesg(buf, BUFFER_SIZE) < 0){
        fprintf(2, "error");
        exit(1);
    }
    printf("%s", buf);
    free(buf);
    exit(0);
}