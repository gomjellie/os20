#include "dev.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>

cdev_t *cdev_new(size_t ttys_length) {
    cdev_t *this = (cdev_t *)malloc(sizeof(cdev_t));
    this->ttys_length = 0;
    this->ttys = malloc(sizeof(tty_t *) * ttys_length);
    for (int i = 0; i < ttys_length; i++)
        this->ttys[i] = tty_new();

    return this;
}

void cdev_update(cdev_t *this) {
    int len = 0;
    DIR *directory;
    struct dirent *dir;
    char *tty_buffer;
    char tid[16];

    directory = opendir("/dev");

    if (directory == NULL) return;

    while ((dir = readdir(directory)) != NULL && len < 256) {
        tty_buffer = dir->d_name;

        if (strncmp(tty_buffer, "tty", 3) != 0)
            continue; // tty로 시작하는 파일만 통과시키고 나머지는 빠꾸

        if (strcmp(tty_buffer, "tty") == 0)
            tid[0] = '\0';
        else
            sscanf(tty_buffer, "tty%7s", tid);
        
        tty_update(this->ttys[len], tid);
        len ++;
    }

    this->ttys_length = len;
}

char *cdev_find(const cdev_t *this, int ttynr) {
    for (int i = 0; i < this->ttys_length; i++) {
        if (tty_is_same(this->ttys[i], ttynr)) {
            return this->ttys[i]->tid;
        }
    }

    return NULL;
}
