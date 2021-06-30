//格式化磁盘
#include "..\include\define.h"
#include <time.h>
#include <stdio.h>


void startsys()  
{  
    FILE *fp;  
    unsigned char buf[SIZE];  
    fcb *root;  
    int i;  
    myvhard = (unsigned char *)malloc(SIZE);//申请虚拟磁盘空间  
    memset(myvhard, 0, SIZE);//将myvhard中前SIZE个字节用 0 替换并返回 myvhard  
    if((fp = fopen(myfilename, "r")) != NULL)  
    {  
        fread(buf, SIZE, 1, fp);//将二进制文件读取到缓冲区  
        fclose(fp);//关闭打开的文件，缓冲区数据写入文件，释放系统提供文件资源  
        if(strcmp(((block0 *)buf)->magic, "10101010"))//判断开始的8个字节内容是否为文件系统魔数  
        {  
            printf("myfilesys is not exist,begin to creat the file...\n");  
            my_format();  
        }  
        else  
        {  
            for(i = 0; i < SIZE; i++)  
                myvhard[i] = buf[i];  
        }  
    }  
    else  
    {  
        printf("myfilesys is not exist,begin to creat the file...\n");  
        my_format();  
    }  
    root = (fcb *)(myvhard + 5 * BLOCKSIZE);  
    strcpy(openfilelist[0].filename, root->filename);  
    strcpy(openfilelist[0].exname, root->exname);  
    openfilelist[0].attribute = root->attribute;  
    openfilelist[0].time = root->time;  
    openfilelist[0].date = root->date;  
    openfilelist[0].first = root->first;  
    openfilelist[0].length = root->length;  
    openfilelist[0].free = root->free;  
    openfilelist[0].dirno = 5;  
    openfilelist[0].diroff = 0;  
    strcpy(openfilelist[0].dir, "\\root\\");  
    openfilelist[0].father = 0;  
    openfilelist[0].count = 0;  
    openfilelist[0].fcbstate = 0;  
    openfilelist[0].topenfile = 1;  
    for(i = 1; i < MAXOPENFILE; i++)  
        openfilelist[i].topenfile = 0;  
    curdir = 0;  
    strcpy(currentdir, "\\root\\");  
    startp = ((block0 *)myvhard)->startblock;  
}  

void my_format()  
{  
    FILE *fp;  
    fat *fat1, *fat2;
    block0 *blk0;
    time_t now;
    struct tm *nowtime;  
    fcb *root;  
    blk0 = (block0 *)myvhard;  
    fat1 = (fat *)(myvhard + BLOCKSIZE);  
    fat2 = (fat *)(myvhard + 3 * BLOCKSIZE);  
    root = (fcb *)(myvhard + 5 * BLOCKSIZE);  
    strcpy(blk0 -> magic, "10101010");  
    strcpy(blk0 -> information, "My FileSystem Ver 1.0 \n Blocksize=1KB Whole size=1000KB Blocknum=1000 RootBlocknum=2\n");  
    blk0 -> root = 5;  
    blk0 -> startblock = (unsigned char *)root;  
    for(int i = 0; i < 5; i++)  
    {  
        fat1[i].id = END;  
        fat2[i].id = END;  
    }  
    fat1[5].id = 6;
    fat2[5].id = 6; 
    fat1[6].id = END;
    fat2[6].id = END; 
    for(int i = 7; i < SIZE / BLOCKSIZE; i++)  
    {  
        fat1[i].id = FREE;  
        fat2[i].id = FREE;  
    }  
    now = time(NULL);  
    nowtime = localtime(&now);  
    strcpy(root -> filename, ".");  
    strcpy(root -> exname, "");  
    root -> attribute = 0x28;  
    root -> time = nowtime -> tm_hour * 2048 + nowtime -> tm_min * 32 + nowtime -> tm_sec / 2;  
    root -> date = (nowtime -> tm_year - 80) * 512 + (nowtime -> tm_mon + 1) * 32 + nowtime -> tm_mday;  
    root -> first = 5;  
    root -> length = 2 * sizeof(fcb);  
    root -> free = 1;  
    root++;  
    now = time(NULL);  
    nowtime = localtime(&now);  
    strcpy(root -> filename, "..");  
    strcpy(root -> exname, "");  
    root -> attribute = 0x28;  
    root -> time = nowtime -> tm_hour * 2048 + nowtime -> tm_min * 32 + nowtime -> tm_sec / 2;  
    root -> date = (nowtime -> tm_year - 80) * 512 + (nowtime -> tm_mon + 1) * 32 + nowtime -> tm_mday;  
    root -> first = 5;  
    root -> length = 2 * sizeof(fcb);  
    root -> free = 1;  
    fp = fopen(myfilename, "w");  
    fwrite(myvhard, SIZE, 1, fp);  
    fclose(fp);  
}

void my_cd(char *dirname)
{
    char *dir;  
    int fd;  
    dir = strtok(dirname, "\\");//分解字符串为一组字符串。dirname为要分解的字符串，"\\"为分隔符字符串  
    if(strcmp(dir, ".") == 0)  
        return;  
    else if(strcmp(dir, "..") == 0)  
    {  
        if(curdir)  
            curdir = my_close(curdir);  
        return;  
    }  
    else if(strcmp(dir, "root") == 0)  
    {  
        while(curdir)  
            curdir = my_close(curdir);  
        dir = strtok(NULL, "\\");  
    }  
    while(dir)  
    {  
        fd = my_open(dir);  
        if(fd != -1)  
            curdir = fd;  
        else  
            return;  
        dir = strtok(NULL, "\\");  
    }  
}