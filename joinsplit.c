#include <stdio.h>     // for fprintf()
#include <unistd.h>    // for close(), read()
#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <string.h>    // for strncmp
#include <sys/errno.h>
#include<stdlib.h>
int join(char *name,int times,int PSIZE)
{
	char PackData[1000]={'\0'};
	sprintf(PackData,"%s%s","new",name);
	FILE *f1 = fopen(PackData,"wb");
	FILE *f2;
	int i;
    for(i=0;i<times;i++)
    {
        bzero(PackData,1000);
        sprintf(PackData,"%d%s",i,name);
        f2 = fopen(PackData,"rb");
        int k=PSIZE,l=0;
        while(k--)
        {
            l++;
            fputc(fgetc(f2),f1);
        }

        fclose(f2);
		char cmd[1010]={'\0'};
		sprintf(cmd,"rm %s",PackData);
		system(cmd);
    }

        bzero(PackData,1000);
        sprintf(PackData,"%d%s",i,name);
        f2 = fopen(PackData,"rb");
        while(!feof(f2))
        {
            fputc(fgetc(f2),f1);
        }
        fclose(f2);
		char cmd[1010]={'\0'};
		sprintf(cmd,"rm %s",PackData);
		system(cmd);
        fclose(f1);


}
int split(char *name,int times,int PSIZE)
{
	
    FILE *fp = fopen(name,"rb");
    fseek(fp,0,SEEK_END);
    fseek(fp,0,SEEK_SET);
    char PackData[1000];
    bzero(PackData,1000);
    int i=0;
    for(i=0;i<times;i++)
    {   
        bzero(PackData,1000);
        sprintf(PackData,"%d%s",i,name);
        FILE *f2 = fopen(PackData,"wb");
        int si = PSIZE;
        while(si--)
            fputc(fgetc(fp),f2);
        fclose(f2);
    }
    bzero(PackData,1000);
    sprintf(PackData,"%d%s",i,name);
    FILE *f2 = fopen(PackData,"wb");
        while(!feof(fp))
            fputc(fgetc(fp),f2);
    fclose(f2);
    fclose(fp);

}
int main()
{
	 char *name = "test.mp3";
	 FILE *fp = fopen(name,"rb");
	 int PSIZE = 10000;
	 fseek(fp,0,SEEK_END);
    long k =ftell(fp);

    fclose(fp);
    int times = k/PSIZE;
	printf("size %ld\n%d ",k,times);
	split(name,times,PSIZE);
	join(name,times,PSIZE);
	
}