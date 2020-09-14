#include "statm.h"

void statm_update(statm_t *this, int pid) {
    char *path = malloc(32);

    sprintf(path, "/proc/%d/statm", pid);

    FILE *fd = fopen(path, "r");

    if (!fd)
        return;

    fscanf(fd, "%zu %zu %zu %zu %zu %zu %zu",
        &this->size,
        &this->resident,
        &this->shared,
        &this->text,
        &this->lib,
        &this->data,
        &this->dt
    );
    
    fclose(fd);
    free(path);
}
