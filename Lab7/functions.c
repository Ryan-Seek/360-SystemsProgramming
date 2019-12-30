#include "header.h"
PROC *running;
PROC *p0;
PROC *p1;

MINODE *root;
MINODE minode[100];

int fd;

char buff[BLOCK_SIZE];
char read_buff[BLOCK_SIZE]; // A special buff for reads


void init() 
{
	int i;
	p0 = calloc(1, sizeof(PROC));
	p0->cwd = 0;
	p0->uid = 0;

	p1 = calloc(1, sizeof(PROC));
	p1->cwd = 0;
	p1->uid = 1;

	root = 0;

	sp = calloc(1, sizeof(SUPER));
	gp = calloc(1, sizeof(GD));
	ip = calloc(1, sizeof(INODE));

	for (i = 0; i < 100; i++) {
		minode[i].refCount = 0;
	}

	return;
} 

/* takes in a diskname path and opens it, stores superblock and group descriptor
 * block is sp and gp respectively, also check to see if file is ext2 and exits
 * if it is not sets root to root of fd and sets running proc
 */
void mount_root( char *path ) 
{
	fd = open(path, O_RDWR );
	lseek(fd, BLOCK_SIZE, SEEK_SET );
	read(fd, sp, sizeof(SUPER));
	lseek(fd, BLOCK_SIZE*GDBLOCK, SEEK_SET);
	read(fd, gp, sizeof(GD));

	if(sp->s_magic != SUPER_MAGIC){
		printf("%x\n", sp->s_magic);
		printf("%d\n",fd);
		perror("NOT AN EXT2FS EXITING");
		exit(-1);
	}
	root = iget(fd, 2);
	p1->cwd = root;
	root->refCount = 2;
	running = p1;
} 

/* Cleans up memory, puts all inodes, and exits the program
 *
 */
void exit_cleanup()
{
    int i = 0;
    //put all dirty inodes
    for (i = 0; i<100;i++)
    {
        if (minode[i].ino != 0)
            iput(&minode[i]);

    }

}
/* this function looks for the inode in the minode array and returns.  if it
 * cannot find the minode it loads it from dev and returns the location in
 * minode array.
 */
MINODE *iget(int dev, unsigned long ino) 
{
	int i=0, freeNode=0, block=0, pos=0;
	freeNode=0;
	for (i = 0; i < 100; i++) {
		if(minode[i].ino == 0){
			if(freeNode == 0)
				freeNode = i;
		}
		if(minode[i].ino == ino){
			minode[i].refCount ++;
			return &minode[i];
		}
	}

	// else it is not in the minode array so find it on the disk
	block = (ino-1) / INODES_PER_BLOCK + gp->bg_inode_table;
	pos = (ino-1) % INODES_PER_BLOCK;
	get_block(fd, block, (char *)&buff);

	minode[freeNode].dev = dev;
	minode[freeNode].dirty = 0;
	minode[freeNode].ino = ino;
	memcpy(&minode[freeNode].INODE, (((INODE*)buff) + pos),sizeof(INODE));
	minode[freeNode].mounted = 0;
	minode[freeNode].mountptr = NULL;
	minode[freeNode].refCount = 1;

	return &minode[freeNode];
} 

/* takes a minode * and writes it to fd
 */
void iput(MINODE *mip) 
{
	int block, pos;
	mip->refCount--;
	INODE *itmp;
	if(mip->refCount > 0)
		return;
	if(mip->dirty == 0)
		return;

    block = (mip->ino - 1) / INODES_PER_BLOCK + gp->bg_inode_table;
	pos = (mip->ino - 1) % INODES_PER_BLOCK;

	itmp = ( (INODE *)buff + pos);

	get_block(fd, block, (char *)&buff);
	memcpy( itmp, &mip->INODE, 128 );
	put_block(fd, block, buff);
	return;
} 

/* gets a block of data from dev and stores it to buf
 */
void get_block(int dev, unsigned long block, char *buf)
{
	lseek(dev,(long)BLOCK_SIZE*block,SEEK_SET);
	read(dev, buf, BLOCK_SIZE);
} 

/* puts a block of data from buf and stores it to fd
 */
void put_block(int dev, int block, char *buf) 
{
	lseek(dev, BLOCK_SIZE*block, SEEK_SET);
	write(dev, buf, BLOCK_SIZE);
	return;
} 

/* traverses the pathname to the last point in pathaname and returns the inode
 * number of that inode
 * TODO error checking, and cases
 */
unsigned long getino(int dev, char *pathname) 
{
	INODE *cwd = malloc(sizeof(INODE));
	char path[128], *token;
	int inodeIndex, seek;
	strncpy(path, pathname, 128);

	if(pathname[0] == '/'){
		strncpy(path, path+1, 127); // remove root symbol
        inodeIndex = 2;
		token = strtok(path, "/");
		//cwd = &root->INODE;
		memcpy(cwd, &root->INODE, sizeof(INODE));

		while(token !=NULL){
			inodeIndex = search(cwd, token);
			if(inodeIndex == 0)
				return 0;
			seek = ((inodeIndex-1) / 8 + gp->bg_inode_table)*BLOCK_SIZE +
				(inodeIndex-1)%8 * 128;
			lseek(fd, seek, SEEK_SET);

			read(fd, cwd, sizeof(INODE));
			token = strtok(NULL, "/");
		}
		return inodeIndex;
	}else {
		token = strtok(path, "/");
		//cwd = &running->cwd->INODE;
		memcpy(cwd, &running->cwd->INODE, sizeof(INODE));

		while(token != NULL) {
			inodeIndex = search(cwd, token);
			if(inodeIndex == 0){
				perror("cannot find that inode");
				return 0;
			}
			seek = ((inodeIndex-1) / 8 + gp->bg_inode_table)*BLOCK_SIZE +
				(inodeIndex-1)%8 * 128;
			lseek(dev, seek, SEEK_SET);
			read(fd, cwd, sizeof(INODE));
			token = strtok(NULL, "/");
		}
		return inodeIndex;
	}

} 

/* unused function to print the binary of a block of data in dev
 */
void print_block(int dev, int block) 
{
	int i,j;
	char arr[1024];
	lseek(dev, block*BLOCK_SIZE, SEEK_SET);
	for (i = 0; i < 1024; i++) {
		for (j = 0; j < 8; j++) {
			printf("%d ", arr[i] & 1 << j);
		}
	}
} 

/* takes in a command and checks it to the list of commands to return an integer
 * value to use in the switch statement in main
 */
int findCmd(char *cname) 
{
	if(strcmp(cname, "ls")==0)
		return 0;
	if(strcmp(cname, "cd")==0)
		return 1;
	if(strcmp(cname, "pwd")==0)
		return 2;
	if(strcmp(cname, "quit")==0)
		return 3;
	return -1;
} 

unsigned long search(INODE *inodePtr, char *name) 
{
	DIR *dp = (DIR *) buff;
	int i,j;
	char *cp, tmp[256];
	for (i = 0; i < 12; i++) {
		cp = buff;
		dp = (DIR *) buff;
		lseek(fd, inodePtr->i_block[i]*BLOCK_SIZE, SEEK_SET);
		read(fd, buff, 1024);

		while(cp < buff + 1024){
			for (j = 0; j < dp->name_len; j++) {
				tmp[j]=(char)dp->name[j];
			}
			tmp[j] = 0;
			if(strcmp(name, tmp) == 0 ) {
				return dp->inode;
			}else{
				cp += dp->rec_len;
				dp = (DIR *)cp;
				if((dp->inode == 0) && (inodePtr->i_block[i+1] == 0))//gotta check if the next block is available
					return 0;
			}
		}
	}
	return 0;
} 

/* finds and allocates a free ibitmap bit to correspond to the inode blocks and
 * returns the bit number
 * working dont touch
 */
unsigned long ialloc(int dev) 
{
	int i;
	lseek(dev, BLOCK_SIZE*IBITMAP, SEEK_SET);

	get_block(dev, IBITMAP, buff);

	for (i = 1; i < 1024*8; i++) {
		if(tstbit(buff, i)==0){
			setbit(buff, i);
			put_block(dev, IBITMAP, buff);
			return (i+1);
		}
	}
	return 0;
} 
//Allocates a OFT pointer and assigns it to the next availble spot in the running process
int falloc(OFT* oftp) 
{
	int i = 0;
	for(i=0;i<10;i++)
	{
		if (running->fd[i]==NULL)
			break;
	}
	if (i==10)
	{
		return -1;
	}
	running->fd[i]=oftp;
	return i;
} 

/* deallocate the ino that was allocated
 */
void idealloc(int dev, unsigned long ino) 
{
	get_block(dev, IBITMAP, buff);
	clearbit(buff, ino-1);
	put_block(dev, IBITMAP, buff);
	return;
} 

/* deallocates all of the dataBlocks for an inode */
void deallocateInodeDataBlocks(int dev, MINODE* mip) 
{
	char bitmap[1024],dblindbuff[1024];
	int i = 0;
	int j = 0;
	int indblk,dblindblk;
	unsigned long *indirect,*doubleindirect;
	get_block(dev,BBITMAP,bitmap);
	for ( i = 0; i<12; i++)
	{
		if (mip->INODE.i_block[i]!=0)
		{
			clearbit(bitmap, mip->INODE.i_block[i]-1);
			mip->INODE.i_block[i]=0;
		}
		else
		{
			put_block(dev,BBITMAP,bitmap);
			return ;
		}
	}
	// on to indirect blocks
	if (mip->INODE.i_block[i]!=0)
	{
		indblk = mip->INODE.i_block[i];
		get_block(dev,indblk,buff);
		indirect = (unsigned long *)buff;
		for (i=0;i<256;i++)
		{
			if(*indirect != 0)
			{
				clearbit(bitmap, *indirect-1);
				*indirect = 0;
				indirect++;
			}
			else
			{
				clearbit(bitmap, indblk-1);
				put_block(dev,indblk,buff);
				put_block(dev,BBITMAP,bitmap);
				mip->INODE.i_block[12] = 0;
				return;
			}
		}
	}
	else
	{
		put_block(dev,BBITMAP,bitmap);
		return;
	}
	//then double indirect
	if (mip->INODE.i_block[13]!=0)
	{
		dblindblk = mip->INODE.i_block[13];
		get_block(dev,dblindblk,dblindbuff);
		doubleindirect = (unsigned long *)dblindbuff;
		for (i=0;i<256;i++)
		{
			indblk = *doubleindirect;
			get_block(dev,indblk,buff);
			indirect = (unsigned long *)buff;
			for (j=0;j<256;j++)
			{
				if(*indirect != 0)
				{
					clearbit(bitmap, *indirect-1);
					*indirect = 0;
					indirect++;
				}
				else
				{
					clearbit(bitmap, indblk-1);
					clearbit(bitmap, dblindblk-1);
					put_block(dev,indblk,buff);
					put_block(dev,BBITMAP,bitmap);
					put_block(dev,dblindblk,dblindbuff);
					mip->INODE.i_block[13] = 0;
					return;
				}
				clearbit(bitmap, indblk-1);

			}
			doubleindirect++;
			if (*doubleindirect == 0)
			{//edge case handling
				clearbit(bitmap, indblk-1);
				clearbit(bitmap, dblindblk-1);
				put_block(dev,indblk,buff);
				put_block(dev,BBITMAP,bitmap);
				put_block(dev,dblindblk,dblindbuff);
				mip->INODE.i_block[13] = 0;
				return;
			}
		}
	}
	else
	{
		put_block(dev,BBITMAP,bitmap);
		return;
	}

} 

//an extension of the deallocate =InodeDataBlocksFunction
void do_truncate(int dev, MINODE *mip) 
{
	deallocateInodeDataBlocks(dev,mip);
	mip->INODE.i_atime = mip->INODE.i_mtime = time(0L);
	mip->INODE.i_size = 0;
	mip->dirty = 1;


} 

/* finds and allocates a free blockbitmap bit for data, returns the number found
 * working dont touch
 */
unsigned long balloc( int dev ) 
{
	int iter = 0, inodeCount = 0;
	char buf[1024];

	inodeCount = sp->s_blocks_count; // /	(BLOCK_SIZE*128);

	get_block( dev, (gp->bg_block_bitmap),(char*)&buf );

	for( iter = 0; iter < inodeCount; iter++  )
	{
		if (tstbit((char*)&buf,iter) ==0 )
		{
			setbit((char*)&buf,iter );
			put_block(dev,gp->bg_block_bitmap,(char*)&buf);
			printf("BALLOCRETURNING:%d\n",iter+1);
			return iter+1;
		}
	}

	return -1;
} 

/* deallocate the ino that was allocated in block bitmap
 */
void bdealloc(int dev, unsigned long block) 
{
	get_block(dev, BBITMAP, buff);
	clearbit(buff, block-1);
	put_block(dev, BBITMAP, buff);
} 

void ls(char *pathname, PROC *parent) 
{
	INODE *cwd = calloc(sizeof(INODE), 1);
	char path[128] = "\0";
	strncpy(path, pathname, 128);
	int inodeIndex, seek;
	// if ls fullpath
	if(pathname[0] == '/') {  
		strncpy(path, path+1, 127);
		char *token = strtok(path, "/");
		memcpy(cwd, &root->INODE,sizeof(INODE));

		while(token !=NULL) {
			inodeIndex =search(cwd, token);
			if (inodeIndex==0) {
				printf("cannot find that directory\n");
				return;
			}
			seek =((inodeIndex-1)/8 + gp->bg_inode_table)*1024+(inodeIndex-1)%8
				*128;

			lseek(fd,seek,SEEK_SET);
			read(fd,cwd,sizeof(INODE));
			token=strtok(NULL,"/");
		}
		// check if path is a dir
		if((cwd->i_mode & 0040000) != 0040000){
			printf("cannot ls non dir");
			return;
		}

		printdir(cwd);
		return;
	} 

	// if ls cwd
	else if(pathname[0]	<=0){
		printf("current dir: Ino %lu, Iblock[0]= %lu\n",(long unsigned
					int)parent->cwd->ino, (long unsigned int)parent->cwd->INODE.i_block[0]);
		printdir(&parent->cwd->INODE);
		return;
	}
	// if ls local path
	else{
		char *token=strtok(path,"/");
		memcpy(cwd,	&parent->cwd->INODE,sizeof(INODE));

		while(token!=NULL){
			inodeIndex=search(cwd,token);
			if (inodeIndex==0) {
				printf("cannot find that directory\n");
				return;
			}
			seek=((inodeIndex-1)/8 + gp->bg_inode_table)*1024+
				(inodeIndex-1)%8* 128;
			lseek(fd,seek,SEEK_SET);
			read(fd,cwd,sizeof(INODE));
			printdir(cwd);
			token=strtok(NULL,"/");
		}
	}
}


void printdir(INODE *inodePtr) 
{
    int i=0;
    while ((i <12)&&(inodePtr->i_block[i]!=0))
    {
        int	data_block=inodePtr->i_block[i];
        DIR *dp;
        char fbuff[1024];
        memset(fbuff, 0, 1024);
        lseek(fd,BLOCK_SIZE*data_block,SEEK_SET);
        read(fd,fbuff,BLOCK_SIZE);
        dp=(DIR *)fbuff;
        char *cp=fbuff;
        MINODE *mip;
        int ino = dp->inode;
        char name[256];
        while(cp<fbuff+1024)
        {
            mip = iget(fd, ino);
            if(S_ISREG( mip->INODE.i_mode ) ) printf("r");
            if(S_ISDIR( mip->INODE.i_mode ) ) printf("d");
            if(S_ISLNK( mip->INODE.i_mode ) ) printf("l");
            //		printf("\n");
            // user permissions
            printf((mip->INODE.i_mode & 1 << 8) ? "r" : "-");
            printf((mip->INODE.i_mode & 1 << 7) ? "w" : "-");
            printf((mip->INODE.i_mode & 1 << 6) ? "x" : "-");
            // group permissions
            printf((mip->INODE.i_mode & 1 << 5) ? "r" : "-");
            printf((mip->INODE.i_mode & 1 << 4) ? "w" : "-");
            printf((mip->INODE.i_mode & 1 << 3) ? "x" : "-");
            // other permissions
            printf((mip->INODE.i_mode & 1 << 2) ? "r" : "-");
            printf((mip->INODE.i_mode & 1 << 1) ? "w" : "-");
            printf((mip->INODE.i_mode & 1 << 0) ? "x" : "-");
            char time_s[64];
            //char *time = time_s;
            //const time_t *t = (unsigned int)&mip->INODE.i_ctime;
            ctime_r((time_t *)&mip->INODE.i_mtime, time_s);
            time_s[strlen(time_s)-1]=0;
            printf(" %3d%3d %3d%6d %20s ", dp->inode, mip->INODE.i_uid,
                    mip->INODE.i_gid, mip->INODE.i_size, time_s);
            //		char name[dp->name_len+1];
            memmove(name, dp->name, dp->name_len);
            //		memcpy(name,dp->name,dp->name_len);
            name[dp->name_len]='\0';
            if(S_ISLNK( mip->INODE.i_mode)){
                printf("%16s->%s\n",name,(char *)mip->INODE.i_block);
            }else{
                printf("%16s\n",name);
            }
            iput(mip);
            cp+=dp->rec_len;
            dp=(DIR *)cp;
            ino = dp->inode;
        }
        i++;
    }
	return;
} 

int do_cd(char *pathname) 
{
	MINODE *mip;
	if(pathname[0]<=0){
		running->cwd = iget(root->dev, 2);
		return 0;
	}
	if(strcmp(pathname, "/") == 0){
		printf("CDing to root\n");
		running->cwd = iget(root->dev, 2);
		return 0;
	}
	int ino = getino(fd, pathname);
	if(ino==0){
		printf("directory doesn't exist\n");
		return 0;
	}
	mip = iget(root->dev, ino);
	//mip = iget (root->dev,ino);
	if((mip->INODE.i_mode & 0100000) == 0100000){
		iput(mip);
		printf("cannot cd to non dir\n");
		return -1;
	}
	running->cwd = mip;
	return 0;
} 

int do_pwd() 
{
	pwd(running->cwd,0);
	printf("\n");
	return 0;
} 

void pwd(MINODE *wd, int childIno) 
{
	if(wd->ino == root->ino){
		printf("/");
	}
	char buf[1024], *cp,name[64];
	DIR *dp;
	MINODE *parentmip;
	//int block = (mip->ino -1) / INODES_PER_BLOCK + gp->bg_inode_table;

	get_block(fd, wd->INODE.i_block[0], (char *)&buf);
	dp = (DIR *)buf; // get first dir "."
	cp = buf + dp->rec_len;
	dp = (DIR *)cp; // get second dir ".."
	if(wd->ino != root->ino){
		int ino = dp->inode; // get the inode number
		parentmip = iget(fd, ino);
		pwd(parentmip,wd->ino);
	}
	if (childIno!=0){
		while (dp->inode != childIno)
		{
			cp += dp->rec_len;
			dp = (DIR *)cp; // get second dir ".."
		}
		strncpy(name,dp->name,dp->name_len);
		name[dp->name_len] = '\0';
		printf("%s/",name);
	}

	return;
} 

//prints the currently open file descriptors and their modes
//TODO pretty up the formatting for long file names (currently the tab spacing goes off)
int pfd() 
{
	int i;
	printf("Filename\tFD\tmode\toffset\n");
	printf("--------\t--\t----\t------\n");
	for(i = 0;i<10;i++)
	{
		if (running->fd[i]!= NULL)
		{
			printfilepath(running->fd[i]->minodeptr);
			printf("\t\t%d\t",i);
			switch(running->fd[i]->mode)
			{
				case 0:
					printf("READ\t");
					break;
				case 1:
					printf("WRITE\t");
					break;
				case 2:
					printf("R/W\t");
					break;
				case 3:
					printf("APPEND\t");
					break;
				default:
					printf("??????\t");//this should never happen
					break;
			}
			printf("%li\n",running->fd[i]->offset);
		}
	}
	return 0;
} 

//prints the path to the current file
int printfilepath(MINODE* mip) 
{
	MINODE* pip = findParent(mip,root);
	pwd(pip,0);
	int i;
	DIR *dp;
	char* cp,name[64];
	for (i = 0; i < 12; i++) {
		get_block(fd,pip->INODE.i_block[i],buff);
		if ( pip->INODE.i_block[i]==0)
		{
			break;
		}
		cp = buff;
		dp = (DIR *) buff;

		//get rid of the first two entries or we will be stuck forever
		cp += dp->rec_len;
		dp = (DIR *)cp;
		cp += dp->rec_len;
		dp = (DIR *)cp;
		//Depth first search
		while(cp < buff + 1024){
			if (dp->inode == mip->ino)
			{
				strncpy(name,dp->name,dp->name_len);
				name[dp->name_len] = '\0';
				printf("%s",name);
			}
			cp += dp->rec_len;
			dp = (DIR *)cp;


		}
	}
	return 0;
} 
//Recusive depth first search function to find the parent
//THIS IS BAD, THERE MUST BE A BETTER WAY
MINODE* findParent(MINODE* mip,MINODE* pip) 
{
	MINODE* result;
	DIR *dp;
	char* cp;
	int i;
	char tmpbuff[1024];
	for (i = 0; i < 12; i++) {
		if ( pip->INODE.i_block[i]==0)
		{
			break;
		}
		cp = tmpbuff;
		dp = (DIR *) tmpbuff;


		lseek(fd, pip->INODE.i_block[i]*BLOCK_SIZE, SEEK_SET);
		read(fd, tmpbuff, 1024);
		//get rid of the first two entries or we will be stuck forever
		cp += dp->rec_len;
		dp = (DIR *)cp;
		cp += dp->rec_len;
		dp = (DIR *)cp;
		//Depth first search
		while(cp < tmpbuff + 1024){
			if (dp->inode == mip->ino)
				return pip;
			else
			{
				MINODE* cip = iget(fd, dp->inode);
				if (S_ISDIR( cip->INODE.i_mode))
				{
					result = findParent(mip,cip);
					if (result!=NULL)
						return result;
				}

			}

			cp += dp->rec_len;
			dp = (DIR *)cp;

		}
	}
	return NULL;
} 


int tstbit(char *buf, int BIT) { 
	int i, j;
	i = BIT / 8;
	j = BIT % 8;
	return buf[i] & (1 << j);
} 

int setbit(char *buf, int BIT) { 
	int i, j;
	i = BIT / 8;
	j = BIT % 8;
	buf[i] |= (1 << j);
	return 1;
} 

int clearbit(char *buf, int BIT) { 
	int i, j;
	i = BIT / 8;
	j = BIT % 8;
	buf[i] &= ~(1 << j);
	return 1;
} 
