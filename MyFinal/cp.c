#include "type.h"

/*
Opens source for read and dest for write
Reads from source and writes to dest
*/

void cp_file(char *path)
{
    int i = 0, j;
    int fdDest = 0;
    int fdSrc = 0;
    char buf[1024];
    char src[1024]; 
    char dest[1024];
    OFT *ofp;
    MINODE *mip; 
    INODE* ip;

    if (!strcmp(path, ""))//checks to see if source file was entered
    {
        printf("Error, there was no source super_pecified.\n");
        return;
    }

    strcpy(src, path);

    if (!strcmp(third, ""))//checks to see if no destination was entered
    {
        printf("Error, there was no destination super_pecified.\n");
        return;
    }
    strcpy(dest, third);//copies the third item that the user entered into the dest variable

    touch_file(dest);//call touch on the dest variable which will

    strcpy(third, "0");//copies zero(READ) into the third string so that the open file knows how to open it

    fdSrc = open_file(src);//opens the source file and save's its file descriptor

    strcpy(third, "1");//copies a one(WRITE) into third for destinaation

    fdDest = open_file(dest);//opens the destination file and save's its file descriptor
    my_pfd("");

    //loop until nothing left to read
    while ((i = read_helper(fdSrc, buf, BLKSIZE)))
    {
        write_helper(fdDest, buf, i);
    }

    ofp = running->fd[fdSrc];
	running->fd[fdSrc] = 0;

	ofp->refCount--;
	if(ofp->refCount == 0)
	{
		ofp->offset = 0;
		iput(ofp->inodeptr);
	}

	ofp = running->fd[fdDest];
	running->fd[fdDest] = 0;

	ofp->refCount--;
	if(ofp->refCount == 0)
	{
		ofp->offset = 0;
		iput(ofp->inodeptr);
	}

    my_pfd("");//calls pfd to disuper_play the file descriptors to the user

    return;
}