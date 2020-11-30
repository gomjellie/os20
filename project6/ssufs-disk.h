#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

#define BLOCKSIZE 64
#define NUM_BLOCKS 35
#define NUM_DATA_BLOCKS 30
#define NUM_INODE_BLOCKS 4
#define NUM_INODES 8
#define NUM_INODES_PER_BLOCK 2
#define MAX_FILE_SIZE 4
#define MAX_FILES 8
#define MAX_OPEN_FILES 20
#define MAX_NAME_STRLEN 8
#define INODE_FREE 'x'
#define INODE_IN_USE '1'
#define DATA_BLOCK_FREE 'x'
#define DATA_BLOCK_USED '1'

struct superblock_t
{
	char name[MAX_NAME_STRLEN];
	char inode_freelist[NUM_INODES];
	char datablock_freelist[NUM_DATA_BLOCKS];
};

struct inode_t
{
	int status;
	char name[MAX_NAME_STRLEN];
	int file_size;
	int direct_blocks[MAX_FILE_SIZE];
};

struct filehandle_t
{
	int offset;
	int inode_number;
};

int open_namei(char *filename);

void ssufs_formatDisk();
int ssufs_allocInode();
void ssufs_freeInode(int inodenum);
void ssufs_readInode(int inodenum, struct inode_t *inodeptr);
void ssufs_writeInode(int inodenum, struct inode_t *inodeptr); 
int ssufs_allocDataBlock();
void ssufs_freeDataBlock(int blocknum);
void ssufs_readDataBlock(int blocknum, char *buf);
void ssufs_writeDataBlock(int blocknum, char *buf);
void ssufs_dump();
