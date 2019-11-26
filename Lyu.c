// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "File.h"

int main()
{
	time_t rawTime;
	struct tm *info;
	char input[30], *opIn,*oprd;
	int index, i;
	char buf[30];
	char op[][10] = {
		"cd",
		"mkdir",
		"rmdir",
		"ls", 
		"create",
		"rm",
		"open", 
		"close",
		"write", 
		"read",
		"format",
		"exit",
		"help",
	};

	pwd = getpwuid(getuid());
	welcome();
	startsys();

	while (1) {
		time(&rawTime);
		info = localtime(&rawTime);

		printf(GREEN"\n# %-s"RESET,pwd->pw_name);
	        printf(" in ");
		printf(YELLOW"%-s "RESET,openfilelist[curdir].dir);
		printf("%s",asctime(info));
		printf(RED"$ "RESET);
		gets(input,20);
		index = -1;
		if (strcmp(input, "") == 0)
			continue;

		opIn = strtok(input, " ");
		for (i = 0; i < 13; i++) 
			if (!strcmp(opIn, op[i])) 
			{
				index = i;
				break;
			}

		switch (index) 
		{
		case 0: // cd
			oprd = strtok(NULL, " ");
			if (oprd != NULL)
				my_cd(oprd);
			else
				printf("please Enter dir name\n");
			break;
		case 1: // mkdir
			oprd = strtok(NULL, " ");
			if (oprd != NULL)
				my_mkdir(oprd);
			else
				printf("please Enter dir name\n");
			break;
		case 2: // rmdir
			oprd = strtok(NULL, " ");
			if (oprd != NULL)
				my_rmdir(oprd);
			else
				printf("please Enter dir name\n");
			break;
		case 3: // ls
			my_ls();
			break;
		case 4: // create
			oprd = strtok(NULL, " ");
			if (oprd != NULL)
				my_create(oprd);
			else
				printf("please Enter file name\n");
			break;
		case 5: // rm
			oprd = strtok(NULL, " ");
			if (oprd != NULL)
				my_rm(oprd);
			else
				printf("please Enter file name\n");
			break;
		case 6: // open
			oprd = strtok(NULL, " ");
			if (oprd != NULL)
				my_open(oprd);
			else
				printf("please Enter file name\n");
			break;
		case 7: // close
			if (openfilelist[curdir].attribute == 1)
				my_close(curdir);
			else
				printf("you not in a open file\n");
			break;
		case 8: // write
			if (openfilelist[curdir].attribute == 1)
			{
				my_write(curdir);
				gets(buf, 30);
			}
			else
				printf("please open a file first\n");
			break;
		case 9: // read
			if (openfilelist[curdir].attribute == 1)
			{
				my_read(curdir);
				gets(buf, 30);
			}
			else
				printf("please open a file first\n");
			break;
		case 10: // format
			my_format();
			printf("Disk now is format\n");
			break;
		case 11: // exit
			exitsys();
			printf("SYS: LyuFis exit!\n");
			return 0;
			break;
		case 12: // help
			help();
			break;
		default:
			printf("Error: operations not find: %s\n", input);
			break;
		}
	}
	return 0;
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
