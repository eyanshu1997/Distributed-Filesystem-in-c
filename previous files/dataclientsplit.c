#include <stdio.h>     // for fprintf()
#include <unistd.h>    // for close(), read()
#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <string.h>    // for strncmp
#include <sys/errno.h>
#include<stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<fcntl.h>
 #include <arpa/inet.h>
 #define MAX 1048576
 int dclient(int port,char *buf)
 {
	char buffer[MAX]={'\0'};
	strcpy(buffer,buf);
	int sock = 0, valread; 
	struct sockaddr_in serv_addr; 
	int count=0;
	printf("Enter Something\n");
	if(buffer[strlen((buffer))-1]  == '\n')
	buffer[strlen((buffer))-1]  = '\0';
	// buffer[strlen((buffer))]  = '\0';

	printf("1buffer %s \n",buffer);
	char name[100]={'\0'};
	int i=0,j=0;
	while(buffer[i]!=' ')
	{
		i++;
	}
	i++;
	if(!buffer[i])
	{
		printf("error\n");
		return -1;
	}
	while(buffer[i])
	{
		name[j++]=buffer[i++];
	}
	// bzero(buffer,MAX);
	// recv(sock,buffer,sizeof(buffer),0);
	printf("1buffer [%s] file name [%s] \n",buffer,name);
	// bzero(buffer,MAX);
	FILE *fp=fopen(name,"rb");
	if(fp==NULL)
	{
		printf("no file found\n");
		return -1;
	}
	else
	{
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
		{ 
			printf("\n Socket creation error \n"); 
			return -1; 
		} 

		serv_addr.sin_family = AF_INET; 
		serv_addr.sin_addr.s_addr = inet_addr("127.0.0.2");
		serv_addr.sin_port = htons(port); 
		
		
	   
		if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
		{ 
			printf("\nConnection Failed \n"); 
			return -1; 
		}
		printf("command sent [%s]\n",buffer);
		write(sock,buffer,strlen(buffer));
		printf("buffer %s \n",buffer);
		bzero(buffer,MAX);
		recv(sock,buffer,sizeof(buffer),0);
		printf("file name %s \n",name);
		fseek(fp,0,SEEK_END);
		int len = ftell(fp);
		fseek(fp,0,SEEK_SET);
		// int nooftimes=len/1000;
		printf("no of times%d\n",len);
		char x[1000];
		sprintf(x,"%d",len);
		send(sock,x,strlen(x),0);
		while(len--)
		{
			bzero(buffer,MAX);
			recv(sock,buffer,sizeof(buffer),0);
			bzero(buffer,sizeof(buffer));
			if(len<100||len%10000==0)
			printf("written %d\n",len);
			unsigned char a='\0';
			j=0;
			a=fgetc(fp);
			send(sock,&a,sizeof(a),0);
									
		}
		fclose(fp);
		bzero(buffer,MAX);
		recv(sock,buf,MAX,0);
		printf("reached here\n"			);
		sleep(5);
		close(sock);
	}    
}
int split(char *name,int times,int PSIZE,int port)
{
	
    FILE *fp = fopen(name,"rb");
	if(fp==NULL)
	{
		printf("fileopen erro [%s]\n",name);
		exit(0);
	}
    fseek(fp,0,SEEK_END);
	int len = ftell(fp);
    fseek(fp,0,SEEK_SET);
    char PackData[1000]={'\0'};
    bzero(PackData,1000);
    int i=0;
	 int si ;
    for(i=0;i<times;i++)
    {   
        bzero(PackData,1000);
        sprintf(PackData,"%d%s",i,name);
        FILE *f2 = fopen(PackData,"wb");
		si= PSIZE;
        while(si--)
            fputc(fgetc(fp),f2);
        fclose(f2);
		char pa[1000]={'\0'};
		strcat(pa,"write ");
		strcat(pa,PackData);
		dclient(port,pa);
		char cmd[1010]={'\0'};
		sprintf(cmd,"rm %s",PackData);
		system(cmd);
    }
    bzero(PackData,1000);
    sprintf(PackData,"%d%s",i,name);
    FILE *f2 = fopen(PackData,"wb");
	si= len%PSIZE;
        while(si--)
            fputc(fgetc(fp),f2);
    fclose(f2);
    fclose(fp);
	char pa[1000]={'\0'};
	strcat(pa,"write ");
	strcat(pa,PackData);
	dclient(port,pa);
	char cmd[1010]={'\0'};
	sprintf(cmd,"rm %s",PackData);
	system(cmd);

}
int main(int argc,char *arg[])
{
	 char name[1000];
	 FILE *fp = fopen(arg[2],"rb");
	 int PSIZE = 1048576;
	 fseek(fp,0,SEEK_END);
    long k =ftell(fp);

    fclose(fp);
    int times = k/PSIZE;
	printf("size %ld\n%d ",k,times);
	split(arg[2],times,PSIZE,atoi(arg[1]));
	// join(name,times,PSIZE);
	
}