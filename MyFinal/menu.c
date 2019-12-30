#include "type.h"


void menu()//prints the menu to the user
{
    printf(
            "======================================================\n"
			"				         MENU						   \n"
			"mkdir: [path] 			    creates a directory\n"
			"cat: [path]    			prints file\n"
			"cd: [path] 			    moves to directory\n"
			"chmod: [path] [Permission]	change file permissions\n"
			"close: [fd] 			    close an open file\n"
			"cp: [src] [dest] 		    copies a file\n"
			"creat: [path] 			    creates an empty file\n"
			"link: [src] [dest] 		hard links a file\n"
			"ls: [path] 			    lists a directory\n"
			"lseek: [fd] [nbytes] 		sets the offset to nbytes of a file\n"
			"menu: [no args] 		    prints this menu\n"
			"mv: [src] [dest] 		    moves a file\n"
			"open: [path] [mode] 		opens a file (0|1|2|3 for R|W|RW|AP)\n"
			"pwd: [no args] 			shows cwd\n"
			"read: [fd] [nbytes] 		read a file for nbytes\n"
			"rmdir: [path] 		    	removes an empty directory\n"
			"stat: [path] 			    shows file stat\n"
			"symlink: [src] [dest] 		symbolically links a file\n"
			"touch: [path] 			    changes mtime of a file\n"
			"unlink: [path] 			unlinks a file\n"
			"write: [fd] 			    writes to a file a string\n"
			"quit: [no args] 		    exits the program\n"
            "======================================================\n\n"

			);
}