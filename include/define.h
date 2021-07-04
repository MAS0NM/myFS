#pragma ONCE
#ifndef __TOOLS_H__
#define __TOOLS_H__

#define MAXOPENFILE 10			//最多同时打开的文件数
#define BLOCKSIZE 1024			//磁盘块大小
#define SIZE 1024000			//虚拟磁盘空间大小
#define END 65535				//FAT的文件结束标志
#define FREE 0					//FAT中盘块空闲标志
#define MAXTEXT 10000			
#define ROOTBLOCKNUM 2			//根目录区所占盘块数

typedef struct FCB              //8 + 3 + 1 + 2 + 2 + 2 + 8 + 1 = 27
{
    char filename[8];           //文件名
    char exname[3];             //文件扩展名
    unsigned char attribute;    //文件属性字段（目录or文件）
    unsigned short time;        //创建时间
    unsigned short date;        //创建日期
    unsigned short first;       //起始盘块号
    unsigned long length;       //文件长度
    char free;                  //表示目录项是否为空
} fcb;

typedef struct FAT
{
    unsigned short id;          //磁盘块的状态（空闲的，最后的，下一个）
} fat;

typedef struct USEROPEN
{
    char filename[8];           //文件名
    char exname[3];             //文件扩展名
    unsigned char attribute;    //文件属性字段（目录or文件）
    unsigned short time;        //创建时间
    unsigned short date;        //创建日期
    unsigned short first;       //起始盘块号
    unsigned long length;      	//文件长度
    char free;                  //表示目录项是否为空

    unsigned short dirno;  		// 相应打开文件的目录项在父目录文件中的盘块号  
    int diroff;  				// 相应打开文件的目录项在父目录文件的dirno盘块中的目录项序号  
    char dir[80];  				// 相应打开文件所在的目录名，这样方便快速检查出指定文件是否已经打开  
    int father;  				// 父目录在打开文件表项的位置  
    int count;  				// 读写指针在文件中的位置,文件的总字符数  
    char fcbstate;  			// 是否修改了文件的FCB的内容，如果修改了置为1，否则为0  
    char topenfile;  			// 表示该用户打开表项是否为空，若值为0，表示为空，否则表示已被某打开文件占据  
}useropen; 

//引导块BLOCK
typedef struct BLOCK0  
{  
    char magic[10];             // 文件系统魔数  
    char information[200];      // 存储一些描述信息，如磁盘块大小、磁盘块数量、最多打开文件数等  
    unsigned short root;        // 根目录文件的起始盘块号  
    unsigned char *startblock;  // 虚拟磁盘上数据区开始位置  
}block0;  


unsigned char *myvhard;                 // 指向虚拟磁盘的起始地址  
useropen openfilelist[MAXOPENFILE];     // 用户打开文件表数组  
int curdir;                             // 用户打开文件表中的当前目录所在打开文件表项的位置  
char currentdir[80];                    // 记录当前目录的目录名（包括目录的路径）  
unsigned char* startp;                  // 记录虚拟磁盘上数据区开始位置  
char myfilename[] = "myfilesys";        //文件系统的文件名  


void startsys();                        // 进入文件系统  
void my_format();                       // 磁盘格式化  
void my_cd(char *dirname);              // 更改当前目录  
void my_mkdir(char *dirname);           // 创建子目录  
void my_rmdir(char *dirname);           // 删除子目录  
void my_ls();                           // 显示目录  
void my_create (char *filename);        // 创建文件  
void my_rm(char *filename);             // 删除文件  
int my_open(char *filename);            // 打开文件  
int my_close(int fd);                   // 关闭文件  
int my_write(int fd);                   // 写文件  
int do_write(int fd, char *text, int len, char wstyle);  // 实际写文件  
int my_read (int fd, int len);          // 读文件  
int do_read (int fd, int len,char *text);  // 实际读文件  
void my_exitsys();                      // 退出文件系统  
unsigned short findblock();             // 寻找空闲盘块  
int findopenfile();                     // 寻找空闲文件表项 


#endif