#ifndef XV6_LAB0_BUFFER_H
#define XV6_LAB0_BUFFER_H

#include "param.h"
#include "types.h"
#include "spinlock.h"

typedef struct cycle_buf{
    int head;
    int tail;
    char buf[BUFFER_SIZE];
    struct spinlock lock;
}cycleBuf;

#endif //XV6_LAB0_BUFFER_H