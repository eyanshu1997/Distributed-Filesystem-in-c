#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<fcntl.h>
#include <arpa/inet.h>
#define MAX 10000
typedef struct node1 filenode;
typedef struct node2 dirnode;
typedef struct node3 inode;
dirnode *root;
int inodecount=0;

void printd(dirnode *dir);

dirnode * findd(dirnode *root,dirnode *dir,char *dirname); //finds the directory in a directry

filenode * findf(dirnode *dir,char *fname); //finds the file in a directry

dirnode * getdir(dirnode *root,char *path); //get the directory from path

filenode * getfile(dirnode *root,char *path); //get the file	 from path

dirnode *adddir(dirnode *dir,char *name);  //add dir to a direcectory

filenode *addfile(dirnode *dir,char *name,long size,long segsize); //add a file to the directory

void printfile(filenode *file); //print a file 

dirnode *createroot();// create a root directory

filenode *findf(dirnode *dir,char *fname);  //find file in folder

int copydir(dirnode *root,char *source,char * destination); //copy file from one node to another 

int copyfile(dirnode *root,char *source,char * destination); //copy file from one path to another

int deletefile(dirnode *root,char*source); //delete file from a path

int deletedir(dirnode *root,char*source); //deletedir from path

int convertToInt(char*);

int noofds=3;
char *ip[1000];
int port[1000];
struct node3
{
	int dsno;   //data server no
	int ino;      //file node no
	long size;     //block size
	int gino;
};
struct node1
{
	char name[100];    //filename
	long size;					//size
	long ninodes;			//no of inodes
	inode **inodes;			//array list of all nodes
	int noofcopies;			//noofcopies
};
struct node2
{
	int ndir;					//no of dirlist
	int nfile;				//no of files
	char name[100]; 	//name
	dirnode *dirlist[100];    //list of directories
	filenode *flist[100];      //list of flists;
	char path[100];
};
 void ls(dirnode *dir,char *result)
{
	for(int i=0;i<MAX;i++)
		result[i]='\0';
	strcat(result,"\nthe name of directpry is");
	strcat(result,dir->name);
	strcat(result,"\nthe path of directpry is");
	strcat(result,dir->path);
	printf("dir name %s \n",dir->name);
	if(dir->nfile==0&&dir->ndir==0)
		strcat(result,"\nempty directory");			
	for(int i=0;i<dir->ndir;i++)
	{
		strcat(result,"\n");
		strcat(result,dir->dirlist[i]->name);
	}
	for(int i=0;i<dir->nfile;i++)
	{
		strcat(result,"\n");
		strcat(result,dir->flist[i]->name);
	}
	strcat(result,"\n");
	printf("result %s\n",result);
	// return result;
}
void printd(dirnode *dir)
{
	printf("the name of directpry is %s \n",dir->name);
	printf("the path of directpry is %s \n",dir->path);
	printf("the no of directories is %d \n",dir->ndir);
	
	for(int i=0;i<dir->ndir;i++)
	{
		printf("%d :%s \n",i,dir->dirlist[i]->name);
	}
	printf("the no of files  is %d \n",dir->nfile);
	for(int i=0;i<dir->nfile;i++)
	{
		printf("%d :%s \n",i,dir->flist[i]->name);
	}
	printf("---------------------------------\n");
}

void printinode(filenode *file,char *result)
{
	char x[10]={'\0'};
	sprintf(x,"%ld %ld",file->ninodes,file->size);
	strcat(result,x);
	// strcat(result,"|");
	char y[MAX]={'\0'};
	for(int i=0;i<file->ninodes;i++)
	{
		inode *in=file->inodes[i];
		sprintf(y,"|%d %s %d",in->gino,ip[in->dsno],port[in->dsno]);
		strcat (result,y);
		// printf("dataserver no%d \n",in->dsno);
	}
	// printf("---------------------------------\n");
}
void printfile(filenode *file)
{
	printf("the name of file is %s \n",file->name);
	printf("the size is %ld \n",file->size);
	printf("the no of copies is %d \n",file->noofcopies);
	printf("the no of inode  is %ld \n",file->ninodes);

	for(int i=0;i<file->ninodes;i++)
	{
		inode *in=file->inodes[i];
		printf("-------\n");
		printf("inode  no%d \n",in->ino);
		printf("size %ld \n",in->size);
		printf("dataserver no%d \n",in->dsno);
	}

	printf("---------------------------------\n");
}
dirnode * findd(dirnode *root,dirnode *dir,char *dirname)
{
	printf("finding [%s] in %s insdei finds\n",dirname,dir->name);
	for(int i=0;i<dir->ndir;i++)
	{
		if(strcmp(dir->dirlist[i]->name,dirname)==0)
		{
			printf("found %d %s\n",i,dir->dirlist[i]->name);
			return dir->dirlist[i];
		}
	}
	if(strcmp(dirname,"..")==0)
	{
		char p[100];
		strcpy(p,dir->path);
		printf("original path is %s \n",p);
		char *x=strchr(p,'/');
		char t[1000]={'\0'};
		char ta[1000]={'\0'};
		int i=0;
		for(i=0;(p+i)!=(x);i++)
		{
			ta[i]=p[i];
			// i++;
			;
		}
		for(int j=0;j<i;j++)
		{
			t[j]=*(p+j);
		}
		if(i>0)
		{
			printf("ta is [%s]\n",ta);
			if(strcmp(ta,"~")==0)
			{
				printf("returning root\n");
				return root;
			}
			printf("path is %s\n",t);
			dirnode *xy=getdir(root,t);
			return xy;
		}
		else
		{
			return root;
		}
		
	}
	printf("returning empty\n");
	dirnode *x=NULL;
	return x;
}
filenode *findf(dirnode *dir,char *fname)
{

	int i=0;
	for(i=0;i<dir->nfile;i++)
	{
		// printf("inside %d\n",i);
		if(strcmp(dir->flist[i]->name,fname)==0)
			return dir->flist[i];
	}
	// printf("outside %d\n",i);
	filenode *x=NULL;
	return x;
}
dirnode * getdir(dirnode *dir,char *path)
{
	char p[100]={'\0'};
	strcpy(p,path);
	dirnode *n=dir;
	int set=0;
	while(1)
	{
		if(strcmp(p,"~")==0)
			return root;	
		printf("searching  %s in %s\n",p,root->name);
		char *x=strchr(p,'/');
		if(x==NULL)
		{
			dirnode *u=findd(root,n,p);
			// printf("hererh [%s]\n",u->name);
			return u;
		}
		else
		{

			char temp[1000]={'\0'};
			char t[1000]={'\0'};
			int j=0;
			for(j=0;p[j]!='/';j++)
			{
				t[j]=p[j];
			}
			printf("searching %s in  %s \n",t,n->name);
			if(j>0)
			{
				n=findd(root,n,t);
				if(n==NULL)
				{
					return n;
				}
			}
			j++;
			int i=0;
			while(p[j])
			{
				temp[i++]=p[j++];
			}
			printf("remaining %s \n",temp);
			bzero(p,100);
			strcpy(p,temp);
			
		}
	}
}
filenode * getfile(dirnode *root,char *path)
{
	char p[1000];
	strcpy(p,path);
	dirnode *n=root;
	int set=0;
	while(1)
	{
		// printf("searching  %s \n",p);
		char *x=strchr(p,'/');
		if(x==NULL)
		{
			// printf("herere\n");
			return findf(n,p);
		}
		else
		{

			char temp[1000]={'\0'};
			char t[1000]={'\0'};
			int j=0;
			for(j=0;p[j]!='/';j++)
			{
				t[j]=p[j];
			}
			// printf("searching %s in  %s \n",t,n->name);
			if(j>0)
			{
				n=findd(root,n,t);
				// printf("found %s \n",n->name);
				if(n==NULL)
				{
					filenode *xy=NULL;
					return xy;
				}
			}
			j++;
			int i=0;
			while(p[j])
			{
				temp[i++]=p[j++];
			}
			// printf("remaining %s \n",temp);
			bzero(p,1000);
			strcpy(p,temp);
		}
	}
}
dirnode *adddir(dirnode *dir,char *name)
{
	// printf("helo\n");
	dirnode *chk=findd(root,dir,name);
	if(chk!=NULL)
	{
		dirnode *x=NULL;
		return x;
	}
	dirnode *new = (dirnode *)malloc(sizeof(dirnode));
	new->ndir=0;
	new->nfile=0;
	bzero(new->path,100);
	if(strcmp(dir->name,"~")!=0)
		strcat(new->path,dir->path);
	char *x=strchr(new->path,'\0');
	if(*(x-1)!='/')
		strcat(new->path,"/");
	if(strcmp(dir->name,"~")!=0)
		strcat(new->path,dir->name);
	strcat(new->name,name);
	dir->dirlist[dir->ndir]=new;
	(dir->ndir)++;
	return new;
}
inode **createinode(long size,long segsize,long n)
{
	inode **li=(inode **)malloc(n*sizeof(inode *));
	for(int i=0;i<n;i++)
	{
		li[i]=(inode *)malloc(sizeof(inode));
		// printf("creating inode %d\n",i);
		li[i]->dsno=i%noofds;
		li[i]->ino=i;
		li[i]->gino=inodecount++;
		if(i!=n-1)
			li[i]->size=segsize;
		else
			li[i]->size=size%segsize;
		// printf("created inode %d dsno %d size %ld\n",i,li[i]->dsno,li[i]->size);
	}
	return li;
}
filenode *addfile(dirnode *dir,char *nam,long size,long segsize)
{
	filenode *new = (filenode *)malloc(sizeof(filenode));
	new->size=size;
	new->noofcopies=0;

	strcpy(new->name,nam);
	printf("adding origi name [%s]\n",nam);
	printf("adding name [%s]\n",new->name);
	long n;
	if(size%segsize==0)
		n=size/segsize;
	else
		n=(size/segsize)+1;
	new->ninodes=n;
	new->inodes= createinode(size,segsize,n);
	dir->flist[dir->nfile]=new;
	(dir->nfile)++;
	printf("no of nodes %ld\n",n);
	return new;
}

dirnode *createroot()
{
	dirnode *root = (dirnode *)malloc(sizeof(dirnode));
	root->ndir=0;
	root->nfile=0;
	bzero(root->path,100);
	strcpy(root->path,"~");
	strcpy(root->name,"~");
	return root;
}
int copydir(dirnode *root,char *source,char * destination)
{
	dirnode *src=getdir(root,source);
	if(src==NULL)
	{	
		printf("error in copy\n");
		printf("culdnt find source\n---------------------------------\n");
		return 1;
	}
	dirnode *dest=getdir(root,destination);
	if(dest==NULL)
	{	
		printf("error in copy\n");
		printf("culdnt find dest\n---------------------------------\n");
		return 1;
	}
	dirnode *new = (dirnode *)malloc(sizeof(dirnode));
	new->ndir=src->ndir;
	for(int i=0;i<src->ndir;i++)
		new->dirlist[i]=src->dirlist[i];
	new->nfile=src->nfile;
	for(int i=0;i<src->nfile;i++)
		new->flist[i]=src->flist[i];
	
	strcpy(new->name,src->name);
	bzero(new->path,100);
	strcat(new->path,destination);

	dest->dirlist[dest->ndir]=new;
	(dest->ndir)++;
	printf("-----%s copied succesfully to %s-------\n",new->name,destination);
	return 0;
	
}
int copyfile(dirnode *root,char *source,char *destination)
{
	filenode *src=getfile(root,source);
	if(src==NULL)
	{	
		printf("error in copy\n");
		printf("culdnt find source\n---------------------------------\n");
		return 1;
	}
	dirnode *dest=getdir(root,destination);
	if(dest==NULL)
	{	
		printf("error in copy\n");
		printf("culdnt find dest\n---------------------------------\n");
		return 1;
	}
	(src->noofcopies)++;
	dest->flist[dest->nfile]=src;
	(dest->nfile)++;
	printf("------------\n%s copied succesfully to %s\nnofo copies %d \n-------\n",src->name,destination,src->noofcopies);
}


int deletefile(dirnode *root,char*source)
{
	char *x=strchr(source,'/');
	char res[1000]={'\0'};
	char t[1000]={'\0'};
	if(x!=NULL)
	{
	
		int i=0;
		for(i=0;(source+i)!=(x);i++)
		{
			// i++;
			;
		}
		for(int j=0;j<i;j++)
		{
			t[j]=*(source+j);
		}
		for(int j=i+1;source[j];j++)
		{
			res[j-i-1]=source[j];
		}
		// printf("t is [%s]\n",t);
		// printf("res is[%s]\n",res);
		root=getdir(root,t);
		if(root ==NULL)
		{
			printf("file not found\n---------------------------------\n");
			return 1;
		}
	}
	else
	{
		strcpy(res,source);
	}
	// printf("deleting from %s\n",root->name);
	int set=0;
	for(int i=0;i<root->nfile;i++)
	{
		if(set==0)
		{
			// printf("checking for file [%s] [%s]\n",root->flist[i]->name,res);
			if(strcmp(root->flist[i]->name,res)==0)
			{
				set=1;
				if(i!=root->nfile-1)
				{
					root->flist[i]=root->flist[i+1];
				}
				if(root->flist[i]->noofcopies>0)
				{
					(root->flist[i]->noofcopies)--;
				}
				else
				{
					free(root->flist[i]);
				}
			}
			
		}
		else
		{
			if(i!=root->nfile-1)
			{
				root->flist[i]=root->flist[i+1];
			}
		}
	}
	if(set==0)
	{
		printf("file not found couldnt delete\n----------------------\n");
		return 1;
	}
	else
	{
		(root->nfile)--;
		printf("file deleted \n----------------------------\n");
		return 0;
	}
}


int deletedir(dirnode *root,char*source)
{
	char *x=strchr(source,'/');
	char res[1000]={'\0'};
	char t[1000]={'\0'};
	if(x!=NULL)
	{
	
		int i=0;
		for(i=0;(source+i)!=(x);i++)
		{
			// i++;
			;
		}
		for(int j=0;j<i;j++)
		{
			t[j]=*(source+j);
		}
		for(int j=i+1;source[j];j++)
		{
			res[j-i-1]=source[j];
		}
		// printf("t is [%s]\n",t);
		// printf("res is[%s]\n",res);
		root=getdir(root,t);
		if(root==NULL)
		{
			printf("filoe not found\n---------------------------------\n");
			return 1;
		}
	}
	else
	{
		strcpy(res,source);
	}
	// printf("deleting from %s\n",root->name);
	int set=0;
	for(int i=0;i<root->ndir;i++)
	{
		if(set==0)
		{
			// printf("checking for file [%s] [%s]\n",root->dirlist[i]->name,res);
			if(strcmp(root->dirlist[i]->name,res)==0)
			{
				set=1;
				if((root->dirlist[i]->ndir>0)||(root->dirlist[i]->nfile>0))
				{
					printf("cant delete the directory it is not empty\n----------------------\n");
					return 1;
				}
				else
				{
					free(root->dirlist[i]);
					if(i!=root->ndir-1)
					{
						root->dirlist[i]=root->dirlist[i+1];
					}
				}

			}
			
		}
		else
		{
			if(i!=root->nfile-1)
			{
				root->dirlist[i]=root->dirlist[i+1];
			}
		}
	}
	if(set==0)
	{
		printf("directory  not found couldnt delete\n----------------------\n");
		return 1;
	}
	else
	{
		(root->ndir)--;
		printf("directory deleted \n----------------------------\n");
		return 0;
	}
}

int convertToInt(char *in){
int i = strlen(in)-1,multiplier=1,result = 0;

while(i>=0){
result+=(in[i]-48)*multiplier;
multiplier*=10;
i--;
}
return result;
}

void getresult(char *buf,char *path,char *result)
{
	// if(cu
	printf("buf is [%s] path is [%s] result is [%s]\n",buf,path,result); 
	dirnode *cur;
	if(strcmp(path,"~")!=0)
		cur=getdir(root,path);
	else
		cur=root;
	if(strncmp(buf,"ls",2)==0)
	{
		// printf("
		ls(cur,result);
		// printf("inside %s\n",result);
		return ;																		
	}
	if(strncmp(buf,"upload",6)==0)
	{
		// printf("
		// ls(cur,result);
		char name[MAX]={'\0'};
		long siz;
		char upload[1000];
		sscanf(buf,"%s %s %ld",upload,name,&siz);
		// char 
		// int i=0;
		// while(buf[i]!=' ')
			// i++;
		// i++;
		// int j=0;
		// if(!buf[i])
		// {				
			// bzero(result,MAX);
			// strcat(result,"wrong command\n");
			// return ;
		// }
		// while(buf[i]!=' ')
		// {
			// name[j++]=buf[i++];
		// }
		// i++;
		// printf("name is %s\n",name);
		// char size[100]={'\0'};
		// j=0;
		// while(buf[i]!=' ')
		// {
			// size[j++]=buf[i++];
		// }
		
		// long siz;
		// char *ptr;
		// siz = strtol(size, &ptr, 10);
		printf("size is%ld\n",siz);
		// char nam[10000]={'\0'};
		// strcat(nam,name);
		// printf("nam is %s\n",nam);
		printf("name is %s\n",name);
		// filenode *f=addfile(cur,nam,siz,1048576);
		filenode *f=addfile(cur,name,siz,1048576);
		printf("created file\n");
		printfile(f);
	
		// printf("inside %s\n",result);
		bzero(result,MAX);
		// strcat(result,"suc\n");
		printinode(f,result);
		printf("returning\n");
		// return ;
		return ;																		
	}
	if(strncmp(buf,"download",8)==0)
	{
		// printf("
		// ls(cur,result);
		char name[MAX]={'\0'};
		char upload[1000];
		sscanf(buf,"%s %s",upload,name);

		printf("name is %s\n",name);
		filenode *f=getfile(root,name);
		printf("created file\n");
		printfile(f);
	
		// printf("inside %s\n",result);
		bzero(result,MAX);
		// strcat(result,"suc\n");
		printinode(f,result);
		printf("returning\n");
		// return ;
		return ;																		
	}
	if(strncmp(buf,"rm",2)==0)
	{
		// printf("
		// ls(cur,result);
		char name[MAX]={'\0'};
		char upload[1000];
		sscanf(buf,"%s %s",upload,name);

		printf("name is %s\n",name);
		deletefile(root,name);
		printf("created file\n");
		// printfile(f);
	
		// printf("inside %s\n",result);
		bzero(result,MAX);
		strcat(result,"suc\n");
		// printinode(f,result);
		printf("returning\n");
		// return ;
		return ;																		
	}
	
	if(strncmp(buf,"mkdir",5)==0)
	{
		// printf(
		char name[MAX]={'\0'};
		int i=0;
		while(buf[i]!=' ')
			i++;
		i++;
		int j=0;
		if(!buf[i])
		{				
			bzero(result,MAX);
			strcat(result,"wrong command\n");
			return ;
		}
		while(buf[i])
		{
			name[j++]=buf[i++];
		}
		dirnode *new=adddir(cur,name);
		if(new==NULL)
		{
			bzero(result,MAX);
			strcat(result,"not found\n");
			return ;
		}
		bzero(result,MAX);
		strcat(result,"successfull\n");
		return ;
																	
	}
	if(strncmp(buf,"rmdir",5)==0)
	{
		// printf(
		char name[MAX]={'\0'};
		int i=0;
		while(buf[i]!=' ')
			i++;
		i++;
		int j=0;
		if(!buf[i])
		{				
			bzero(result,MAX);
			strcat(result,"wrong command\n");
			return ;
		}
		while(buf[i])
		{
			name[j++]=buf[i++];
		}
		int new;
		if(name[0]=='/')
		{
			int j=0;
			while(name[j+1])
			{
				name[j]=name[j+1];
				j++;
			}
			name[j]='\0';
			printf("deleting file [%s]\n",name);
			new=deletedir(root,name);
		}
		else
		{
			char pa[MAX]={'\0'};
			strcat(pa,path);
			strcat(pa,"/");
			strcat(pa,name);
			printf("2deleting file [%s]\n",pa);
			new=deletedir(root,pa);
		}
		if(new==1)
		{
			bzero(result,MAX);
			strcat(result,"error couldnt find directory or directory not empty\n");
			return ;
		}
		bzero(result,MAX);
		strcat(result,"successfull\n");
		return ;
																	
	}
	if(strncmp(buf,"cd",2)==0)
	{
		// printf("
		int i=0;
		while(buf[i]!=' ')
		{
			i++;
		}
		i++;
		int j=0;
		char pa[MAX]={'\0'};
		while(buf[i])
		{
			pa[j++]=buf[i++];
		}
		if(pa[0]=='/')
		{
			j=0;
			while(pa[i+1])
			{
				pa[i]=pa[i+1];
				i++;
			}
			pa[i]='\0';
			cur=getdir(root,pa);
			if(cur==NULL)
			{
				bzero(result,MAX);
				strcat(result,"Directory not found\n ");
				return ;											
			}
		}
		else
		{
			cur=getdir(cur,pa);
			if(cur==NULL)
			{
				bzero(result,MAX);
				strcat(result,"Directory not found\n ");
				return ;											
			}
			printf("succesfull %s \n",cur->name);
		}
		bzero(path,MAX);
		strcpy(path,cur->path);
		if(cur!=root)
		{
			strcpy(path,cur->path);
			strcat(path,"/");
		}
		strcpy(path,cur->name);
		bzero(result,MAX);
		strcat(result,"succesfully changed inside: ");
		strcat(result,path);
		strcat(result,"\n");
		// printf("inside %s\n",result);
		return ;																		
	}
	printf("no command found\n");
	strcat(result,"no command found\n");
	return ;
}

int main(int argc, char *argv[])
{
	if(argc!=2){
		printf("Oops Please give Server port no as Command Line Argument\n");
		exit(1);
	}
	int portNo = convertToInt(argv[1]);
	//master file descriptor list 
	fd_set master;

	//temp file descriptor list for select()
	fd_set read_fds;

	struct sockaddr_in serveraddr,clientaddr;
	root=createroot();
	  
	char path[1000][100];
	char buf[MAX];
	int maxfd1;    // maximum file descriptor number + 1
	int listenfd;
	int newfd;
	int bytes_read;
	int addrlen = 5;
	int i, j;
	port[0]=8080;
	port[1]=8081;
	port[2]=8082;
	ip[0]="127.0.0.1";
	ip[1]="127.0.0.1";
	ip[2]="127.0.0.1";
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
					strcpy(path[newfd],root->path);
					// printf("path successfully set to [%s] [%s] \n",path[newfd],root->path);
					 printf("Client with ip %s has successfully been connected on socket no %d\n ", inet_ntoa(clientaddr.sin_addr),newfd);
					}
				}

				else

				{
					bzero(buf,MAX);
					bytes_read = recv(i, buf, sizeof(buf), 0);

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
						exit(1);
					}
					else
					{
						printf("current directory for the client %s \n",path[i]);
						// char *result="hello from server";
						char result[MAX]={'\0'};
						printf("hello\n");
						getresult(buf,path[i],result);
						printf("hello\n");
						printf("result:%s\n",result);			
						// printf("result:\n");			
						// for(int i=0;result[i];i++)
						// {
							// printf("%c",result[i]);
						// }
						// printf("path is [%s] \n",path[i]);
						// printf("root name is %s\n",root->name);
						if(send(i, result, MAX, 0) == -1)
							perror("Error Sending Data");
					}

				}

			}

		}

	}

	return 0;

}

