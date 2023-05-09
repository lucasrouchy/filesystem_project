#include "free.h"
#include <stdio.h>
#include "block.h"
int find_low_clear_bit(unsigned char x)
{
    for (int i = 0; i < 8; i++)
        if (!(x & (1 << i)))
            return i;
    
    return -1;
}
void set_free(unsigned char *block, int num, int set){
    int byte = num / 8;
    int bit = num % 8;

    if (set){
        block[byte] |= (1 << bit);
    }
    else{
        block[byte] &= ~(1 << bit);
    }
    
}

int find_free(unsigned char *block){

    for (int i = 0; i < BLOCK_SIZE; i++){

        int bit = find_low_clear_bit(block[i]);
        if (bit != -1){
            return (i * 8) + bit;
        }
    }
    return -1;            
}
