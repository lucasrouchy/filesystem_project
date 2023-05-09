#include "inode.h"
#include "block.h"
#include "free.h"
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
