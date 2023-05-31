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
    strncpy((char *)dir_block + 2, ".", 16);

    write_u16(dir_block + DIRECTORY_SIZE, inode_num);
    strncpy((char *)dir_block + DIRECTORY_SIZE + 2, "..", 16);
    
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
    strncpy(ent->name, (char *)block + offset + 2, 16);
    ent->name[15] = '\0';

    dir->offset += 32;
    return 0;
}

void directory_close(struct directory *d){
    iput(d->inode);
    free(d);
}
struct inode *namei(char *path){
    struct directory *dir = directory_open(ROOT_INODE_NUM);
    struct directory_entry ent;
    int i = 0;
    //while the path is not invalid 
    while(path[i] != '\0'){
        //if tthe directory get fails return null
        if(directory_get(dir, &ent) == -1){
            return NULL;
        }

        if(strcmp(ent.name, path) == 0 && dir->inode->ref_count != 0){
            return iget(ent.inode_num);
        }
    }
    return NULL;

}
int directory_make(char *path){
    unsigned char dir_block[BLOCK_SIZE];
    struct directory *dir = directory_open(ROOT_INODE_NUM);
    struct directory_entry ent;
    int i = 0;
    while(path[i] != '/'){
        if(directory_get(dir, &ent) == -1){
            return -1;
        }
        if(strcmp(ent.name, path) == 0){
            return -1;
        }
    }
    int inode_num = ialloc();
    int data_block_num = alloc();
    struct inode *dir_in = iget(inode_num);
    dir_in->size = 64;
    dir_in->flags = 2;
    dir_in->inode_num = inode_num;
    dir_in->block_ptr[0] = data_block_num;

    bread(dir_in->block_ptr[0], dir_block);

    write_u16(dir_block, inode_num);
    strncpy((char *)dir_block + 2, ".", 16);
    strncpy((char *)dir_block + 18, "..", 16);
    
    bwrite(dir_in->block_ptr[0], dir_block);
    iput(dir_in);
    return 0;
}