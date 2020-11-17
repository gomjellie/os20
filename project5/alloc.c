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
            }
            piv->assigned = false;

            // merge left and piv
            if (l_seeker == NULL || l_seeker->assigned) break;

            if (l_seeker->prev)
                l_seeker->prev->next = piv;
            
            l_seeker->next = piv->next;
            l_seeker->sz += piv->sz;

            return;
        }
    }
}

// #define RUN
#ifdef RUN
int main() {
    //mmap to get page
    if (init_alloc())
        return 1; //mmap failed

    char *str = alloc(512);
    chunk_print(mman.chunk_list);
    char *str2 = alloc(512);
    chunk_print(mman.chunk_list);

    if (str == NULL || str2 == NULL) {
        printf("alloc failed\n");
        return 1;
    }

    strcpy(str, "Hello, world!");
    if (strcmp("Hello, world!", str))
        printf("Hello, world! test failed\n");

    else
        printf("Hello, world! test passed\n");

    dealloc(str);
    chunk_print(mman.chunk_list);
    dealloc(str2);
    chunk_print(mman.chunk_list);

    printf("Elementary tests passed\n");

    printf("Starting comprehensive tests (see details in code)\n");

    /*
    Comprehensive tests:
    1. Populate entire thing in blocks of for loop (a's, b's, c's, and d's) equal size.
    2. Dealloc c's, reallocate and replace with x's.
    3. 
    */

    /*** test 1 ****/

    //Generating ideal strings for comparison
    char stringA[1024], stringB[1024], stringC[1024], stringD[1024], stringX[1024];
    for (int i = 0; i < 1023; i++) {
        stringA[i] = 'a';
        stringB[i] = 'b';
        stringC[i] = 'c';
        stringD[i] = 'd';
        stringX[i] = 'x';
    }

    stringA[1023] = stringB[1023] = stringC[1023] = stringD[1023] = stringX[1023] = '\0';

    char *strA = alloc(1024);
    chunk_print(mman.chunk_list);
    char *strB = alloc(1024);
    chunk_print(mman.chunk_list);
    char *strC = alloc(1024);
    chunk_print(mman.chunk_list);
    char *strD = alloc(1024);
    chunk_print(mman.chunk_list);

    for (int i = 0; i < 1023; i++) {
        strA[i] = 'a';
        strB[i] = 'b';
        strC[i] = 'c';
        strD[i] = 'd';
    }
    strA[1023] = strB[1023] = strC[1023] = strD[1023] = '\0';

    if (strcmp(stringA, strA) == 0 && strcmp(stringB, strB) == 0 && strcmp(stringC, strC) == 0 && strcmp(stringD, strD) == 0)
        printf("Test 1 passed: allocated 4 chunks of 1KB each\n");
    else
        printf("Test 1 failed: A: %d, B: %d, C: %d, D: %d\n", strcmp(stringA, strA), strcmp(stringB, strB), strcmp(stringC, strC), strcmp(stringD, strD));

    /**** test 2 ****/

    dealloc(strC);
    chunk_print(mman.chunk_list);

    char *strX = alloc(1024);
    chunk_print(mman.chunk_list);
    for (int i = 0; i < 1023; i++) {
        strX[i] = 'x';
    }
    strX[1023] = '\0';

    if (strcmp(stringX, strX) == 0)
        printf("Test 2 passed: dealloc and realloc worked\n");
    else
        printf("Test 2 failed: X: %d\n", strcmp(stringX, strX));

    /*** test 3 ***/

    char stringY[512], stringZ[512];
    for (int i = 0; i < 511; i++) {
        stringY[i] = 'y';
        stringZ[i] = 'z';
    }
    stringY[511] = stringZ[511] = '\0';

    dealloc(strB);
    chunk_print(mman.chunk_list);

    char *strY = alloc(512);
    chunk_print(mman.chunk_list);
    char *strZ = alloc(512);
    chunk_print(mman.chunk_list);

    for (int i = 0; i < 511; i++) {
        strY[i] = 'y';
        strZ[i] = 'z';
    }
    strY[511] = strZ[511] = '\0';

    if (strcmp(stringY, strY) == 0 && strcmp(stringZ, strZ) == 0)
        printf("Test 3 passed: dealloc and smaller realloc worked\n");
    else
        printf("Test 3 failed: Y: %d, Z: %d\n", strcmp(stringY, strY), strcmp(stringZ, strZ));

    // merge checks
    //test 4: free 2x512, allocate 1024

    dealloc(strZ);
    chunk_print(mman.chunk_list);
    dealloc(strY);
    chunk_print(mman.chunk_list);
    strY = alloc(1024);
    chunk_print(mman.chunk_list);
    for (int i = 0; i < 1023; i++) {
        strY[i] = 'x';
    }
    strY[1023] = '\0';

    if (strcmp(stringX, strY) == 0)
        printf("Test 4 passed: merge worked\n");
    else
        printf("Test 4 failed: X: %d\n", strcmp(stringX, strX));

    //test 5: free 2x1024, allocate 2048

    dealloc(strX);
    chunk_print(mman.chunk_list);
    dealloc(strY);
    chunk_print(mman.chunk_list);
    strX = alloc(2048);
    chunk_print(mman.chunk_list);
    char stringM[2048];
    for (int i = 0; i < 2047; i++) {
        strX[i] = stringM[i] = 'z';
    }
    strX[2047] = stringM[2047] = '\0';
    if (!strcmp(stringM, strX))
        printf("Test 5 passed: merge alloc 2048 worked\n");
    else
        printf("Test5 failed\n");
    ///////////////////////////

    if (cleanup())
        return 1; //munmap failed
    return 0;
}

#endif /* RUN */
