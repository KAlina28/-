#pragma once

typedef  struct procinfo {
    char name[16];
    int state;
    int parent_pid;
} pcinfo;