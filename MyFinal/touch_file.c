#include "type.h"

/*
Tries to get inode of pathname
if exists:
Changes access time

if not exists:
Creates file

Puts inode back on disk
*/

void touch_file(char path[124])
{
        int ino;
        int newmode = 0;
        MINODE *mip = running->cwd;
        MINODE *touchmip = NULL;
        INODE *ip = NULL;
        char fullpath[128];
        strcpy(fullpath, path);//copy the path into a holding variable

        
        if (!strcmp(path, "")) //checks to see if a file name was entered
        {
                printf("No file name given.\n");//no file was entered thus error out
                return;
        }

        ino = get_Inode(running->cwd, path);//
        if (ino != 0) //inode is not zero, thus it exists
        {
                touchmip = iget(dev, ino);
                ip = &touchmip->INODE;//sets the inode pointer to the memory inode pointer that touch file is
                
                ip->i_mtime = time(0L);//set the modify time to the current time
                touchmip->dirty = 1;//set dirty to 1 for the touch memory inode

                iput(touchmip);//disuper_pose of the memory inode 
                return;
        }
        else //file does not exist, hence we need to create it
                creat_file(fullpath);//create the file, pass in the full path that we saved earlier

        return;
}