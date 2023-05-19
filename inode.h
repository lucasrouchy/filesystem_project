#ifndef INODE_H
#define INODE_H
#define BLOCK_SIZE 4096
#define INODE_SIZE 64
#define INODE_FIRST_BLOCK 3
#define INODE_PTR_COUNT 16
#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)
#define MAX_SYS_OPEN_FILES 64

struct inode {
    unsigned int size;
    unsigned short owner_id;
    unsigned char permissions;
    unsigned char flags;
    unsigned char link_count;
    unsigned short block_ptr[INODE_PTR_COUNT];

    unsigned int ref_count;  // in-core only
    unsigned int inode_num;
};

int ialloc(void);


#endif