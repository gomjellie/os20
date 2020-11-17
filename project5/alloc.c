#include "alloc.h"

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdbool.h>

typedef struct chunk {
    struct chunk *next;
    struct chunk *prev;
    int sz;
    size_t offset;
    bool assigned;
    bool u;
} chunk_t;

#define CHUNK_BUF_SIZE (PAGESIZE / MINALLOC)
static chunk_t chunks[CHUNK_BUF_SIZE];

chunk_t *chunk_new(int sz) {
    static int counter = 0;

    if (sz % MINALLOC != 0) {
        perror("chunk_new sizeerror");
        exit(-1);
    }

    for (; chunks[counter].u; counter = (counter + 1) % CHUNK_BUF_SIZE);

    chunks[counter] = (chunk_t) {
        .u = true,
        .assigned = false,
        .sz = sz,
        .next = NULL,
        .prev = NULL,
    };
    
    // printf("counter %d", counter);
    return &chunks[counter];
}

void chunk_print(chunk_t *chunk_list) {
    chunk_t *iter = chunk_list;
    puts("----chunk print----");

    for (; iter != NULL; iter = iter->next) {
        printf("offset: %zu, sz: %d, assigned: %d\n", iter->offset, iter->sz, iter->assigned);
    }
}

typedef struct _mman {
    int initialized;
    char *addr;
    chunk_t *chunk_list;
} mman_t;

static mman_t mman;

int init_alloc() {
    // void * mmap(void *__addr, size_t __len, int __prot, int __flags, int __fd, off_t __offset)
    static int __prot = PROT_READ | PROT_WRITE;
    static int __flags = MAP_PRIVATE | MAP_ANONYMOUS;
    mman.addr = mmap(NULL, PAGESIZE, __prot, __flags, -1, (off_t)0);
    mman.initialized = 1;
    if (mman.addr == MAP_FAILED) {
        perror("MAP_FAILED");
        exit(1);
    }
    
    chunk_t *chunk = chunk_new(PAGESIZE);
    chunk->offset = 0;

    mman.chunk_list = chunks;
    return 0;
}

/* Returns 0 if successful, -1 for errors */
int cleanup() {
    
    return munmap(mman.addr, PAGESIZE);
}

char *alloc(int __size) {
    chunk_t *iter = mman.chunk_list;

    if (__size % MINALLOC != 0)
        return (void *) -1;
    if (__size < MINALLOC)
        return (void *) -1;

    
    for (; iter != NULL; iter = iter->next) {
        
        if (iter->assigned) continue;

        // find first fit
        if (__size <= iter->sz) {
            if (iter->sz != __size) {
                chunk_t *next = iter->next;
                iter->next = chunk_new(iter->sz - __size);
                if (next)
                    iter->next->next = next;
            }

            if (iter->next) {
                iter->next->offset = iter->offset + __size;
                iter->next->prev = iter;
            }
            
            iter->assigned = true;
            iter->sz = __size;
            
            return mman.addr + iter->offset;
        }
    }

    return NULL;
}

void dealloc(char *__ptr) {
    size_t offset = __ptr - mman.addr;
    chunk_t *iter = mman.chunk_list;

    for (; iter != NULL; iter = iter->next) {
        if (iter->assigned == false) continue;

        // 찾음
        if (iter->offset == offset) {
            chunk_t *piv = iter; // pivot
            chunk_t *r_seeker = iter->next; // right seeker
            chunk_t *l_seeker = iter->prev; // left seeker

            // merge piv and right
            if (r_seeker != NULL && r_seeker->assigned == false) { // not assigned, needs merge
                if (r_seeker->next)
                    r_seeker->next->prev = piv;
                
                piv->next = r_seeker->next;
                piv->sz += r_seeker->sz;
                r_seeker->u = false;
            }
            piv->assigned = false;

            // merge left and piv
            if (l_seeker == NULL || l_seeker->assigned) break;

            if (l_seeker->prev)
                l_seeker->prev->next = piv;
            
            l_seeker->next = piv->next;
            l_seeker->sz += piv->sz;
            piv->u = false;

            return;
        }
    }
}
