#include "inode.h"
#include "block.h"
#include "free.h"
int ialloc(void){
    unsigned char block[BLOCK_SIZE];
    int i = 0;
    while (1){
        bread(i, block);
        for (int j = 0; j < 4096; j++){
            if (block[j] != 0xff){
                int bit = find_low_clear_bit(block[j]);
                set_free(block, i * 8 + bit, 1);
                bwrite(i, block);
                return i * 8 + bit;
            }
        }
        i++;
    }

}
