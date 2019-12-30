#include "type.h"

/*
Opens file of pathname
Takes fd = openfile(path) and reads from it until read returns 0
Continues until all bytes are read
*/

void my_cat(char *path)
{
    int n, i;
    int fd = 0;
    char buf[1024];
    int size = 1024;

    //check for path, if nothing then print error message
    if(!path)
    {
        printf("Error, missing file name.\n");
        return;
    }

    strcpy(third, "0");//MODE is read
    fd = open_file(path);//opens the filename and returns the file descriptor
    my_pfd("");//calls print file descriptor

    while((n = read_helper(fd, buf, size)))//
    {
        buf[n] = '\0';//sets the last character to null terminator
        i = 0;
        while(buf[i])//while char is not null terminator loop
        {
            putchar(buf[i]);//prints the char that is in the buf to the screen
            if(buf[i] == '\n')//if newline
                putchar('\r');//print return
            i++;//get next char
        }
    }

    printf("\n\r"); 
    close_file(fd);//closes the file descriptor

    return;
}