#include <stdio.h>     // for fprintf()
#include <unistd.h>    // for close(), read()
#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <string.h>    // for strncmp
#include <sys/errno.h>
#include<stdlib.h>
int join(char *name,int times,int PSIZE,int l)
{
	printf("hello\n");
	char PackData[1000]={'\0'};
	sprintf(PackData,"%s%s","new",name);
	FILE *f1 = fopen(PackData,"wb");
	FILE *f2;
	int i=0;
    for(i=0;i<times;i++)
    {
        bzero(PackData,1000);
        sprintf(PackData,"new%d%s",i,name);
        printf("using new%d%s\n",i,name);
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
	if(l%PSIZE!=0)
	{
        bzero(PackData,1000);
        sprintf(PackData,"new%d%s",i,name);
        printf(	"new%d%s\n",i,name);
        f2 = fopen(PackData,"rb");
		    fseek(f2,0,SEEK_END);
	
		int len = ftell(f2);
		fseek(f2,0,SEEK_SET);
        while(len--)
        {
            fputc(fgetc(f2),f1);
        }
        fclose(f2);
		char cmd[1010]={'\0'};
		sprintf(cmd,"rm %s",PackData);
		system(cmd);
	}
        fclose(f1);


}

int main(int argc,char *argv[])
{
	 char *name = "a.txt";
	 FILE *fp = fopen(argv[1],"rb");
	 int PSIZE = 1048576;
	 fseek(fp,0,SEEK_END);
    long k =ftell(fp);

    fclose(fp);
    int times = k/PSIZE;
	printf("size %ld\n%d ",k,times);
	//split(name,times,PSIZE);
	// char *n="newa.txt";
	 join(name,times,PSIZE,k);
	
}