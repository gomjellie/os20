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
    
    printf("%p\n", mman.addr);

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
            iter->next = chunk_new(iter->sz - __size);
            iter->next->offset = iter->offset + __size;
            iter->next->prev = iter;
            
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

    printf("찾으려는 주소(%p)의 offset %zu\n", __ptr, offset);

    for (; iter != NULL; iter = iter->next) {
        if (iter->assigned == false) continue;

        // 찾음
        if (iter->offset == offset) {
            printf("offset: %zu, prev: %p, next: %p \n", offset, iter->prev, iter->next);
            return;
        }
    }
}

#define RUN
#ifdef RUN
int main() {
    init_alloc();
    char *buf = alloc(8);
    char *buf2 = alloc(16);
    dealloc(buf2);

    cleanup();
}
#endif /* RUN */
