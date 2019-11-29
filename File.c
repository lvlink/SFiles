// pch.cpp: 与预编译标头对应的源文件；编译成功所必需的

#include "File.h"

// 一般情况下，忽略此文件，但如果你使用的是预编译标头，请保留它。

const char* FILENAME = "LyuFis";

void help()
{
	printf("+--------------------------- HELP ---------------------------+\n");
	printf("|operations:                                                 |\n");
	printf("|(0)  cd:     get in a dir                                   |\n");
	printf("|(1)  mkdir:  create dir                                     |\n");
	printf("|(2)  rmdir:  remove dir                                     |\n");
	printf("|(3)  ls:     list the file under current dir                |\n");
	printf("|(4)  create: create the data file                           |\n");
	printf("|(5)  rm:     remove the data file                           |\n");
	printf("|(6)  open:   open the data file                             |\n");
	printf("|(7)  close:  close the data file                            |\n");
	printf("|(8)  write:  write the data file(open first)                |\n");
	printf("|(9)  read:   read the data file(open first)                 |\n");
	printf("|(10) format: format the Disk                                |\n");
	printf("|(11) exit:   exit the LyuFis and save the LyuFis as file.   |\n");
	printf("|(12) help:   print operations helps                         |\n");
	printf("+------------------------------------------------------------+\n\n");
	return;
}

void welcome()
{
	printf("+-------------------------- LyuFis --------------------------+\n");
	printf("                        Welcome %-6s                       \n",pwd->pw_name);
	printf("|function: simple operations & core dump                     |\n");
	printf("|operations:                                                 |\n");
	printf("|(0)  cd:     get in a dir                                   |\n");
	printf("|(1)  mkdir:  create dir                                     |\n");
	printf("|(2)  rmdir:  remove dir                                     |\n");
	printf("|(3)  ls:     list the file under current dir                |\n");
	printf("|(4)  create: create the data file                           |\n");
	printf("|(5)  rm:     remove the data file                           |\n");
	printf("|(6)  open:   open the data file                             |\n");
	printf("|(7)  close:  close the data file                            |\n");
	printf("|(8)  write:  write the data file(open first)                |\n");
	printf("|(9)  read:   read the data file(open first)                 |\n");
	printf("|(10) format: format the Disk                                |\n");
	printf("|(11) exit:   exit the LyuFis and save the LyuFis as file.   |\n");
	printf("|(12) help:   print operations helps                         |\n");
	printf("+------------------------------------------------------------+\n\n");
}

void init()
{
	Fat = (fat*)(myvhard + BLOCKSIZE);
	ilist = (inode*)(myvhard + 3 * BLOCKSIZE);
	cpBasicInfo(&openfilelist[0], &ilist[0]);
	strcpy(openfilelist[0].filename, "root");
	openfilelist[0].dirBnum = 5;
	openfilelist[0].offset = 0;
	strcpy(openfilelist[0].dir, "/root/");
	openfilelist[0].count = 0;
	openfilelist[0].fcbstate = 0;
	openfilelist[0].topenfile = 1;
        curdir = 0;
	return;
}

void startsys()
{
	myvhard = (char*)malloc(SIZE);
	
	FILE* file;
	if ((file = fopen(FILENAME, "r")) != NULL)
	{
		fread(buffer, SIZE, 1, file);
		fclose(file);
		memcpy(myvhard, buffer, SIZE);
		printf("LyuFis File Load successful\n");
		
	}
	else
	{
		printf("LyuFis File not found, init now\n");
		do_format();
	}
	
	init();
	return;
}
void exitsys()
{
	
	while (curdir)
	{
		curdir = my_close(curdir);
	}
	
	FILE* file = fopen(FILENAME, "w");
	fwrite(myvhard, SIZE, 1, file);
	fclose(file);
	return;
}
void my_format()
{
	do_format();
	init();
	return;
}
void do_format()
{
	int i;
	time_t rawTime;
	struct tm *info;
	block0* superBlock = (block0*)myvhard;
	strcpy(superBlock->information, "ニャー　ニャー　ニャー\nsuperBlock[1] => Fat List[2] => inode list[2] => root[1] => otherFile[994] ");
	superBlock->root = 5;
	superBlock->startblock = myvhard + 5 * BLOCKSIZE;

	Fat = (fat*)(myvhard + BLOCKSIZE);
	Fat[0].id = END;
	Fat[1].id = 2;
	Fat[2].id = END;
	Fat[3].id = 4;
	Fat[4].id = END;
	Fat[5].id = END;
	for (i = 6; i < BLOCKNUM; i++)
		Fat[i].id = FREE;

	// root info < = > ilist[0]
	ilist = (inode*)(myvhard + 3 * BLOCKSIZE);
	ilist[0].attribute = 0;

	time(&rawTime);
	info = localtime(&rawTime);
	ilist[0].time = info->tm_hour * 2048 + info->tm_min * 32 + info->tm_sec / 2;
	ilist[0].date = (info->tm_year - 100) * 512 + (info->tm_mon + 1) * 32 + (info->tm_mday);
	ilist[0].first = 5;
	ilist[0].free = 1;
	ilist[0].length = 2 * sizeof(fnode);

	fnode* root = (fnode*)(myvhard + 5 * BLOCKSIZE);
	strcpy(root->filename, ".");
	root->iptr = 0;
	strcpy((root + 1)->filename, "..");
	(root + 1)->iptr = 0;

	for (i = 2; i < MAXDIRFILENUM; i++)
		(root + i)->iptr = -1;

	for (i = 1; i < MAXINODENUM; i++)
		ilist[i].free = 0;

	for(i = 0;i<MAXOPENFILE;i++)
		openfilelist[i].topenfile = 0;
	return;
}

void my_cd(char* dirname)
{
	int i, fd;
	char *dir;
	if (openfilelist[curdir].attribute == 1)
	{
		printf("you just open a datafile, exit first!");
		return;
	}
	if (!strcmp(dirname, "."))
		return;
	if (!strcmp(dirname, ".."))
	{
		if (curdir == 0)
			return;
		else
		{
			curdir = my_close(curdir);
			return;
		}
	}
	dir = (char*)malloc(BLOCKSIZE);
	openfilelist[curdir].count = 0;
	do_read(curdir, openfilelist[curdir].length, dir);

	fnode* flist = (fnode*)dir;
	for (i = 0; i < (int)(openfilelist[curdir].length / sizeof(fnode)); i++)
	{
		if (!strcmp(flist[i].filename, dirname) && flist[i].iptr!=-1&& ilist[flist[i].iptr].attribute == 0)
			goto FIND;
	}
	printf("file not find\n");
	return;

FIND:
	if ((fd = getOpenNode()) == -1)
	{
		printf("OpenFileList full!\n");
		return;
	}
	cpBasicInfo(&openfilelist[fd], &ilist[flist[i].iptr]);
	strcpy(openfilelist[fd].filename, flist[i].filename);
	openfilelist[fd].count = 0;
	addDir(&openfilelist[fd], &openfilelist[curdir], dirname,0);
	openfilelist[fd].iptr = flist[i].iptr;
	openfilelist[fd].dirBnum = openfilelist[curdir].first;
	openfilelist[fd].offset = i;
	curdir = fd;
	return;
}

int my_mkdir(char* dirname)
{
	int i, fd, blockNum, inum;
	char buf[BLOCKSIZE + 1];
	fnode* newfn;
	time_t rawTime;
	struct tm* info;

	openfilelist[curdir].count = 0;
	do_read(curdir, MAXOPENFILE*sizeof(fnode) , buf);
	fnode* flist = (fnode*)buf;

	for (i = 0; i < (int)(openfilelist[curdir].length / sizeof(fnode)); i++)
		if ((!strcmp(dirname, flist[i].filename)) && flist[i].iptr != -1 && (ilist[flist[i].iptr].attribute == 0))
		{
			printf("the same name dir exist!\n");
			return -1;
		}
	for (i = 0; i < MAXDIRFILENUM; i++)
		if (flist[i].iptr == -1)
			goto GET;
	printf("Dir full!\n");
	return -1;
GET:
	if ((inum = getNewInode()) == -1)
	{
		printf("Inode list full\n");
		return -1;
	}
	if ((fd = getOpenNode()) == -1)
	{
		printf("Openfilelist is full\n");
		ilist[inum].free = 0;
		return -1;
	}
	if ((blockNum = getNewBlock()) == END)
	{
		ilist[inum].free = 0;
		printf("Disk is full\n");
		openfilelist[fd].topenfile = 0;
		return -1;
	}

	flist[i].iptr = inum;
	strcpy(flist[i].filename, dirname);
	Fat[blockNum].id = END;
	openfilelist[curdir].fcbstate = 1;

	newfn = (fnode*)malloc(sizeof(fnode));
	strcpy(newfn->filename, dirname);
	newfn->iptr = inum;
	ilist[inum].free = 1;
	ilist[inum].attribute = 0;

	time(&rawTime);
	info = localtime(&rawTime);

	ilist[inum].date = (info->tm_year - 100) * 512 + (info->tm_mon + 1) * 32 + (info->tm_mday);
	ilist[inum].time = (info->tm_hour) * 2048 + (info->tm_min) * 32 + (info->tm_sec) / 2;
	ilist[inum].first = blockNum;
	ilist[inum].length = 2 * sizeof(fnode);

	openfilelist[curdir].count = i * sizeof(fnode);
	do_write(curdir, (char*)newfn, sizeof(fnode), 2);


	cpBasicInfo(&openfilelist[fd], &ilist[inum]);
	strcpy(openfilelist[fd].filename, dirname);
	openfilelist[fd].count = 0;
	openfilelist[fd].offset = i;
	openfilelist[fd].dirBnum = openfilelist[curdir].first;
	addDir(&openfilelist[fd], &openfilelist[curdir], dirname,0);

	strcpy(newfn->filename, ".");
	newfn->iptr = inum;
	do_write(fd, (char*)newfn, sizeof(fnode), 2);

	strcpy(newfn->filename, "..");
	newfn->iptr = openfilelist[curdir].iptr;
	do_write(fd, (char*)newfn, sizeof(fnode), 2);

	fnode* tmp = (fnode *)(myvhard + BLOCKSIZE * blockNum);

	for (i = 2; i < MAXDIRFILENUM; i++)
		(tmp + i)->iptr = -1;

	my_close(fd);
	free(newfn);

	ilist[flist[0].iptr].length = openfilelist[curdir].length;
	openfilelist[curdir].count = 0;
	openfilelist[curdir].fcbstate = 1;

	return 0;
}

void my_rmdir(char* dirname)
{
	int i, blockNum, mnum;
	char buf[BLOCKSIZE + 1];

	if (!strcmp(dirname, ".") || !strcmp(dirname, ".."))
	{
		printf("you cannot rm .&..\n");
		return;
	}
	openfilelist[curdir].count = 0;
	do_read(curdir, openfilelist[curdir].length, buf);

	fnode* flist = (fnode*)buf;
	for (i = 0; i < (int)(openfilelist[curdir].length / sizeof(fnode)); i++)
	{
		if (flist[i].iptr == -1)
			continue;
		if (!strcmp(flist[i].filename, dirname) && ilist[flist[i].iptr].attribute == 0)
			goto FIND;
	}

	printf("dir not find\n");
	return;

FIND:
	if (ilist[flist[i].iptr].length > 2 * sizeof(fnode))
	{
		printf("reverse del not support\n");
		return;
	}

	blockNum = ilist[flist[i].iptr].first;
	Fat[blockNum].id = FREE;
		
	ilist[flist[i].iptr].free = 0;
	ilist[flist[i].iptr].length = 0;
	ilist[flist[i].iptr].first = 0;
	ilist[flist[i].iptr].time = 0;
	ilist[flist[i].iptr].date = 0;
	strcpy(flist[i].filename, "\0");
	flist[i].iptr = -1;

	openfilelist[curdir].count = i * sizeof(fnode);
	do_write(curdir, (char*)(flist + i), sizeof(fnode), 2);

	mnum = i;
	if ((mnum + 1) * sizeof(fnode) == openfilelist[curdir].length)
	{
		openfilelist[curdir].length -= sizeof(fnode);
		mnum--;
		while (flist[mnum].iptr == -1)
		{
			mnum--;
			openfilelist[curdir].length -= sizeof(fnode);
		}
	}

	ilist[flist[0].iptr].length = openfilelist[curdir].length;
	openfilelist[curdir].fcbstate = 1;

}

int my_create(char* filename)
{
	int i, blockNum, inum;
	char buf[BLOCKSIZE + 1];
	time_t rawTime;
	struct tm* info;

	if (openfilelist[curdir].attribute == 1)
	{
		printf("you just open a datafile, exit first!\n");
		return -1;
	}

	openfilelist[curdir].count = 0;
	do_read(curdir, MAXOPENFILE*sizeof(fnode), buf);

	fnode* flist = (fnode*)buf;

	for (i = 0; i < MAXDIRFILENUM; i++)
	{
		if (flist[i].iptr == -1)
			continue;
		if ((!strcmp(flist[i].filename, filename)) && ilist[flist[i].iptr].attribute == 1)
		{
			printf("same name file find!\n");
			return -1;
		}
	}

	for (i = 0; i < MAXDIRFILENUM; i++)
	{
		if (flist[i].iptr == -1)
			goto FIND;
	}

	printf("Dir full!\n");
	return -1;

FIND:
	if ((inum = getNewInode()) == -1)
	{
		printf("Ilist full!\n");
		return -1;
	}
	if ((blockNum = getNewBlock()) == -1)
	{
		ilist[inum].free = 0;
		printf("Disk full\n");
		return -1;
	}
	flist[i].iptr = inum;
	ilist[inum].first = blockNum;
	Fat[blockNum].id = END;

	strcpy(flist[i].filename, filename);
	time(&rawTime);
	info = localtime(&rawTime);
	ilist[flist[i].iptr].date = (info->tm_year - 100) * 512 + (info->tm_mon + 1) * 32 + (info->tm_mday);
	ilist[flist[i].iptr].time = (info->tm_hour) * 2048 + (info->tm_min) * 32 + (info->tm_sec) / 2;
	ilist[flist[i].iptr].free = 1;
	ilist[flist[i].iptr].attribute = 1;
	ilist[flist[i].iptr].length = 0;

	openfilelist[curdir].count = i * sizeof(fnode);
	do_write(curdir, (char*)(flist + i), sizeof(fnode), 2);
	//
	ilist[flist[0].iptr].length = openfilelist[curdir].length;
	openfilelist[curdir].fcbstate = 1;
	return 1;
}

void my_rm(char* filename)
{
	int i, blockNum, nextBNum = 0, mnum;
	char buf[MAXFILESIZE];
	openfilelist[curdir].count = 0;
	do_read(curdir, openfilelist[curdir].length, buf);

	fnode* flist = (fnode*)buf;

	for (i = 0; i < (int)(openfilelist[curdir].length / sizeof(fnode)); i++)
	{
		if (flist[i].iptr == -1)
			continue;
		if ((!strcmp(flist[i].filename, filename)) && ilist[flist[i].iptr].attribute == 1)
			goto FIND;
	}

	printf("File not find!\n");
	return;

FIND:
	blockNum = ilist[flist[i].iptr].first;
	while (1)
	{
		nextBNum = Fat[blockNum].id;
		Fat[blockNum].id = FREE;
		if (nextBNum != END)
			blockNum = nextBNum;
		else
			break;
	}

	ilist[flist[i].iptr].date = 0;
	ilist[flist[i].iptr].time = 0;
	ilist[flist[i].iptr].first = -1;
	ilist[flist[i].iptr].free = 0;
	ilist[flist[i].iptr].length = 0;
	strcpy(flist[i].filename, "\0");
	flist[i].iptr = -1;

	openfilelist[curdir].count = i * sizeof(fnode);
	do_write(curdir, (char*)(flist + i), sizeof(fnode), 2);

	mnum = i;
	if ((mnum + 1) * sizeof(fnode) == openfilelist[curdir].length)
	{
		openfilelist[curdir].length -= sizeof(fnode);
		mnum--;
		while (flist[mnum].iptr == -1)
		{
			mnum--;
			openfilelist[curdir].length -= sizeof(fnode);
		}
	}

	ilist[flist[0].iptr].length = openfilelist[curdir].length;
	openfilelist[curdir].count = 0;
	//do_write(curdir, (char*)(flist), sizeof(fnode), 2);

	openfilelist[curdir].fcbstate = 1;
}

int my_read(int fd)
{
	char buf[MAXFILESIZE];
	if (fd < 0 || fd >= MAXOPENFILE)
	{
		printf("wrong fd num\n");
		return -1;
	}
	char flag = '0';
	printf("1.read all 2.random read \n");
	while (1)
	{
		flag = getchar();
		if(flag == '\n')
			continue;
		if (flag == '1' || flag == '2')
			break;
		else
			printf("wrong flag!\n1.read all 2.random read \n");
	}
	if (flag == '1')
	{
		openfilelist[fd].count = 0;
		do_read(fd, openfilelist[fd].length, buf);
		printf("%s\n", buf);
		return 1;
	}
	else
	{
		int pos, len;
		printf("Enter start position & length\n");
		scanf("%d%d", &pos, &len);
		openfilelist[fd].count = pos-1;
		do_read(fd, len, buf);
		buf[len] = '\0';
		printf("%s\n", buf);
		return 1;
	}
}

int do_read(int fd, int len, char* text)
{
	int readLen = len;
	char* textptr = text;
	char buf[BLOCKSIZE + 1];
	int  offset = openfilelist[fd].count, blockNum = openfilelist[fd].first;
	char* blockPtr;
	while (offset >= BLOCKSIZE)
	{
		offset -= BLOCKSIZE;
		if ((blockNum = Fat[blockNum].id) == END)
		{
			printf("File read Error, Overread!\n");
			return -1;
		}
	}

	blockPtr = (char*)(myvhard + blockNum * BLOCKSIZE);
	memcpy(buf, blockPtr, BLOCKSIZE);

	// pro
	while (readLen > 0)
	{
		if (BLOCKSIZE - offset > readLen)
		{
			memcpy(textptr, buf + offset, readLen);
			openfilelist[fd].count += readLen;
			readLen = 0;
		}
		else
		{
			memcpy(textptr, buf + offset, BLOCKSIZE - offset);
			textptr += BLOCKSIZE - offset;
			readLen -= BLOCKSIZE - offset;
			if ((blockNum = Fat[blockNum].id) == END)
			{
				printf("overread!\n");
				goto OUT;
			}
			blockPtr = (char*)myvhard + BLOCKSIZE * blockNum;
			memcpy(buf, blockPtr, BLOCKSIZE);
			offset = 0;
			openfilelist[fd].count+=BLOCKSIZE-offset;
		}

	}
OUT:
	return len - readLen;
}

int my_write(int fd)
{
	int wstyle, tp = 0;
	char input;
	char buf[MAXFILESIZE];
	if (fd<0 || fd>MAXOPENFILE)
	{
		printf("wrong fd num\n");
		return -1;
	}
	printf("Choose write style!\n");
	if (openfilelist[fd].attribute != 1)
	{
		printf("you can write datafile only!\n");
		return -1;
	}
	printf("1.Cur write 2.Cover write 3.Append write 4. Random write\n");
	while (1)
	{
		wstyle = getchar();
		if(wstyle == '\n')
			continue;
		if (wstyle == '1' || wstyle == '2' || wstyle == '3' || wstyle == '4')
			break;
		else
			printf("num error! please retype!\n1.Cur write 2.Cover write 3.Append write 4. Random write\n");
	}
	wstyle -='0';
	printf("type the data and enter # as end!\n");
	getchar();
	while ((input = getchar()) != '#')
	{
		buf[tp] = input;
		tp++;
	}
	buf[tp]='\0';

	do_write(fd, buf, strlen(buf) + 1, wstyle);
	openfilelist[fd].fcbstate = 1;
	return 1;
}

int do_write(int fd, char* text, int len, char wstyle)
{
	int blockNum = openfilelist[fd].first;
	int i, offset, curlen = 0;
	char buf[BLOCKSIZE + 1];
	char * blockPtr;

	if (wstyle == 1)
	{
		openfilelist[fd].count = 0;
		openfilelist[fd].length = 0;
	}
	else if (wstyle == 3)
	{
		openfilelist[fd].count = openfilelist[fd].length;
		if (openfilelist[fd].attribute == 1)
		{
			if (openfilelist[fd].length != 0)
				openfilelist[fd].count = openfilelist[fd].length - 1;
		}
	}
	else if (wstyle == 4)
	{
		unsigned int pos;
		printf("Enter start pos \n");
		scanf("%ud", &pos);
		pos--;
		openfilelist[fd].count = pos;
		if(pos+len<openfilelist[fd].length)
			len--;
	}

	offset = openfilelist[fd].count;

	while (offset >= BLOCKSIZE)
	{
		if ((blockNum = Fat[blockNum].id) == END)
		{
			printf("Fat search error!\n");
			return -1;
		}
		offset -= BLOCKSIZE;
	}

	blockPtr = (char*)(myvhard + blockNum * BLOCKSIZE);

	while (len > curlen)
	{
		memcpy(buf, blockPtr, BLOCKSIZE);
		for (i = 0; offset < BLOCKSIZE; offset++, i++)
		{
			buf[offset] = text[i];
			curlen++;
			if (len == curlen)
				break;
		}
		memcpy(blockPtr, buf, BLOCKSIZE);

		if (offset == BLOCKSIZE && len != curlen)
		{
			offset = 0;
			if ((blockNum = Fat[blockNum].id) == END)
			{
				int prv = blockNum;
				if ((blockNum = getNewBlock()) == END)
				{
					printf("Disk full!\n");
					return -1;
				}
				blockPtr = (char*)(myvhard + BLOCKSIZE * blockNum);
				Fat[prv].id = blockNum;
				Fat[blockNum].id = END;
			}
			else
			{
				blockPtr = (char*)(myvhard + BLOCKSIZE * blockNum);
			}
		}
	}
	openfilelist[fd].count += len;
	if (openfilelist[fd].count > openfilelist[fd].length)
		openfilelist[fd].length = openfilelist[fd].count;
	
	blockNum = Fat[openfilelist[fd].first].id;
	if (wstyle)
	{

		offset = openfilelist[fd].length;
		while (offset >= BLOCKSIZE)
		{
			offset -= BLOCKSIZE;
			blockNum = Fat[blockNum].id;
		}
		int record = blockNum;
		while (1)
		{
			if (Fat[blockNum].id != END)
			{
				i = Fat[blockNum].id;
				Fat[blockNum].id = FREE;
				blockNum = i;
			}
			else
			{
				Fat[blockNum].id = FREE;
				break;
			}
		}

		Fat[record].id = END;
	}
	return len;
}

int my_open(char* filename)
{
	int i, fd;
	char buf[MAXFILESIZE];
	openfilelist[curdir].count = 0;
	do_read(curdir, openfilelist[curdir].length, buf);

	fnode* flist = (fnode*)buf;

	for (i = 0; i < (int)(openfilelist[curdir].length) / (int)sizeof(fnode); i++)
	{
		if (flist[i].iptr == -1)
			continue;
		if ((!strcmp(flist[i].filename, filename)) && ilist[flist[i].iptr].attribute == 1)
			goto FIND;
	}
	printf("File not find!\n");
	return -1;

FIND:
	if ((fd = getOpenNode()) == -1)
	{
		printf("Openlist full!\n");
		return -1;
	}

	cpBasicInfo(&openfilelist[fd], &ilist[flist[i].iptr]);
	strcpy(openfilelist[fd].filename, flist[i].filename);
	addDir(&openfilelist[fd], &openfilelist[curdir], filename,1);
	openfilelist[fd].iptr = flist[i].iptr;
	openfilelist[fd].dirBnum = openfilelist[curdir].first;
	openfilelist[fd].offset = i;
	openfilelist[fd].fcbstate = 0;
	curdir = fd;
	return 1;
}

int my_close(int fd)
{
	int i, dir;
	char buf[MAXFILESIZE];
	fnode* flist;
	if (fd > MAXFILESIZE || fd < 0)
	{
		printf("fd wrong!\n");
		return -1;
	}
	for (i = 0; i < MAXOPENFILE; i++)
	{
		if (openfilelist[i].first == openfilelist[fd].dirBnum)
		{
			dir = i;
			goto FIND;
		}
	}

	printf("dir not find!\n");
	return -1;

FIND:
	if (openfilelist[fd].fcbstate == 1)
	{
		openfilelist[dir].count = 0;
		do_read(dir, openfilelist[dir].length, buf);
		flist = (fnode*)buf;

		int off = openfilelist[fd].offset;
		int ip = openfilelist[fd].iptr;

		strcpy(flist[off].filename, openfilelist[fd].filename);
		ilist[ip].first = openfilelist[fd].first;
		ilist[ip].free = openfilelist[fd].free;
		ilist[ip].date = openfilelist[fd].date;
		ilist[ip].time = openfilelist[fd].time;
		ilist[ip].length = openfilelist[fd].length;
		ilist[ip].attribute = openfilelist[fd].attribute;
		openfilelist[dir].count = openfilelist[fd].offset * sizeof(fnode);
		do_write(dir, (char*)(flist + off), sizeof(fnode), 2);
	}
	memset(&openfilelist[fd], 0, sizeof(useropen));
	curdir = dir;
	return dir;
}


void my_ls()
{
	fnode* flist;
	int i;
	char buf[MAXFILESIZE];
	if (openfilelist[curdir].attribute == 1)
	{
		printf("you just open a data file! exit first!\n");
		return;
	}

	openfilelist[curdir].count = 0;
	do_read(curdir, openfilelist[curdir].length, buf);
	flist = (fnode*)buf;

	for (i = 0; i < (int)(openfilelist[curdir].length / sizeof(fnode)); i++)
	{
		if (flist[i].iptr == -1)
			continue;
		else
		{
			int ip = flist[i].iptr;
			if (ilist[ip].attribute == 0)
				printf("d %-s -     %d/%d/%d %d:%d %-8s\n",
					pwd->pw_name,
					(ilist[ip].date >> 9) + 2000,
					(ilist[ip].date >> 5) & 0x00f,
					(ilist[ip].date & 0x001f),
					(ilist[ip].time >> 11),
					(ilist[ip].time >> 5) & 0x003f,
					flist[i].filename);
			else
				printf("- %-s %-5d %d/%d/%d %d:%d %-8s \n",
					pwd->pw_name,
					ilist[ip].length,
					(ilist[ip].date >> 9) + 2000,
					(ilist[ip].date >> 5) & 0x00f,
					(ilist[ip].date & 0x001f),
					(ilist[ip].time >> 11),
					(ilist[ip].time >> 5) & 0x003f,
					flist[i].filename);
		}
	}
	return;
}


int getNewInode()
{
	int i;
	for (i = 1; i < MAXINODENUM; i++)
	{
		if (ilist[i].free == 0)
		{
			ilist[i].free = 1;
			return i;
		}
	}
	printf("Inode list full\n");
	return -1;
}

int getOpenNode()
{
	int i;
	for (i = 1; i < MAXOPENFILE; i++)
	{
		if (openfilelist[i].topenfile == 0)
		{
			openfilelist[i].topenfile = 1;
			return i;
		}
	}
	printf("OpenFilelist full!\n");
	return -1;
}
int getNewBlock()
{
	int i;
	for (i = 7; i < BLOCKNUM; i++)
		if (Fat[i].id == FREE)
			return i;

	printf("Disk full!\n");
	return -1;
}

void cpBasicInfo(useropen* uo, inode* in)
{
	uo->attribute = in->attribute;
	uo->time = in->time;
	uo->date = in->date;
	uo->first = in->first;
	uo->length = in->length;
	uo->free = in->free;
	uo->fcbstate = 0;
	uo->topenfile = 1;
}
void addDir(useropen* dest, useropen* src, char * dirname,char flag)
{
	if(flag == 0)
		strcat(strcat(strcpy(dest->dir, src->dir), dirname), "/");
	else
		strcat(strcat(strcpy(dest->dir, src->dir), "/"), dirname);
}


