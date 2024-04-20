#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "proc.h"
#include "defs.h"

struct mutex mutex_[NMUTEX];

void init_mutex(void){ // создание
    for (int i = 0; i < NMUTEX; ++i){
        initlock(&mutex_[i].sp_lock, "spin_lock");
        initsleeplock(&mutex_[i].sl_lock, "sleep_lock");
        mutex_[i].locked = 0;
        mutex_[i].pid = -1;
    }
}

int checker_mutex(int descriptor){
    if (descriptor < 0){
        return -1;
    }
    if (descriptor >= NOMUTEX) {
        return -1;
    }
    return 0;
}

//блокировка мьютекса по его дескриптору - возвращает 0 при успехе, отрицательное число при ошибке;
int acquire_mutex(int descriptor) {
    if (checker_mutex(descriptor) == -1){
        return -1;
    }
    struct proc *p = myproc();
    acquire(&p->lock);
    struct mutex * mut = p->table_mutex[descriptor];
    if (mut == 0 || holdingsleep(&mut->sl_lock)){
        return -1;
    }
    uint pid_ = p->pid;
    release(&p->lock);
    acquiresleep(&mut->sl_lock);
    acquire(&mut->sp_lock);
    mut->pid = pid_;
    release(&mut->sp_lock);
    return 0;
}

//разблокировка мьютекса по его дескриптору - возвращает 0 при успехе, отрицательное число при ошибке;
int release_mutex(int descriptor){
    if (checker_mutex(descriptor) == -1){
        return -1;
    }
    struct proc *p = myproc();
    acquire(&p->lock);
    struct mutex* mut = p->table_mutex[descriptor];
    if (mut == 0 || !holdingsleep(&mut->sl_lock)){
        return -1;
    }
    uint pid_ = p->pid;
    release(&p->lock);
    acquire(&mut->sp_lock);
    if (pid_ != mut->pid){
        release(&mut->sp_lock);
        return -1;
    }
    mut->pid = -1;
    release(&mut->sp_lock);
    releasesleep(&mut->sl_lock);
    return 0;
}

int create_mutex(void){
    struct proc* p = myproc();
    int ans = -1;
    for (int i = 0; i < NMUTEX; ++i){
        acquire(&mutex_[i].sp_lock);
        if (mutex_[i].locked != 0){
            release(&mutex_[i].sp_lock);
        }else{
            mutex_[i].locked = 1;
            acquire(&p->lock);
            for (int j = 0; j < NOMUTEX; ++j){
                if (p->table_mutex[j] == 0){
                    p->table_mutex[j] = &mutex_[i];
                    release(&p->lock);
                    ans = j;
                    break;
                }
            }
            if (ans != -1){
                release(&mutex_[i].sp_lock);
                return  ans;
            }
            release(&p->lock);
            return -1;
        }
    }
    return -1;
}



int free_mutex(int descriptor){
    if (checker_mutex(descriptor) == -1){
        return -1;
    }
    struct proc* p = myproc();
    acquire(&p->lock);
    struct mutex *mut = p->table_mutex[descriptor];
    release(&p->lock);
    if (mut == 0){
        return -1;
    }
    p->table_mutex[descriptor] = 0;

    acquire(&mut->sp_lock);
    if (mut->locked == 0){
        release(&mut->sp_lock);
        return -1;
    }
    mut->locked--;
    release(&mut->sp_lock);
    if (holdingsleep(&mut->sl_lock)){
        releasesleep(&mut->sl_lock);
    }
    return 0;
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
