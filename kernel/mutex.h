typedef struct mutex {
    uint64 locked;
    struct sleeplock sl_lock;
    struct spinlock sp_lock;
} mutex;