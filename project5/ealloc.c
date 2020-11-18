#include "ealloc.h"

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdbool.h>

#define CHUNK_BUF_SIZE (PAGESIZE / MINALLOC)

typedef struct chunk {
    struct chunk *next;
    struct chunk *prev;
    int sz;
    size_t offset;
    bool assigned;
} chunk_t;

typedef struct _chunkman {
    char *addr;
    chunk_t *chunk_list;
    chunk_t chunks[CHUNK_BUF_SIZE];
} chunkman_t;

static chunkman_t cman;

typedef struct _mman {
    chunkman_t cmans[16];
    size_t sz;
} mman_t;

static mman_t mman;

chunk_t *chunk_new(size_t offset, int sz) {
    size_t chunk_idx = (offset / MINALLOC);

    if (sz % MINALLOC != 0) {
        perror("chunk_new sizeerror");
        exit(-1);
    }

    cman.chunks[chunk_idx] = (chunk_t) {
        .assigned = false,
        .sz = sz,
        .next = NULL,
        .prev = NULL,
    };
    
    return &cman.chunks[chunk_idx];
}

void chunk_print(chunk_t *chunk_list) {
    chunk_t *iter = chunk_list;
    puts("----chunk print----");

    for (; iter != NULL; iter = iter->next) {
        printf("offset: %zu, sz: %d, assigned: %d\n", iter->offset, iter->sz, iter->assigned);
    }
}

void init_alloc(void) {
    static int __prot = PROT_READ | PROT_WRITE;
    static int __flags = MAP_PRIVATE | MAP_ANONYMOUS;
    cman.addr = mmap(NULL, PAGESIZE, __prot, __flags, -1, (off_t)0);
    if (cman.addr == MAP_FAILED)
        return;
    
    chunk_t *chunk = chunk_new(0, PAGESIZE);
    chunk->offset = 0;

    cman.chunk_list = chunk;
}

void cleanup(void) {
    munmap(cman.addr, PAGESIZE);
}

/* Returns address if successful, NULL for errors */
char *alloc(int __size) {
    chunk_t *iter = cman.chunk_list;

    if (__size % MINALLOC != 0)
        return NULL;
    if (__size < MINALLOC)
        return NULL;

    
    for (; iter != NULL; iter = iter->next) {
        
        if (iter->assigned) continue;

        // find first fit
        if (__size > iter->sz) continue;

        if (iter->sz != __size) {
            chunk_t *next = iter->next;
            iter->next = chunk_new(iter->offset + __size, iter->sz - __size);
            if (next)
                iter->next->next = next;
        }

        if (iter->next) {
            iter->next->offset = iter->offset + __size;
            iter->next->prev = iter;
        }
        
        iter->assigned = true;
        iter->sz = __size;
        
        return cman.addr + iter->offset;
    }

    return NULL;
}

void dealloc(char *__ptr) {
    size_t offset = __ptr - cman.addr;
    chunk_t *victim = &cman.chunks[(offset / MINALLOC)];

    chunk_t *piv = victim; // pivot
    chunk_t *r_seeker = victim->next; // right seeker
    chunk_t *l_seeker = victim->prev; // left seeker

    // merge piv and right
    if (r_seeker != NULL && r_seeker->assigned == false) { // not assigned, needs merge
        if (r_seeker->next)
            r_seeker->next->prev = piv;
        
        piv->next = r_seeker->next;
        piv->sz += r_seeker->sz;
    }
    piv->assigned = false;

    // merge left and piv
    if (l_seeker == NULL || l_seeker->assigned) return;

    if (l_seeker->prev)
        l_seeker->prev->next = piv;
    
    l_seeker->next = piv->next;
    l_seeker->sz += piv->sz;
}

int main() {
    init_alloc();
    
    char *b1 = alloc(1024);
    chunk_print(cman.chunk_list);
    char *b2 = alloc(1024);
    chunk_print(cman.chunk_list);
    char *b3 = alloc(1024);
    chunk_print(cman.chunk_list);
    char *b4 = alloc(2048);
    chunk_print(cman.chunk_list);
    char *b5 = alloc(1024);
    chunk_print(cman.chunk_list);
    
    cleanup();

    return 0;
}
