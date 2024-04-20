#include "spinlock.h"
#include "sleeplock.h"

struct mutex {
    uint64 locked;
    struct sleeplock sl_lock;
    struct spinlock sp_lock;
    uint64 pid;
};

