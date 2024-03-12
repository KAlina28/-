// task 4
#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"
#include "kernel/procinfo.h"

int main(int argc, char *argv[]){
    pcinfo buf[NPROC];
    int cnt = ps_listinfo(buf, NPROC);
    if (cnt < 0 || cnt > NPROC){
        fprintf(2, "error: uncorrect cnt processes\n");
        exit(1);
    }
    char *procstate[] = { "UNUSED", "USED", "SLEEPING", "RUNNABLE", "RUNNING", "ZOMBIE" };
    for (int i = 0; i < cnt; ++i){
        printf(" name: %s \n state: %s \n parent_pid: %d \n", buf[i].name, procstate[buf[i].state], buf[i].parent_pid);
    }
    exit(0);
}