// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<fcntl.h>
#include <sys/wait.h>
 #include <arpa/inet.h>
 #define MAX 10000
int max(int a,int b){
    return (a>b)?a:b;
} 
int dsclient(char *ip,int port,char *buf)
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
		// printf("file name %s \n",name);
		fseek(fp,0,SEEK_END);
		int len = ftell(fp);
		fseek(fp,0,SEEK_SET);
		// int nooftimes=len/1000;
		printf("no of times%d\n",len);
		char x[1000];
		sprintf(x,"%d",len);
		send(sock,x,strlen(x),0);
		// printf("sedinfig");
		while(len--)
		{
			bzero(buffer,MAX);
			recv(sock,buffer,4,0);
			bzero(buffer,sizeof(buffer));
			// if(len%10000==0)
			// printf(".");
			fflush(stdout);
			unsigned char a='\0';
			j=0;
			a=fgetc(fp);
			send(sock,&a,sizeof(a),0);
									
		}
		printf("\n");
		fclose(fp);
		char cmd[1010]={'\0'};
		sprintf(cmd,"rm %s",name);
		system(cmd);
		bzero(buffer,MAX);
		recv(sock,buf,MAX,0);
		printf("reached here\n");
		sleep(1);
		close(sock);
		exit(0);
	}    
}
int split(char *name,int times,int PSIZE,int *ino)
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
        sprintf(PackData,"inode%d",ino[i]);
        printf("split inode%d\n",ino[i]);
        FILE *f2 = fopen(PackData,"wb");
        si = PSIZE;
        while(si--)
            fputc(fgetc(fp),f2);
        fclose(f2);
    }
	if(len%PSIZE!=0)
	{
		bzero(PackData,1000);
		sprintf(PackData,"inode%d",ino[i]);
		printf("split inode%d\n",ino[i]);
		FILE *f2 = fopen(PackData,"wb");
		si= len%PSIZE;
			while(si--)
				fputc(fgetc(fp),f2);
		fclose(f2);
	}
    fclose(fp);

}
int upload(long k,char *nam,int *port,char ip[1000][100],int PSIZE,int *ino)
{
    int times = k/PSIZE;
	printf("size %ld\ntimes:%d\n ",k,times);
	split(nam,times,PSIZE,ino);
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
			char cmd[1000]={'\0'};
			sprintf(cmd,"write inode%d",ino[i]);
			dsclient(ip[i],port[i],cmd);
			printf("child %d closed\n",i);
			exit(0);
		}
		
	}
	for(int i=0;i<times;i++)
	wait(NULL);
	// exit(0);
	return 0;
}
int convertToInt(char*);
int main(int argc, char const *argv[]) 
{ 
    int portNo = 0;
	int port[1000]={'\0'};
	char ip[1000][100];
	int ino[1000];
    char buffer[MAX] = {'\0'};
    // printf("please Enter port No of Server\n");
    // fgets(buffer,sizeof(buffer),stdin);
    // fflush(stdin);
	// buffer[strlen((buffer))-1]  = '\0';
    // portNo = convertToInt(buffer);
	portNo=5001;
	int sock = 0, valread; 
	struct sockaddr_in serv_addr; 
    int maxfd1;
    fd_set rset;
    FD_ZERO(&rset);
	 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.2");
	serv_addr.sin_port = htons(atoi(argv[1])); 
	
	
   
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	}
    maxfd1 = max(0,sock)+1;
    int count=0,i;
    while (1)
    { 
        FD_SET(0,&rset);
        FD_SET(sock,&rset);
        printf(">");
		fflush(stdout);
        select(maxfd1,&rset,NULL,NULL,NULL);
        if(FD_ISSET(0,&rset)){
            bzero(buffer,MAX);
            fgets(buffer,sizeof(buffer),stdin);
            fflush(stdin);
		    buffer[strlen((buffer))]  = '\0';
			if(strncmp(buffer,"upload",6)==0)
			{
				char name[100];
				sscanf(buffer,"upload %s",name);
				printf("name is%s\n",name);
				char *x=strchr(buffer,'\n');
				*x='\0';
				FILE *fp = fopen(name,"rb");
				int PSIZE = 1048576;
				fseek(fp,0,SEEK_END);
				long k =ftell(fp);
				fseek(fp,0,SEEK_SET);
				fclose(fp);
				char str[1000];
				sprintf(str," %ld\n",k);
				strcat(buffer,str);
				printf("buffer is:[%s]\n",buffer);	
			}
            write(sock,buffer,strlen(buffer)-1);
            
        }
        if(FD_ISSET(sock,&rset))
		{
			if(strncmp(buffer,"upload",6)==0)
			{
				printf("buffer%s\n",buffer);
				char name[100];
				sscanf(buffer,"upload %s",name);
				bzero(buffer,MAX);
				valread = read(sock,buffer,sizeof(buffer));
				char xno[100]={'\0'};
				int i=0,j=0;
				printf("buffer %s\n",buffer);
				while(buffer[i]!='|')
				{
					xno[j++]=buffer[i++];
				}
				i++;
				printf("x no is %s \n",xno);
				int n=atoi(xno);
				for(int k=0;k<n;k++)
				{
					j=0;
					if(buffer[i]=='|')
						i++;
					char p[100]={'\0'};
					while(buffer[i]!=' ')
					{
						// printf("i :%d\n",i);
						p[j++]=buffer[i++];
					}
					i++;
					printf("ino [%s]\n",p);
					ino[k]=atoi(p);
					bzero(p,100);
					j=0;
					while(buffer[i]!=' ')
						p[j++]=buffer[i++];
					i++;
					bzero(ip[k],100);

					strcpy(ip[k],p);
										printf("ip from p is %s \n ",p);
										printf("ip is %s \n ",ip[k]);
					j=0;
					bzero(p,100);
					while(buffer[i]!='|')
						p[j++]=buffer[i++];
					port[k]=atoi(p);

					printf("port no with p is [%s] \n",p);
					printf("port no is [%d] \n",port[k]);
					bzero(p,100);
					j=0;
					
					// i++;
					printf("ino [%d] port no [%d] ip [%s]\n",ino[k],port[k],ip[k]);
				}
				FILE *fp = fopen(name,"rb");
				int PSIZE = 1048576;
				fseek(fp,0,SEEK_END);
				long k =ftell(fp);
				fseek(fp,0,SEEK_SET);
				fclose(fp);
				// int PSIZE = 1048576;
				// for(int l=0;l<n;l++)
				{
					printf("--------------\nname is %s",name);
					
				}
					upload(k,name,port,ip,PSIZE,ino);
			}
			else
			{
				
				bzero(buffer,MAX);
				valread = read(sock,buffer,sizeof(buffer));
				if(valread==0){
					printf("connection closed by peer\n");
					FD_CLR(sock,&rset);
					close(sock);
					break;
				}
				buffer[valread] = '\0'; 
				printf("4buffer :%s",buffer);
			}
        }
       
        
        
		

    }
    

	return 0; 
} 
// Start of utility function to convert character to integer

int convertToInt(char *in){
int i = strlen(in)-1,multiplier=1,result = 0;

while(i>=0){
result+=(in[i]-48)*multiplier;
multiplier*=10;
i--;
}
return result;
}

//End of utility function to convert character to integer
