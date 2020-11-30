#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ssufs-disk.h"

int ssufs_allocFileHandle();
int ssufs_create(char *filename);
int ssufs_open(char *filename);
void ssufs_delete(char *filename); 
void ssufs_close(int file_handle);
int ssufs_read(int file_handle, char *buf, int nbytes);
int ssufs_write(int file_handle, char *buf, int nbytes);
int ssufs_lseek(int file_handle, int nseek);
