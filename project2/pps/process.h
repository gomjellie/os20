#include "stat.h"
#include "cmdline.h"
#include "status.h"

#ifndef __PROCESS_H__
#define __PROCESS_H__

typedef struct _process {
    stat_t *stat;
    cmdline_t *cmdline;
    status_t *status;
    int pid;
    char tid[16];
} process_t;

process_t *process_new();
void process_del(process_t *this);

void process_update(process_t *this, int pid);

#endif /* __PROCESS_H__ */
