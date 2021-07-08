#pragma ONCE
#define MAXOPENFILE 10			//���ͬʱ�򿪵��ļ���
#define BLOCKSIZE 1024			//���̿��С
#define SIZE 1024000			//������̿ռ��С
#define END 65535				//FAT���ļ�������־
#define FREE 0					//FAT���̿���б�־
#define MAXTEXT 10000			//д���ļ�����ı���
#define ROOTBLOCKNUM 2			//��Ŀ¼����ռ�̿���

typedef struct FCB
{
    char filename[8];           //�ļ���
    char exname[3];             //�ļ���չ��
    unsigned char type;         //�ļ������ֶΣ�0Ŀ¼ or 1�ļ���attribute
    unsigned short time;        //����ʱ��
    unsigned short date;        //��������
    unsigned short first;       //��ʼ�̿��
    unsigned long length;       //�ļ�����
    char free;                  //��ʾĿ¼���Ƿ�Ϊ��
} fcb;

typedef struct FAT
{
    unsigned short id;          //FAT���ӵ���һ���̿�
} fat;

typedef struct USEROPEN
{
    char filename[8];           //�ļ���
    char exname[3];             //�ļ���չ��
    unsigned char type;         //�ļ������ֶΣ�0Ŀ¼ or 1�ļ���
    unsigned short time;        //����ʱ��
    unsigned short date;        //��������
    unsigned short first;       //��ʼ�̿��
    unsigned long length;      	//�ļ�����
    char free;                  //��ʾĿ¼���Ƿ�Ϊ��

    unsigned short dirno;  		// ��Ӧ���ļ���Ŀ¼���ڸ�Ŀ¼�ļ��е��̿��  
    int diroff;  				// ��Ӧ���ļ���Ŀ¼���ڸ�Ŀ¼�ļ���dirno�̿��е�Ŀ¼�����  
    char dir[80];  				// ��Ӧ���ļ����ڵ�Ŀ¼����  
    int father;  				// ��Ŀ¼�ڴ��ļ������λ�á�  
    int count;  				// ��дָ�����ļ��е�λ��,�ļ������ַ�����  
    char ischanged;  			// �Ƿ��޸����ļ���FCB�����ݣ�����޸�����Ϊ1������Ϊ0��  
    char topenfile;  			// ��ʾ���û��򿪱����Ƿ�Ϊ�գ���ֵΪ0����ʾΪ�գ������ʾ�ѱ�ĳ���ļ�ռ�ݡ�  
}useropen;

//������BLOCK
typedef struct BLOCK0
{
    char magic[10];             // �ļ�ϵͳħ��  
    char information[200];      // �洢һЩ������Ϣ������̿��С�����̿������������ļ�����  
    unsigned short root;        // ��Ŀ¼�ļ�����ʼ�̿��  
    unsigned char* startblock;  // �����������������ʼλ��  
}block0;


unsigned char* myvhard;                 // ָ��������̵���ʼ��ַ  
useropen openfilelist[MAXOPENFILE];     // �û����ļ�������  
int curdir;                             // �û����ļ����еĵ�ǰĿ¼���ڴ��ļ������λ��  
char currentdir[80];                    // ��¼��ǰĿ¼��Ŀ¼��������Ŀ¼��·����
unsigned char* startp;                  // ��¼�����������������ʼλ��  
        // �ļ�ϵͳ���ļ���  


void startsys();                        // �����ļ�ϵͳ  
void my_format();                       // ���̸�ʽ��  
int my_cd(char* dirname);              // ���ĵ�ǰĿ¼  
int my_mkdir(char* dirname);           // ������Ŀ¼  
int my_rmdir(char* dirname);           // ɾ����Ŀ¼  
void my_ls(char *res);                 // ��ʾĿ¼  
int my_create(char* filename);         // �����ļ�  
int my_rm(char* filename);             // ɾ���ļ�  
int my_open(char* filename);            // ���ļ�  
int my_close(int fd);                   // �ر��ļ�  
int my_write(int fd, int wstyle, char* text);                   // д�ļ�  
int do_write(int fd, char* text, int len, char wstyle);  // д����  
int my_read(int fd, int len, char* text);           // ���ļ�  
int do_read(int fd, int len, char* text);  // ������  
void my_exitsys();                      // �˳��ļ�ϵͳ  
unsigned short findblock();             // Ѱ�ҿ����̿�  
int findopenfile();                     // Ѱ�ҿ����ļ����� 
int my_rename(int fd, char* oldName, char* newName);//������
int my_move(char* filename, char* dir);

