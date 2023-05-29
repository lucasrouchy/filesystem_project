#ifndef MKFS_H
#define MKFS_H

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
#endif