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

    INODE inode = get_inode(fd, 2);
    int block_size = get_block_size(fd);


    printf("\n\tDIR OF INODE: 2 \n");
    for(int i = 0; i < (inode.i_size / block_size); i++)
    {
        char* block = get_block(fd, inode.i_block[i]);
        char* cp = block; 
        DIR* dp = (DIR*)block;

        printf("i block[%d]\n", i);
        printf(" name  name_len  inode  rec_len\n");

        while (cp < (block + block_size))
        {
            printf(" %3u    %5u      %3u     %s\n", 
                    dp->name, dp->name_len, dp->inode, dp->rec_len);
            cp += dp->rec_len;       
            dp = (DIR*)cp;           
        } 
        putchar('\n');

        free(block);
    }

    return 0;
}
