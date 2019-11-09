#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef struct node1 filenode;
typedef struct node2 dirnode;
typedef struct node3 inode;


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

int noofds=3;
struct node3
{
	int dsno;   //data server no
	int ino;      //file node no
	long size;     //block size
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
	// printf("finding %s in %s insdei finds\n",dirname,dir->name);
	if(strcmp(dirname,"..")==0)
	{
		char p[100];
		strcpy(p,dir->path);
		// printf("original path is %s \n",p);
		char *x=strchr(p,'/');
		char t[1000]={'\0'};
		int i=0;
		for(i=0;(p+i)!=(x);i++)
		{
			// i++;
			;
		}
		for(int j=0;j<i;j++)
		{
			t[j]=*(p+j);
		}
		if(i>0)
		{
			// printf("path is %s\n",t);
			dirnode *xy=getdir(root,t);
			return xy;
		}
		else
		{
			return root;
		}
		
	}
	
	for(int i=0;i<dir->ndir;i++)
	{
		if(strcmp(dir->dirlist[i]->name,dirname)==0)
		{
			// printf("found %d %s\n",i,dir->dirlist[i]->name);
			return dir->dirlist[i];
		}
	}
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
dirnode * getdir(dirnode *root,char *path)
{
	char p[1000];
	strcpy(p,path);
	dirnode *n=root;
	int set=0;
	while(1)
	{
		printf("searching  %s \n",p);
		char *x=strchr(p,'/');
		if(x==NULL)
		{
			return findd(root,n,p);
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
			bzero(p,1000);
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
	dirnode *new = (dirnode *)malloc(sizeof(dirnode));
	new->ndir=0;
	new->nfile=0;
	bzero(new->path,100);
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
		if(i!=n-1)
			li[i]->size=segsize;
		else
			li[i]->size=size%segsize;
		// printf("created inode %d dsno %d size %ld\n",i,li[i]->dsno,li[i]->size);
	}
	return li;
}
filenode *addfile(dirnode *dir,char *name,long size,long segsize)
{
	filenode *new = (filenode *)malloc(sizeof(filenode));
	new->size=size;
	new->noofcopies=0;
	strcpy(new->name,name);
	long n;
	if(size%segsize==0)
		n=size/segsize;
	else
		n=(size/segsize)+1;
	new->ninodes=n;
	new->inodes= createinode(size,segsize,n);
	dir->flist[dir->nfile]=new;
	(dir->nfile)++;
	return new;
}

dirnode *createroot()
{
	dirnode *root = (dirnode *)malloc(sizeof(dirnode));
	root->ndir=0;
	root->nfile=0;
	bzero(root->path,100);
	strcpy(root->path,"");
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

int main()
{
	dirnode *root=createroot();
	printd(root);
	filenode *tempf=addfile(root,"file1",11,2);
	// printfile(getfile(root,"/file1"));
	dirnode *temp=adddir(root,"first");
	// printd(temp);
	temp=adddir(root,"second");
	dirnode *first =getdir(root,"first");
	temp=adddir(first,"ifirst");
	temp=adddir(temp,"sfirst");
	copydir(root,"first/ifirst","second");											
	copyfile(root,"file1","first");
	copyfile(root,"file1","first/ifirst/sfirst");
	// printfile(getfile(root,"/file1"));
	// printd(first);
	// deletefile(root,"first/file1");
	// deletefile(root,"file1");
	// printd(root);
	// printd(first);
	// printf("hello\n");
	// printd(getdir(root,"first/ifirst/sfirst"));
	// printfile(getfile(root,"first/ifirst/sfirst/file1"));
		printf("hello\n");
	// printfile(getfile(root,"first/../first/ifirst/sfirst/file1"));
	printd(getdir(root,"first/../first/ifirst/sfirst"));
	// deletedir(root,"second");
	// deletedir(root,"first");
	// dirnode *res= getdir(root,"first/ifirst");
	// if(res!=NULL)
		// printd(res);
	// else
		// printf("directory not ofund\n");
	// printd(root);
	// filenode *resf= getfile(root,"/file1");
		// printf("done\n");

	// if(resf!=NULL)
	// {
		// printfile(resf);
	// }
	// else{
		// printf("file not found\n----------------\n");
	// }
	// if(copydir(root,"first/ifirst","second")==0)
	// {
		// printf("done\n");
	// }
	// else
	// {
		// printf("error in copy\n");
	// }
	// printd(getdir(root,"second"));
	// printd(getdir(root,"second/ifirst"));

	// if(first ==NULL)
	// {
		// printf("node not found\n");
	// }
	// else
	// {
		// printd(first);
	// }
	// dirnode *fir =findd(first,"ifirst");
	// if(fir ==NULL)
	// {
		// printf("node not found\n");
	// }
	// else
	// {
		// printd(fir);
	// }
		// printfile(tempf);
	printd(root);
}
