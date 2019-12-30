#include "type.h"

/*
Changes offset of OFT provided to it by the argument
*/

int my_lseek(char *path)
{
    int fd; 
    int position;
    OFT *oft;

    if(!path)//checks to see if a file was entered
    {
        printf("Error, a file name not given.\n");
        return;
    }
    if(!third)//checks to ee if there was a position entered
    {
        printf("Error, the position was not given.\n");
        return;
    }

    fd = atoi(path);//converts the path to an int for file descriptor
    position = atoi(third);//converts the third arguement to int for the position

    if(running->fd[fd] && running->fd[fd]->refCount > 0)//if the file descriptor exists and it's ref count is greater than zero(it isnt empty), then continue
    {
        oft = running->fd[fd];//get the open file table to the file desc

        
        if(position <= oft->inodeptr->INODE.i_size)//checks to see if the position is within the bounds of the file 
        {
                        
            oft->offset = position;//set the open file offset to the position that was entered
            running->fd[fd] = oft;//set the file descriptor to the current open file 
        }
        else
        {
            printf("Error, the position is out of bounds.\n");
            return -1;
        }
        return position;
    }
    else
        return -1;
}