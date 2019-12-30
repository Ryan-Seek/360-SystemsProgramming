#include "type.h"

void my_pfd(char *path)
{
    int i;
    OFT *ofp;
    printf("\n  fd   mode   offset   device   inode \n");
    printf("  --   ----   ------   ------   ----- \n");

    //loops OFT
    for(i = 0; i < NOFT; i++)
    {
        ofp = &OpenFileTable[i];//pointer to the current item in the open file table

        if(ofp->refCount == 0)//if the current file has no references then return 
            return;

        printf("  %02d    ", i);

        switch(ofp->mode)//switch based upon what mode the file has
        {
        case 0:
            printf("RD");
            break;
        case 1:
            printf("WR");
            break;
        case 2:
            printf("RW");
            break;
        case 3:
            printf("AP");
            break;
        default:
            printf("--");
            break;
        }

        printf("    %6d     %2d     %5d\n", ofp->offset, ofp->inodeptr->dev, ofp->inodeptr->ino);//print statistics on the current file
    }
    return 0;
}