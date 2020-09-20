#ifndef __DEV_H__
#define __DEV_H__

#include "tty.h"

#include <stdio.h>

typedef struct _cdev {
    tty_t **ttys;
    int ttys_length;
} cdev_t;

cdev_t *cdev_new(size_t ttys_length);
void cdev_del(cdev_t *this);
void cdev_update(cdev_t *this);
char *cdev_find(const cdev_t *this, int ttynr);

#endif /* __DEV_H__ */
