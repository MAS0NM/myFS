/*
���������
1   success
-1  malloc failed
-2  Read out of range
-3  �̿�ȫ��
-4  �ļ��򿪱����������ֻ��ͬʱ��10���ļ���
-5  ·��������
-6  Error,the dirname is already exist!����������Ŀ¼���ļ���
-7  �ļ��Ѵ�
-8  .//��..//���ɱ�ɾ��
-9  ��Ŀ¼������
-10 ����ɾ����Ŀ¼��Ϊ��
-11 Error,creating file must have a right filename.
-12 Error,creating file must have a extern name.
-13 Error,the filename is already exist!
-14 ָ����дģʽ������
-15 Error,removing file must have a right name.
-16 Error,removing file must have a extern name.
-17 Error, you have to close the file before renaming
-18 Error, you have to close the file before moving.
*/
#include ".\lib.h"
#include <time.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#pragma warning (disable : 4996)
/*
д����
������:
-1          malloc failed
write_size  success
*/
char myfilename[] = "myfilesys";
int do_write(int fd, char* text, int len, char wstyle) {
    fat* fat1, * fat2, * fatptr1, * fatptr2;
    unsigned char* buf, * blkptr;
    unsigned short blkno, blkoff;
    int i, write_size;
    fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat2 = (fat*)(myvhard + 3 * BLOCKSIZE);
    //д�뻺��
    buf = (unsigned char*)malloc(BLOCKSIZE);
    if (buf == NULL)
    {
        return -1;
    }
    //д���ļ��ĵ�һ�̿��
    blkno = openfilelist[fd].first;
    //дָ��λ��
    blkoff = openfilelist[fd].count;
    //д���ļ���fat
    fatptr1 = fat1 + blkno;
    fatptr2 = fat2 + blkno;
    //��д����ʼλ�ò��ڵ�ǰ��,�ҵ�ָ��ָ���λ��
    while (blkoff >= BLOCKSIZE)
    {
        blkno = fatptr1->id;
        if (blkno == END)
        {
            blkno = findblock();
            if (blkno == -1)
            {
                free(buf);
                return -1;
            }
            fatptr1->id = blkno;
            fatptr2->id = blkno;
            fatptr1 = fat1 + blkno;
            fatptr2 = fat2 + blkno;
            fatptr1->id = END;
            fatptr2->id = END;
        }
        else
        {
            fatptr1 = fat1 + blkno;
            fatptr2 = fat2 + blkno;
        }
        blkoff = blkoff - BLOCKSIZE;
    }
    //д�볤��
    write_size = 0;
    while (write_size < len)
    {
        //д��ַ
        blkptr = (unsigned char*)(myvhard + blkno * BLOCKSIZE);
        //������Ӵ�����ȡ���ڴ���
        for (i = 0; i < BLOCKSIZE; i++)
            buf[i] = blkptr[i];
        //�޸�Ϊд����Ϣ
        for (; blkoff < BLOCKSIZE; blkoff++)
        {
            buf[blkoff] = text[write_size++];
            openfilelist[fd].count++;
            if (write_size == len)
                break;
        }
        //��д������Ϣ���ڴ�д�����
        for (i = 0; i < BLOCKSIZE; i++)
            blkptr[i] = buf[i];
        //��һ��ûд�꣬����д��һ��
        if (write_size < len)
        {
            blkno = fatptr1->id;
            //��û����һ�飬��Ѱ�ҿ��̿�
            if (blkno == END)
            {
                blkno = findblock();
                if (blkno == -1)
                    break;
                fatptr1->id = blkno;
                fatptr2->id = blkno;
                fatptr1 = fat1 + blkno;
                fatptr2 = fat2 + blkno;
                fatptr1->id = END;
                fatptr2->id = END;
            }
            //ָ��ָ����һ��
            else
            {
                fatptr1 = fat1 + blkno;
                fatptr2 = fat2 + blkno;
            }
            blkoff = 0;
        }
    }
    //��д����Ϣ֮��ĳ��ȱ�ԭ�����ȳ������޸ĳ���
    if (openfilelist[fd].count > openfilelist[fd].length)
        openfilelist[fd].length = openfilelist[fd].count;
    openfilelist[fd].ischanged = 1;
    free(buf);
    return write_size;
}
/*
������
������:
-1          malloc failed
-2          Read out of range
read_size   success
*/
int do_read(int fd, int len, char* text) {
    fat* fat1, * fatptr;
    unsigned char* buf, * blkptr;
    unsigned short blkno, blkoff;
    int i, read_size;
    fat1 = (fat*)(myvhard + BLOCKSIZE);
    buf = (unsigned char*)malloc(BLOCKSIZE);

    if (buf == NULL)
    {
        return -1;
    }
    //Ҫ�����ļ������̿��
    blkno = openfilelist[fd].first;
    //��ָ��
    blkoff = openfilelist[fd].count;
    //��ָ�볬���ļ����ȣ�����
    if (blkoff >= openfilelist[fd].length)
    {
        free(buf);
        return -2;
    }
    //Ҫ�����ļ��׿��FAT
    fatptr = fat1 + blkno;
    //��ָ��λ�ò��ڵ�ǰ�飬���ҵ�Ҫ������һ��
    while (blkoff >= BLOCKSIZE)
    {
        blkno = fatptr->id;
        blkoff = blkoff - BLOCKSIZE;
        fatptr = fat1 + blkno;
    }
    read_size = 0;
    while (read_size < len)
    {
        //Ҫ���ļ����׿�
        blkptr = (unsigned char*)(myvhard + blkno * BLOCKSIZE);
        //������Ӵ��̶����ڴ�
        for (i = 0; i < BLOCKSIZE; i++)
            buf[i] = blkptr[i];
        //�Ӷ�ָ���λ�ÿ�ʼ������
        for (; blkoff < BLOCKSIZE; blkoff++)
        {
            text[read_size++] = buf[blkoff];
            openfilelist[fd].count++;
            //����ָ�����Ȼ�����ļ�����
            if (read_size == len || openfilelist[fd].count == openfilelist[fd].length)
                break;
        }
        //������ÿ�֮����δ���꣬��Ѱ���ļ���һ��
        if (read_size < len && openfilelist[fd].count != openfilelist[fd].length)
        {
            blkno = fatptr->id;
            //���������һ�飬������
            if (blkno == END)
                break;
            blkoff = 0;
            fatptr = fat1 + blkno;
        }
    }
    //Ϊ�������ı�����һ���ַ���������Ƿ�
    text[read_size] = '\0';
    free(buf);
    return read_size;
}
/*
��ȡ���п�
�����룺
i       �����̿��
-3      �̿�ȫ��
*/
unsigned short findblock() {
    unsigned short i;
    fat* fat1, * fatptr;
    fat1 = (fat*)(myvhard + BLOCKSIZE);
    for (i = 7; i < SIZE / BLOCKSIZE; i++)
    {
        fatptr = fat1 + i;
        if (fatptr->id == FREE)
            return i;
    }
    return -3;
}
/*
��ȡ�����ļ��򿪱���
�����룺
i   �����ļ��򿪱����±�
-4  �ļ��򿪱����������ֻ��ͬʱ��10���ļ���
*/
int findopenfile() {
    int i;
    for (i = 0; i < MAXTEXT; i++)
    {
        if (openfilelist[i].topenfile == 0)
            return i;
    }
    return -4;
}

/*
ϵͳ��������
*/
void startsys() {
    FILE* fp;
    unsigned char* buf = (unsigned char*)malloc(SIZE);
    fcb* root;
    int i;
    //����������̿ռ�
    myvhard = (unsigned char*)malloc(SIZE);
    //��myvhard��ǰSIZE���ֽ��� 0 �滻
    memset(myvhard, 0, SIZE);
    if ((fp = fopen(myfilename, "r")) != NULL)
    {   //���������ļ���ȡ��������
        fread(buf, SIZE, 1, fp);
        //�رմ򿪵��ļ�������������д���ļ����ͷ�ϵͳ�ṩ�ļ���Դ
        fclose(fp);
        //�жϿ�ʼ��8���ֽ������Ƿ�Ϊ�ļ�ϵͳħ��
        if (strcmp(((block0*)buf)->magic, "10101010"))
        {
            my_format();
        }
        else
        {
            //�������ļ���ʼ����������
            for (i = 0; i < SIZE; i++)
                myvhard[i] = buf[i];
        }
    }
    else
    {
        my_format();
    }
    //ָ���������׵�ַ
    root = (fcb*)(myvhard + 5 * BLOCKSIZE);
    //���ø�Ŀ¼������Ŀ¼���ļ��򿪱��еĵ�һ���
    strcpy(openfilelist[0].filename, root->filename);
    strcpy(openfilelist[0].exname, root->exname);
    openfilelist[0].type = root->type;
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
    openfilelist[0].ischanged = 0;
    openfilelist[0].topenfile = 1;
    for (i = 1; i < MAXOPENFILE; i++)
        openfilelist[i].topenfile = 0;
    curdir = 0;
    strcpy(currentdir, "\\root\\");
    startp = ((block0*)myvhard)->startblock;
}
/*
ϵͳ��ʽ��
*/
void my_format() {
    FILE* fp;
    fat* fat1, * fat2;
    block0* blk0;
    time_t now;
    struct tm* nowtime;
    fcb* root;
    //���������顢FAT1��FAT2�͸�Ŀ¼�Ŀռ�
    blk0 = (block0*)myvhard;
    fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat2 = (fat*)(myvhard + 3 * BLOCKSIZE);
    root = (fcb*)(myvhard + 5 * BLOCKSIZE);
    //���������������
    strcpy(blk0->magic, "10101010");
    strcpy(blk0->information, "Filesys\n Blocksize=1KB Whole size=1000KB Blocknum=1000 RootBlocknum=2\n");
    blk0->root = 5;
    blk0->startblock = (unsigned char*)root;
    //��ʼ��FAT
    for (int i = 0; i < 5; i++)
    {
        fat1[i].id = END;
        fat2[i].id = END;
    }
    fat1[5].id = 6;
    fat2[5].id = 6;
    fat1[6].id = END;
    fat2[6].id = END;
    for (int i = 7; i < SIZE / BLOCKSIZE; i++)
    {
        fat1[i].id = FREE;
        fat2[i].id = FREE;
    }
    //���ø�Ŀ¼��'.','..'Ŀ¼
    now = time(NULL);
    nowtime = localtime(&now);
    strcpy(root->filename, ".");
    strcpy(root->exname, "");
    root->type = 0;
    root->time = nowtime->tm_hour * 2048 + nowtime->tm_min * 32 + nowtime->tm_sec / 2;
    root->date = (nowtime->tm_year - 80) * 512 + (nowtime->tm_mon + 1) * 32 + nowtime->tm_mday;
    root->first = 5;
    root->length = 2 * sizeof(fcb);
    root->free = 1;
    root++;

    now = time(NULL);
    nowtime = localtime(&now);
    strcpy(root->filename, "..");
    strcpy(root->exname, "");
    root->type = 0;
    root->time = nowtime->tm_hour * 2048 + nowtime->tm_min * 32 + nowtime->tm_sec / 2;
    root->date = (nowtime->tm_year - 80) * 512 + (nowtime->tm_mon + 1) * 32 + nowtime->tm_mday;
    root->first = 5;
    root->length = 2 * sizeof(fcb);
    root->free = 1;
    //�����ļ�ϵͳ��Ϣ
    fp = fopen(myfilename, "w");
    fwrite(myvhard, SIZE, 1, fp);
    fclose(fp);
}
/*
���ĵ�ǰĿ¼
�����룺
0   success
-5  ·��������
*/
int my_cd(char* dirname) {
    char* dir;
    int fd;
    dir = strtok(dirname, "\\");            //�ֽ��ַ���Ϊһ���ַ�����dirnameΪҪ�ֽ���ַ�����"\\"Ϊ�ָ����ַ���  
    if (strcmp(dir, ".") == 0)
        return 0;
    else if (strcmp(dir, "..") == 0)
    {
        if (curdir)
            curdir = my_close(curdir);      //�����ϼ�Ŀ¼
        return 0;
    }
    else if (strcmp(dir, "root") == 0)       //����root�ڵ�
    {
        while (curdir)
            curdir = my_close(curdir);
        dir = strtok(NULL, "\\");
    }
    while (dir)
    {
        fd = my_open(dir);
        if (fd != -1)
            curdir = fd;
        else
            return -5;
        dir = strtok(NULL, "\\");
    }
}
/*
������Ŀ¼
������:
1       success
-3      ��������
-4      �ļ��򿪱����������ֻ��ͬʱ��10���ļ���
-5      �ļ�������
-6      Error,the dirname is already exist!����������Ŀ¼��
-7      �ļ��Ѵ�
*/
int my_mkdir(char* dirname) {
    fcb* fcbptr;
    fat* fat1, * fat2;
    time_t now;
    struct tm* nowtime;
    char text[MAXTEXT];
    unsigned short blkno;
    int read_size, fd, i;
    fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat2 = (fat*)(myvhard + 3 * BLOCKSIZE);
    openfilelist[curdir].count = 0;
    //������ǰĿ¼��Ϣ
    read_size = do_read(curdir, openfilelist[curdir].length, text);
    fcbptr = (fcb*)text;
    //�ڵ�ǰĿ¼���ң��Ƿ�������Ŀ¼
    for (i = 0; i < read_size / sizeof(fcb); i++)
    {
        if (strcmp(fcbptr->filename, dirname) == 0 && strcmp(fcbptr->exname, "") == 0)
        {
            return -5;
        }
        fcbptr++;
    }
    //Ѱ�ҿ�Ŀ¼��
    fcbptr = (fcb*)text;
    for (i = 0; i < read_size / sizeof(fcb); i++)
    {
        if (fcbptr->free == 0)
            break;
        fcbptr++;
    }
    //Ѱ�ҿ����̿�  
    blkno = findblock();
    if (blkno == -3)return -3;
    (fat1 + blkno)->id = END;
    (fat2 + blkno)->id = END;
    now = time(NULL);
    nowtime = localtime(&now);
    //���õ�ǰĿ¼��FCB
    strcpy(fcbptr->filename, dirname);
    strcpy(fcbptr->exname, "");
    fcbptr->type = 0;
    fcbptr->time = nowtime->tm_hour * 2048 + nowtime->tm_min * 32 + nowtime->tm_sec / 2;
    fcbptr->date = (nowtime->tm_year - 80) * 512 + (nowtime->tm_mon + 1) * 32 + nowtime->tm_mday;
    fcbptr->first = blkno;
    fcbptr->length = 2 * sizeof(fcb);
    fcbptr->free = 1;
    openfilelist[curdir].count = i * sizeof(fcb);
    do_write(curdir, (char*)fcbptr, sizeof(fcb), 2);
    //������Ŀ¼��'.','..'Ŀ¼ 
    fd = my_open(dirname);
    if (fd == -1 || fd == -5 || fd == -7)return fd;
    fcbptr = (fcb*)malloc(sizeof(fcb));
    now = time(NULL);
    nowtime = localtime(&now);
    strcpy(fcbptr->filename, ".");
    strcpy(fcbptr->exname, "");
    fcbptr->type = 0;
    fcbptr->time = nowtime->tm_hour * 2048 + nowtime->tm_min * 32 + nowtime->tm_sec / 2;
    fcbptr->date = (nowtime->tm_year - 80) * 512 + (nowtime->tm_mon + 1) * 32 + nowtime->tm_mday;
    fcbptr->first = blkno;
    fcbptr->length = 2 * sizeof(fcb);
    fcbptr->free = 1;
    do_write(fd, (char*)fcbptr, sizeof(fcb), 2);

    now = time(NULL);
    nowtime = localtime(&now);
    strcpy(fcbptr->filename, "..");
    strcpy(fcbptr->exname, "");
    fcbptr->type = 0;
    fcbptr->time = nowtime->tm_hour * 2048 + nowtime->tm_min * 32 + nowtime->tm_sec / 2;
    fcbptr->date = (nowtime->tm_year - 80) * 512 + (nowtime->tm_mon + 1) * 32 + nowtime->tm_mday;
    fcbptr->first = blkno;
    fcbptr->length = 2 * sizeof(fcb);
    fcbptr->free = 1;
    do_write(fd, (char*)fcbptr, sizeof(fcb), 2);
    free(fcbptr);
    my_close(fd);
    //��ǰĿ¼����Ŀ¼Ŀ¼��FCB����Ϣ����
    fcbptr = (fcb*)text;
    fcbptr->length = openfilelist[curdir].length;
    openfilelist[curdir].count = 0;
    do_write(curdir, (char*)fcbptr, sizeof(fcb), 2);
    openfilelist[curdir].ischanged = 1;
    return 1;
}
/*
ɾ��ָ��Ŀ¼
������:
1       success
-8      .//��..//���ɱ�ɾ��
-9      ��Ŀ¼������
-10     ����ɾ����Ŀ¼��Ϊ��
*/
int my_rmdir(char* dirname) {
    fcb* fcbptr, * fcbptr2;
    fat* fat1, * fat2, * fatptr1, * fatptr2;
    char text[MAXTEXT], text2[MAXTEXT];
    unsigned short blkno;
    int read_size, read_size2, fd, i, j;
    fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat2 = (fat*)(myvhard + 3 * BLOCKSIZE);
    //.Ŀ¼��..Ŀ¼���ɱ�ɾ��
    if (strcmp(dirname, ".") == 0 || strcmp(dirname, "..") == 0)
        return -8;
    //��ȡ��ǰĿ¼��Ϣ
    openfilelist[curdir].count = 0;
    read_size = do_read(curdir, openfilelist[curdir].length, text);
    fcbptr = (fcb*)text;
    //����Ҫɾ����Ŀ¼  
    for (i = 0; i < read_size / sizeof(fcb); i++)
    {
        if (strcmp(fcbptr->filename, dirname) == 0 && strcmp(fcbptr->exname, "") == 0)
            break;
        fcbptr++;
    }
    if (i == read_size / sizeof(fcb))
        return -9;
    //��Ҫɾ����Ŀ¼���ж�Ҫɾ��Ŀ¼�Ƿ�Ϊ��
    fd = my_open(dirname);
    read_size2 = do_read(fd, openfilelist[fd].length, text2);
    fcbptr2 = (fcb*)text2;
    for (j = 0; j < read_size2 / sizeof(fcb); j++)
    {
        if (strcmp(fcbptr2->filename, ".") && strcmp(fcbptr2->filename, "..") && strcmp(fcbptr2->filename, ""))
        {
            my_close(fd);
            return -10;
        }
        fcbptr2++;
    }
    //����FAT
    blkno = openfilelist[fd].first;
    while (blkno != END)
    {
        fatptr1 = fat1 + blkno;
        fatptr2 = fat2 + blkno;
        blkno = fatptr1->id;
        fatptr1->id = FREE;
        fatptr2->id = FREE;
    }
    my_close(fd);
    //����FCB
    strcpy(fcbptr->filename, "");
    fcbptr->free = 0;
    openfilelist[curdir].count = i * sizeof(fcb);
    do_write(curdir, (char*)fcbptr, sizeof(fcb), 2);
    openfilelist[curdir].ischanged = 1;
    return 1;
}
/*
��ʾ��ǰ�б�
*/
void my_ls(char *res) {
    // char res[MAXTEXT];
    fcb* fcbptr;
    char text[MAXTEXT];
    int read_size, i, ptr;
    ptr = 0;
    openfilelist[curdir].count = 0;
    //读取当前目录信息
    read_size = do_read(curdir, openfilelist[curdir].length, text);
    fcbptr = (fcb*)text;
    for (i = 0; i < read_size / sizeof(fcb); i++) {
        if (fcbptr->free) {
            if (fcbptr->type == 0)
             ptr+=   sprintf(res+ptr, "%s\\\t\t<DIR>\t\t%d/%d/%d\t%02d:%02d:%02d\n", fcbptr->filename, (fcbptr->date >> 9) + 1980, (fcbptr->date >> 5) & 0x000f, fcbptr->date & 0x001f, fcbptr->time >> 11, (fcbptr->time >> 5) & 0x003f, fcbptr->time & 0x001f * 2);

            else

              ptr+=  sprintf(res+ptr, "%s.%s\t\t%dB\t\t%d/%d/%d\t%02d:%02d:%02d\t\n", fcbptr->filename, fcbptr->exname, (int)(fcbptr->length), (fcbptr->date >> 9) + 1980, (fcbptr->date >> 5) & 0x000f, fcbptr->date & 0x1f, fcbptr->time >> 11, (fcbptr->time >> 5) & 0x3f, fcbptr->time & 0x1f * 2);
        }
        fcbptr++;
    }
}
/*
�����ļ�
������:
1       success
-3      ��������
-11     Error,creating file must have a right filename.
-12     Error,creating file must have a extern name.
-13     Error,the filename is already exist!
*/
int my_create(char* filename)
{
    fcb* fcbptr;
    fat* fat1, * fat2;
    char* fname, * exname, text[MAXTEXT];
    unsigned short blkno;
    int rbn, i;
    time_t now;
    struct tm* nowtime;
    fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat2 = (fat*)(myvhard + BLOCKSIZE);
    //����ļ���
    fname = strtok(filename, ".");
    exname = strtok(NULL, ".");
    if (strcmp(fname, "") == 0)
        return -11;
    if (!exname)
        return -12;
    openfilelist[curdir].count = 0;
    //��ȡ��ǰĿ¼��Ϣ
    rbn = do_read(curdir, openfilelist[curdir].length, text);
    fcbptr = (fcb*)text;
    //����Ƿ����ļ�����
    for (i = 0; i < rbn / sizeof(fcb); i++)
    {
        if (strcmp(fcbptr->filename, fname) == 0 && strcmp(fcbptr->exname, exname) == 0)
            return -13;
        fcbptr++;
    }
    //�ҵ��յ�Ŀ¼��
    fcbptr = (fcb*)text;
    for (i = 0; i < rbn / sizeof(fcb); i++)
    {
        if (fcbptr->free == 0)
            break;
        fcbptr++;
    }
    //��ȡ�յ��̿�
    blkno = findblock();
    if (blkno == -3)
        return -3;
    (fat1 + blkno)->id = END;
    (fat2 + blkno)->id = END;
    //���ļ���Ϣд���µ�FCB
    now = time(NULL);
    nowtime = localtime(&now);
    strcpy(fcbptr->filename, fname);
    strcpy(fcbptr->exname, exname);
    fcbptr->type = 1;
    fcbptr->time = nowtime->tm_hour * 2048 + nowtime->tm_min * 32 + nowtime->tm_sec / 2;
    fcbptr->date = (nowtime->tm_year - 80) * 512 + (nowtime->tm_mon + 1) * 32 + nowtime->tm_mday;
    fcbptr->first = blkno;
    fcbptr->length = 0;
    fcbptr->free = 1;
    openfilelist[curdir].count = i * sizeof(fcb);
    do_write(curdir, (char*)fcbptr, sizeof(fcb), 2);
    //���¸�Ŀ¼��Ϣ
    fcbptr = (fcb*)text;
    fcbptr->length = openfilelist[curdir].length;
    openfilelist[curdir].count = 0;
    do_write(curdir, (char*)fcbptr, sizeof(fcb), 2);
    openfilelist[curdir].ischanged = 1;
    return 1;
}
/*
ɾ���ļ�
������:
1       success
-5      Error,the file is not exist.
-15     Error,removing file must have a right name.
-16     Error,removing file must have a extern name.
*/
int my_rm(char* filename)
{
    fcb* fcbptr;
    fat* fat1, * fat2, * fatptr1, * fatptr2;
    char* fname, * exname, text[MAXTEXT];
    unsigned short blkno;
    int read_size, i;
    fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat2 = (fat*)(myvhard + 3 * BLOCKSIZE);
    //�ֽ��ļ�������չ��
    fname = strtok(filename, ".");
    exname = strtok(NULL, ".");
    //�����ļ�������չ���ĺϷ���
    if (strcmp(fname, "") == 0)
        return -15;
    if (!exname)
        return -16;
    //��ȡ��ǰĿ¼��Ϣ
    openfilelist[curdir].count = 0;
    read_size = do_read(curdir, openfilelist[curdir].length, text);
    fcbptr = (fcb*)text;
    //����Ҫɾ�����ļ�
    for (i = 0; i < read_size / sizeof(fcb); i++)
    {
        if (strcmp(fcbptr->filename, fname) == 0 && strcmp(fcbptr->exname, exname) == 0)
            break;
        fcbptr++;
    }
    //δ���ҵ���
    if (i == read_size / sizeof(fcb))
        return -5;
    /*
    �����ظ�
    openfilelist[curdir].count = 0;
    rbn = do_read(curdir, openfilelist[curdir].length, text);
    fcbptr = (fcb*)text;
    for (i = 0; i < rbn / sizeof(fcb); i++)
    {
        if (strcmp(fcbptr->filename, fname) == 0 && strcmp(fcbptr->exname, exname) == 0)
            break;
        fcbptr++;
    }
    if (i == rbn / sizeof(fcb))
    {
        printf("Error,the file is not exist.\n");
        return;
    }
    */
    //��ȡ�ļ��洢�����̿��
    blkno = fcbptr->first;
    //�ͷ��ļ�ռ�õĿռ�
    while (blkno != END)
    {
        fatptr1 = fat1 + blkno;
        fatptr2 = fat2 + blkno;
        blkno = fatptr1->id;
        fatptr1->id = FREE;
        fatptr2->id = FREE;
    }
    strcpy(fcbptr->filename, "");
    fcbptr->free = 0;
    //���µ�ǰĿ¼��Ϣ
    openfilelist[curdir].count = i * sizeof(fcb);
    do_write(curdir, (char*)fcbptr, sizeof(fcb), 2);
    openfilelist[curdir].ischanged = 1;
    return 1;
}
/*
�򿪵�ǰĿ¼�µ��ļ�
������:
fd      ��ǰ���ļ����ļ��򿪱��±�
-4      �ļ��򿪱����������ֻ��ͬʱ��10���ļ���
-5      �ļ�������
-7      �ļ��Ѵ�
*/
int my_open(char* filename)
{
    fcb* fcbptr;
    char* fname, exname[3], * str, text[MAXTEXT];
    int read_size, fd, i;
    //�ֽ���չ�����ļ���
    fname = strtok(filename, ".");
    str = strtok(NULL, ".");
    if (str)
        strcpy(exname, str);
    //Ŀ¼��չ��Ϊ��
    else
        strcpy(exname, "");
    //��鵱ǰ�Ƿ����Ѵ򿪵������ļ�
    for (i = 0; i < MAXOPENFILE; i++)
        if (!strcmp(openfilelist[i].filename, fname) && !strcmp(openfilelist[i].exname, exname) && i != curdir)
            return -7;
    //����ǰĿ¼��Ϣ��Ѱ��Ҫ���ļ���FCB
    openfilelist[curdir].count = 0;
    read_size = do_read(curdir, openfilelist[curdir].length, text);
    fcbptr = (fcb*)text;
    for (i = 0; i < read_size / sizeof(fcb); i++)
    {
        if (strcmp(fcbptr->filename, fname) == 0 && strcmp(fcbptr->exname, exname) == 0)
            break;
        fcbptr++;
    }
    if (i == read_size / sizeof(fcb))
        return -5;
    //��ȡ�����ļ��򿪱�����
    fd = findopenfile();
    if (fd == -4)
        return -4;
    strcpy(openfilelist[fd].filename, fcbptr->filename);
    strcpy(openfilelist[fd].exname, fcbptr->exname);
    openfilelist[fd].type = fcbptr->type;
    openfilelist[fd].time = fcbptr->time;
    openfilelist[fd].date = fcbptr->date;
    openfilelist[fd].first = fcbptr->first;
    openfilelist[fd].length = fcbptr->length;
    openfilelist[fd].free = fcbptr->free;
    openfilelist[fd].dirno = openfilelist[curdir].first;
    openfilelist[fd].diroff = i;
    strcpy(openfilelist[fd].dir, openfilelist[curdir].dir);
    strcat(openfilelist[fd].dir, filename);
    //���򿪵���Ŀ¼����·��������"\\"
    if (fcbptr->type == 0)
        strcat(openfilelist[fd].dir, "\\");
    openfilelist[fd].father = curdir;
    openfilelist[fd].count = 0;
    openfilelist[fd].ischanged = 0;
    openfilelist[fd].topenfile = 1;
    return fd;
}
/*
�رյ�ǰĿ¼
������:
-5      �ļ�������
father  ��Ŀ¼�ڴ򿪱��е�λ��
*/
int my_close(int fd)
{
    fcb* fcbptr;
    int father;
    if (fd < 0 || fd >= MAXOPENFILE)
        return -5;
    //�����ļ��Ѿ����޸ģ�Ҫ���б������
    if (openfilelist[fd].ischanged)
    {
        fcbptr = (fcb*)malloc(sizeof(fcb));
        strcpy(fcbptr->filename, openfilelist[fd].filename);
        strcpy(fcbptr->exname, openfilelist[fd].exname);
        fcbptr->type = openfilelist[fd].type;
        fcbptr->time = openfilelist[fd].time;
        fcbptr->date = openfilelist[fd].date;
        fcbptr->first = openfilelist[fd].first;
        fcbptr->length = openfilelist[fd].length;
        fcbptr->free = openfilelist[fd].free;
        father = openfilelist[fd].father;
        openfilelist[father].count = openfilelist[fd].diroff * sizeof(fcb);
        do_write(father, (char*)fcbptr, sizeof(fcb), 2);
        free(fcbptr);
        openfilelist[fd].ischanged = 0;
    }
    //��չر��ļ�ռ�õĴ��ļ���
    strcpy(openfilelist[fd].filename, "");
    strcpy(openfilelist[fd].exname, "");
    openfilelist[fd].topenfile = 0;
    return father;
}
/*
д�ļ�(�ļ��򿪱��±꣬дģʽ��д����)
дģʽ��1���ض�д��2������д��3��׷��д��
������:
write_size  success
-1          malloc failed
-5      �ļ�������
-14     ָ����дģʽ������
*/
int my_write(int fd, int wstyle, char* text)
{
    fat* fat1, * fat2, * fatptr1, * fatptr2;
    int len, write_size;
    len = 0;
    unsigned short blkno;
    fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat2 = (fat*)(myvhard + 3 * BLOCKSIZE);
    //���fd�ĺϷ���
    if (fd < 0 || fd >= MAXOPENFILE)
        return -5;
    //����дģʽ�ĺϷ���
    if (wstyle < 0 && wstyle > 4)
        return -14;
    switch (wstyle) {
        //�ض�д������ļ�������������д��
    case 1:
        blkno = openfilelist[fd].first;
        fatptr1 = fat1 + blkno;
        fatptr2 = fat2 + blkno;
        blkno = fatptr1->id;
        fatptr1->id = END;
        fatptr2->id = END;
        while (blkno != END)
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
        //����д����дָ��λ�ÿ�ʼд������
    case 2:
        openfilelist[fd].count = 0;
        break;
        //׷��д�����ļ���Ϣβ��׷��д����
    case 3:
        openfilelist[fd].count = openfilelist[fd].length;
        break;
    default:
        break;
    }
    len=strlen(text);
    write_size = do_write(fd, text, len, wstyle);

    return write_size;
}
/*
���ļ�(�ļ��򿪱��±꣬�����ȣ������)
������:
write_size  success
-1          malloc failed
-2          Read out of range
-5      �ļ�������
*/
int my_read(int fd, int len, char* text)
{
    int read_size;
    if (fd < 0 || fd >= MAXOPENFILE)
        return -5;
    openfilelist[fd].count = 0;
    read_size = do_read(fd, len, text);
    return read_size;
}
/*
�˳�ϵͳ
*/
void my_exitsys()
{
    FILE* fp;
    while (curdir)
        curdir = my_close(curdir);
    //���ļ�ϵͳ����
    fp = fopen(myfilename, "w");
    fwrite(myvhard, SIZE, 1, fp);
    fclose(fp);
    free(myvhard);
}

/*
������
������:
1       success
-5      �ļ�������
-17     Error, you have to close the file before renaming
*/
int my_rename(int fd, char* oldName, char* newName)
{
    fcb* fcbptr;
    char* oldFileName, * newFileName, exnameOld[3], * str, text[MAXTEXT], exnameNew[3];
    int rbn, i;
    rbn = 0;
    unsigned short blkno;
    oldFileName = strtok(oldName, ".");
    str = strtok(NULL, ".");
    if (str)
        strcpy(exnameOld, str);
    else
        strcpy(exnameOld, "");
    newFileName = strtok(newName, ".");
    str = strtok(NULL, ".");
    if (str)
        strcpy(exnameNew, str);
    else
        strcpy(exnameNew, "");
    for (i = 0; i < MAXOPENFILE; i++)  //�����ļ��Ƿ��Ѿ����򿪣�����򿪣����޷�����
        if (strcmp(openfilelist[i].filename, oldFileName) == 0 && strcmp(openfilelist[i].exname, exnameOld) == 0 && i != curdir)
            return -15;
    blkno = openfilelist[fd].first;
    fcbptr = (fcb*)(myvhard + blkno * BLOCKSIZE);
    for (i = 0; i < BLOCKSIZE / sizeof(fcb); i++)
    {

        if (strcmp(fcbptr->filename, oldFileName) == 0 && strcmp(fcbptr->exname, exnameOld) == 0)
            break;
        fcbptr++;
    }
    if (i == rbn / sizeof(fcb))
        return -5;
    strcpy(fcbptr->filename, newFileName);
    strcpy(fcbptr->exname, exnameNew);
    return 1;
}
//�ƶ��ļ�
int my_move(char* filename, char* dir)
{
    fcb* fcbptr1, * fcbptr2;
    char* fname, exname[3], * str, text[MAXTEXT];
    int rbn, fd, i;
    fname = strtok(filename, ".");
    str = strtok(NULL, ".");
    if (str)
        strcpy(exname, str);
    for (i = 0; i < MAXOPENFILE; i++)        //�����ļ��Ƿ��Ѿ�����
    {
        if (strcmp(openfilelist[i].filename, fname) == 0 && strcmp(openfilelist[i].exname, exname) == 0 && i != curdir)
            return -18;
    }
    openfilelist[curdir].count = 0;
    do_read(curdir, openfilelist[curdir].length, text);
    fcbptr1 = (fcb*)text;
    for (i = 0; i < BLOCKSIZE / sizeof(fcb); i++)
    {
        if (strcmp(fcbptr1->filename, fname) == 0 && strcmp(fcbptr1->exname, exname) == 0)
            break;
        fcbptr1++;
    }
    if (i == BLOCKSIZE / sizeof(fcb))
        return -18;
    fd = my_open(dir);
    if (fd != -1)
        curdir = fd;

    openfilelist[curdir].count = 0;
    do_read(curdir, openfilelist[curdir].length, text);
    fcbptr2 = (fcb*)text;

    do_write(curdir, (char*)fcbptr1, sizeof(fcb), 2);
    return 1;
}