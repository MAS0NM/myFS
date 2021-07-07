#include <time.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include ".\utils.c"
#include "..\include\define.h"

void startsys()  
{  
    FILE *fp;  
    unsigned char buf[SIZE];  
    fcb *root;  
    int i;  
    myvhard = (unsigned char *)malloc(SIZE);                        //申请虚拟磁盘空间  
    memset(myvhard, 0, SIZE);                                       //将myvhard中前SIZE个字节用 0 替换并返回 myvhard  
    if((fp = fopen(myfilename, "r")) != NULL)  
    {  
        fread(buf, SIZE, 1, fp);                                    //将二进制文件读取到缓冲区  
        fclose(fp);                                                 //关闭打开的文件，缓冲区数据写入文件，释放系统提供文件资源  
        if(strcmp(((block0 *)buf) -> magic, "10101010"))            //判断开始的8个字节内容是否为文件系统魔数  
        {  
            printf("myfilesys not exist, begin to creat the file...\n");  
            my_format();  
        }  
        else  
        {  
            for(i = 0; i < SIZE; i++)  
                myvhard[i] = buf[i];                                //用所读文件初始化整个磁盘
        }  
    }  
    else  
    {  
        printf("myfilesys not exist,begin to creat the file...\n");  
        my_format();  
    }  
    root = (fcb *)(myvhard + 5 * BLOCKSIZE);                        //指向数据区首地址
    strcpy(openfilelist[0].filename, root->filename);               //为一个文件的fcb赋值
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
    blk0 = (block0 *)myvhard;               //把blk0写入第一个盘块
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
    fat1[5].id = 6;                                     //第6，7个块都是root
    fat2[5].id = 6; 
    fat1[6].id = END;
    fat2[6].id = END; 
    for(int i = 7; i < SIZE / BLOCKSIZE; i++)           //从8号开始都是空的
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
    dir = strtok(dirname, "\\");            //分解字符串为一组字符串。dirname为要分解的字符串，"\\"为分隔符字符串  
    if(strcmp(dir, ".") == 0)  
        return;  
    else if(strcmp(dir, "..") == 0)  
    {  
        if(curdir)  
            curdir = my_close(curdir);      //返回上级目录
        return;  
    }  
    else if(strcmp(dir, "root") == 0)       //返回root节点
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

void my_mkdir(char *dirname)  
{  
    fcb *fcbptr;  
    fat *fat1, *fat2;  
    time_t now;  
    struct tm *nowtime;  
    char text[MAXTEXT];  
    unsigned short blkno;  
    int rbn, fd, i;  
    fat1 = (fat *)(myvhard + BLOCKSIZE);  
    fat2 = (fat *)(myvhard + 3 * BLOCKSIZE);  
    openfilelist[curdir].count = 0;  
    rbn = do_read(curdir, openfilelist[curdir].length, text);  
    fcbptr = (fcb *)text;  
    for(i = 0; i < rbn / sizeof(fcb); i++)//在当前目录下找，是否有重名目录  
    {  
        if(strcmp(fcbptr->filename, dirname) == 0 && strcmp(fcbptr->exname, "") == 0)  
        {  
            printf("Error,the dirname is already exist!\n");  
            return;  
        }  
        fcbptr++;  
    }  
    fcbptr = (fcb *)text;  
    for(i = 0; i < rbn / sizeof(fcb); i++)  
    {  
        if(fcbptr->free == 0)  
            break;  
        fcbptr++;  
    }  
    blkno = findblock();//寻找空闲盘块  
    if(blkno == -1)  
        return;  
    (fat1 + blkno)->id = END;  
    (fat2 + blkno)->id = END;  
    now = time(NULL);  
    nowtime = localtime(&now);  
    strcpy(fcbptr->filename, dirname);  
    strcpy(fcbptr->exname, "");  
    fcbptr->attribute = 0x30;  
    fcbptr->time = nowtime->tm_hour * 2048 + nowtime->tm_min * 32 + nowtime->tm_sec / 2;  
    fcbptr->date = (nowtime->tm_year - 80) * 512 + (nowtime->tm_mon + 1) * 32 + nowtime->tm_mday;  
    fcbptr->first = blkno;  
    fcbptr->length = 2 * sizeof(fcb);  
    fcbptr->free = 1;  
    openfilelist[curdir].count = i * sizeof(fcb);  
    do_write(curdir, (char *)fcbptr, sizeof(fcb), 2);  
  
    fd = my_open(dirname);//建立新目录的'.','..'目录  
    if(fd == -1)  
        return;  
    fcbptr = (fcb *)malloc(sizeof(fcb));  
    now = time(NULL);  
    nowtime = localtime(&now);  
    strcpy(fcbptr->filename, ".");  
    strcpy(fcbptr->exname, "");  
    fcbptr->attribute = 0x28;  
    fcbptr->time = nowtime->tm_hour * 2048 + nowtime->tm_min * 32 + nowtime->tm_sec / 2;  
    fcbptr->date = (nowtime->tm_year - 80) * 512 + (nowtime->tm_mon + 1) * 32 + nowtime->tm_mday;  
    fcbptr->first = blkno;  
    fcbptr->length = 2 * sizeof(fcb);  
    fcbptr->free = 1;  
    do_write(fd, (char *)fcbptr, sizeof(fcb), 2);  
    now = time(NULL);  
    nowtime = localtime(&now);  
    strcpy(fcbptr->filename, "..");  
    strcpy(fcbptr->exname, "");  
    fcbptr->attribute = 0x28;  
    fcbptr->time = nowtime->tm_hour * 2048 + nowtime->tm_min * 32 + nowtime->tm_sec / 2;  
    fcbptr->date = (nowtime->tm_year - 80) * 512 + (nowtime->tm_mon + 1) * 32 + nowtime->tm_mday;  
    fcbptr->first = blkno;  
    fcbptr->length = 2 * sizeof(fcb);  
    fcbptr->free = 1;  
    do_write(fd, (char *)fcbptr, sizeof(fcb), 2);  
    free(fcbptr);  
    my_close(fd);  
  
    fcbptr = (fcb *)text;  
    fcbptr->length = openfilelist[curdir].length;  
    openfilelist[curdir].count = 0;  
    do_write(curdir, (char *)fcbptr, sizeof(fcb), 2);  
    openfilelist[curdir].fcbstate = 1;  
}  
  
void my_rmdir(char *dirname)  
{  
    fcb *fcbptr,*fcbptr2;  
    fat *fat1, *fat2, *fatptr1, *fatptr2;  
    char text[MAXTEXT], text2[MAXTEXT];  
    unsigned short blkno;  
    int rbn, rbn2, fd, i, j;  
    fat1 = (fat *)(myvhard + BLOCKSIZE);  
    fat2 = (fat *)(myvhard + 3 * BLOCKSIZE);  
    if(strcmp(dirname, ".") == 0 || strcmp(dirname, "..") == 0)  
    {  
        printf("Error,can't remove this directory.\n");  
        return;  
    }  
    openfilelist[curdir].count = 0;  
    rbn = do_read(curdir, openfilelist[curdir].length, text);  
    fcbptr = (fcb *)text;  
    for(i = 0; i < rbn / sizeof(fcb); i++)//查找要删除的目录  
    {  
        if(strcmp(fcbptr->filename, dirname) == 0 && strcmp(fcbptr->exname, "") == 0)  
            break;  
        fcbptr++;  
    }  
    if(i == rbn / sizeof(fcb))  
    {  
        printf("Error,the directory is not exist.\n");  
        return;  
    }  
    fd = my_open(dirname);  
    rbn2 = do_read(fd, openfilelist[fd].length, text2);  
    fcbptr2 = (fcb *)text2;  
    for(j = 0; j < rbn2 / sizeof(fcb); j++)//判断要删除目录是否为空  
    {  
        if(strcmp(fcbptr2->filename, ".") && strcmp(fcbptr2->filename, "..") && strcmp(fcbptr2->filename, ""))  
        {  
            my_close(fd);  
            printf("Error,the directory is not empty.\n");  
            return;  
        }  
        fcbptr2++;  
    }  
    blkno = openfilelist[fd].first;  
    while(blkno != END)  
    {  
        fatptr1 = fat1 + blkno;  
        fatptr2 = fat2 + blkno;  
        blkno = fatptr1->id;  
        fatptr1->id = FREE;  
        fatptr2->id = FREE;  
    }  
    my_close(fd);  
    strcpy(fcbptr->filename, "");  
    fcbptr->free = 0;  
    openfilelist[curdir].count = i * sizeof(fcb);  
    do_write(curdir, (char *)fcbptr, sizeof(fcb), 2);  
    openfilelist[curdir].fcbstate = 1;  
}  
void my_ls()  
{  
    fcb *fcbptr;  
    char text[MAXTEXT];  
    int rbn, i;  
    openfilelist[curdir].count = 0;  
    rbn = do_read(curdir, openfilelist[curdir].length, text);  
    fcbptr = (fcb *)text;  
    for(i = 0; i < rbn / sizeof(fcb); i++)  
    {  
        if(fcbptr->free)  
        {  
            if(fcbptr->attribute & 0x20)  
                printf("%s\\\t\t<DIR>\t\t%d/%d/%d\t%02d:%02d:%02d\n", fcbptr->filename, (fcbptr->date >> 9) + 1980, (fcbptr->date >> 5) & 0x000f, fcbptr->date & 0x001f, fcbptr->time >> 11, (fcbptr->time >> 5) & 0x003f, fcbptr->time & 0x001f * 2);  
            else  
                printf("%s.%s\t\t%dB\t\t%d/%d/%d\t%02d:%02d:%02d\t\n", fcbptr->filename, fcbptr->exname, (int)(fcbptr->length), (fcbptr->date >> 9) + 1980, (fcbptr->date >> 5) & 0x000f, fcbptr->date & 0x1f, fcbptr->time >> 11, (fcbptr->time >> 5) & 0x3f, fcbptr->time & 0x1f * 2);  
        }  
        fcbptr++;  
    }  
}  
void my_create(char *filename)  
{  
    fcb *fcbptr;  
    fat *fat1, *fat2;  
    char *fname, *exname, text[MAXTEXT];  
    unsigned short blkno;  
    int rbn, i;  
    time_t now;  
    struct tm *nowtime;  
    fat1 = (fat *)(myvhard + BLOCKSIZE);  
    fat2 = (fat *)(myvhard + BLOCKSIZE);  
    fname = strtok(filename, ".");  
    exname = strtok(NULL, ".");  
    if(strcmp(fname, "") == 0)  
    {  
        printf("Error,creating file must have a right name.\n");  
        return;  
    }  
    if(!exname)  
    {  
        printf("Error,creating file must have a extern name.\n");  
        return;  
    }  
    openfilelist[curdir].count = 0;  
    rbn = do_read(curdir, openfilelist[curdir].length, text);  
    fcbptr = (fcb *)text;  
    for(i = 0; i < rbn / sizeof(fcb); i++)  
    {  
        if(strcmp(fcbptr->filename, fname) == 0 && strcmp(fcbptr->exname, exname) == 0)  
        {  
            printf("Error,the filename is already exist!\n");  
            return;  
        }  
        fcbptr++;  
    }  
    fcbptr = (fcb *)text;  
    for(i = 0; i < rbn / sizeof(fcb); i++)  
    {  
        if(fcbptr->free == 0)  
            break;  
        fcbptr++;  
    }  
    blkno = findblock();  
    if(blkno == -1)  
        return;  
    (fat1 + blkno)->id = END;  
    (fat2 + blkno)->id = END;  
  
    now = time(NULL);  
    nowtime = localtime(&now);  
    strcpy(fcbptr->filename, fname);  
    strcpy(fcbptr->exname, exname);  
    fcbptr->attribute = 0x00;  
    fcbptr->time = nowtime->tm_hour * 2048 + nowtime->tm_min * 32 + nowtime->tm_sec / 2;  
    fcbptr->date = (nowtime->tm_year - 80) * 512 + (nowtime->tm_mon + 1) * 32 + nowtime->tm_mday;  
    fcbptr->first = blkno;  
    fcbptr->length = 0;  
    fcbptr->free = 1;  
    openfilelist[curdir].count = i * sizeof(fcb);  
    do_write(curdir, (char *)fcbptr, sizeof(fcb), 2);  
    fcbptr = (fcb *)text;  
    fcbptr->length = openfilelist[curdir].length;  
    openfilelist[curdir].count = 0;  
    do_write(curdir, (char *)fcbptr, sizeof(fcb), 2);  
    openfilelist[curdir].fcbstate = 1;  
}  
void my_rm(char *filename)  
{  
    fcb *fcbptr;  
    fat *fat1, *fat2, *fatptr1, *fatptr2;  
    char *fname, *exname, text[MAXTEXT];  
    unsigned short blkno;  
    int rbn, i;  
    fat1 = (fat *)(myvhard + BLOCKSIZE);  
    fat2 = (fat *)(myvhard + 3 * BLOCKSIZE);  
    fname = strtok(filename, ".");  
    exname = strtok(NULL, ".");  
    if(strcmp(fname, "") == 0)  
    {  
        printf("Error,removing file must have a right name.\n");  
        return;  
    }  
    if(!exname)  
    {  
        printf("Error,removing file must have a extern name.\n");  
        return;  
    }  
    openfilelist[curdir].count = 0;  
    rbn = do_read(curdir, openfilelist[curdir].length, text);  
    fcbptr = (fcb *)text;  
    for(i = 0; i < rbn / sizeof(fcb); i++)  
    {  
        if(strcmp(fcbptr->filename, fname) == 0 && strcmp(fcbptr->exname, exname) == 0)  
            break;  
        fcbptr++;  
    }  
    if(i == rbn / sizeof(fcb))  
    {  
        printf("Error,the file is not exist.\n");  
        return;  
    }  
    openfilelist[curdir].count = 0;  
    rbn = do_read(curdir, openfilelist[curdir].length, text);  
    fcbptr = (fcb *)text;  
    for(i = 0; i < rbn / sizeof(fcb); i++)  
    {  
        if(strcmp(fcbptr->filename, fname) == 0 && strcmp(fcbptr->exname, exname) == 0)  
            break;  
        fcbptr++;  
    }  
    if(i == rbn / sizeof(fcb))  
    {  
        printf("Error,the file is not exist.\n");  
        return;  
    }  
    blkno = fcbptr->first;  
    while(blkno != END)  
    {  
        fatptr1 = fat1 + blkno;  
        fatptr2 = fat2 + blkno;  
        blkno = fatptr1->id;  
        fatptr1->id = FREE;  
        fatptr2->id = FREE;  
    }  
    strcpy(fcbptr->filename, "");  
    fcbptr->free = 0;  
    openfilelist[curdir].count = i * sizeof(fcb);  
    do_write(curdir, (char *)fcbptr, sizeof(fcb), 2);  
    openfilelist[curdir].fcbstate = 1;  
}  
int my_open(char *filename)  
{  
    fcb *fcbptr;  
    char *fname, exname[3], *str, text[MAXTEXT];  
    int rbn, fd, i;  
    fname = strtok(filename, ".");  
    str = strtok(NULL, ".");  
    if(str)  
        strcpy(exname, str);  
    //冗余
    else  
        strcpy(exname, "");  
    for(i = 0; i < MAXOPENFILE; i++)        //检查该文件是否已经被打开
    {  
        if(strcmp(openfilelist[i].filename, fname) == 0 && strcmp(openfilelist[i].exname, exname) == 0 && i != curdir)  
        {  
            printf("Error,the file is already open.\n");  
            return -1;  
        }  
    }  
    openfilelist[curdir].count = 0;  
    rbn = do_read(curdir, openfilelist[curdir].length, text);  
    fcbptr = (fcb *)text;  
    for(i = 0; i < rbn / sizeof(fcb); i++)  
    {  
        if(strcmp(fcbptr->filename, fname) == 0 && strcmp(fcbptr->exname, exname) == 0)  
            break;  
        fcbptr++;  
    }  
    if(i == rbn / sizeof(fcb))  
    {  
        printf("Error, file not exist.\n");  
        return -1;  
    }  
    fd = findopenfile();    //找到一个空的表项，把文件数据写入，模拟把磁盘文件写入内存的过程
    if(fd == -1)  
        return -1;  
    strcpy(openfilelist[fd].filename, fcbptr->filename);  
    strcpy(openfilelist[fd].exname, fcbptr->exname);  
    openfilelist[fd].attribute = fcbptr->attribute;  
    openfilelist[fd].time = fcbptr->time;  
    openfilelist[fd].date = fcbptr->date;  
    openfilelist[fd].first = fcbptr->first;  
    openfilelist[fd].length = fcbptr->length;  
    openfilelist[fd].free = fcbptr->free;  
    openfilelist[fd].dirno = openfilelist[curdir].first;  
    openfilelist[fd].diroff = i;  
    strcpy(openfilelist[fd].dir, openfilelist[curdir].dir);  
    strcat(openfilelist[fd].dir, filename);  
    if(fcbptr->attribute & 0x20)  
        strcat(openfilelist[fd].dir, "\\");  
    openfilelist[fd].father = curdir;  
    openfilelist[fd].count = 0;  
    openfilelist[fd].fcbstate = 0;  
    openfilelist[fd].topenfile = 1;  
    return fd;  
}  

int my_close(int fd)  
{  
    fcb *fcbptr;  
    int father;  
    if(fd < 0 || fd >= MAXOPENFILE)  
    {  
        printf("Error, the file does not exist.\n");  
        return -1;  
    }  
    if(openfilelist[fd].fcbstate)  
    {  
        fcbptr = (fcb *)malloc(sizeof(fcb));  
        strcpy(fcbptr->filename, openfilelist[fd].filename);                    //把openfilelist里的东西写到fcb里，模拟将内存写入磁盘的过程
        strcpy(fcbptr->exname, openfilelist[fd].exname);  
        fcbptr->attribute = openfilelist[fd].attribute;  
        fcbptr->time = openfilelist[fd].time;  
        fcbptr->date = openfilelist[fd].date;  
        fcbptr->first = openfilelist[fd].first;  
        fcbptr->length = openfilelist[fd].length;  
        fcbptr->free = openfilelist[fd].free;  
        father = openfilelist[fd].father;  
        openfilelist[father].count = openfilelist[fd].diroff * sizeof(fcb);  
        do_write(father, (char *)fcbptr, sizeof(fcb), 2);  
        free(fcbptr);  
        openfilelist[fd].fcbstate = 0;  
    }  
    strcpy(openfilelist[fd].filename, "");  
    strcpy(openfilelist[fd].exname, "");  
    openfilelist[fd].topenfile = 0;  
    return father;  
}  

int my_write(int fd)  
{  
    fat *fat1, *fat2, *fatptr1, *fatptr2;  
    int wstyle, len, ll, tmp;  
    char text[MAXTEXT];  
    unsigned short blkno;  
    fat1 = (fat *)(myvhard + BLOCKSIZE);  
    fat2 = (fat *)(myvhard + 3 * BLOCKSIZE);  
    if(fd < 0 || fd >= MAXOPENFILE)  
    {  
        printf("The file is not exist!\n");  
        return -1;  
    }  
    while(1)  
    {  
        printf("Please enter the number of write style:\n1.cut write\t2.cover write\t3.add write\n");  
        scanf("%d", &wstyle);  
        if(wstyle > 0 && wstyle < 4)  
            break;  
        printf("Input Error!");  
    }  
    getchar();  
    switch(wstyle)  
    {  
        case 1:  
            blkno = openfilelist[fd].first;  
            fatptr1 = fat1 + blkno;  
            fatptr2 = fat2 + blkno;  
            blkno = fatptr1->id;  
            fatptr1->id = END;  
            fatptr2->id = END;  
            while(blkno != END)  
            {  
                fatptr1 = fat1 + blkno;  
                fatptr2 = fat2 + blkno;  
                blkno = fatptr1->id;  
                fatptr1->id = FREE;  
                fatptr2->id = FREE;  
            }  
            openfilelist[fd].count = 0;  
            openfilelist[fd].length = 0;  
            break;  
        case 2:  
            openfilelist[fd].count = 0;  
            break;  
        case 3:  
            openfilelist[fd].count = openfilelist[fd].length;  
            break;  
        default:  
            break;  
    }  
    ll = 0;  
    printf("please input write data(end with Ctrl+Z):\n");  
    while(gets(text))  
    {  
        len = strlen(text);  
        text[len++] = '\n';  
        text[len] = '\0';  
        tmp = do_write(fd, text, len, wstyle);  
        if(tmp != -1)  
            ll += tmp;  
        if(tmp < len)  
        {  
            printf("Wirte Error!");  
            break;  
        }  
    }  
    return ll;  
}  
  
int my_read(int fd, int len)  
{  
    char text[MAXTEXT];  
    int ll;  
    if(fd < 0 || fd >= MAXOPENFILE)  
    {  
        printf("The File is not exist!\n");  
        return -1;  
    }  
    openfilelist[fd].count = 0;  
    ll = do_read(fd, len, text);  
    if(ll != -1)  
        printf("%s", text);  
    else  
        printf("Read Error!\n");  
    return ll;  
}  

void my_exitsys()  
{  
    FILE *fp;  
    while(curdir)  
        curdir = my_close(curdir);  
    fp = fopen(myfilename, "w");  
    fwrite(myvhard, SIZE, 1, fp);  
    fclose(fp);  
    free(myvhard);  
}  
 
int my_rename(int fd, char *oldName, char *newName)
{
    fcb *fcbptr;
    char *oldFileName, *newFileName, exnameOld[3], *str, text[MAXTEXT], exnameNew[3];  
    int rbn, i;
    oldFileName = strtok(oldName, ".");
    str = strtok(NULL, ".");
    printf("str = %s\noldFileName = %s\n", str, oldFileName);
    if (str)
        strcpy(exnameOld, str);
    else
        strcpy(exnameOld, "");
    // printf("11111111exnameOld = %s\n", exnameOld);
    newFileName = strtok(newName, ".");
    str = strtok(NULL, ".");
    // printf("222222222222exnameOld = %s\n", exnameOld);
    if (str)
        strcpy(exnameNew, str);
    else
        strcpy(exnameNew, "");
    // printf("3333333333exnameOld = %s\n", exnameOld);
    for (i = 0; i < MAXOPENFILE; i++)  //检查该文件是否已经被打开，如果打开，则无法改名
    {  
        if(strcmp(openfilelist[i].filename, oldFileName) == 0 && strcmp(openfilelist[i].exname, exnameOld) == 0 && i != curdir)
        {  
            printf("Error, you have to close the file before renaming\n");
            return -1;
        }  
    }
    openfilelist[curdir].count = 0;
    rbn = do_read(curdir, openfilelist[curdir].length, text);
    fcbptr = (fcb*)text;
    for(i = 0; i < rbn / sizeof(fcb); i++)  
    {  
        printf("fcb content is %s.%s\ntarget file is %s.%s\n", fcbptr -> filename, fcbptr -> exname, oldFileName, exnameOld);
        if(strcmp(fcbptr -> filename, oldFileName) == 0 && strcmp(fcbptr -> exname, exnameOld) == 0)  
            break;  
        fcbptr++;  
    } 
    if (i == rbn / sizeof(fcb)) 
    {
        printf("Error, file not exist.\n");
        return -1;
    }
    strcpy(fcbptr -> filename, newFileName);
    strcpy(fcbptr -> exname, exnameNew);
}