#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<fcntl.h>
 #include <arpa/inet.h>
 #define MAX 100000
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
		int nooftimes=len/1000;
		printf("no of times%d\n",nooftimes);
		char x[1000];
		sprintf(x,"%d",nooftimes);
		send(sock,x,strlen(x),0);
		for(i=0;i<nooftimes;i++)
		{
			
			
			bzero(buffer,MAX);
			recv(sock,buffer,sizeof(buffer),0);
			bzero(buffer,MAX);
			j=0;
			while(j<1000)
			{
				buffer[j++]=fgetc(fp);
				printf("%d : [%c]\n",j-1,buffer[j-1]);
			}
			buffer[j]='\0';
			printf("file : [%s]\n",buffer);
			send(sock,buffer,j+1,0);
			bzero(buffer,MAX);
		}
		fclose(fp);
		recv(sock,buf,MAX,0);
	}    
}
int main(int argc,char *argv[])
{
	return dclient(atoi(argv[1]),"write 1test.mp3\0");
	
}
