#include "proc.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

static bool __is_number(const char *input);

proc_t *proc_new(size_t process_buff_len) {
    proc_t *this = malloc(sizeof(proc_t));
    this->processes = malloc(sizeof(process_t *) * process_buff_len);

    for (int i = 0; i < process_buff_len; i++) {
        this->processes[i] = process_new();
    }

    return this;
}

void proc_del(proc_t *this) {
    for (int i = 0; i < this->processes_length; i++) {
        process_del(this->processes[i]);
    }
    free(this);
}

void proc_update(proc_t *this) {
    int len = 0;
    DIR *directory;
    struct dirent *dir;
    char *pid_buffer;

    directory = opendir("/proc");

    if (directory == NULL) return;

    while ((dir = readdir(directory)) != NULL && len < 256) {
        pid_buffer = dir->d_name;

        if (__is_number(pid_buffer) == false)
            continue;

        process_update(this->processes[len], atoi(pid_buffer));
        len ++;
    }

    this->processes_length = len;
}

static bool __is_number(const char *input) {
    size_t input_len = strlen(input);

    for (int i = 0; i < input_len; i++) {
        if (isdigit(input[i]) == 0) // 숫자가 아니면 0 리턴함
            return false;
    }

    return true;
}
