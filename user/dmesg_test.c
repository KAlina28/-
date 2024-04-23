#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]){
    char *buf = malloc(BUFFER_SIZE + 1);
    if (!buf){
        fprintf(2, "error\n");
        exit(1);
    }
    if (dmesg(buf, BUFFER_SIZE) < 0){
        fprintf(2, "error: address\n");
        exit(1);
    }
    for (int i = 0; i < BUFFER_SIZE; i++){
        printf("%c", buf[i]);
    }
    free(buf);
    exit(0);
}