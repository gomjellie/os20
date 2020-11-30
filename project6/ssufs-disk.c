#include "ssufs-disk.h"

int DISK_FD;   // ssufs을 가리키는 파일 디스크립터
struct filehandle_t file_handle_array[MAX_OPEN_FILES]; // 열린 파일의 목록을 관리한다.

void ssufs_readSuperBlock(struct superblock_t *superblock){
	/*
		ssufs에서 superblock_t 구조체를 읽어오는 함수
	*/
	char tempBuf[BLOCKSIZE];
	lseek(DISK_FD, 0, SEEK_SET);
	int ret = read(DISK_FD ,tempBuf, BLOCKSIZE);
	assert(ret == BLOCKSIZE);
	memcpy(superblock, tempBuf, sizeof(struct superblock_t));
}

void ssufs_writeSuperBlock(struct superblock_t *superblock){
	/*
		ssufs에 superblock_t 구조체를 작성하는 함수
	*/
	char tempBuf[BLOCKSIZE];
	memcpy(tempBuf, superblock, sizeof(struct superblock_t));
	lseek(DISK_FD, 0, SEEK_SET);
	int ret = write(DISK_FD ,tempBuf, BLOCKSIZE);
	assert(ret == BLOCKSIZE);
}

void ssufs_formatDisk(){
	/*
		ssufs를 초기화하는 함수
	*/

	FILE* fp;
	fp = fopen("ssufs", "w+");
	DISK_FD = fileno(fp);

	// superblock 초기화
	struct superblock_t *superblock = (struct superblock_t *)malloc(sizeof(struct superblock_t));
	memcpy(superblock->name, "ssufs", 8);
	for(int i=0; i<NUM_INODES; i++)
		superblock->inode_freelist[i] = INODE_FREE;
	for(int i=0; i<NUM_DATA_BLOCKS; i++){
		superblock->datablock_freelist[i] = DATA_BLOCK_FREE;
	}
	ssufs_writeSuperBlock(superblock);
	free(superblock);
	
	// inode 초기화
	struct inode_t *inode = (struct inode_t *)malloc(sizeof(struct inode_t));
	memcpy(inode->name, "", 1);
	inode->status = INODE_FREE;
	inode->file_size = 0;
	for(int i=0; i<MAX_FILE_SIZE; i++)
		inode->direct_blocks[i] = -1;
	for(int i=0; i<NUM_INODES; i++)
		ssufs_writeInode(i, inode);
	free(inode);

	// file_handle_array 초기화
	for(int i=0; i<MAX_OPEN_FILES; i++){
		file_handle_array[i].inode_number = -1;
		file_handle_array[i].offset = 0;
	}
}

int open_namei(char *filename) {
	/*
		ssufs에서 filename을 이름으로 갖는 파일의 inodenum을 반환한다.
	*/
	struct superblock_t *superblock = (struct superblock_t *) malloc(sizeof(struct superblock_t));
	struct inode_t *tmp = (struct inode_t *) malloc(sizeof(struct inode_t));

	if (strlen(filename) > MAX_NAME_STRLEN) {
		return -1;
	}

	ssufs_readSuperBlock(superblock);

	for (int i = 0; i < NUM_INODES; i++) {
		if (superblock->inode_freelist[i] == INODE_IN_USE) {
			ssufs_readInode(i, tmp);
			if (strcmp(tmp->name, filename) == 0) {
				free(tmp);
				free(superblock);
				return i;
			}
		}
	}
	free(superblock);

	return -1;
}

int ssufs_allocInode(){
	/*
		inode_freelist에서 비어있는 첫 노드의 인덱스를 반환한다.
	*/
	struct superblock_t *superblock = (struct superblock_t *)malloc(sizeof(struct superblock_t));
	ssufs_readSuperBlock(superblock);
	for(int i=0; i<NUM_INODES; i++){
		if(superblock->inode_freelist[i] == INODE_FREE){
			superblock->inode_freelist[i] = INODE_IN_USE;
			ssufs_writeSuperBlock(superblock);
			free(superblock);
			return i;
		}
	}
	free(superblock);
	return -1;
}

void ssufs_freeInode(int inodenum){
	/*
		inodenum에 해당하는 inode를 free한다.
	*/
	assert(inodenum < NUM_INODES);
	struct superblock_t *superblock = (struct superblock_t *)malloc(sizeof(struct superblock_t));
	struct inode_t *inode = (struct inode_t *)malloc(sizeof(struct inode_t));
	ssufs_readSuperBlock(superblock);
	ssufs_readInode(inodenum, inode);
	assert(superblock->inode_freelist[inodenum] == INODE_IN_USE);
	superblock->inode_freelist[inodenum] = INODE_FREE;
	inode->status = INODE_FREE;
	inode->file_size = 0;
	for (int i = 0; i < MAX_FILE_SIZE; i++) {
		if (inode->direct_blocks[i] != -1) {
			ssufs_writeSuperBlock(superblock);
			ssufs_freeDataBlock(inode->direct_blocks[i]);
			ssufs_readSuperBlock(superblock);
		}
		inode->direct_blocks[i] = -1;
	}
	ssufs_writeSuperBlock(superblock);
	ssufs_writeInode(inodenum, inode);
	free(inode);
	free(superblock);
}

void ssufs_readInode(int inodenum, struct inode_t *inodeptr){
	/*
		inodenum에 해당하는 inode_t의 정보를 inodeptr에 읽어온다.
	*/
	assert(inodenum < NUM_INODES);
	char tempBuf[BLOCKSIZE / NUM_INODES_PER_BLOCK];
	lseek(DISK_FD, BLOCKSIZE + inodenum * sizeof(struct inode_t), SEEK_SET);
	int ret = read(DISK_FD, tempBuf, sizeof(struct inode_t));
	assert(ret == sizeof(struct inode_t));
	memcpy(inodeptr, tempBuf, sizeof(struct inode_t));
}

void ssufs_writeInode(int inodenum, struct inode_t *inodeptr){
	/*
		inodeptr이 가리키는 inode_t의 정보를 inodenum에 작성한다.
	*/
	assert(inodenum < NUM_INODES);
	char tempBuf[BLOCKSIZE / NUM_INODES_PER_BLOCK];
	memcpy(tempBuf, inodeptr, sizeof(struct inode_t));
	lseek(DISK_FD, BLOCKSIZE + inodenum * sizeof(struct inode_t), SEEK_SET);
	int ret = write(DISK_FD, tempBuf, sizeof(struct inode_t));
	assert(ret == sizeof(struct inode_t));
}

int ssufs_allocDataBlock(){
	/*
		datablock_freelist에서 비어있는 첫 노드의 인덱스를 반환한다.
	*/
	struct superblock_t *superblock = (struct superblock_t *)malloc(sizeof(struct superblock_t));
	ssufs_readSuperBlock(superblock);
	for (int i = 0; i < NUM_DATA_BLOCKS; i++){
		if (superblock->datablock_freelist[i] == DATA_BLOCK_FREE){
			superblock->datablock_freelist[i] = DATA_BLOCK_USED;
			ssufs_writeSuperBlock(superblock);
			free(superblock);
			return i;
		}
	}
	free(superblock);
	return -1;
}

void ssufs_freeDataBlock(int blocknum){
	/*
	 	blocknum에 해당하는 DataBlock을 free한다.
	*/
	struct superblock_t *superblock = (struct superblock_t *)malloc(sizeof(struct superblock_t));
	ssufs_readSuperBlock(superblock);
	assert(superblock->datablock_freelist[blocknum] == DATA_BLOCK_USED);
	superblock->datablock_freelist[blocknum] = DATA_BLOCK_FREE;
	ssufs_writeSuperBlock(superblock);
	free(superblock);
}

void ssufs_readDataBlock(int blocknum, char *buf){
	/*
		buf 배열에 blocknum에 해당하는 DataBlock의 데이터를 읽어온다.
	*/
	assert(blocknum < NUM_DATA_BLOCKS);
	char tempBuf[BLOCKSIZE];
	lseek(DISK_FD, BLOCKSIZE*(5 + blocknum), SEEK_SET);
	int ret = read(DISK_FD, tempBuf, BLOCKSIZE);
	assert(ret == BLOCKSIZE);
	memcpy(buf, tempBuf, BLOCKSIZE);
}

void ssufs_writeDataBlock(int blocknum, char *buf){
	/*
		blocknum에 해당하는 DataBlock에 buf 배열의 데이터를 작성한다.
	*/
	assert(blocknum < NUM_DATA_BLOCKS);
	char tempBuf[BLOCKSIZE];
	lseek(DISK_FD, BLOCKSIZE * (5 + blocknum), SEEK_SET);
	memcpy(tempBuf, buf, BLOCKSIZE); 
	int ret = write(DISK_FD, tempBuf, BLOCKSIZE);
	assert(ret == BLOCKSIZE);
}

void ssufs_dump(){
	/*
		현재 ssufs의 상태를 출력한다.
	*/

	printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<DISK STATE>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	struct superblock_t *superblock = (struct superblock_t *)malloc(sizeof(struct superblock_t));
	ssufs_readSuperBlock(superblock);
	char buf[MAX_NAME_STRLEN + 1];
	buf[MAX_NAME_STRLEN] = '\0';
	memcpy(buf, superblock->name, sizeof(buf) - 1);
	printf("DISK NAME: %s\nINODE FREELIST:      ", buf);
	for(int i=0; i<NUM_INODES; i++)
		printf("%c ", superblock->inode_freelist[i]);
	printf("\nDATA BLOCK FREELIST: ");
	for(int i=0; i<NUM_DATA_BLOCKS; i++)
		printf("%c ", superblock->datablock_freelist[i]);
	printf("\n");

	struct inode_t *inode = (struct inode_t *)malloc(sizeof(struct inode_t));
	for(int i=0; i<NUM_INODES; i++){
		ssufs_readInode(i, inode);
		if(inode->status == INODE_IN_USE){
			printf("INODE %d\nSTATUS:\t%c\tNAME\t%s\tSIZE\t%d\tDATABLOCK\t", i, inode->status, inode->name, inode->file_size);
			for (int j = 0; j < MAX_FILE_SIZE; j++)
				printf("%d ", inode->direct_blocks[j]);
			printf("\n");
			for (int j = 0; j < MAX_FILE_SIZE; j++){
				if (inode->direct_blocks[j] != -1 ){
					char tempBuf[BLOCKSIZE+1];
					tempBuf[BLOCKSIZE] = '\0';
					ssufs_readDataBlock(inode->direct_blocks[j], tempBuf);
					printf("DATA BLOCK %d: %s\n", j, tempBuf);
				}
			}
			printf("\n");
		}
	}
	free(inode);
	free(superblock);
	printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
}
