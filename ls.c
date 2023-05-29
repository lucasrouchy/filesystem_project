#include "ls.h"
#include "block.h"
#include "free.h"
#include "inode.h"
#include "pack.h"
#include "mkfs.h"
#include <stdio.h>

void ls(void)
{
    struct directory *dir;
    struct directory_entry ent;

    dir = directory_open(ROOT_INODE_NUM);

    while (directory_get(dir, &ent) != -1)
        printf("%d %s\n", ent.inode_num, ent.name);

    directory_close(dir);
}
