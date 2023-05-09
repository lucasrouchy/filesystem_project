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
    unsigned char block[BLOCK_SIZE] = {0};
    int bit_num;
   
    bread(0, block);
    
    bit_num = find_free(block);
    if (bit_num != -1) {
        
        set_free(block, bit_num, 1);   
    }
    bwrite(0, block);
    return bit_num;
}