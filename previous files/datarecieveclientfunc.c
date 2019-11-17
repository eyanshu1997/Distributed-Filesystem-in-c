#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<fcntl.h>
 #include <arpa/inet.h>
 #include <sys/errno.h>
 #define MAX 100000
 int join(char *name,int times,int PSIZE,int l,int *ino)
{
	printf("hello\n");
	char PackData[1000]={'\0'};
	sprintf(PackData,"%s%s","saved",name);
	FILE *f1 = fopen(PackData,"wb");
	FILE *f2;
	int i=0;
    for(i=0;i<times;i++)
    {
        bzero(PackData,1000);
        sprintf(PackData,"inode%d",ino[i]);
        printf("using inode%d\n",ino[i]);
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
        sprintf(PackData,"inode%d",ino[i]);
        printf(	"inode%d\n",ino[i]);
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
 int drclient(char *ip,int port,char *buf)
 {
	char buffer[MAX]={'\0'};
	strcpy(buffer,buf);
	int sock = 0, valread; 
	struct sockaddr_in serv_addr; 
	int count=0;
	// printf("Enter Something\n");
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
	// printf("1buffer [%s] file name [%s] \n",buffer,name);
	// bzero(buffer,MAX);
	FILE *fp=fopen(name,"wb");
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
		serv_addr.sin_addr.s_addr = inet_addr(ip);
		serv_addr.sin_port = htons(port); 
		
		
	   
		if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
		{ 
			printf("\nConnection Failed \n"); 
			return -1; 
		}
		printf("command sent [%s]\n",buffer);
		write(sock,buffer,strlen(buffer));
		// printf("buffer %s \n",buffer);
		bzero(buffer,MAX);
		recv(sock,buffer,sizeof(buffer),0);
		int len=atoi(buffer);
		// printf("file name %s \n",name);
		send(sock,"done",4,0);
		// printf("sedinfig");
		int n=0;
		int no=0;	
		while(len--)
		{
			unsigned char a='\0';
			n=recv(sock,&a,sizeof(a),0);
			fputc(a,fp);
			no=n+no;
										
			send(sock,"done",4,0);
			bzero(buffer,MAX);				
		}
		printf("\n");
		fclose(fp);

		bzero(buffer,MAX);
		recv(sock,buffer,MAX,0);
		printf("reached here\n");
		sleep(1);
		close(sock);
		exit(0);
	}    
}
int download(long k,char *nam,int *port,char **ip,int PSIZE,int *ino)
{


    int times = k/PSIZE;
	printf("size %ld\ntimes:%d\n ",k,times);
	// split(argv[2],times,PSIZE);
	printf("size %ld\nntimes:%d \n",k,times);
	
	if(k%PSIZE!=0)
	{
		times=times+1;
	}
	for(int i=0;i<times;i++)
	{
		int ret=fork();
		if(ret==0)
		{
			char cmd[1000];
			sprintf(cmd,"read inode%d",ino[i]);
			drclient(ip[i],port[i],cmd);
			printf("child %d closed\n",i);
			exit(0);
		}
		
	}
	for(int i=0;i<times;i++)
	wait(NULL);
	// exit(0);
	printf("done recieveing merging\n");
	// join(nam,k/PSIZE,PSIZE,k,ino);
	return 0;
}
int main(int argc,char *argv[])
{
	char nam[100];
	strcpy(nam,argv[1]);
	printf("name %s\n",nam);
	FILE *fp = fopen(nam,"rb");
	int PSIZE = 1048576;
	fseek(fp,0,SEEK_END);
    long k =ftell(fp);
    fseek(fp,0,SEEK_SET);
	fclose(fp);
	printf("here");
	char *ip[1000];
		int ino[1000];
	ino[0]=0;
	ino[1]=1;
	ino[2]=2;
	ino[3]=3;
	ino[4]=4;
	ino[5]=5;
	
	ip[0]="127.0.0.1";
	ip[1]="127.0.0.1";
	ip[2]="127.0.0.1";
	ip[3]="127.0.0.1";
	ip[4]="127.0.0.1";
	printf("hello\n");
	int port[1000];
	port[0]=8080;
	port[1]=8081;
	port[2]=8082;
	port[3]=8080;
	port[4]=8081;
	port[5]=8082;
	// char *name[]={"node1","node2","node3","node4","node5"};
	printf("reached here\n");
	// upload(k,nam,name,port,ip,PSIZE);
	// upload(k,nam,port,ip,PSIZE);
	download(k,nam,port,ip,PSIZE,ino);
	// printf("k : %ld, nam %s, psize %d\n", k,nam,PSIZE);
	
	return 0;
}