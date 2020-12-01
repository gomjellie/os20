#include "ssufs-ops.h"

extern struct filehandle_t file_handle_array[MAX_OPEN_FILES];

int ssufs_allocFileHandle() {
    for(int i = 0; i < MAX_OPEN_FILES; i++) {
        if (file_handle_array[i].inode_number == -1) {
            file_handle_array[i].inode_number = i; // 있어야 될거같은데 없어서 추가
            // file_handle_array[i].offset = 0;
            return i;
        }
    }
    return -1;
}

int ssufs_create(char *filename){
    int inode_idx = ssufs_allocInode();
    struct inode_t inode;

    if (inode_idx == -1)
        return -1;
    
    ssufs_readInode(inode_idx, &inode);
    inode.status = INODE_IN_USE;

    strncpy(inode.name, (const char *)filename, 8);
    ssufs_writeInode(inode_idx, &inode);
    
    return inode_idx;
}

void ssufs_delete(char *filename){
    int inode_idx = open_namei(filename);
    struct inode_t inode;

    if (inode_idx == -1)
        return;
    
    ssufs_freeInode(inode_idx);
}

int ssufs_open(char *filename){
    int inode_idx = open_namei(filename);
    int offset;

    if (inode_idx == -1)
        return -1;

    return ssufs_allocFileHandle();
}

void ssufs_close(int file_handle){
    file_handle_array[file_handle].inode_number = -1;
    file_handle_array[file_handle].offset = 0;
}

int ssufs_read(int file_handle, char *buf, int nbytes){
    char tmp[BLOCKSIZE];
    struct inode_t inode;
    struct filehandle_t *file_handler = &file_handle_array[file_handle];
    int inodenum = file_handler->inode_number;
    int blocknum;
    
    if (inodenum == -1)
        return -1;
    
    ssufs_readInode(inodenum, &inode);
    
    if (inode.file_size < nbytes)
        return -1;
    
    int nblock_in_use = 0;
    for (int i = 0; i < NUM_INODE_BLOCKS; i++)
        if (inode.direct_blocks[i] != -1)
            nblock_in_use ++;
    
    if (nblock_in_use * 4 - file_handler->offset < nbytes)
        return -1;

    for (int read = 0; read < nbytes; ) {
        size_t block = file_handler->offset / BLOCKSIZE;
        size_t off = file_handler->offset % BLOCKSIZE;
        size_t jump = BLOCKSIZE - off;
        
        blocknum = inode.direct_blocks[block];
        ssufs_readDataBlock(blocknum, tmp);
        
        if (jump > nbytes - read) {
            jump = nbytes - read;
        }
        memcpy(&buf[read], &tmp[off], jump);
        ssufs_lseek(file_handle, jump);
        read = read + jump;
    }

    return 0;
}

int ssufs_write(int file_handle, char *buf, int nbytes){
    static char clean_block[BLOCKSIZE];
    char tmp[BLOCKSIZE];
    struct inode_t inode;
    struct filehandle_t *file_handler = &file_handle_array[file_handle];
    int blocknum;
    int inodenum = file_handle_array[file_handle].inode_number;

    if (inodenum == -1)
        return -1;
    
    ssufs_readInode(inodenum, &inode);
    size_t buf_size = strlen(buf);
    
    //  available space < nbytes
    if (BLOCKSIZE * 4 - file_handler->offset < nbytes)
        return -1;
    
    for (int read = 0, i = 0; read < nbytes; i++) {
        size_t block = file_handler->offset / BLOCKSIZE;
        size_t off = file_handler->offset % BLOCKSIZE;
        size_t jump = BLOCKSIZE - off;
        
        blocknum = inode.direct_blocks[i];
        if (blocknum == -1) {
            blocknum = ssufs_allocDataBlock();
            inode.direct_blocks[i] = blocknum;
            ssufs_writeDataBlock(blocknum, clean_block);
        }
        ssufs_readDataBlock(blocknum, tmp);
        
        if (jump > nbytes - read) {
            jump = nbytes - read;
        }
        memcpy(&tmp[off], &buf[read], jump);
        ssufs_writeDataBlock(blocknum, tmp);
        ssufs_lseek(file_handle, jump);
        read = read + jump;
    }

    ssufs_writeInode(inodenum, &inode);
    ssufs_lseek(file_handle, nbytes);
    return 0;
}

int ssufs_lseek(int file_handle, int nseek){
    int offset = file_handle_array[file_handle].offset;

    struct inode_t *tmp = (struct inode_t *) malloc(sizeof(struct inode_t));
    ssufs_readInode(file_handle_array[file_handle].inode_number, tmp);
    
    int fsize = tmp->file_size;
    
    offset += nseek;

    if ((fsize == -1) || (offset < 0) || (offset > fsize)) {
        free(tmp);
        return -1;
    }

    file_handle_array[file_handle].offset = offset;
    free(tmp);

    return 0;
}
