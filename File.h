// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

#ifndef PCH_H
#define PCH_H
#include<malloc.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<pwd.h>
#include<unistd.h>

#define BLOCKNUM 1000
#define BLOCKSIZE 1024
#define SIZE 1024000
#define END 65535
#define FREE 0
#define ROOTBLOCKNUM 2
#define MAXOPENFILE 11
#define MAXFILESIZE 5120
#define MAXDIRFILENUM 80
#define MAXINODENUM 160

#define GREEN     "\e[0;32m"
#define YELLOW    "\e[1;33m"
#define RED       "\e[0;31m"
#define RESET     "\033[0m"

// config   superblock -> fat -> inode -> data
//              1          2       2      995
typedef struct inode {
	unsigned char attribute;
	unsigned short time;
	unsigned short date;
	unsigned short first;
	unsigned short length;
	char free;
}inode; // size 12           1024*2/12 = 170 >160 max file num

typedef struct FAT {
	unsigned short id;
}fat;

typedef struct dirNode {
	char filename[8];
	int iptr;
}fnode; // size 12           1024/12 = 85 > 80 max dir file num

typedef struct USEROPEN {
	char filename[8];
	unsigned char attribute;
	unsigned short time;
	unsigned short date;
	unsigned short first;
	unsigned short length;
	char free;

	char dir[80];
	int dirBnum;
	int offset;
	int count;
	int iptr;
	char fcbstate;
	char topenfile;
}useropen;

typedef struct BLOCK0 {
	char information[200];
	unsigned short root;
	unsigned char* startblock;
}block0;

char* myvhard;
useropen openfilelist[MAXOPENFILE];
int curdir;
inode* ilist;
fat* Fat;
char buffer[SIZE];
struct passwd *pwd;

void startsys();
void do_format();
void my_format();
void my_cd(char* dirname);
int  my_mkdir(char *dirname);
void my_rmdir(char *dirname);
void my_ls();
int my_create(char *filename);
void my_rm(char* filename);
int my_open(char* filename);
int my_close(int fd);
int my_write(int fd);
int do_write(int fd, char *text, int len, char wstyle);
int my_read(int fd);// int len);
int do_read(int fd, int length, char* text);
void exitsys();

int getNewInode();
int getOpenNode();
int getNewBlock();
void help();
void welcome();
void addDir(useropen*, useropen*, char*,char);
void cpBasicInfo(useropen*, inode*);

// TODO: 添加要在此处预编译的标头

#endif //PCH_H
