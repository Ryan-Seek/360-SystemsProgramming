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

    SUPER* sp = get_super(fd);
    for(int i =0; i < 5; i++)
    {
        int blocknum = balloc(fd);
        printf("allocated block number= %d\n", blocknum);
    }


    putchar('\n');
    free(sp);
    

    return 0;
}
