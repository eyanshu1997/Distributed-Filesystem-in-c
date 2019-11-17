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
int convertToInt(char*);
int main(int argc, char *argv[])
{
	if(argc!=2){
		printf("Oops Please give Server port no as Command Line Argument\n");
		exit(1);
	}
	int portNo = convertToInt(argv[1]);

	fd_set master;

	fd_set read_fds;

	struct sockaddr_in serveraddr,clientaddr;

	  

	char buf[MAX];
	int maxfd1;    // maximum file descriptor number + 1
	int listenfd;
	int newfd;
	int bytes_read;
	int addrlen = 5;
	int i, j;

	//clearing the master and read_fds set

	FD_ZERO(&master);
	FD_ZERO(&read_fds);


	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Erro While Listening");
		
		exit(1);

	}
	memset(&serveraddr, 0, sizeof(serveraddr)); 
	memset(&clientaddr, 0, sizeof(clientaddr)); 
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_port = htons(portNo);


	//binding to port
	if(bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
	{
		perror("Erro While binding");
		exit(1);
	}

	printf("Server has successfully binded on port number %d\n",portNo);

	//listening

	if(listen(listenfd, 10) == -1)
	{
		perror("Error While trying to listen on given port");
		exit(1);
	}

	// adding the listenfd to the master set

	FD_SET(listenfd, &master);
	maxfd1 = listenfd+1;

	while(1)
	{
		// printf("reached here\n");
		read_fds = master;
		if(select(maxfd1, &read_fds, NULL, NULL, NULL) == -1)
		{

			perror("Error returned by select");
			exit(1);

		}

		for(i = 0; i <maxfd1; i++)
		{

			if(FD_ISSET(i, &read_fds))
			{ 
				if(i == listenfd)

				 {
					//code to accept new connection
					
					if((newfd = accept(listenfd, (struct sockaddr *)&clientaddr, &addrlen)) == -1)

					{
						perror("Error while Accepting connection");
					}
					else
					{
					

					if(newfd > maxfd1-1)

					{ 
						maxfd1 = newfd+1;

					}

					//adding new fd to master list
					 FD_SET(newfd, &master);

					 printf("Client with ip %s has successfully been connected on socket no %d\n ", inet_ntoa(clientaddr.sin_addr),newfd);
					}
				}

				else
				{
					bzero(buf,MAX);	
					bytes_read = recv(i, buf, sizeof(buf), 0);
					printf("command recieved %s from %d\n",buf,i);
				   //client has closed the connection
					if(bytes_read == 0)

					{
				   
					printf("Client on Socket No %d has closed the connection\n", i);
					close(i);
					//clearing the fd from masterset
					FD_CLR(i, &master);

					}
					else if(bytes_read<0){
						perror("error while receiving");
						// exit(1);
						continue;
					}
					else
					{
						if(strncmp(buf,"write",5)==0)
						{
							int ret=fork();
							if(ret==0)
							{
								char name[100]={'\0'};
								int k=0,j=0;
								while(buf[k]!=' ')
								{
									k++;
								}
								k++;
								name[0]='n';
								name[1]='e';
								name[2]='w';
								j=3;
								while(buf[k])
								{
									name[j++]=buf[k++];
								}
								printf("buffer [%s] file name %s \n",buf,name);
								send(i,"done",sizeof("done"),0);
								FILE *fp=fopen(name,"wb");
								if(fp<0)
								{
									printf("eroornopening file\n");
									bzero(buf,MAX);
									strcat(buf,"file error\n");
									if(send(i, buf, MAX, 0) == -1)
										perror("Error Sending Data");
									continue;
								}
								bzero(buf,MAX);
								int n=recv(i,buf,sizeof(buf),0);
								int nooftime=atoi(buf);
								printf("no of times:[%d]\n",nooftime);
								// printf("sending");
								// int no=0;
								while(nooftime>0)
								{
									unsigned char a[10000]={'\0'};
									send(i,"done",4,0);
									// if(nooftime%10000==0)
									// printf(".");
									fflush(stdout);
									n=recv(i,a,sizeof(a),0);
									// printf("result recieved[%s]\n",buf);
									// printf("n is %ld\n",strlen(buf));
									// fputc(a,fp);
									fwrite(a,10000,1,fp);
									// no=n+no;
									nooftime=nooftime-10000;
								}
								fclose(fp);
								// printf("n %d",no);
								// printf("\n");
								bzero(buf,MAX);
								// bzero(result,MAX);
								strcat(buf,"succesffull\n");
								if(send(i, buf, MAX, 0) == -1)
									perror("Error Sending Data");
							}
							else
							{
								
								// printf("Client on Socket No %d has closed the connection\n", i);
								close(i);
								//clearing the fd from masterset
								FD_CLR(i, &master);
								maxfd1=maxfd1-1;
							}
						}
						else
						{
							if(strncmp(buf,"read",4)==0)
							{
								int ret=fork();
								if(ret==0)
								{
									char name[100]={'\0'};
									int k=0,j=0;
									while(buf[k]!=' ')
									{
										k++;
									}
									k++;
									name[0]='n';
									name[1]='e';
									name[2]='w';
									j=3;
									while(buf[k])
									{
										name[j++]=buf[k++];
									}
									printf("buffer [%s] file name %s \n",buf,name);

									FILE *fp=fopen(name,"rb");
									if(fp<0)
									{
										printf("eroornopening file\n");
										bzero(buf,MAX);
										strcat(buf,"file error\n");
										if(send(i, buf, MAX, 0) == -1)
											perror("Error Sending Data");
										continue;
									}
									fseek(fp,0,SEEK_END);
									int len = ftell(fp);
									fseek(fp,0,SEEK_SET);
									// int nooftimes=len/1000;
									printf("no of times%d\n",len);
									char x[1000];
									sprintf(x,"%d",len);
									send(i,x,sizeof(x),0);
									bzero(buf,MAX);
									int n=recv(i,buf,sizeof(buf),0);
									// printf("sending");
									int no=0;
									while(len>0)
									{
										unsigned char a[10000]={'\0'};
										j=0;
										// a=fgetc(fp);
										fread(a,10000,1,fp);
										send(i,a,sizeof(a),0);
										bzero(buf,MAX);
										recv(i,buf,4,0);
										bzero(buf,sizeof(buf));
										len=len-10000;
									}
									fclose(fp);
									// char cmd[1010]={'\0'};
									// sprintf(cmd,"rm %s",name);
									// system(cmd);
									printf("n %d",no);
									printf("\n");
									bzero(buf,MAX);
									// bzero(result,MAX);
									strcat(buf,"succesffull\n");
									if(send(i, buf, MAX, 0) == -1)
										perror("221Error Sending Data");
								}
								else
								{
									
									// printf("Client on Socket No %d has closed the connection\n", i);
									close(i);
									//clearing the fd from masterset
									FD_CLR(i, &master);
									maxfd1=maxfd1-1;
								}
							}
							bzero(buf,MAX);
							strcat(buf,"no command\n");
							if(send(i, buf, MAX, 0) == -1)
										perror("2121Error Sending Data");
						}

					}
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
