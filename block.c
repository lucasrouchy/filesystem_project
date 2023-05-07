#include "block.h"
#include "free.h"
#include "image.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



unsigned char *bread(int block_num, unsigned char *block){

    if (lseek(image_fd, block_num * BLOCK_SIZE, SEEK_SET) == -1){
        perror("lseek");
        exit(1);
    }
    if (read(image_fd, block, BLOCK_SIZE)== -1){
        perror("read");
        exit(1);
    }
    return block;
}
void bwrite(int block_num, unsigned char *block){
    if(lseek(image_fd, block_num * BLOCK_SIZE, SEEK_SET)== -1){
        perror("lseek");
        exit(1);
    }
    if (write(image_fd, block, BLOCK_SIZE)== -1){
        perror("write");
        exit(1);
    }
}
int alloc(void){
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