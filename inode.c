#include "inode.h"
#include "block.h"
#include "free.h"
static struct inode incore[MAX_SYS_OPEN_FILES] = {0};

int ialloc(void){
    unsigned char block[BLOCK_SIZE] = {0};
    int bit_num = -1;
    // read inode map from disk
    bread(0, block);
    // find a free inode
    bit_num = find_free(block);
    if (bit_num != -1) {
        // mark inode as allocated
        set_free(block, bit_num, 1);
    }
    bwrite(0, block);
    return bit_num;
}
struct inode *find_incore_free(void){
    //goes through all inodes looking for indoes with ref_count == 0
    for (int i = 0; i < INODES_PER_BLOCK; i++) {
        if (incore[i].ref_count == 0) {
            return &incore[i];
        }
    }
    return NULL;
}
struct inode *find_incore(unsigned int inode_num){
    int i;
    for (i = 0; i < INODES_PER_BLOCK; i++) {
        if (incore[i].inode_num == inode_num && incore[i].ref_count != 0) {
            return &incore[i];
        }
    }
    return NULL;
}
