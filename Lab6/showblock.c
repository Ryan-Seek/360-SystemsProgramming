#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h> 
#include <sys/stat.h> 

typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#define BLOCKSIZE 1024
#define BLOC_OFFSET(block) (BLOCKSIZE + block-1)*BLOCKSIZE


GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp;

char dbuffer[1024];
int InodesBeginBlock = 0;
char *name[128];
char *pathname = "/";
int i = 0, n = 0;
char buffer[BLOCKSIZE];
int fd;
int inumber;





// Gets a block from the file descriptor
int get_block(int fd, int blk, char buffer[ ]) {
  lseek(fd, (long)blk*BLOCKSIZE, 0);
  read(fd, buffer, BLOCKSIZE);
}

void get_inode(int fd, int ino, int inode_table,INODE *inode) {
	lseek(fd, BLOC_OFFSET(inode_table) + (ino - 1) * sizeof(INODE), 0);
	read(fd, inode, sizeof(INODE));
}

// Checks to make sure that the open fs is ext2
void verifyext2fs() {

  printf("FILE descriptor: %d", fd);
  printf("buffer: %d", buffer);

  // read SUPER block
  get_block(fd, 1, buffer);
  sp = (SUPER *)buffer;

  // check for EXT2 magic number:
  printf("This is a valid EXT2 filesystem: s_magic = %x\n", sp->s_magic);
  if (sp->s_magic != 0xEF53) {
    printf("Not an EXT2 filesystem\n");
    exit(1);
  }

  printf("\n------------  super block info: -------------\n");

  printf("inodes count		%d\n", sp->s_inodes_count);
  printf("blocks count		%d\n", sp->s_blocks_count);
  printf("r blocks count		%d\n", sp->s_r_blocks_count);
  printf("free blocks count	%d\n", sp->s_free_blocks_count);
  printf("free inodes count	%d\n", sp->s_free_inodes_count);
  printf("log blk size		%d\n", sp->s_log_block_size);
  printf("first data block	%d\n", sp->s_first_data_block);
  printf("magic number		%x\n", sp->s_magic);
  printf("rev level		%d\n", sp->s_rev_level);
  printf("inode size		%d\n", sp->s_inode_size);
  printf("BLOCKSIZE			%d\n", BLOCKSIZE);
  printf("inodes per group	%d\n", sp->s_inodes_per_group);

  printf("*********************************************\n");

}


// Set GP ptr to group descriptor
void get_group_descriptor_get_inodebegin()
{
  get_block(fd, 2, buffer);
  gp = (SUPER *)buffer;

  InodesBeginBlock = gp->bg_inode_table;
}

void read_InodeBeginBlock()
{

  // get inode start block
  get_block(fd, InodesBeginBlock, buffer);

  ip = (INODE *)buffer + 1;         // ip points at 2nd INODE
  printf(" - mode=%4x ", ip->i_mode);
  printf("  uid=%d  gid=%d\n", ip->i_uid, ip->i_gid);
  printf(" - size=%d\n", ip->i_size);
  printf(" - time=%s", ctime(&ip->i_ctime));
  printf(" - link=%d\n", ip->i_links_count);
  printf(" - i_block[0]=%d\n", ip->i_block[0]);
}



void get_tokens_from_pathname() {
  printf("*********************************************\n");
  printf("Pathname: %s\n", pathname);


  name[0] = strtok(pathname, "/");
  printf(" - name[0]: %s\n", name[0]);

  while (name[i] != NULL) {
    i++;
    name[i] = strtok(NULL, "/");
    printf(" - name[%d]: %s\n", i, name[i]);
  }

  n = i;
  printf(" - n = %d\n", n);
}



//search for node/ prints out dir
int search(INODE * inodePtr, char * name)
  {
	//printf("\nSEARCHING FOR: %s", name);
	for (int i = 0; i < 12; i++)
	{
		if (inodePtr->i_block[i] == 0)
			return 0;

		get_block(fd, inodePtr->i_block[i], dbuffer);

		DIR *dp = (SUPER *)dbuffer;
		char *cp = dbuffer;
		int temp = 0;
		int found = 0;
		printf("********* root dir entries ***********\n");
		printf("i_number  rec_len   name_len	name\n");
		while (cp < &dbuffer[1024])
		{
			printf("%d	  %d	    %d		%s\n", dp->inode, dp->rec_len, dp->name_len, dp->name);
			if (strcmp(name, dp->name) == 0)
			{
				temp = dp->inode;
				found = 1;
			}
			cp += dp->rec_len;
			dp = (DIR *)cp;

		}
		if(found)
		{
			printf(" - Found at INODE: %d\n", temp);
			return temp;
		}
		printf(" - Not Found\n");
		return 0;
	}
}


int showblock()
{

  verifyext2fs();

  get_group_descriptor_get_inodebegin();

  read_InodeBeginBlock();

  get_tokens_from_pathname();


  for (i = 0; i < n; i++){
    inumber = search(ip, name[i]);
    if (inumber == 0) {
      printf("\nName could not be found[%d]: '%s'\n", i, name[i]);
      exit(1);
    }
    int INODES_PER_BLOCK = BLOCKSIZE / sizeof(INODE);

  	get_block(fd, (((inumber-1)/INODES_PER_BLOCK)+InodesBeginBlock), buffer);
    ip = (INODE *)buffer + ((inumber-1)%INODES_PER_BLOCK);

    printf(" - Inodes Per Block: %d\n", INODES_PER_BLOCK);
    printf(" - Found in Block: %d\n", (((inumber-1)/INODES_PER_BLOCK)+InodesBeginBlock));
    printf(" - Found at Offset: %d\n", ((inumber - 1) % INODES_PER_BLOCK));
    printf(" - mode=%4x ", ip->i_mode);
    printf("  uid=%d  gid=%d\n", ip->i_uid, ip->i_gid);
    printf(" - size=%d\n", ip->i_size);
    printf(" - time=%s", ctime(&ip->i_ctime));
    printf(" - link=%d\n", ip->i_links_count);

    printf("-------------  DISK BLOCKS  -----------------\n");
    printf(" - i_block[0]=%d\n", ip->i_block[0]);

    if(S_ISDIR(ip->i_mode))
    {
    }
    else
    {
      if(i == n-1)
      {
      }
      else {
        printf("\nname[%d]: '%s' is not a directory, quiting program.\n", i, name[i]);
        exit(1);
      }
    }
  }

  int blocks[256], dblocks[256], dblocks2[256];
  printf("\n----------- Inode Blocks -----------\n", inumber);
   for(int i = 0; i < 14;i++)
   {
	   if (ip->i_block[i] != 0)
		   printf("%d \n", ip->i_block[i]);
	   else
		   continue;

   }

   printf("Direct blocks: \n");
    for (i = 0; i < 12; i++)
    {
        if (i && i % 4 == 0)
            putchar('\n');
        else if(i)
            putchar(' ');

        if (ip->i_block[i] == 0)
        {
            printf("\n\n");
            return;
        }
        else
            printf("%4d", ip->i_block[i]);
    }
    putchar('\n');
}




// Vars for mainline
// Name of disk to open
char *disk = "mydisk";

// Mainline handles opening of disk, then calls showblock()
main(int argc, char *argv[ ]) {

  // If given a diskname, use it instead of mydisk - DEFAULT: "mydisk"
  if (argc > 1)
    disk = argv[1];

  // If given a pathname, set pathname - DEFAULT: "/"
  if (argc > 2)
    pathname = argv[2];

  fd = open(disk, O_RDONLY);
  if (fd < 0)
  {
    printf("Open failed\n");
    exit(1);
  }

  showblock();   // Call main function
}