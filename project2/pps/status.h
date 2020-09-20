#ifndef __STATUS_H__
#define __STATUS_H__

typedef struct _status {
    int uid[4];
} status_t;

status_t *status_new();
void status_del(status_t *this);
void status_update(status_t *this, int pid);

#endif /* __STATUS_H__ */
