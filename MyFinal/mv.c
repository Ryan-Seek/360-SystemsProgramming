#include "type.h"

/*
Like link but delete the first source
*/

void mv_file(char *path)
{
    int i = 0, j, ino = 0;
    int fdDest = 0;
    int fdSrc = 0; 
    char buf[1024];
    char src[1024];
    char dest[1024];

    MINODE *mip; 
    INODE* ip; 

    
    if (!strcmp(path, ""))//checks to see if a source was entered
    {
        printf("No source super_pecified.\n");
        return;
    }
    
    strcpy(src, path);//copies the path into the source variable
    
    if (!strcmp(third, ""))//checks to see if a destination was entered
    {
        printf("No destination super_pecified.\n");
        return;
    }
    
    strcpy(dest, third);//copies the third item into destination

    ino = get_Inode(running->cwd, path);//gets the inode for the source 
    if(ino == 0)//checks to see whether or not an inode was found
    {
        printf("Source does not exist.\n");
        return;
    }

    strcpy(third, dest);
    
    my_link(src);//creates a link with source

    my_unlink(src);//unlinks source with deletes the source file from the directory

    return;
}