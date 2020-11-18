#include "ealloc.h"

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/param.h>
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

typedef struct _mman {
    chunkman_t cmans[16];
    size_t sz;
} mman_t;

static mman_t mman;

chunk_t *chunk_new(size_t cmi, size_t offset, int sz) {
    size_t chunk_idx = (offset / MINALLOC);
    chunkman_t *cman = &mman.cmans[cmi];

    if (sz % MINALLOC != 0) {
        perror("chunk_new sizeerror");
        exit(-1);
    }

    cman->chunks[chunk_idx] = (chunk_t) {
        .assigned = false,
        .sz = sz,
        .next = NULL,
        .prev = NULL,
    };
    
    return &cman->chunks[chunk_idx];
}

void small_init_alloc(size_t cmi) {
    static int __prot = PROT_READ | PROT_WRITE;
    static int __flags = MAP_PRIVATE | MAP_ANONYMOUS;
    chunkman_t *cman = &mman.cmans[cmi];
    
    cman->addr = mmap(NULL, PAGESIZE, __prot, __flags, -1, (off_t)0);
    if (cman->addr == MAP_FAILED)
        return;
    
    chunk_t *chunk = chunk_new(cmi, 0, PAGESIZE);
    chunk->offset = 0;

    cman->chunk_list = chunk;
}

void init_alloc(void) {
    // small_init_alloc(0);
    mman.sz = 0;
}

void cleanup(void) {
    for (int i = 0; i < mman.sz; i++) {
        munmap(mman.cmans[i].addr, PAGESIZE);
    }
}

/* Returns address if successful, NULL for errors */
char *small_alloc(size_t cmi, int __size) {
    chunk_t *iter = mman.cmans[cmi].chunk_list;

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
            iter->next = chunk_new(cmi, iter->offset + __size, iter->sz - __size);
            if (next)
                iter->next->next = next;
        }

        if (iter->next) {
            iter->next->offset = iter->offset + __size;
            iter->next->prev = iter;
        }
        
        iter->assigned = true;
        iter->sz = __size;
        
        return mman.cmans[cmi].addr + iter->offset;
    }

    return NULL;
}

char *alloc(int __size) {
    char *res;
    for (int cmi = 0; cmi < mman.sz; cmi++) {
        res = small_alloc(cmi, __size);
        if (res)
            return res;
    }

    small_init_alloc(mman.sz);
    res = small_alloc(mman.sz, __size);
    mman.sz ++;

    return res;
}

void small_dealloc(size_t cmi, char *__ptr) {
    size_t offset = __ptr - mman.cmans[cmi].addr;
    chunk_t *victim = &mman.cmans[cmi].chunks[(offset / MINALLOC)];

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

    if (piv->next)
        piv->next->prev = l_seeker;
    
    l_seeker->next = piv->next;
    l_seeker->sz += piv->sz;
}

void dealloc(char *__ptr) {
    for (int cmi = 0; cmi < mman.sz; cmi++) {
        char *l_addr = mman.cmans[cmi].addr;
        char *r_addr = l_addr + PAGESIZE;

        if (l_addr <= __ptr && __ptr < r_addr) {
            small_dealloc(cmi, __ptr);
            break;
        }
    }
}
