#include "cmdline.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

cmdline_t *cmdline_new() {
    cmdline_t *this = malloc(sizeof(cmdline_t));

    return this;
}

void cmdline_del(cmdline_t *this) {
    free(this);
}

void cmdline_update(cmdline_t *this, int pid) {
    char path[32];
    sprintf(path, "/proc/%d/cmdline", pid);

    FILE *file = fopen(path, "r");

    if (!file)
        return;
    
    fgets(this->string, sizeof(this->string), file);

    for (int i = 0; i < sizeof(this->string) - 1; i++) {
        if (this->string[i] == '\0' && this->string[i + 1] == '\0') continue;

        if (this->string[i] == '\0')
            this->string[i] = ' ';
    }

    fclose(file);
}
