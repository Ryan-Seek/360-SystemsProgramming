/* James Jessen             
 * 10918967                  
 *
 * CptS 360
 * PreLab #5
 */

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "stuff.h"

int main(int argc, char *argv[])
{ 
    int fd;
    char *disk = "mydisk";

    if (argc > 1)
        disk = argv[1];

    fd = open(disk, O_RDONLY);

    putchar('\n');


    GROUPD* gp = get_gd(fd);

    printf("block map:\t%u\n", gp->bg_block_bitmap);
    printf("inode map:\t%u\n", gp->bg_inode_bitmap);
    printf("inode_table:\t%u\n", gp->bg_inode_table);
    printf("free_blocks_count:\t%u\n", gp->bg_free_blocks_count);
    printf("free_inodes_count    %u\n", gp->bg_free_inodes_count);
    printf("used_dirs_count      %u\n", gp->bg_used_dirs_count);

    free(gp);

    putchar('\n');

    return 0;
}
