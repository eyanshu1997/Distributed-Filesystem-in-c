#include <stdio.h>     // for fprintf()
#include <unistd.h>    // for close(), read()
#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <string.h>    // for strncmp
#include <sys/errno.h>
#include<stdlib.h>
int split(char *name,int times,int PSIZE)
{
	
    FILE *fp = fopen(name,"rb");
    fseek(fp,0,SEEK_END);
	int len = ftell(fp);
    fseek(fp,0,SEEK_SET);
    char PackData[1000]={'\0'};
    bzero(PackData,1000);
    int i=0;
        int si = PSIZE;
    for(i=0;i<times;i++)
    {   
        bzero(PackData,1000);
        sprintf(PackData,"%d%s",i,name);
        FILE *f2 = fopen(PackData,"wb");
        si = PSIZE;
        while(si--)
            fputc(fgetc(fp),f2);
        fclose(f2);
    }
    bzero(PackData,1000);
    sprintf(PackData,"%d%s",i,name);
    FILE *f2 = fopen(PackData,"wb");
	si= len%PSIZE;
        while(si--)
            fputc(fgetc(fp),f2);
    fclose(f2);
    fclose(fp);

}
int main(int argc,char *argv[])
{
	 char *name = "test.mp3";
	 FILE *fp = fopen(argv[1],"rb");
	 int PSIZE = 1048576;
	 fseek(fp,0,SEEK_END);
    long k =ftell(fp);

    fclose(fp);
    int times = k/PSIZE;
	printf("size %ld\n%d ",k,times);
	split(name,times,PSIZE);
	// char *n="newa.txt";
	// join(name,times,PSIZE);
	
}