#include<stdio.h>
int main()
{
FILE *fp=fopen("a.txt","wb");
int i=1048576;
while(i--)
{
	fputc('a',fp);
}
fclose(fp);
}