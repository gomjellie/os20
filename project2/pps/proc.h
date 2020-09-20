#ifndef __PROC_H__
#define __PROC_H__

#include "process.h"

typedef struct _proc {
    process_t **processes;
    int processes_length;
} proc_t;

proc_t *proc_new(size_t process_buff_len);
void proc_update(proc_t *this);

#endif /* __PROC_H__ */