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
    acquire(&buf_.lock);
    buf_.buf[buf_.tail] = sym;
    if ((buf_.tail + 1)%BUFFER_SIZE == buf_.head){
        buf_.head = (buf_.head + 1) % BUFFER_SIZE;
    }
    buf_.tail = (buf_.tail + 1) % BUFFER_SIZE;
    release(&buf_.lock);
}


void write_string(char *s){
    if (s == 0){
        s = "(null)";
    }
    for(;*s; ++s){
        release(&buf_.lock);
        add_to_queue(*s);
        acquire(&buf_.lock);
    }
}

void write_int(int sign, int num, int base){
    int x = (num < 0 ? -num: num);
    const char cnvrtr[] = "0123456789abcdef";
    int i = 0;
    char buffer[16];
    buffer[i++] = cnvrtr[x % base];
    x /= base;
    while (x != 0){
        buffer[i++] = cnvrtr[x % base];
        x /= base;
    }
    if (sign && num < 0){
        buffer[i] = '-';
    }
    release(&buf_.lock);
    while (i-- >= 0){
        add_to_queue(buffer[i]);
    }
    acquire(&buf_.lock);
}

void write_pointer(uint64 x){
    release(&buf_.lock);
    add_to_queue('0');
    add_to_queue('x');
    const char cnvrtr[] = "0123456789abcdef";
    int auto size_ = (sizeof (uint64)*2);
    for (int i = 0; i < size_; i++, x<<=4){
        add_to_queue(cnvrtr[x >> (size_*4-4)]);
    }
    acquire(&buf_.lock);
}

void vprintf_(const char *fmt, va_list args){
    int c;
    for(; (c = *fmt) != '\0'; fmt++){
        if (c != '%'){
            release(&buf_.lock);
            add_to_queue(c);
            acquire(&buf_.lock);
            continue;
        }
        c = *(++fmt);
        if (c == 0){
            break;
        }
        switch (c) {
            case '%':
                release(&buf_.lock);
                add_to_queue('%');
                acquire(&buf_.lock);
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
                write_string(va_arg(args, char*));
                break;
            default:  //просто текст
                release(&buf_.lock);
                add_to_queue('%');
                add_to_queue(c);
                acquire(&buf_.lock);
                break;
        }
    }
}


void pr_msg(const char *fmt, ...){
    acquire(&buf_.lock);
    acquire(&tickslock);
    release(&tickslock);
    release(&buf_.lock);
    add_to_queue('[');
    write_int(1,ticks, 10);//вывод тек время
    add_to_queue(']');
    add_to_queue(' ');
    acquire(&buf_.lock);
    va_list args;
    if (fmt == 0){
        release(&buf_.lock);
        panic("error: null fmt");
    }
    va_start(args, fmt);
    vprintf_(fmt, args);
    va_end(args);
    release(&buf_.lock);
    add_to_queue('\n');
}




