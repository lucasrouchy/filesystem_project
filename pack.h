#ifndef PACK_H
#define PACK_H

#define INODE_NUM_SIZE 2
unsigned int read_u32(void *addr);
unsigned short read_u16(void *addr);
unsigned char read_u8(void *addr);
void write_u32(void *addr, unsigned long value);
void write_u16(void *addr, unsigned int value);
void write_u8(void *addr, unsigned char value);

#endif
