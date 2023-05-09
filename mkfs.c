#include "mkfs.h"
#include "free.h"
#include "inode.h"
#include "block.h"
#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void mkfs(void){
    
    unsigned char block[BLOCK_SIZE * 1024] = {0};
    write(image_fd, block, BLOCK_SIZE * 1024);
    for (int i = 0; i < 7; i++){
        alloc();
    }
}