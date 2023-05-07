#include "mkfs.h"
#include "free.h"
#include "inode.h"
#include "block.h"
#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void mkfs(void){
      // Open the image file for writing
    int fd = open("image", O_WRONLY | O_CREAT | O_TRUNC);

    // Write 1024 blocks of all zeroes
    char buf[BLOCK_SIZE];
    memset(buf, 0, BLOCK_SIZE);
    for (int i = 0; i < 1024; i++) {
        write(fd, buf, BLOCK_SIZE);
    }

    // Mark blocks 0-6 as allocated
    for (int i = 0; i < 7; i++) {
        int block_no = alloc();
        if (block_no == -1) {
            printf("mkfs: allocation failed\n");
            exit(1);
        }
    }

    // Close the image file
    close(fd);
}