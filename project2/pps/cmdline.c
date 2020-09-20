#include "cmdline.h"

#include <stdio.h>
#include <stdlib.h>

cmdline_t *cmdline_new() {
    cmdline_t *this = malloc(sizeof(cmdline_t));
    return this;
}

void cmdline_update(cmdline_t *this, int pid) {
    char path[32];
    sprintf(path, "/proc/%d/cmdline", pid);

    FILE *file = fopen(path, "r");

    if (!file)
        return;
    
    fscanf(file, "%s", this->string);

    fclose(file);
}
