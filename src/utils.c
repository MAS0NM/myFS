#include "..\include\define.h"
#include <string.h>

int do_write(int fd, char *text, int len, char wstyle)  
{  
    fat *fat1, *fat2, *fatptr1, *fatptr2;  
    unsigned char *buf, *blkptr;  
    unsigned short blkno, blkoff;  
    int i, ll;  
    fat1 = (fat *)(myvhard + BLOCKSIZE);  
    fat2 = (fat *)(myvhard + 3 * BLOCKSIZE);  
    buf = (unsigned char *)malloc(BLOCKSIZE);  
    if(buf == NULL)  
    {  
        printf("malloc failed!\n");  
        return -1;  
    }  
    blkno = openfilelist[fd].first;  
    blkoff = openfilelist[fd].count;  
    fatptr1 = fat1 + blkno;  
    fatptr2 = fat2 + blkno;  
    while(blkoff >= BLOCKSIZE)  
    {  
        blkno = fatptr1->id;  
        if(blkno == END)  
        {  
            blkno = findblock();  
            if(blkno == -1)  
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
  
    ll = 0;  
    while(ll < len)  
    {  
        blkptr = (unsigned char *)(myvhard + blkno * BLOCKSIZE);  
        for(i = 0; i < BLOCKSIZE; i++)  
            buf[i] = blkptr[i];  
        for(;blkoff < BLOCKSIZE; blkoff++)  
        {  
            buf[blkoff] = text[ll++];  
            openfilelist[fd].count++;  
            if(ll == len)  
                break;  
        }  
        for(i = 0; i < BLOCKSIZE; i++)  
            blkptr[i] = buf[i];  
        if(ll < len)  
        {  
            blkno = fatptr1->id;  
            if(blkno == END)  
            {  
                blkno = findblock();  
                if(blkno == -1)  
                    break;  
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
            blkoff = 0;  
            }  
    }  
    if(openfilelist[fd].count > openfilelist[fd].length)  
        openfilelist[fd].length = openfilelist[fd].count;  
    openfilelist[fd].fcbstate = 1;  
    free(buf);  
    return ll;  
}  

int do_read(int fd, int len, char *text)  
{  
    fat *fat1, *fatptr;  
    unsigned char *buf, *blkptr;  
    unsigned short blkno, blkoff;  
    int i, ll;  
    fat1 = (fat *)(myvhard + BLOCKSIZE);  
    buf = (unsigned char *)malloc(BLOCKSIZE);  
    if(buf == NULL)  
    {  
        printf("malloc failed!\n");  
        return -1;  
    }  
    blkno = openfilelist[fd].first;  
    blkoff = openfilelist[fd].count;  
    if(blkoff >= openfilelist[fd].length)  
    {  
        puts("Read out of range!");  
        free(buf);  
        return -1;  
    }  
    fatptr = fat1 + blkno;  
    while(blkoff >= BLOCKSIZE)  
    {  
        blkno = fatptr->id;  
        blkoff = blkoff - BLOCKSIZE;  
        fatptr = fat1 + blkno;  
    }  
    ll = 0;  
    while(ll < len)  
    {  
        blkptr = (unsigned char *)(myvhard + blkno * BLOCKSIZE);  
        for(i = 0; i < BLOCKSIZE; i++)  
            buf[i] = blkptr[i];  
        for(; blkoff < BLOCKSIZE; blkoff++)  
        {  
            text[ll++] = buf[blkoff];  
            openfilelist[fd].count++;  
            if(ll == len || openfilelist[fd].count == openfilelist[fd].length)  
                break;  
        }  
        if(ll < len && openfilelist[fd].count != openfilelist[fd].length)  
        {  
            blkno = fatptr->id;  
            if(blkno == END)  
                break;  
            blkoff = 0;  
            fatptr = fat1 + blkno;  
        }  
    }  
    text[ll] = '\0';  
    free(buf);  
    return ll;  
}  

unsigned short findblock()  
{  
    unsigned short i;  
    fat *fat1, *fatptr;  
    fat1 = (fat *)(myvhard + BLOCKSIZE);  
    for(i = 7; i < SIZE / BLOCKSIZE; i++)  
    {  
        fatptr = fat1 + i;  
        if(fatptr->id == FREE)  
            return i;  
    }  
    printf("Error,Can't find free block!\n");  
    return -1;  
}  
  
int findopenfile()  
{  
    int i;  
    for(i = 0; i < MAXTEXT; i++)  
    {  
        if(openfilelist[i].topenfile == 0)  
            return i;  
    }  
    printf("Error,open too many files!\n");  
    return -1;  
} 