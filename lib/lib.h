#pragma ONCE
#define MAXOPENFILE 10			
#define BLOCKSIZE 1024			
#define SIZE 1024000		
#define END 65535				
#define FREE 0				
#define MAXTEXT 10000			
#define ROOTBLOCKNUM 2		

typedef struct FCB
{
    char filename[8];       // 文件名  
    char exname[3];         // 文件扩展名  
    unsigned char type;     // 文件属性字段，值为0时表示目录文件，值为1时表示数据文件  
    unsigned short time;    // 文件创建时间  
    unsigned short date;    // 文件创建日期  
    unsigned short first;   // 文件起始盘块号  
    unsigned long length;   // 文件长度  
    char free;              // 表示目录项是否为空，若值为0，表示空，值为1，表示已分配  
} fcb;

typedef struct FAT
{
    unsigned short id;      // 磁盘块的状态（空闲的，最后的，下一个）
} fat;

typedef struct USEROPEN  
{  
    char filename[8];       // 文件名  
    char exname[3];         // 文件扩展名  
    unsigned char type;     // 文件属性字段，值为0时表示目录文件，值为1时表示数据文件  
    unsigned short time;    // 文件创建时间  
    unsigned short date;    // 文件创建日期  
    unsigned short first;   // 文件起始盘块号  
    unsigned long length;   // 文件长度（对数据文件是字节数，对目录文件可以是目录项个数）  
    char free;              // 表示目录项是否为空，若值为0，表示空，值为1，表示已分配  
  
    unsigned short dirno;   // 相应打开文件的目录项在父目录文件中的盘块号  
    int diroff;             // 相应打开文件的目录项在父目录文件的dirno盘块中的目录项序号  
    char dir[80];           // 相应打开文件所在的目录名，这样方便快速检查出指定文件是否已经打开  
    int father;             // 父目录在打开文件表项的位置  
    int count;              // 读写指针在文件中的位置,文件的总字符数  
    char ischanged;         // 是否修改了文件的FCB的内容，如果修改了置为1，否则为0  
    char topenfile;         // 表示该用户打开表项是否为空，若值为0，表示为空，否则表示已被某打开文件占据  
}useropen; 


typedef struct BLOCK0  
{  
    char magic[10];         // 文件系统魔数  
    char information[200];  // 存储一些描述信息，如磁盘块大小、磁盘块数量、最多打开文件数等  
    unsigned short root;    // 根目录文件的起始盘块号  
    unsigned char *startblock;  // 虚拟磁盘上数据区开始位置  
}block0;  


unsigned char* myvhard;                 // ָ指向虚拟磁盘的起始地址   
useropen openfilelist[MAXOPENFILE];     // 用户打开文件表数组 
int curdir;                             // 用户打开文件表中的当前目录所在打开文件夹表项的位置
char currentdir[80];                    // 记录当前目录的目录名（包括目录的路径）
unsigned char* startp;                  // 虚拟磁盘上数据区开始位置 


void startsys();                     
void my_format();                    
int my_cd(char* dirname);            
int my_mkdir(char* dirname);         
int my_rmdir(char* dirname);         
void my_ls(char *res);               
int my_create(char* filename);       
int my_rm(char* filename);           
int my_open(char* filename);         
int my_close(int fd);                
int my_write(int fd, int wstyle, char* text);            
int do_write(int fd, char* text, int len, char wstyle);  
int my_read(int fd, int len, char* text);           
int do_read(int fd, int len, char* text);  
void my_exitsys();                      
unsigned short findblock();             
int findopenfile();                     
int my_rename(int fd, char* oldName, char* newName);
int my_move(char* filename, char* dir);

