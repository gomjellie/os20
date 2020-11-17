#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include "ealloc.h"

void printvsz(char *hint) {
  char buffer[256];
  sprintf(buffer, "echo -n %s && echo -n VSZ: && cat /proc/%d/stat | cut -d\" \" -f23", hint, getpid());
  system(buffer);
  //getchar();
}

int main() {
  printf("\nInitializing memory manager\n\n");
  init_alloc();

  //Start tests

  printf("Test1: checking heap expansion; allocate 4 X 4KB chunks\n");
  printvsz("start test 1:");

  char *a[4];
  for (int i = 0; i < 4; i++) {
    a[i] = alloc(4096);

    //write to chunk
    for (int j = 0; j < 4096; j++)
      *(a[i] + j) = 'a';

    printvsz("should increase by 4KB:");
  }

  //read all content and verify;
  int mismatch = 0;
  for (int i = 0; i < 4; i++) {
    
    //read each chunk
    for (int j = 0; j < 4096; j++) {
      char x = *(a[i] + j);
      if (x != 'a')
        mismatch = 1;
    }
  }

  if (mismatch) {
    printf("ERROR: Chunk contents did not match\n");
    exit(1);
  }

  for (int i = 0; i < 4; i++) {
    dealloc(a[i]);
  }

  printvsz("should not change:");
  printf("Test1: complete\n\n");

  printf("Test2: Check splitting of existing free chunks: allocate 64 X 256B chunks\n");
  printvsz("start test 2:");

  //we know the heap has 4 X 4KB free chunks
  //now ask for 64 X 256B chunks
  //no new memory should be used

  char *b[64];
  for (int i = 0; i < 64; i++) {
    b[i] = alloc(256);

    for (int j = 0; j < 256; j++)
      *(b[i] + j) = 'b';
  }
  printvsz("should not change:");

  //read each chunk
  mismatch = 0;
  for (int i = 0; i < 64; i++) {
    for (int j = 0; j < 256; j++) {
      char x = *(b[i] + j);
      if (x != 'b')
        mismatch = 1;
    }
  }

  if (mismatch) {
    printf("ERROR: Chunk contents did not match\n");
    exit(1);
  }

  for (int i = 0; i < 64; i++) {
    dealloc(b[i]);
  }

  printvsz("should not change:");
  printf("Test2: complete\n\n");

  printf("Test3: checking merging of existing free chunks; allocate 4 X 4KB chunks\n");
  printvsz("start test 3:");

  char *c[4];
  for (int i = 0; i < 4; i++) {
    c[i] = alloc(4096);

    //write to chunk
    for (int j = 0; j < 4096; j++)
      *(c[i] + j) = 'c';

    printvsz("should not change:");
  }

  //read all content and verify;
  mismatch = 0;
  for (int i = 0; i < 4; i++) {
    
    //read each chunk
    for (int j = 0; j < 4096; j++) {
      char x = *(c[i] + j);
      if (x != 'c')
        mismatch = 1;
    }
  }

  if (mismatch) {
    printf("ERROR: Chunk contents did not match\n");
    exit(1);
  }

  for (int i = 0; i < 4; i++) {
    dealloc(c[i]);
  }

  printvsz("should not change:");
  printf("Test3: complete\n\n");

  cleanup();
  printf("All tests complete\n");
}
