
#ifndef __CMDLINE_H__
#define __CMDLINE_H__

typedef struct _cmdline {
    char string[128];
} cmdline_t;

cmdline_t *cmdline_new();
void cmdline_update(cmdline_t *this, int pid);

#endif /* __CMDLINE_H__ */
