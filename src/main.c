// #include ".\func.c"
// #include "..\include\define.h"
#include "..\lib\lib.h"
#include <stdio.h>
#include <malloc.h>
void my_error(int info)
{
    switch(info)
    {
        case -1:
            printf(" malloc failed\n");
            break;
        case -2:
            printf("Read out of range\n");
            break;
        case -3:
            printf("vhard is full\n");
            break;
        case -4:
            printf("open file list is full\n");
            break;
        case -5:
            printf("path not exist\n");
            break;
        case -6:
            printf("Error, the dirname is already exist!\n");
            break;
        case -7:
            printf("file is opened\n");
            break;
        case -8:
            printf("can't delete\n");
            break;
        case -9:
            printf("directory not exist\n");
            break;
        case -10:
            printf("directory is not empty\n");
            break;
        case -11:
            printf("Error, creating file must have a right filename.\n");
            break;
        case -12:
            printf("Error, creating file must have a extern name.n");
            break;
        case -13:
            printf("Error, the filename is already exist!\n");
            break;
        case -14:
            printf("is not a write style\n");
            break;
        case -15:
            printf("Error, removing file must have a right name.\n");
            break;
        case -16:
            printf("Error, removing file must have a extern name.\n");
            break;
        case -17:
            printf("Error, you have to close the file before renaming\n");
            break;
        case -18:
            printf("Error, you have to close the file before moving.\n");
            break;
    }
}

int main()  
{  
    char cmd[15][10] = {"cd", "mkdir", "rmdir", "ls", "create", "rm", "open", "close", "write", "read", "exit", "rename", "mv"};  
    char s[30], *sp;
    char *list = (char*)malloc(10000);
    int cmdn, flag = 1, i, info;  
    startsys();  
    printf("*********************File System V1.0*******************************\n\n");  
    printf("命令名\t\t命令参数\t\t命令说明\n\n");  
    printf("cd\t\t目录名(路径名)\t\t切换当前目录到指定目录\n");  
    printf("mkdir\t\t目录名\t\t\t在当前目录创建新目录\n");  
    printf("rmdir\t\t目录名\t\t\t在当前目录删除指定目录\n");  
    printf("ls\t\t无\t\t\t显示当前目录下的目录和文件\n");  
    printf("create\t\t文件名\t\t\t在当前目录下创建指定文件\n");  
    printf("rm\t\t文件名\t\t\t在当前目录下删除指定文件\n");  
    printf("open\t\t文件名\t\t\t在当前目录下打开指定文件\n");  
    printf("rename\t\t文件名\t\t文件名\t\t\t在当前目录下修改指定文件的文件名\n");  
    printf("mv\t\t文件名\t\t文件夹名\t\t\t将当前目录下的指定文件复制到指定文件夹下\n"); 
    printf("write\t\t无\t\t\t在打开文件状态下，写该文件\n");  
    printf("read\t\t无\t\t\t在打开文件状态下，读取该文件\n");  
    printf("close\t\t无\t\t\t在打开文件状态下，读取该文件\n");  
    printf("exit\t\t无\t\t\t退出系统\n\n");  
    printf("*********************************************************************\n\n"); 
    while(flag)  
    {  
        printf("%s>", openfilelist[curdir].dir);  
        gets(s);  
        cmdn = -1;                                  //当前指令的编号
        if(strcmp(s, ""))                           //和空字符串比较，判断是否为空
        {  
            sp = strtok(s, " ");                    //获取指令串按空格分隔的第一个子字符串
            for(i = 0; i < 15; i++)  
            {  
                if(strcmp(sp, cmd[i]) == 0)         //将输入指令同指令表中的指令进行比较
                {  
                    cmdn = i;  
                    break;  
                }  
            }  

            switch(cmdn)  
            {  
                case 0:  
                    sp = strtok(NULL, " ");                             //此时sp指向第一个子串后面的字符串
                    if(sp && (openfilelist[curdir].type == 0))   //和b'10000按位与
                    {
                        info = my_cd(sp);  
                        my_error(info);
                    }
                    else  
                        printf("Please input the right command.\n");  
                    break;  
                case 1:  
                    sp = strtok(NULL, " ");  
                    if(sp && (openfilelist[curdir].type == 0))  
                    {
                        info = my_mkdir(sp); 
                        my_error(info);
                    }
                    else  
                        printf("Please input the right command.\n");  
                    break;  
                case 2:  
                    sp = strtok(NULL, " ");  
                    if(sp && (openfilelist[curdir].type == 0))  
                    {
                        info = my_rmdir(sp);
                        my_error(info);
                    }  
                    else  
                        printf("Please input the right command.\n");  
                    break;  
                case 3:  
                    if(openfilelist[curdir].type == 0)
                    {
                        my_ls(list);  
                        printf("list in this directory is\n%s", list);
                    }
                    else  
                        printf("Please input the right command.\n");  
                    break;  
                case 4:  
                    sp = strtok(NULL, " ");  
                    if(sp && (openfilelist[curdir].type == 0))  
                        my_create(sp);  
                    else  
                        printf("Please input the right command.\n");  
                    break;  
                case 5:  
                    sp = strtok(NULL, " ");  
                    if(sp && (openfilelist[curdir].type == 0))  
                    {
                        info = my_rm(sp);  
                        my_error(info);
                    }
                    else  
                        printf("Please input the right command.\n");  
                    break;  
                case 6:  
                    sp = strtok(NULL, " ");  
                    if(sp && (openfilelist[curdir].type == 0))  
                    {  
                        if(strchr(sp, '.'))                             //查找sp中'.'首次出现的位置  
                        {
                            info = my_open(sp);
                            if (info >=0 )
                                curdir = info;
                            else 
                                my_error(info);
                        }
                        else  
                            printf("the openfile should have exname.\n");  
                    }  
                    else  
                        printf("Please input the right command.\n");  
                    break; 
                case 7:  
                    if(!(openfilelist[curdir].type == 0))  
                    {
                        curdir = my_close(curdir); 
                        my_error(info);
                    } 
                    else  
                        printf("No files opened.\n");  
                    break;  
                case 8:  
                    if(!(openfilelist[curdir].type == 0))  
                    {
                        int wstyle;
                        char text[MAXTEXT] = "\0";
                        while(1)  
                        {  
                            printf("Please enter the number of write style:\n1.cut write\t2.cover write\t3.add write\n");  
                            scanf("%d", &wstyle);  
                            if(wstyle > 0 && wstyle < 4)  
                                break;  
                            printf("Input Error!");
                        } 
                        getchar();
                        // printf("please input write data(end with Ctrl+Z):\n");   
                        gets(text);
                        info = my_write(curdir, wstyle, text); 
                        my_error(info);
                    }
                    else  
                        printf("No files opened.\n");  
                    break;  
                case 9:  
                    if(!(openfilelist[curdir].type == 0))  
                    {
                        char text[MAXTEXT];
                        info = my_read(curdir, openfilelist[curdir].length, text);
                        printf("%s", text);         
                        my_error(info);
                    }
                    else  
                        printf("No files opened.\n");  
                    break;  
                case 10:  
                    if(openfilelist[curdir].type == 0)  
                    {  
                        my_exitsys();  
                        flag = 0;  
                    }  
                    else  
                        printf("Please input the right command.\n");  
                    break;  
                case 11:;
                    char *oldName; //
                    char *newName; //
                    oldName = strtok(NULL, " ");             //获取旧文件名
                    newName = oldName;
                    newName = strtok(NULL, " ");             //获取新文件名
                    if(openfilelist[curdir].type == 0 && oldName && newName)
                    {
                        info = my_rename(curdir, oldName, newName);
                        my_error(info);
                    }
                    else
                        printf("Please input the right command.\n");
                    break;
                case 12:;
                    char *filename, *dir;
                    filename = strtok(NULL, " ");
                    dir = filename;
                    dir = strtok(NULL, " ");
                    if (openfilelist[curdir].type == 0 && filename && dir)
                    {
                        info = my_move(filename, dir);
                        my_error(info);
                    }
                    else
                        printf("Please input the right command.\n");
                    break;
                default:  
                    printf("Please input the right command.\n");  
                    break;                      
            }  
        }  
    }  
    return 0;  
}