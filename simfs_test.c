#include "image.h"
#include "ctest.h"
#include "block.h"
#include "free.h"
#include "inode.h"
#include "mkfs.h"
#include <stdio.h>
#include <string.h>
#include <time.h>



// int check = image_open("test.img", 1);
// if (check < 0){
//     printf("image open");
//     return 1;
// }
// printf("image file opened successfully\n");
// check = image_close();
// if (check < 0){
//     printf("image close");
//     return 1;
// }
// printf("image file closed successfully\n");

#ifdef CTEST_ENABLE

void test_image_open(void){
    CTEST_ASSERT(image_open("test.img", 1) > 0, "testing image open");
}
void test_image_close(void){
    CTEST_ASSERT(image_close() == 0, "testing image close");
}
void test_ialloc(void){
    int inode1 = ialloc();
    // int inode2 = ialloc();
    CTEST_ASSERT(inode1 != -1, "testing ialloc");
    CTEST_ASSERT(inode1 >= 0 && inode1 < 4096, "testing ialloc");
    // CTEST_ASSERT(inode1 != inode2, "testing ialloc");
}
void test_mkfs(void){
    mkfs();
    int image_fd = image_open("tester.img", 1);
    CTEST_ASSERT(image_fd != -1, "testing mkfs");

    char buf[BLOCK_SIZE];
    memset(buf, 0, BLOCK_SIZE);
    
    for (int i = 0; i < BLOCK_SIZE; i++) {
        CTEST_ASSERT(buf[i] == 0, "testing mkfs");
    }
    for (int i = 1; i <= 6; i++) {
        memset(buf, 1, BLOCK_SIZE);
        CTEST_ASSERT((char)buf[i] == 1, "testing mkfs");
    }
    for (int i = 7; i < 1024; i++) {
        memset(buf, 0, BLOCK_SIZE);
        CTEST_ASSERT(buf[i] == 0x00, "testing mkfs");
    }
    image_close();
}
void test_bread_and_bwrite(void){
    unsigned char block[BLOCK_SIZE];
    bwrite(1, block);
    
    int memory_compare = memcmp(bread(1, block), block, BLOCK_SIZE);
    CTEST_ASSERT(memory_compare == 0, "bwrite / bread work correctly");
}

void test_alloc(void){
    int node = alloc();
    // int node2 = alloc();
    CTEST_ASSERT(node != -1, "testing alloc");
    CTEST_ASSERT(node >= 0 && node < 4096, "testing alloc");
    // CTEST_ASSERT(node != node2, "testing alloc");
    
}
void test_find_free(void){
    unsigned char block[BLOCK_SIZE];
    memset(block, 0, BLOCK_SIZE);
    CTEST_ASSERT(find_free(block) == 0, "testing find_free");

}
void test_find_low_clear_bit(void){
    unsigned char x = 0xAA;
    int bit = find_low_clear_bit(x);
    CTEST_ASSERT(bit == 0, "testing find_low_clear_bit");
    x = 0x55;
    bit = find_low_clear_bit(x);
    CTEST_ASSERT(bit == 1, "testing find_low_clear_bit");
    x = 0x00;
    bit = find_low_clear_bit(x);
    CTEST_ASSERT(bit == 0, "testing find_low_clear_bit");

}
void test_set_free(void){
    unsigned char block[BLOCK_SIZE];
    memset(block, 1, BLOCK_SIZE);
    set_free(block, 0, 1);
    CTEST_ASSERT(block[0] == 1, "testing set_free");
    set_free(block, 0, 0);
    CTEST_ASSERT(block[0] == 0, "testing set_free");
}
int main(void){
    CTEST_VERBOSE(1);
    test_image_open();
    test_image_close();
    test_ialloc();
    test_mkfs();
    test_bread_and_bwrite();
    // test_bwrite();
    test_alloc();
    test_find_free();
    test_find_low_clear_bit();
    test_set_free();  
    CTEST_RESULTS();
    CTEST_EXIT();
}
#else

int main(void){
    unsigned char block[BLOCK_SIZE];
    //unsigned char iblock[BLOCK_SIZE];
    printf("Running Normal\n");
    printf("image_open: %i\n", image_open("test.img", 1));
    printf("image_close: %d\n", image_close());
    printf("ialloc: %d\n", ialloc());
    
    printf("bread: %s\n", bread(0, block));
    bwrite(0, block);
    printf("Block written successfully\n");
    printf("alloc: %d\n", alloc());
    printf("find_free: %d\n", find_free(NULL));
    printf("find_low_clear_bit: %d\n", find_low_clear_bit(0));
    
}
#endif