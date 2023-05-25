#include "inode.h"
#include "block.h"
#include "free.h"
#include "pack.h"
struct inode incore[MAX_SYS_OPEN_FILES] = {0};

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
    
    for (int i = 0; i < INODES_PER_BLOCK; i++) {
        //checks for both ref count and inode number
        if (incore[i].inode_num == inode_num && incore[i].ref_count == 0) {
            return &incore[i];
        }
    }
    return NULL;
}

void read_inode(struct inode *in, int inode_num){
    unsigned char block[BLOCK_SIZE] = {0};
    
    int block_offset = inode_num % INODES_PER_BLOCK;
    int inode_block = inode_num / INODES_PER_BLOCK + FIRST_INODE_BLOCK;
    int block_offset_bytes = block_offset * INODE_SIZE;
    bread(inode_block, block);
    //reads the inode and maps it to the in inode struct from the argument
    in->size = read_u32(block + block_offset_bytes);
    in->owner_id = read_u16(block + block_offset_bytes + 4);
    in->permissions = read_u8(block + block_offset_bytes + 6);
    in->flags = read_u8(block + block_offset_bytes + 7);
    in->link_count = read_u8(block + block_offset_bytes + 8);
    //reads the block pointers
    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        in->block_ptr[i] = read_u16(block + block_offset_bytes + 9 + i * 2);
    }
    //not sure if I should set these to zero since they are incore variables or if I should read them like the other on disk variables.
    in->ref_count = 0;
    in->inode_num = inode_num;
    
}
void write_inode(struct inode *in){
    unsigned char block[BLOCK_SIZE] = {0};
    
    int block_offset = in->inode_num % INODES_PER_BLOCK;
    int inode_block = in->inode_num / INODES_PER_BLOCK + FIRST_INODE_BLOCK;
    int block_offset_bytes = block_offset * INODE_SIZE;
    //read the block containing the inode from disk
    bread(inode_block, block);
    //writes the inode to the block
    write_u32(block + block_offset_bytes, in->size);
    write_u16(block + block_offset_bytes + 4, in->owner_id);
    write_u8(block + block_offset_bytes + 6, in->permissions);
    write_u8(block + block_offset_bytes + 7, in->flags);
    write_u8(block + block_offset_bytes + 8, in->link_count);
    //writes the block pointers
    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        write_u16(block + block_offset_bytes + 9 + i * 2, in->block_ptr[i]);
    }
    bwrite(inode_block, block);
}

struct inode *iget(int inode_num){
    struct inode *in = NULL;
    //search for the inode number incore
    in = find_incore(inode_num);
    //if found
    if(in != NULL){
        //increment the ref count and return the pointer
        in->ref_count++;
        return in;
    }
    else{
        //find a free inode incore
        in = find_incore_free();
        //if none found
        if(in == NULL){
            //return null
            return NULL;
        }
        // read the data from the disk into read_inode
        read_inode(in, inode_num);
        //set the ref count to 1 and the inode number to the argument
        in->ref_count = 1;
        in->inode_num = inode_num;
        return in;
    }
}

void iput(struct inode *in){
    if(in->ref_count == 0){
        return;
    }
    //decrement the ref count
    in->ref_count--;
    //if the ref count is 0
    if(in->ref_count == 0){
        //write the inode to disk
        write_inode(in);
    }
}

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
    else{
        return -1;
    }
    //get an incore version of the inode
    struct inode *in = iget(bit_num);
    //set the size, owner id, permissions, flags to 0
    in->size = 0;
    in->owner_id = 0;
    in->permissions = 0;
    in->flags = 0;
    //set the block pointers to 0
    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        in->block_ptr[i] = 0;
    }
    //set the inode number argument to the inode number
    in->inode_num = bit_num;

    write_inode(in);
    
    return bit_num;
}