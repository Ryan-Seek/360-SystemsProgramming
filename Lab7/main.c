#include "header.h"
int main(int argc, const char *argv[])
{
	char line[128], command[4], path[64], filepath[64],scndpath[64];
	int cmd;
	long tmp;
	int tmp2,tmp3;

	init();
	printf("input diskimage: ");
	fgets(line, 128, stdin);
	line[strlen(line)-1] = 0; 		// null terminate line
	mount_root(line);

	while(1){
		line[0]=0;
		printf("input command: ");
		fgets(line, 128, stdin);
		line[strlen(line)-1] = 0; // kill \r char at end
		if(line[0]==0)
			continue;
		path[0] = '\0';
		command[0]='\0';
		scndpath[0]='\0';
		sscanf(line, "%s %s %s", command, path,scndpath);
		cmd = findCmd(command);
		switch(cmd){ 
			case 0 :
				ls(path, running);
				break;
			case 1 :
				do_cd(path);
				break;
			case 2 :
				do_pwd();
				break;
			case 3 :
				printf("Exiting\n");
				exit_cleanup();
				exit(0);
				break;			
			default:
				printf("invalid command\n");
				break;
		} 
	} 
	return 0;
}