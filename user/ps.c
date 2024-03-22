// task 4
#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"
#include "kernel/procinfo.h"

int main(int argc, char *argv[]){
    int buf_size = 1;
    pcinfo *buf = malloc(buf_size * sizeof(pcinfo));
    if (buf == 0) {
        fprintf(2, "error: memory allocation failed\n");
        exit(1);
    }
    int cnt;
    while (1){
        cnt = ppcinfo((uint64)buf, buf_size);
        if (cnt < 0 ) {
            fprintf(2, "error: memory allocation failed\n");
            exit(1);
        }
        if (cnt <= buf_size){
            break;
        }
        buf_size *= 2;
        buf = malloc(buf_size * sizeof(pcinfo));
        if (buf == 0) {
            free(buf);
            fprintf(2, "error: memory reallocation failed\n");
            exit(1);
        }
    }
    printf("total processes: %d\n", cnt);
    char *procstate[] = { "UNUSED", "USED", "SLEEPING", "RUNNABLE", "RUNNING", "ZOMBIE" };
    for (int i = 0; i < cnt; ++i) {
        printf(" name: %s \n state: %s \n parent_pid: %d \n", buf[i].name, procstate[buf[i].state], buf[i].parent_pid);
    }
    free(buf);
    exit(0);
}