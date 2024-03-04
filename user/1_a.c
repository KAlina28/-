#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define TIME 150

int main(){
    int parent_id = fork();
    int status;
    if (parent_id == 0){
        sleep(TIME);
        exit(1);
    }else if (parent_id > 0){
        fprintf(1, "parent_id %d\n", getpid());
        fprintf(1, "child_id %d\n", parent_id);
        wait(&status);
        fprintf(1, "id_process: %d exit_code: %d \n", parent_id, status);
    }else{
        fprintf(2, "ERROR: problem with create child process");
        exit(-1);
    }
    exit(0);
}