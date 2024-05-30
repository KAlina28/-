#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define A 1103515245
#define B 12345

void write_read_blocks(int sz, int cur, int fd){
    int r;
    uint8 x[BSIZE], y[BSIZE];
    int cur_ = cur;
    for (int i = 0; i < sz; ++i) {
        for (int j = 0; j < BSIZE; j += sizeof(uint64)){
            memcpy(x + j, &cur, sizeof (uint64));
            cur = cur * A + B;
        }
        r = write(fd, &x, BSIZE);
        if (r != BSIZE){
            fprintf(2, "error: writing failed\n");
            exit(1);
        }
    }
    close(fd);
    fd = open("test.dat", O_RDONLY);
    if (fd < 0) {
        fprintf(2, "error: can't open file\n");
        exit(1);
    }
    for (int i = 0; i < sz; i++) {
        for (int j = 0; j < BSIZE; j += sizeof(uint64)){
            memcpy(x + j, &cur_, sizeof (uint64));
            cur_ = cur_ * A + B;
        }
        r = read(fd, &y, BSIZE);
        if (r != BSIZE){
            fprintf(2, "error: reading failed\n");
            exit(1);
        }
        if (memcmp(x, y, BSIZE) != 0){
            fprintf(2, "error: not match\n");
            exit(1);
        }
    }
    char s;
    if ((r = read(fd, &s, 1)) != 0) {
        fprintf(2, "error: read\n");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "error: arg \n");
        exit(1);
    }

    int sz = atoi(argv[1]);
    int sd = atoi(argv[2]);
    if (sz <= 0 || sz > MAXFILE) {
        fprintf(2, "error: size\n");
        exit(1);
    }
    int fd = open("test.dat", O_WRONLY | O_CREATE);
    if (fd < 0) {
        fprintf(2, "error: can't create file\n");
        exit(1);
    }
    if (sd < 0){
        fprintf(2, "error: sd nonnegative\n");
    }
    write_read_blocks(sz, sd, fd);
    printf("yra!\n");
    close(fd);
    exit(0);
}