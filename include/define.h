#define MAXOPENFILE 10
#define BLOCKSIZE 1024
#define END 10
#define FREE 10

typedef struct FCB
{
    char filename[8];           //文件名
    char exname[3];             //文件扩展名
    unsigned char attribute;    //文件属性字段（目录or文件）
    unsigned short time;        //创建时间
    unsigned short date;        //创建日期
    unsigned short first;       //起始盘块号
    unsigned short length;      //文件长度
    char free;                  //表示目录项是否为空
} fcb;

typedef struct FAT
{
    unsigned short id;          //
} fat;

typedef struct USEROPEN
{
    char filename[8];           //
    char exname[3];             //
    unsigned char attribute;    //
    unsigned short time;        //
    unsigned short date;        //
    unsigned short first;       //
    unsigned short length;      //
    char free;

    int dirno;                  // 父目录文件起始盘块号
    int diroff;                 // 该文件对应的 fcb 在父目录中的逻辑序号
    char dir[MAXOPENFILE][80];  // 全路径信息
    int count;                  //
    char fcbstate;              // 是否修改 1是 0否
    char topenfile;             // 0: 空 openfile
} useropen;

//引导块BLOCK
typedef struct BLOCK {
    char magic_number[8];       //
    char information[200];      //
    unsigned short root;        //
    unsigned char* startblock;  //在整个磁盘中的首地址
} block0;
