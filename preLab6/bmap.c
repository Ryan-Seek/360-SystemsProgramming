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
    
    char* bmap = get_bmap(fd);
    int blocks_count = get_blocks_count(fd);

    printf("\n\tBlock Bitmap\n");
    for(int i = 0; i < blocks_count; i++)
    {
        if(i && (i % (8 * 8)) == 0)
            putchar('\n');
        else if(i && (i % 8) == 0)
            putchar(' ');

        (test_bit(bmap, i)) ? putchar('1') : putchar('0');
    }
    putchar('\n');

    free(bmap);
    return 0;
}