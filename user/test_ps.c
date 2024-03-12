//task3
#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"
#include "kernel/procinfo.h"


int main(int argc, char *argv[]){
    int total_procs = ps_listinfo((void*)0, 0);
    fprintf(1, "total processes: %d\n", total_procs);

    pcinfo pc_[2]; // в системе больше 2 процессов
    int res = ps_listinfo(pc_, 2);
    if (res > 2 || res < 0) {
        fprintf(2, "error: buffer too small, need at least %d slots\n", res);
    }

    res = ps_listinfo((void*)-1, 10); // невалидный адрес
    if (res < 0) {
        fprintf(2, "error: failed test with invalid address\n");
    }

    pcinfo plist[NPROC];
    res = ps_listinfo(plist, NPROC);
    if (res >= 0) {
        fprintf(1, "success processes\n", res);
    }else {
        fprintf(2, "error: test failed\n");
    }
    exit(0);
}