#include "inode.h"
#include "block.h"
#include "free.h"
#include "pack.h"

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
    
    for (int i = 0; i < INODES_PER_BLOCK; i++) {
        //checks for both ref count and inode number
        if (incore[i].inode_num == inode_num && incore[i].ref_count != 0) {
            return &incore[i];
        }
    }
    return NULL;
}

void read_inode(struct inode *in, int inode_num){
    unsigned char block[BLOCK_SIZE] = {0};
    
    int block_offset = inode_num % INODES_PER_BLOCK;
    int block_offset_bytes = block_offset * INODE_SIZE;
    bread(block_offset, block);
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
    in->inode_num = 0;
    
}
void write_inode(struct inode *in){
    unsigned char block[BLOCK_SIZE] = {0};
    
    int block_offset = in->inode_num % INODES_PER_BLOCK;
    int block_offset_bytes = block_offset * INODE_SIZE;
    //read the block containing the inode from disk
    bread(block_offset, block);
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
    bwrite(block_offset, block);
}