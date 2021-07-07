#include ".\func.c"
#include "..\include\define.h"


int main()  
{  
    char cmd[15][10] = {"cd", "mkdir", "rmdir", "ls", "create", "rm", "open", "close", "write", "read", "exit", "rename"};  
    char s[30], *sp;  
    int cmdn, flag = 1, i;  
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
                    if(sp && (openfilelist[curdir].attribute & 0x20))   //和b'10000按位与
                        my_cd(sp);  
                    else  
                        printf("Please input the right command.\n");  
                    break;  
                case 1:  
                    sp = strtok(NULL, " ");  
                    if(sp && (openfilelist[curdir].attribute & 0x20))  
                        my_mkdir(sp);  
                    else  
                        printf("Please input the right command.\n");  
                    break;  
                case 2:  
                    sp = strtok(NULL, " ");  
                    if(sp && (openfilelist[curdir].attribute & 0x20))  
                        my_rmdir(sp);  
                    else  
                        printf("Please input the right command.\n");  
                    break;  
                case 3:  
                    if(openfilelist[curdir].attribute & 0x20)
                        my_ls();  
                    else  
                        printf("Please input the right command.\n");  
                    break;  
                case 4:  
                    sp = strtok(NULL, " ");  
                    if(sp && (openfilelist[curdir].attribute & 0x20))  
                        my_create(sp);  
                    else  
                        printf("Please input the right command.\n");  
                    break;  
                case 5:  
                    sp = strtok(NULL, " ");  
                    if(sp && (openfilelist[curdir].attribute & 0x20))  
                        my_rm(sp);  
                    else  
                        printf("Please input the right command.\n");  
                    break;  
                case 6:  
                    sp = strtok(NULL, " ");  
                    if(sp && (openfilelist[curdir].attribute & 0x20))  
                    {  
                        if(strchr(sp, '.'))                             //查找sp中'.'首次出现的位置  
                            curdir = my_open(sp);  
                        else  
                            printf("the openfile should have exname.\n");  
                    }  
                    else  
                        printf("Please input the right command.\n");  
                    break;  
                case 7:  
                    if(!(openfilelist[curdir].attribute & 0x20))  
                        curdir = my_close(curdir);  
                    else  
                        printf("No files opened.\n");  
                    break;  
                case 8:  
                    if(!(openfilelist[curdir].attribute & 0x20))  
                        my_write(curdir);  
                    else  
                        printf("No files opened.\n");  
                    break;  
                case 9:  
                    if(!(openfilelist[curdir].attribute & 0x20))  
                        my_read(curdir, openfilelist[curdir].length);  
                    else  
                        printf("No files opened.\n");  
                    break;  
                case 10:  
                    if(openfilelist[curdir].attribute & 0x20)  
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
                    if(openfilelist[curdir].attribute & 0x20)
                        my_rename(curdir, oldName, newName);
                    else
                        printf("Files not found");
                    break;
                default:  
                    printf("Please input the right command.\n");  
                    break;  
            }  
        }  
    }  
    return 0;  
}