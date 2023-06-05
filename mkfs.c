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

struct inode *namei(char *path) {
    if (strcmp(path, "/") == 0) {
        return iget(ROOT_INODE_NUM);
    }
    return NULL;
}


int directory_make(char *path) {
    char dir_path[1024];
    char *dirname = get_dirname(path, dir_path);
    char *basename = get_basename(path, dir_path);
    struct inode *parent_in = namei(dirname);
    if (parent_in == NULL) {
        return -1; // Parent directory doesn't exist
    }
    // Check if the new directory already exists
    struct directory_entry ent;
    struct directory *parent_dir = directory_open(parent_in->inode_num);
    while (directory_get(parent_dir, &ent) != -1) {
        if (strcmp(ent.name, basename) == 0) {
            directory_close(parent_dir);
            iput(parent_in);
            return -1; // Directory already exists
        }
    }
    int inode_num = ialloc();
    int data_block_num = alloc();
    struct inode *new_in = iget(inode_num);
    // Initialize the new directory inode
    new_in->size = 2 * sizeof(struct directory_entry); // Size of . and ..
    new_in->flags = 2; // Directory type
    new_in->inode_num = inode_num;
    new_in->block_ptr[0] = data_block_num;
    unsigned char dir_block[BLOCK_SIZE];
    bread(new_in->block_ptr[0], dir_block);
    // Add . and .. entries to the new directory data block
    write_u16(dir_block, inode_num);
    strncpy((char *)dir_block + sizeof(struct directory_entry), ".", sizeof(ent.name));
    strncpy((char *)dir_block + 2 * sizeof(struct directory_entry), "..", sizeof(ent.name));
    bwrite(new_in->block_ptr[0], dir_block);
    // Add the new directory entry to the parent directory
    struct directory_entry new_entry;
    new_entry.inode_num = inode_num;
    strncpy(new_entry.name, basename, sizeof(ent.name));
    int block_idx = parent_in->size / BLOCK_SIZE;
    bread(parent_in->block_ptr[block_idx], dir_block);

    int offset = parent_in->size % BLOCK_SIZE;
    struct directory_entry *entry_ptr = (struct directory_entry *)(dir_block + offset);
    *entry_ptr = new_entry;

    bwrite(parent_in->block_ptr[block_idx], dir_block);

    parent_in->size += sizeof(struct directory_entry);

    iput(new_in);
    iput(parent_in);
    directory_close(parent_dir);
    return 0;
}
