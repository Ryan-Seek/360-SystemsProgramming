#include <stdio.h>
#include <libgen.h>

typedef struct node{
          char  name[64];       // node's name string
          char  type;
   struct node *child, *sibling, *parent;
}NODE;


NODE *root, *cwd, *start;
char line[128];                   // for gettting user input line
char command[64], pathname[64];   // for command and pathname strings
char dname[64], bname[64];        // for dirname and basename 
char *name[100];                  // token string pointers 
int  n;                           // number of token strings in pathname 
FILE *fp;
char *cmd[] = {"mkdir", "rmdir", "ls", "cd", "pwd", "creat", "rm",
 "reload", "save", “menu”, "quit", NULL};


/**********************************************************************/
int tokenize(char *pathname)
{
    int i = 0;
    name[i] = strtok(path, "/"); // first call to strtok()
    i++;
    n+;

    while(s)
        {
        printf(“%s “, s);
        name[i] = strtok(0, "/"); // call strtok() until it returns NULL
        i++;
        n++;
        }

 
    return n = number of token strings
}

int findCmd(char *command)
{
    int i = 0;
    while(cmd[i]){
        if (!strcmp(command, cmd[i]))
            return i; // found command: return index i
        i++;
                }
 return -1; // not found: return -1
}

int dbname(char *pathname)
{
 char temp[128]; // dirname(), basename() destroy original pathname
 strcpy(temp, pathname);
 strcpy(dname, dirname(temp));
 strcpy(temp, pathname);
 strcpy(bname, basename(temp));
}

 
NODE *search_child(NODE *parent, char *name)
{ 
    NODE* temp = null;

    temp = parent->child;

    while(temp != NULL)
    {
        if(temp->name == name)
            return temp;
        temp = temp->sibling;
    }
        

    return 0;
}

NODE *path2node(char *pathname)
{
   return the node pointer of a pathname, or 0 if the node does not exist.  

   if pathname[0] = '/' start = root;
   else                 start = cwd;

   n = tokenize(pathname); // NOTE: pass a copy of pathname

   NODE *p = start;

   for (int i=0; i < n; i++){
       p = search_child(p, name[i]);
       if (p==0) return 0;            // if name[i] does not exist
   }
   return p;
}

bool mkdir(char *pathname)
{
    if(cwd = null)
        *cwd = new NODE();
        
}

bool rmdir(char* pathname)
{


}

bool ls(char* pathname)
{

}

bool rm(char* pathname)
{

}

bool cd(char* pathname)
{

}

bool pwd(void)
{
    //walk up the tree, concat dirnames into string and print that string out to the screen
}

bool creat(char* pathname)
{

}

bool rm(char* pathname)
{


} 

void save(FILE *filename)
{

}

void reload(FILE* filename)
{

}

bool menu(void)
{

}

bool quit(void)
{
    //calls save to file then exits the program

}
//tree list of lists, each node is either a file or a directory


int main()
{
    int index;
    char line[128], command[16], pathname[64];

    initialize(); //initialize root node of the file system tree

    while(1){
        printf("input a commad line : ");
        fgets(line,128,stdin);
        line[strlen(line)-1] = 0;
        sscanf(line, “%s %s”, command, pathname);

        int find = findCmd(command);
        index = find;
 
        switch(index){
            case 0 : mkdir(pathname); break;
            case 1 : rmdir(pathname); break;
            case 2 : ls(pathname); break;
            case 3 : cd(pathname); break;
            case 4 : pwd(); break;
            case 5 : creat(pathname); break;
            case 6 : rm(pathname); break;
            case 7 : reload(fp); break;
            case 8 : save(fp); break;
            case 9 : menu(); break;
            case 10 : quit(); break;
 
            default: printf(“invalid command %s\n”, command);
                    }
            }

 }