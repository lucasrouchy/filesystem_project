#define CTEST_MAIN
#include "image.h"
#include "ctest.h"
#include "block.h"
#include "free.h"
#include "inode.h"
#include "pack.h"
#include "mkfs.h"
#include "ls.h"
#include <stdio.h>
#include <string.h>
#include <time.h>



#ifdef CTEST_ENABLE

void test_make_directory(void){
    image_open("make_directory", 1);
    mkfs();
    int make = directory_make("/");
    CTEST_ASSERT(make == 0, "testing make_directory");
    image_close();
}

void test_image_open(void){
    CTEST_ASSERT(image_open("test.img", 1) > 0, "testing image open");
}

void test_image_close(void){
    CTEST_ASSERT(image_close() == 0, "testing image close");
}

void test_ialloc(void){
    unsigned char block[BLOCK_SIZE] = { 0 };
    int bit_num;
    int allocated_bit_num;
    image_open("test img", 1);
    bread(1, block);
    bit_num = find_free(block);
    allocated_bit_num = ialloc();
    CTEST_ASSERT(bit_num == allocated_bit_num, "testing ialloc");
    CTEST_ASSERT(allocated_bit_num != -1, "testing ialloc");

    for(int i=0; i < BLOCK_SIZE; i++) { 
        block[i] = 255;
    }
    bwrite(1, block);
    int full_bit_num = ialloc();
    CTEST_ASSERT(full_bit_num == -1, "testing ialloc");
    image_close();
}

void test_find_incore_free(void){
    incore[0].ref_count = 1;
    incore[1].ref_count = 0;
    incore[1].inode_num = 2;
    incore[2].ref_count = 2;
    incore[3].ref_count = 0;
    incore[4].ref_count = 0;
    struct inode *free_inode = find_incore_free();
    CTEST_ASSERT(free_inode !=NULL, "testing find_incore_free");
    CTEST_ASSERT(free_inode->ref_count == 0, "testing find_incore_free");
    CTEST_ASSERT(free_inode->inode_num == 2, "testing find_incore_free");
}

void test_find_incore(void){ 
    struct inode *f_inode = find_incore(2);
    CTEST_ASSERT(f_inode != NULL, "testing find_incore");
    CTEST_ASSERT(f_inode->ref_count == 0, "testing find_incore");
    CTEST_ASSERT(f_inode->inode_num == 2, "testing find_incore");
}

void test_read_inode(void){
   image_open("read inode", 1); 
   mkfs(); 
   struct inode in;
   in.inode_num = 2;
   unsigned char block[BLOCK_SIZE] = { 0 };
   int block_offset = in.inode_num % INODES_PER_BLOCK;
   int inode_block = in.inode_num / INODES_PER_BLOCK  + FIRST_INODE_BLOCK;
   int block_offset_bytes = block_offset * INODE_SIZE; 
   write_u32(block + block_offset_bytes, 10);
   write_u16(block + block_offset_bytes + 4, 20);
   write_u8(block + block_offset_bytes + 6, 1);
   write_u8(block + block_offset_bytes + 7, 3);
   write_u8(block + block_offset_bytes + 8, 5); 

   for(int i = 0; i < INODE_PTR_COUNT; i++){
       write_u16(block + block_offset_bytes + 9 + i*2, 10 + i);
   }
   bwrite(inode_block, block);
   read_inode(&in, 2);
   CTEST_ASSERT(in.size == 10, "testing read_inode");
   CTEST_ASSERT(in.owner_id == 20, "testing read_inode");
   CTEST_ASSERT(in.permissions == 1, "testing read_inode");
   CTEST_ASSERT(in.flags == 3, "testing read_inode");
   CTEST_ASSERT(in.link_count == 5, "testing read_inode");
    for(int i = 0; i < INODE_PTR_COUNT; i++){
         CTEST_ASSERT(in.block_ptr[i] == 10 + i, "testing read_inode");
    }
    image_close();  
}

void test_write_inode(void){
    image_open("write inode", 1);
    mkfs();    
    struct inode in;
    in.inode_num = 2;
    in.size = 10;
    in.owner_id = 20;
    in.permissions = 1;
    in.flags = 3;
    in.link_count = 5;
    for(int i = 0; i < INODE_PTR_COUNT; i++){
        in.block_ptr[i] = 10 + i;
    }
    unsigned char block[BLOCK_SIZE] = { 0 };
    int block_offset = in.inode_num % INODES_PER_BLOCK;
    int inode_block = in.inode_num / INODES_PER_BLOCK  + FIRST_INODE_BLOCK;
    int block_offset_bytes = block_offset * INODE_SIZE; 
    write_inode(&in);

    bread(inode_block, block);
    CTEST_ASSERT(read_u32(block + block_offset_bytes) == 10, "testing write_inode");
    CTEST_ASSERT(read_u16(block + block_offset_bytes + 4) == 20, "testing write_inode");
    CTEST_ASSERT(read_u8(block + block_offset_bytes + 6) == 1, "testing write_inode");
    CTEST_ASSERT(read_u8(block + block_offset_bytes + 7) == 3, "testing write_inode");
    CTEST_ASSERT(read_u8(block + block_offset_bytes + 8) == 5, "testing write_inode");
    for(int i = 0; i < INODE_PTR_COUNT; i++){
        CTEST_ASSERT(read_u16(block + block_offset_bytes + 9 + i*2) == 10 + i, "testing write_inode");
    }
    image_close();
}

void test_iget(void){
    image_open("iget", 1);
    mkfs();
    struct inode *in = iget(2);
    CTEST_ASSERT(in->ref_count == 1, "testing iget");
    CTEST_ASSERT(in->inode_num == 2, "testing iget");
    CTEST_ASSERT(in->size == 0, "testing iget");
    CTEST_ASSERT(in->owner_id == 0, "testing iget");
    CTEST_ASSERT(in->permissions == 0, "testing iget");
    CTEST_ASSERT(in->flags == 0, "testing iget");
    CTEST_ASSERT(in->link_count == 0, "testing iget");
    for(int i = 0; i < INODE_PTR_COUNT; i++){
        CTEST_ASSERT(in->block_ptr[i] == 0, "testing iget");
    }
    image_close();
}

void test_iput(void){
    image_open("iput", 1);
    mkfs();
    struct inode *in = iget(2);
    iput(in);
    CTEST_ASSERT(in->ref_count == 0, "testing iput");
    image_close();
}

void test_ialloc2(void){
    image_open("ialloc2", 1);
    mkfs(); 
    unsigned int bit_num = ialloc();
    unsigned int tester = -1;
    CTEST_ASSERT(bit_num != tester, "testing ialloc");

    struct inode *in = iget(bit_num);
    CTEST_ASSERT(in != NULL, "testing ialloc");
    CTEST_ASSERT(in->ref_count == 1, "testing ialloc");
    CTEST_ASSERT(in->inode_num == bit_num, "testing ialloc");
    CTEST_ASSERT(in->size == 0, "testing ialloc");
    CTEST_ASSERT(in->owner_id == 0, "testing ialloc");
    CTEST_ASSERT(in->permissions == 0, "testing ialloc");
    CTEST_ASSERT(in->flags == 0, "testing ialloc");
    CTEST_ASSERT(in->link_count == 0, "testing ialloc");
    for(int i = 0; i < INODE_PTR_COUNT; i++){
        CTEST_ASSERT(in->block_ptr[i] == 0, "testing ialloc");
    }
    image_close();
}

void test_mkfs(void){
    unsigned char dir_block[BLOCK_SIZE];
    unsigned char expected_block[BLOCK_SIZE] = { 0 };
    
    image_open("tester.img", 1);
    int inode_num = ialloc();
    int data_block_num = alloc();
    struct inode *root_in = iget(inode_num);
    root_in->size = 64;
    root_in->flags = 2;
    root_in->inode_num = 0;
    root_in->block_ptr[0] = data_block_num;

    iput(root_in);

    bread(2, dir_block);

    write_u16(dir_block, inode_num);
    strcpy((char *)dir_block + 18, ".");
    strcpy((char *)dir_block + 34, "..");

    bwrite(2, dir_block);
    iput(root_in);

    expected_block[0] = inode_num & 0xFF; 
    expected_block[1] = (inode_num >> 8) & 0xFF; 
    strcpy((char *)expected_block + 18, ".");
    strcpy((char *)expected_block + 34, "..");

    int mem_comp = memcmp(dir_block, expected_block, BLOCK_SIZE);
    CTEST_ASSERT(mem_comp == 0, "Testing mkfs");
    image_close();
}

void test_directory_open(void){
    image_open("directory_open", 1);
    mkfs();
    struct directory *dir = directory_open(2);
    CTEST_ASSERT(dir != NULL, "testing directory_open");
    CTEST_ASSERT(dir->inode->inode_num == 2, "testing directory_open");
    CTEST_ASSERT(dir->offset == 0, "testing directory_open");
    directory_close(dir);
    image_close();
}

void test_directory_get(void){
    image_open("directory_get", 1);
    mkfs();
    struct directory *dir = directory_open(0);
    struct directory_entry ent;
    int get = directory_get(dir, &ent);
    CTEST_ASSERT(get == 0, "testing directory_get");
    CTEST_ASSERT(ent.inode_num == 0, "testing directory_get");

    //I'm pretty sure my offset math is correct for where the directory name should be but this test still fails
    CTEST_ASSERT(strcmp(ent.name, ".") == 0, "testing directory_get");
    get = directory_get(dir, &ent);
    get = directory_get(dir, &ent);
    get = directory_get(dir, &ent);
    get = directory_get(dir, &ent);
    
    
    CTEST_ASSERT(get == -1, "testing directory_get");

    directory_close(dir);
    image_close();
}

void test_directory_close(void){
    image_open("directory_close", 1);
    mkfs();
    struct directory *dir = directory_open(2);
    struct inode *in = dir->inode;
    int offset = dir->offset;
    int ref_count = in->ref_count;
    directory_close(dir);
    
    CTEST_ASSERT(offset == 0, "testing directory_close");
    CTEST_ASSERT(ref_count == 1, "testing directory_close");
    
    image_close();
}

void test_ls(void){
    image_open("ls", 1);
    mkfs();
    ls();
    image_close();
}

void test_namei(void){
    image_open("namei", 1);
    mkfs();
    struct inode *in = namei("/");
    CTEST_ASSERT(in != NULL, "testing namei");
    struct inode *fake = namei("/fake");
    CTEST_ASSERT(fake == NULL, "testing namei");
    iput(in);
    image_close();
}



void test_bread_and_bwrite(void){
    image_open("bread and bwrite", 1);
    unsigned char block[BLOCK_SIZE];
    bwrite(1, block);
    
    int memory_compare = memcmp(bread(1, block), block, BLOCK_SIZE);
    CTEST_ASSERT(memory_compare == 0, "bwrite / bread work correctly");
    image_close();
}

void test_alloc(void){
    unsigned char block[BLOCK_SIZE] = { 0 };
    int bit_num;
    int allocated_bit_num;
    image_open("test img", 1);
    bread(2, block);
    bit_num = find_free(block);
    allocated_bit_num = alloc();
    CTEST_ASSERT(bit_num == allocated_bit_num, "testing ialloc");
    CTEST_ASSERT(allocated_bit_num != -1, "testing ialloc");

    for(int i=0; i < BLOCK_SIZE; i++) { 
        block[i] = 0xFF;
    }
    bwrite(2, block);
    int full_bit_num = alloc();
    CTEST_ASSERT(full_bit_num == -1, "testing ialloc");
    image_close();
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
    test_make_directory();
    test_image_open();
    test_image_close();
    test_ialloc();
    test_find_incore_free();
    test_find_incore();
    test_read_inode();
    test_write_inode();
    test_iget();
    test_iput();
    test_ialloc2();
    test_mkfs();
    test_directory_open();
    test_directory_get();
    test_directory_close();
    test_ls();
    test_namei();
    
    test_bread_and_bwrite();
    // // test_bwrite();
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