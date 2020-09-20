#include "process.h"

#include <stdlib.h>

process_t *process_new() {
    process_t *this = malloc(sizeof(process_t));
    this->stat = malloc(sizeof(stat_t));
    this->cmdline = cmdline_new();
    this->status = status_new();

    return this;
}

void process_del(process_t *this) {
    free(this->stat);
    cmdline_del(this->cmdline);
    status_del(this->status);
}

void process_update(process_t *this, int pid) {
    this->pid = pid;
    stat_update(this->stat, pid);
    cmdline_update(this->cmdline, pid);
    status_update(this->status, pid);
}
