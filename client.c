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
 #include <dirent.h> 
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

	// printf("1buffer %s \n",buffer);
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
		// printf("command sent [%s]\n",buffer);
		write(sock,buffer,strlen(buffer));
		// printf("buffer %s \n",buffer);
		bzero(buffer,MAX);
		recv(sock,buffer,sizeof(buffer),0);
		// printf("file name %s \n",name);
		fseek(fp,0,SEEK_END);
		int len = ftell(fp);
		fseek(fp,0,SEEK_SET);
		// int nooftimes=len/1000;
		// printf("no of times%d\n",len);
		char x[1000];
		sprintf(x,"%d",len);
		send(sock,x,strlen(x),0);
		// printf("sedinfig");
		int loop=len;
		while(len>=10000)
		{
			bzero(buffer,MAX);
			recv(sock,buffer,4,0);
			bzero(buffer,sizeof(buffer));
			// if(len%10000==0)
			// printf(".");
			// fflush(stdout);
			unsigned char a[10000]={'\0'};
			j=0;
			// a=fgetc(fp);
			fread(a,10000,1,fp);
			send(sock,a,sizeof(a),0);
			len=len-10000;				
		}
		if(loop%10000!=0)
		{
			bzero(buffer,MAX);
			recv(sock,buffer,4,0);
			bzero(buffer,sizeof(buffer));
			// if(len%10000==0)
			// printf(".");
			// fflush(stdout);
			unsigned char a[10000]={'\0'};
			j=0;
			// a=fgetc(fp);
			fread(a,loop%10000,1,fp);
			send(sock,a,loop%10000,0);
		}
		
		// printf("\n");
		fclose(fp);
		char cmd[1010]={'\0'};
		sprintf(cmd,"rm %s",name);
		system(cmd);
		bzero(buffer,MAX);
		recv(sock,buf,MAX,0);
		printf("segment uploaded \n");
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
        // printf("split inode%d\n",ino[i]);
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
		// printf("split inode%d\n",ino[i]);
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
	// printf("size %ld\ntimes:%d\n ",k,times);
	split(nam,times,PSIZE,ino);
	// printf("size %ld\nntimes:%d \n",k,times);
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
			// printf("child %d closed\n",i);
			exit(0);
		}
		
	}
	for(int i=0;i<times;i++)
	wait(NULL);
	// exit(0);
	return 0;
}
 int join(char *name,int times,int PSIZE,int l,int *ino)
{
	printf("Download complete\n");
	char PackData[1000]={'\0'};
	sprintf(PackData,"%s%s","saved",name);
	FILE *f1 = fopen(PackData,"wb");
	FILE *f2;
	int i=0;
    for(i=0;i<times;i++)
    {
        bzero(PackData,1000);
        sprintf(PackData,"inode%d",ino[i]);
        // printf("using inode%d\n",ino[i]);
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
        // printf(	"inode%d\n",ino[i]);
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

	// printf("1buffer %s \n",buffer);
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
		// printf("command sent [%s]\n",buffer);
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
		int loop=len;
		while(len>=10000)
		{
			unsigned char a[10000]={'\0'};
			n=recv(sock,a,sizeof(a),0);
			// fputc(a,fp);
			fwrite(a,10000,1,fp);
			no=n+no;
			printf("n is %d\n",n);			
			send(sock,"done",4,0);
			bzero(buffer,MAX);				
			len=len-10000;
		}
		if(loop%10000!=0)
		{
			unsigned char a[10000]={'\0'};
			n=recv(sock,a,loop%10000,0);
			// fputc(a,fp);
			fwrite(a,loop%10000,1,fp);
			no=n+no;
			printf("n is %d\n",n);				
			send(sock,"done",4,0);
			bzero(buffer,MAX);
		}
		printf("\n");
		fclose(fp);

		bzero(buffer,MAX);
		recv(sock,buffer,MAX,0);
		printf("segment downloaded\n");
		sleep(1);
		close(sock);
		exit(0);
	}    
}
int download(long k,char *nam,int *port,char ip[1000][100],int PSIZE,int *ino)
{


    int times = k/PSIZE;
	// printf("size %ld\ntimes:%d\n ",k,times);
	// split(argv[2],times,PSIZE);
	// printf("size %ld\nntimes:%d \n",k,times);
	
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
			// printf("child %d closed\n",i);
			exit(0);
		}
		
	}
	for(int i=0;i<times;i++)
	wait(NULL);
	// exit(0);
	// printf("done recieveing merging\n");
	join(nam,k/PSIZE,PSIZE,k,ino);
	return 0;
}

int convertToInt(char*);
int main(int argc, char const *argv[]) 
{ 
if(argc!=3)
{
	printf("enter the name server ip and port no as argument"); 
	exit(0);
}
	int set=0;
    int portNo = 0;
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
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2])); 
	
	
   
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
        if(FD_ISSET(0,&rset))
		{
            bzero(buffer,MAX);
            fgets(buffer,sizeof(buffer),stdin);
            fflush(stdin);
		    buffer[strlen((buffer))]  = '\0';
			if(strncmp(buffer,"upload",6)==0)
			{
				char name[100]={'\0'};
				sscanf(buffer,"upload %s",name);
				// printf("name is%s\n",name);
				char *x=strchr(buffer,'\n');
				*x='\0';
				struct dirent *de;  
  
				DIR *dr = opendir(name); 
  
				if(dr!=NULL)
				{
					bzero(buffer,MAX);
					sprintf(buffer,"mkdir %s\n",name);
					// printf("buffer %s\n",buffer);
					write(sock,buffer,strlen(buffer)-1);
					bzero(buffer,MAX);
					read(sock,buffer,sizeof(buffer));
					bzero(buffer,MAX);
					sprintf(buffer,"cd %s\n",name);
					// printf("buffer %s\n",buffer);
					write(sock,buffer,strlen(buffer)-1);
					bzero(buffer,MAX);
					read(sock,buffer,sizeof(buffer));
					while ((de = readdir(dr)) != NULL)
					{		
						if(strcmp(de->d_name,".")!=0&&strcmp(de->d_name,"..")!=0&&de->d_type==DT_REG)
						{
							printf("uploading file %s\n", de->d_name);
							chdir(name);
							// char x[10000]={'\0'};
							// sprintf(x,"%s/%s",name,de->d_name);
							// printf("file [%s]\n",x);
							FILE *fp = fopen(de->d_name,"rb");
							if(fp==NULL)
								printf("error in file opening \n ");
							int PSIZE = 1048576;
							fseek(fp,0,SEEK_END);
							long k =ftell(fp);
							fseek(fp,0,SEEK_SET);
							fclose(fp);
							char str[1000];
							sprintf(str," %ld\n",k);
							bzero(buffer,MAX);
							sprintf(buffer,"upload %s %ld\n",de->d_name,k);
							printf("buffer %s\n",buffer);
							write(sock,buffer,strlen(buffer)-1);
							int port[1000]={'\0'};
							char ip[1000][100];
							int ino[1000];
							// printf("buffer%s\n",buffer);
							char name[100];
							sscanf(buffer,"upload %s",de->d_name);
							bzero(buffer,MAX);
							valread = read(sock,buffer,sizeof(buffer));
							char xno[100]={'\0'};
							int i=0,j=0;
							// printf("buffer %s\n",buffer);
						
							while(buffer[i]!=' ')
							{
								xno[j++]=buffer[i++];
							}
							i++;
							printf("please wait while uploading \nno of segment is %s \n",xno);
							int n=atoi(xno);
							bzero(xno,100);
							j=0;
							while(buffer[i]!='|')
							{
								xno[j++]=buffer[i++];
							}
							i++;
							// printf("size is %d\n",size);
							int size=atoi(xno);
							printf("size is %d\n",size);
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
								// printf("ino [%s]\n",p);
								ino[k]=atoi(p);
								bzero(p,100);
								j=0;
								while(buffer[i]!=' ')
									p[j++]=buffer[i++];
								i++;
								bzero(ip[k],100);

								strcpy(ip[k],p);
													// printf("ip from p is %s \n ",p);
													// printf("ip is %s \n ",ip[k]);
								j=0;
								bzero(p,100);
								while(buffer[i]!='|')
									p[j++]=buffer[i++];
								port[k]=atoi(p);

								// printf("port no with p is [%s] \n",p);
								// printf("port no is [%d] \n",port[k]);
								bzero(p,100);
								j=0;
								
								// i++;
								// printf("ino [%d] port no [%d] ip [%s]\n",ino[k],port[k],ip[k]);
							}
							// int PSIZE = 1048576;
							upload(size,de->d_name,port,ip,PSIZE,ino);
						}
					}
					closedir(dr);     
					chdir("..");
					bzero(buffer,MAX);
					sprintf(buffer,"cd ..\n");
					// printf("buffer %s\n",buffer);
					write(sock,buffer,strlen(buffer)-1);
					bzero(buffer,MAX);
					read(sock,buffer,sizeof(buffer));
				}								
				else
				{
					FILE *fp = fopen(name,"rb");
					if(fp==NULL)
					{
						printf("file not found");
					}
					else{
						int PSIZE = 1048576;
						fseek(fp,0,SEEK_END);
						long k =ftell(fp);
						fseek(fp,0,SEEK_SET);
						fclose(fp);
						char str[1000];
						sprintf(str," %ld\n",k);
						strcat(buffer,str);
						printf("buffer is:[%s]\n",buffer);	
						write(sock,buffer,strlen(buffer)-1);
					}
				}
			}
			else
			{
				if(strncmp(buffer,"cat",3)==0)
				{
					char name[100];
					// buf[MAX]={'\0'};
					sscanf(buffer,"cat %s",name);
					// printf("name is%s\n",name);
					bzero(buffer,MAX);
					sprintf(buffer,"download %s\n",name);
					set=1;
				}
				write(sock,buffer,strlen(buffer)-1);
			}
        }
        if(FD_ISSET(sock,&rset))
		{
			if(strncmp(buffer,"upload",6)==0)
			{
				int port[1000]={'\0'};
				char ip[1000][100];
				int ino[1000];
				// printf("buffer%s\n",buffer);
				char name[100];
				sscanf(buffer,"upload %s",name);
				bzero(buffer,MAX);
				valread = read(sock,buffer,sizeof(buffer));
				char xno[100]={'\0'};
				int i=0,j=0;
				// printf("buffer %s\n",buffer);
			
				while(buffer[i]!=' ')
				{
					xno[j++]=buffer[i++];
				}
				i++;
				printf("please wait while uploading \nno of segment is %s \n",xno);
				int n=atoi(xno);
				bzero(xno,100);
				j=0;
				while(buffer[i]!='|')
				{
					xno[j++]=buffer[i++];
				}
				i++;
				// printf("size is %d\n",size);
				int size=atoi(xno);
				// printf("size is %d\n",size);
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
					// printf("ino [%s]\n",p);
					ino[k]=atoi(p);
					bzero(p,100);
					j=0;
					while(buffer[i]!=' ')
						p[j++]=buffer[i++];
					i++;
					bzero(ip[k],100);

					strcpy(ip[k],p);
										// printf("ip from p is %s \n ",p);
										// printf("ip is %s \n ",ip[k]);
					j=0;
					bzero(p,100);
					while(buffer[i]!='|')
						p[j++]=buffer[i++];
					port[k]=atoi(p);

					// printf("port no with p is [%s] \n",p);
					// printf("port no is [%d] \n",port[k]);
					bzero(p,100);
					j=0;
					
					// i++;
					// printf("ino [%d] port no [%d] ip [%s]\n",ino[k],port[k],ip[k]);
				}
				int PSIZE = 1048576;
				upload(size,name,port,ip,PSIZE,ino);
			}
			else
			{
				if(strncmp(buffer,"download",6)==0)
				{
					int port[1000]={'\0'};
					char ip[1000][100];
					int ino[1000];
					// printf("buffer%s\n",buffer);
					char name[100];
					sscanf(buffer,"download %s",name);
					// printf("-------------\n name is [%s]",name);
					bzero(buffer,MAX);
					valread = read(sock,buffer,sizeof(buffer));
					char xno[100]={'\0'};
					int i=0,j=0;
					// printf("buffer %s\n",buffer);
				
					while(buffer[i]!=' ')
					{
						xno[j++]=buffer[i++];
					}
					i++;
					printf("please wait while downloading \n no of segment  is %s \n",xno);
					int n=atoi(xno);
					bzero(xno,100);
					j=0;
					while(buffer[i]!='|')
					{
						xno[j++]=buffer[i++];
					}
					i++;
					// printf("size is %d\n",size);
					int size=atoi(xno);
					// printf("size is %d\n",size);
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
						// printf("ino [%s]\n",p);
						ino[k]=atoi(p);
						bzero(p,100);
						j=0;
						while(buffer[i]!=' ')
							p[j++]=buffer[i++];
						i++;
						bzero(ip[k],100);

						strcpy(ip[k],p);
											// printf("ip from p is %s \n ",p);
											// printf("ip is %s \n ",ip[k]);
						j=0;
						bzero(p,100);
						while(buffer[i]!='|')
							p[j++]=buffer[i++];
						port[k]=atoi(p);

						// printf("port no with p is [%s] \n",p);
						// printf("port no is [%d] \n",port[k]);
						bzero(p,100);
						j=0;
						
						// i++;
						// printf("ino [%d] port no [%d] ip [%s]\n",ino[k],port[k],ip[k]);
					}
					int PSIZE = 1048576;
					// printf("name is %s\n",name);
					download(size,name,port,ip,PSIZE,ino);
					if(set==1)
					{
						char np[1000]={'\0'};
						sprintf(np,"saved%s",name);
						// printf("np is %s \n",np);
						FILE *fdp=fopen(np,"rb");
						while(!feof(fdp))
						{
							printf("%c",getc(fdp));
						}
						printf("\n");
						set=0;
					}
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
					printf("\n%s",buffer);
				}
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
