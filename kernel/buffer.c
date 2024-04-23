#include "stdarg.h"
#include "buffer.h"
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "proc.h"
#include "defs.h"


cycleBuf buf_;

void init_buf(void){
    initlock(&buf_.lock, "buffer");
    buf_.tail = 0;
    buf_.head = 0;
}

void add_to_queue(char sym){
    buf_.buf[buf_.tail] = sym;
    buf_.tail = (buf_.tail + 1) % BUFFER_SIZE;
    if (buf_.tail == buf_.head){
        buf_.head = (buf_.head + 1) % BUFFER_SIZE;
    }
}


void write_int(int sign, int num, int base){
    uint x;
    if (sign && (sign = num < 0)){
        x = -num;
    }else{
        x = num;
    }
    const char cnvrtr[] = "0123456789abcdef";
    int i = 0;
    char buffer[16];
    buffer[i++] = cnvrtr[x % base];
    x /= base;
    while ((x/=base) != 0){
        buffer[i++] = cnvrtr[x % base];
        x /= base;
    }
    if (sign){
        buffer[i++] = '-';
    }
    while (i-- >= 0){
        add_to_queue(buffer[i]);
    }
}

void write_pointer(uint64 x){
    add_to_queue('0');
    add_to_queue('x');
    char cnvrtr[] = "0123456789abcdef";
    for (int i = 0; i < (sizeof (uint64)*2); i++, x<<=4){
        add_to_queue(cnvrtr[x >> ((sizeof (uint64))*8-4)]);
    }
}

void vprintf_(const char *fmt, va_list args){
    int c;
    char *s;
    for(int i = 0; (c = fmt[i]) != 0; i++){
        if (c != '%'){
            add_to_queue(c);
            continue;
        }
        c = fmt[++i];
        if (c == 0){
            break;
        }
        switch (c) {
            case '%':
                add_to_queue('%');
                break;
            case 'd': //десятичное число
                write_int(1, va_arg(args, int), 10);
                break;
            case 'x':  //шестнадцатиричное число
                write_int(1, va_arg(args, int), 16);
                break;
            case 'p':  // указатель
                write_pointer(va_arg(args,uint64));
                break;
            case 's':   //строка
                if ((s = va_arg(args, char*)) == 0){
                    s = "(null)";
                }
                for(;*s; ++s){
                    add_to_queue(*s);
                }
                break;
            default:  //просто текст
                add_to_queue('%');
                add_to_queue(c);
                break;
        }
    }
}


void pr_msg(const char *fmt, ...){
    acquire(&tickslock);
    release(&tickslock);
    acquire(&buf_.lock);
    add_to_queue('[');
    write_int(1,ticks, 10);//вывод тек время
    add_to_queue(']');
    add_to_queue(' ');
    va_list args;
    va_start(args, fmt);
    if (fmt == 0){
        panic("error: null fmt");
    }
    vprintf_(fmt, args);
    va_end(args);
    add_to_queue('\n');
    release(&buf_.lock);
}


int sys_dmesg(void){
    uint64 buffer, size_;
    argaddr(0, &buffer);
    argaddr(1, &size_);
    if (!buffer){
        return -1;
    }
    acquire(&buf_.lock);
    int ptr_head = buf_.head;
    int i;
    for (i = 0; i < size_ - 1; i++) {
        if (ptr_head == buf_.tail){
            break;
        }
        if (copyout(myproc()->pagetable, buffer + i, &buf_.buf[ptr_head], 1) < 0){
            release(&buf_.lock);
            return -1;
        }
        ptr_head = (ptr_head + 1) % BUFFER_SIZE;
    }
    char not_end = 0;
    if (copyout(myproc()->pagetable, buffer + i, &not_end, 1) < 0){
        release(&buf_.lock);
        return -1;
    }
    release(&buf_.lock);
    return 0;
}


