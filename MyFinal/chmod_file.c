#include "type.h"

/*
Load inode of file source pathname
Change (Bitmask) the permissions of the file to the given permissions
*/

void chmod_file(char path[124])
{
        int ino = 0;
        int newmode = 0;
        MINODE *mip = running->cwd;
        INODE *ip = NULL; 

        if (!strcmp(third, ""))//user did not provide a mode 
        {
                printf("Error, there was no mode given!\n");
                return;
        }

        newmode = (int) strtol(third, (char **)NULL, 8);//converts to octal int

        printf("Path = %s\n", path);
        ino = get_Inode(mip, path);//get inode number from file path provided

        if (ino)//if the inode is occupied
                mip = iget(dev, ino);
        else//if the inode is not occupied
        {
                strcpy(third, "");
                return;
        }
        ip = &mip->INODE;//set the inode to the memory inode pointer

        ip->i_mode = (ip->i_mode & 0xF000) | newmode;//set the mode of the inode

        mip->dirty = 1;//set dirty to one for the memory inode
        iput(mip);//put the memory inode back
        strcpy(third, "");
}