#ifndef MKFS_H
#define MKFS_H
#define FREE_BLOCK_MAP_NUM 2
#define ROOT_INODE_NUM 0
#define ROOT_DATA_BLOCK_NUM 3
#define ROOT_DIR_INODE_NUM 1
#define ROOT_DIR_DATA_BLOCK_NUM 4
#define INODE_MAP_NUM 1
#define DIRECTORY_SIZE 32
#define DIRECTORY_ENTRY_COUNT 16
#define MAX_DIR_SPACE 128
#define INODE_SIZE 64
#include "inode.h"
#include "dirbasename.h"
void mkfs(void);
struct directory *directory_open(int inode_num);

struct directory {
    struct inode *inode;
    unsigned int offset;
};
struct directory_entry {
    unsigned int inode_num;
    char name[16];
};
int directory_get(struct directory *dir, struct directory_entry *ent);
void directory_close(struct directory *d);
struct inode *namei(char *path);
int directory_make(char *path);
#endif