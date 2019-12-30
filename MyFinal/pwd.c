#include "type.h"

/*
Recursively traverses up parent directories (..)
Prints name
Stops at root
Base Case: Parent = curret inode (root)
*/

int my_pwd()
{
        char path[128] = "";
        char temp[128] = "";
        char name[64] = "";
        int ino, parent_ino;
        MINODE *mip = running->cwd; 

        //check if root
        if(mip == root)//check to see if we are in root
        {
                printf("/\n");
                strcpy(teststr, "/");
                return;
        }

        while(mip != root)// while we are not in the root
        {
                findino(mip, &ino, &parent_ino);//find the parent inode
                mip = iget(dev, parent_ino);//get the memory inode of parent
                findmyname(mip, ino, name);//find the name of the parent inode

                //use string commands to create the path by cat-ing onto the end of it the names that are found
                strcpy(temp, path);
                strcpy(path, "");
                strcat(path, "/");
                strcat(path, name);
                strcat(path, temp);
        }
        printf("%s\n", path);

        return 1;
}