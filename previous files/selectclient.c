// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<fcntl.h>
 #include <arpa/inet.h>
int max(int a,int b){
    return (a>b)?a:b;
}
int convertToInt(char*);
int main(int argc, char const *argv[]) 
{ 
    int portNo = 0;
    char buffer[1024] = {0};
    // printf("please Enter port No of Server\n");
    // fgets(buffer,sizeof(buffer),stdin);
    // fflush(stdin);
	// buffer[strlen((buffer))-1]  = '\0';
    // portNo = convertToInt(buffer);
	portNo=5001;
	int sock = 0, valread; 
	struct sockaddr_in serv_addr; 
    int maxfd1=1;
    fd_set rset;
    FD_ZERO(&rset);
	int i=0,count=0;
    while (1)
    { 
        FD_SET(0,&rset);
        printf("Enter Something\n");
        select(maxfd1,&rset,NULL,NULL,NULL);
        if(FD_ISSET(0,&rset))
		{
            fgets(buffer,sizeof(buffer),stdin);
            fflush(stdin);
		    buffer[strlen((buffer))]  = '\0';
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
            write(sock,buffer,strlen(buffer)-1);
			FD_SET(sock,&rset);
            
        }
        if(FD_ISSET(sock,&rset))
		{
			bzero(buffer,1024);
            valread = read(sock,buffer,sizeof(buffer));
            if(valread==0){
                printf("connection closed by peer\n");
                FD_CLR(sock,&rset);
                close(sock);
                break;
            }
            buffer[valread] = '\0';
            printf("Server Response : %s\n",buffer);
			if(strcmp(buffer,"exit")==0)
			{
				close(sock);
				FD_CLR(i, &rset);
				// exit(0);
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
