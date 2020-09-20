#include "status.h"

#include <stdlib.h>
#include <stdio.h>

status_t *status_new() {
    status_t *this = malloc(sizeof(status_t));

    return this;
}

void status_del(status_t *this) {
    free(this);
}

void status_update(status_t *this, int pid) {
    char path[32];
    sprintf(path, "/proc/%d/status", pid);

    FILE *file = fopen(path, "r");

    if (!file)
        return;
    
    fscanf(file, "Uid: %d %d %d %d", &this->uid[0], &this->uid[1], &this->uid[2], &this->uid[3]);

    fclose(file);
}
