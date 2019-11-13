#include<stdio.h>
int main()
{
FILE *fp=fopen("a.txt","wb");
int i=1048576;
while(i--)
{
	char a[10];
	sprintf(a,"%d",i%10);
	fputc(a[0],fp);
}
fclose(fp);
}