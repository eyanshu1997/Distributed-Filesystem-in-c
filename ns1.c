#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef struct node1 filenode;
typedef struct node2 dirnode;
typedef struct node3 fpnode;
struct node3
{
	int dsno;   //data server no
	int ino;      //file node no
	int size;     //block size
};
struct node1
{
	char name[100];    //filename
	int size;					//size
	fpnode **loc;			//location of first node
	char path[1000];   //path in bigfs
};
struct node2
{
	int ndir;					//no of dirlist
	int nfile;				//no of files
	char name[100]; 	//name
	dirnode *dirlist[100];    //list of directories
	filenode *flist[100];      //list of flists;
};
int adddir(dirnode *dir,char *name)
{
	dirnode *new = (dirnode *)malloc(sizeof(dirnode));
	new->ndir=0;
	new->nfile=0;
	strcpy(new->name,name);
	dir->dirlist[dir->ndir]=new;
	(dir->ndir)++;
	return 1;
}
void print(dirnode *dir)
{
	printf("the name of directpry is %s \n",dir->name);
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
}
dirnode * findd(dirnode *dir,char *dirname)
{
	for(int i=0;i<dir->ndir;i++)
	{
		if(strcmp(dir->dirlist[i]->name,dirname)==0)
			return dir->dirlist[i];
	}
	dirnode *x=NULL;
	return x;
}
filenode *findf(dirnode *dir,char *fname)
{
	for(int i=0;i<dir->ndir;i++)
	{
		if(strcmp(dir->flist[i]->name,fname)==0)
			return dir->flist[i];
	}
	filenode *x=NULL;
	return x;
}
void getdir(dirnode *dir,char *path)
{
	char p[1000];
	strcpy(p,path);
	dirnode *n=dir;
	int set=0;
	while(1)
	{
		char *x=strchr(p,'/');
		if(x==NULL)
		{
			return findd(n,p);
		}
		else
		{
			char *temp[1000];
			x++;
			i=0;
			while(*x)
			{
				temp[i++]=*x++;
			}
			bzero(p,1000);
			strcpy(p,temp);
		}
	}
}
int main()
{
	dirnode *root = (dirnode *)malloc(sizeof(dirnode));
	root->ndir=0;
	root->nfile=0;
	strcpy(root->name,"~");
	adddir(root,"first");
	adddir(root,"second");
	print(root);
	dirnode *first =findd(root,"first");
	if(first ==NULL)
	{
		printf("node not found\n");
	}
	else
	{
		print(first);
	}
	dirnode *fir =findd(root,"fir");
	if(fir ==NULL)
	{
		printf("node not found\n");
	}
	else
	{
		print(fir);
	}
}
