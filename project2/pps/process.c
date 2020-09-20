#include "process.h"

#include <stdlib.h>

process_t *process_new() {
    process_t *this = malloc(sizeof(process_t));
    this->stat = malloc(sizeof(stat_t));

    return this;
}

void process_update(process_t *this, int pid) {
    this->pid = pid;
    stat_update(this->stat, pid);
    cmdline_update(this->cmdline, pid);
}
