#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <stdbool.h>

typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

#define BLKSIZE 1024
#define SUPER_BLOCK_OFFSET 1024
#define MAX_FILE_NAME 256
#define INITIAL_BUF_SIZE 8
#define ROOT_INODE 2

int fd;
int block_size;

SUPER *sp;
GD *gp;

char **name;

bool is_space(int c)
{
	if(c == ' ')
		return true;
	return false;
}

char *strdup(const char *src)
{
	size_t len;
	char *dest;

	len = strlen(src) + 1;
	if((dest = (char*)malloc(len)) == NULL)
	{
		printf("strdup(): malloc failed\n");
		return NULL;
	}
	return (char*)memcpy(dest, src, len);
}

SUPER *get_sup(int dev)
{
	int bytes;
	SUPER *sp;

	sp = (SUPER*)malloc(sizeof(SUPER));

	lseek(dev, (long)BLKSIZE, 0);
	bytes = read(dev, sp, BLKSIZE);

	if(bytes != BLKSIZE)
	{
		printf("get_sup(): read failed\n");
		return NULL;
	}

	return sp;
}

int get_block_size(int dev)
{
	SUPER *sp;
	int block_size;

	sp = get_sup(dev);
	block_size = 1024 << sp->s_log_block_size;

	free(sp);

	return block_size;
}

char *get_block(int dev, int block)
{
	int block_size, bytes;
	char *buf;

	block_size = get_block_size(dev);
	if((buf = (char*)malloc(block_size)) == NULL)
	{
		printf("get_block(): buf malloc failed\n");
		return NULL;
	}

	lseek(dev, (long)(block * block_size), 0);
	bytes = read(dev, buf, block_size);

	if(bytes != block_size)
	{
		printf("get_block(): read failed\n");
		return NULL;
	}

	return buf;
}


GD* get_gd(int dev)
{
	if(get_block_size(dev) > SUPER_BLOCK_OFFSET + BLKSIZE)
	{
		return (GD*)get_block(dev, 1);
	}
	else
	{
		return (GD*)get_block(dev, 2);
	}
}

bool is_ext2(int dev)
{
	SUPER *sp = get_sup(dev);

	if(sp->s_magic == 0xEF53)
	{
		free(sp);
		return true;
	}
	free(sp);
	return false;
}

char **parse(char *line, char *delimiters)
{
	int i = 0, size = INITIAL_BUF_SIZE;
	char *tok, *cpy;
	char **buf;

	if((cpy = strdup(line)) == NULL)
	{
		printf("parse(): strdup failed\n");
		return NULL;
	}

	if((buf = (char**)malloc(size * sizeof(char*))) == NULL)
	{
		printf("parse(): return pointer malloc failed\n");
		return NULL;
	}

	if((tok = strtok(cpy, delimiters)) == NULL)
		return NULL;

	while(tok)
	{
		int len = strlen(tok) + 1;

		if((buf[i] = (char*)malloc(len)) == NULL)
		{
			printf("parse(): *(return + i) malloc failed\n");
			return NULL;
		}

		strcpy(buf[i], tok);

		tok = strtok(NULL, delimiters);
		i++;

		if((i + 1) == size)
		{
			size *= 2;

			if((buf = (char**)realloc(buf, size * sizeof(char*))) == NULL)
			{
				printf("parse(): return pointer realloc failed\n");
				return NULL;
			}
		}
	}

	buf[i] = NULL;

	free(cpy);
	return buf;
}

INODE *get_ino(int dev, int inode_num)
{
	SUPER *sp = get_sup(dev);
	GD *gp = get_gd(dev);

	int inodes_per_block = ((1024 << sp->s_log_block_size) / (sp->s_inode_size));

	int block = (inode_num - 1) / inodes_per_block + gp->bg_inode_table;
	int index = (inode_num - 1) % inodes_per_block;

	INODE* inode_table = (INODE*)get_block(dev, block);
    INODE* inode = (INODE*)malloc(sizeof(INODE));
    *inode = inode_table[index];

    free(sp);
    free(gp);
    free(inode_table);
    return inode;
}

int search(INODE *ip, char *name)
{
	char *sbuf;
	char *sblk, *sblkcpy;
	DIR *sdp;

	for(int i = 0; i < (ip->i_size / block_size); i++)
	{
		sbuf = get_block(fd, ip->i_block[i]);
		sblk = sbuf;
		sblkcpy = sblk;

		sdp = (DIR *)sblk;

		while (sblkcpy < (sblk + block_size))
        {
            if(strncmp(name, sdp->name, strlen(name)) == 0)
            {
            	free(sbuf);
            	printf("Returning inode for file: %s\n", sdp->name);
            	return sdp->inode;
            }
            sblkcpy += sdp->rec_len;       // advance cp by rec_len BYTEs
            sdp = (DIR*)sblkcpy;           // pull dp along to the next record
        } 
	}
	free(sbuf);
	return 0;
}
int print_indirect_block(int* buf, int level)
{
    int i;

    if(level - 1)
    {
        for(i = 0; i < block_size / sizeof(int); i++)
        {
            if(!print_indirect_block((int*)get_block(fd, buf[i]), level - 1))
            {
                free(buf);
                return 0;
            }
        }
    }
    else
    {
        for(i = 0; i < block_size / sizeof(int); i++)
        {
            if (i && i % 4 == 0)
                putchar('\n');
            else if(i)   
                putchar(' ');

            if (buf[i] == 0)
            //{
                //free(buf);
                return 0;
            //}
            else
                printf("%4d", buf[i]);
        }
        putchar('\n');
    }

    free(buf);
    return 1;
}

void print_file_blocks(INODE *ip)
{
    int i;
    printf("Inode Blocks: \n");
    for (i = 0; i < 15 && ip->i_block[i] > 0; i++)
        printf("block[%2d] = %d\n", i, ip->i_block[i]);

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
    printf("Indirect blocks: \n");

    if(!print_indirect_block((int*)get_block(fd, ip->i_block[12]), 1))
    {
    	printf("\n");
        return;
    }
    putchar('\n');

    printf("Double indirect blocks: \n");

    print_indirect_block((int*)get_block(fd, ip->i_block[13]), 1);
    putchar('\n');

    if(!print_indirect_block((int*)get_block(fd, ip->i_block[13]), 2))
    {
    	printf("\n");
        return;
    }
    putchar('\n');
    
    printf("Triple indirect blocks: \n");

    print_indirect_block((int*)get_block(fd, ip->i_block[14]), 3);
    putchar('\n');

    if(!print_indirect_block((int*)get_block(fd, ip->i_block[14]), 3))
    {
    	printf("\n");
        return;
    }
    putchar('\n');
}
void showblock()
{
	int i = 0;

	int ino, blk, offset;
	int iblk = gp->bg_inode_table;
	INODE *ip = get_ino(fd, ROOT_INODE);

	char *ibuf;

	while(name[i])
	{
		ino = search(ip, name[i]);
		if(ino == 0)
		{
			printf("cant find %s\n", name[i]);
			exit(1);
		}
		blk    = (ino - 1) / 8 + iblk;  // disk block contain this INODE 
    	offset = (ino - 1) % 8;         // offset of INODE in this block
   		ibuf = get_block(fd, blk);
    	ip = (INODE *)ibuf + offset;
		i++;
	}
    print_file_blocks(ip);
}

int main(int argc, char *argv[])
{

	char *disk, *path;
	int ino;

	INODE *ip;
	DIR *dp;

	if(argc == 3)
	{
		disk = argv[1];
		path = argv[2];
	}
	else
	{
		printf("Invalid parameters. <showblock diskimage pathname>\n");
	}


	fd = open(disk, O_RDONLY);
  	if (fd < 0){
    	printf("open %s failed\n", disk);
   	 	exit(1);
  	}
	

	sp = get_sup(fd);
	gp = get_gd(fd);
	name = parse(path, "/");	

	block_size = 1024 << sp->s_log_block_size;

	showblock();

	

	return 0;
}