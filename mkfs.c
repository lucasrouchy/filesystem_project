#include "mkfs.h"
#include "free.h"
#include "inode.h"
#include "block.h"
#include "image.h"
#include "pack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void mkfs(void){
    unsigned char dir_block[BLOCK_SIZE];
    
    int inode_num = ialloc();
    int data_block_num = alloc();
    struct inode *root_in = iget(inode_num);
    root_in->size = 64;
    root_in->flags = 2;
    root_in->inode_num = 0;
    root_in->block_ptr[0] = data_block_num;

    bread(root_in->block_ptr[0], dir_block);

    write_u16(dir_block, inode_num);
    strncpy((char *)dir_block + 2, ".", DIRECTORY_ENTRY_COUNT);

    write_u16(dir_block + DIRECTORY_SIZE, inode_num);
    strncpy((char *)dir_block + DIRECTORY_SIZE + 2, "..", DIRECTORY_ENTRY_COUNT);
    
    bwrite(root_in->block_ptr[0], dir_block);
    iput(root_in);
}

struct directory *directory_open(int inode_num){
    struct inode* dir_inode = iget(inode_num);
    if(dir_inode == NULL){
        return NULL;
    }
    struct directory *dir = malloc(sizeof(struct directory));
    dir->inode = dir_inode;
    dir->offset = 0;
    return dir;
}

int directory_get(struct directory *dir, struct directory_entry *ent){
    unsigned char block[BLOCK_SIZE];
    unsigned int dir_size = dir->inode->size;
    
    if(dir->offset >= dir_size){
        return -1;
    }

    unsigned int block_index = dir->offset / BLOCK_SIZE;
    unsigned int block_num = dir->inode->block_ptr[block_index];
    bread(block_num, block);
    unsigned int offset = dir->offset % BLOCK_SIZE;
   
    ent->inode_num = read_u16(block + offset);
    strncpy(ent->name, (char *)block + offset + 2, DIRECTORY_ENTRY_COUNT);
    ent->name[15] = '\0';

    dir->offset += DIRECTORY_SIZE;
    return 0;
}

void directory_close(struct directory *d){
    iput(d->inode);
    free(d);
}

struct inode *namei(char *path) {
    if (strcmp(path, "/") == 0) {
        return iget(ROOT_INODE_NUM);
    }
    // if (strcmp(path, "/foo") == 0) {
    //     return iget(ROOT_INODE_NUM);
    // }
    return NULL;
}


int directory_make(char *path) {
    char dir_path[1024];
    char base_name_path[1024];
    get_dirname(path, dir_path);
    get_basename(path, base_name_path);
    struct inode *parent_in = namei(dir_path);
    if (parent_in == NULL) {
        return -1; 
    }

    int inode_num = ialloc();
    int data_block_num = alloc();
    //create new block sized array for new directory
    unsigned char dir_block[BLOCK_SIZE];

    struct inode *new_in = iget(inode_num);

    //step 6
    write_u16(dir_block, inode_num);
    strncpy((char *)dir_block + 2, ".", DIRECTORY_ENTRY_COUNT);

    write_u16(dir_block + DIRECTORY_SIZE, parent_in->inode_num);
    strncpy((char *)dir_block + DIRECTORY_SIZE + 2, "..", DIRECTORY_ENTRY_COUNT);
    //step 7
    new_in->size = INODE_SIZE;
    new_in->flags = 2;
    new_in->inode_num = inode_num;
    new_in->block_ptr[0] = data_block_num;
    //step 8 write the new directory data block to the disk
    bwrite(new_in->block_ptr[0], dir_block);    
    //Step 9: from the parent directory inode find the block that will contain the new directory using the size and block_ptr
    
    unsigned int parent_size = parent_in->size;
    unsigned int parent_block_index = parent_size / BLOCK_SIZE;
    unsigned int parent_dir_space = parent_size / MAX_DIR_SPACE;
    unsigned int parent_block_num = parent_in->block_ptr[parent_block_index];
    unsigned int parent_block_offset = parent_size % BLOCK_SIZE;
    if (parent_dir_space == 1){
        return -1;
    }
    bread(parent_block_num, dir_block);
    write_u16(dir_block + parent_block_offset, inode_num);
    strncpy((char *)dir_block + parent_block_offset + 2, base_name_path, DIRECTORY_ENTRY_COUNT);

    bwrite(parent_block_num, dir_block);
    parent_in->size += DIRECTORY_SIZE;
    iput(new_in);
    iput(parent_in);
    
    return 0;
}
