#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "mutex.h"
#include "riscv.h"
#include "defs.h"
#include "memlayout.h"
#include "proc.h"

mutex mutex_[NMUTEX];

void init_mutex(void){ // создание
    for (int i = 0; i < NMUTEX; ++i){
        initlock(&mutex_[i].sp_lock, "spin_lock");
        initsleeplock(&mutex_[i].sl_lock, "sleep_lock");
        mutex_[i].locked = 0;
    }
}

int checker_mutex(int descriptor){
    if (descriptor < 0){
        return -1;
    }
    if (descriptor >= NMUTEX) {
        return -1;
    }
    return 0;
}

//блокировка мьютекса по его дескриптору - возвращает 0 при успехе, отрицательное число при ошибке;
int acquire_mutex(int descriptor) {
    if (checker_mutex(descriptor) == -1){
        return -1;
    }
    acquiresleep(&mutex_[descriptor].sl_lock);
    return 0;
}

//разблокировка мьютекса по его дескриптору - возвращает 0 при успехе, отрицательное число при ошибке;
int release_mutex(int descriptor){
    if (checker_mutex(descriptor) == -1){
        return -1;
    }
    releasesleep(&mutex_[descriptor].sl_lock);
    return 0;
}

int create_mutex(void){
    struct proc* p = myproc();
    for (int i = 0; i < NMUTEX; ++i){
        acquire(&mutex_[i].sp_lock);
        if (mutex_[i].locked != 0){
            release(&mutex_[i].sp_lock);
        }else{
            mutex_[i].locked = 1;
            acquire(&p->lock);
            for (int j = 0; j < NOMUTEX; ++j){
                if (p->table_mutex[j] == 0){
                    p->table_mutex[j] = i;
                    release(&p->lock);
                    return j;
                }
            }
            release(&p->lock);
            release(&mutex_[i].sp_lock);
            return -1;
        }
    }
    return -1;
}

int use_mutex(int descriptor){
    if (checker_mutex(descriptor) == -1){
        return -1;
    }
    acquire(&mutex_[descriptor].sp_lock); mutex_[descriptor].locked ++;
    release(&mutex_[descriptor].sp_lock);
    return 0;
}


int free_mutex(int descriptor){
    if (checker_mutex(descriptor) == -1){
        return -1;
    }
    acquire(&mutex_[descriptor].sp_lock);
    if (mutex_[descriptor].locked == 0){
        release(&mutex_[descriptor].sp_lock);
        return -1;
    }
    struct proc* p = myproc();
    acquire(&p->lock);
    for (int i =0 ; i< NOMUTEX; ++i){
        if (p->table_mutex[i] == descriptor){
            mutex_[descriptor].locked --;
            p->table_mutex[i] = -1;
            release(&mutex_[descriptor].sp_lock);
            return 0;
        }
    }
    return -1;
}




uint64 sys_create_mutex(){
    int descriptor = create_mutex();
    return descriptor;
}

uint64 sys_acquire_mutex(){
    int descriptor; argint(0, &descriptor);
    return acquire_mutex(descriptor);
}

uint64 sys_release_mutex(){
    int descriptor; argint(0, &descriptor);
    return release_mutex(descriptor);
}

uint64 sys_free_mutex(){
    int descriptor; argint(0, &descriptor);
    return free_mutex(descriptor);
}
