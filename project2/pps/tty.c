#include "tty.h"

#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>

tty_t *tty_new() {
    tty_t *this = malloc(sizeof(tty_t));

    return this;
}

void tty_update(tty_t *this, char *tid) {
    struct stat statbuf;
    char path_buf[256];
    
    sprintf(path_buf, "/dev/tty%s", tid);

    lstat(path_buf, &statbuf);

    this->major = (int)MAJOR(statbuf.st_rdev);
    this->minor = (int)MINOR(statbuf.st_rdev);
    snprintf(this->tid, 8, "%s", tid);
}

int tty_is_same(tty_t *this, int ttynr) {
    int maj = (int)MAJOR(ttynr), min = (int)MINOR(ttynr);
    // printf("major: %d maj: %d, minor: %d min: %d\n", this->major, maj, this->minor, min);
    return (this->major == maj && this->minor == min);
}
