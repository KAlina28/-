#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUFFER 512



void checking_exist(int ptr, int i) {
    if (i == 1 && ptr == BUFFER) {
        fprintf(2, "ERROR: can't read first number\n");
        exit(1);
    }
    if (i == 2 && ptr == BUFFER) {
        fprintf(2, "ERROR: can't read second number\n");
        exit(1);
    }
}

void checking_space(char *buffer, int i, int ptr) {
    if (i == 1 && buffer[ptr] != ' ') {
        fprintf(2, "ERROR: need two integer numbers separated by a space\n");
        exit(1);
    }
    if (i == 2 && !(buffer[ptr] == '\n' || buffer[ptr] == '\0' )) {
        fprintf(2, "ERROR: need two integers numbers separated by a space\n");
        exit(1);
    }
}

void checking_correct(char *buffer, int i, int *ptr) {
    if (i == 2 && !(*ptr < BUFFER && '0' <= buffer[*ptr] && buffer[*ptr] <= '9')) {
        fprintf(2, "ERROR: the second number was incorrect\n");
        exit(1);
    }
    if (i == 1 && !(*ptr < BUFFER && '0' <= buffer[*ptr] && buffer[*ptr] <= '9')) {
        fprintf(2, "ERROR: the first number was incorrect\n");
        exit(1);
    }
}


int main() {
    int ptr = 0;
    char buffer_[BUFFER];
    int ptr_first = 0, ptr_second = 0;
    char sym;
    while (ptr < BUFFER - 1) {
        int bytes_read = read(0,&sym, 1);
        if (bytes_read < 0) {
            fprintf(2, "ERROR: failed to read file\n");
            exit(1);
        }else if (ptr == 0 && bytes_read == 0){
            fprintf(2, "ERROR: empty input file\n");
            exit(1);
        }else if (bytes_read < 1){
            break;
        }
        buffer_[ptr] = sym;
        ptr++;
        if (sym == '\n'){
            break;
        }
    }
    buffer_[ptr] = '\0';
    ptr = 0;
    checking_exist(ptr, 1); checking_correct(buffer_, 1, &ptr);
    if (ptr < BUFFER && '0' <= buffer_[ptr] && buffer_[ptr] <= '9'){
        ptr_first = ptr;
        ptr++;
    }
    while (ptr < BUFFER && '0' <= buffer_[ptr] && buffer_[ptr] <= '9') {
        ptr++;
    }
    checking_exist(ptr, 2); checking_space(buffer_, 1, ptr); ptr++;
    checking_correct(buffer_, 2, &ptr);
    if (ptr < BUFFER && '0' <= buffer_[ptr] && buffer_[ptr] <= '9'){
        ptr_second = ptr;
        ptr++;
    }
    while (ptr < BUFFER && '0' <= buffer_[ptr] && buffer_[ptr] <= '9') {
        ptr++;
    }
    checking_space(buffer_, 2, ptr);
    printf("%d\n", atoi(&buffer_[ptr_first]) + atoi(&buffer_[ptr_second]));
    exit(0);
}
