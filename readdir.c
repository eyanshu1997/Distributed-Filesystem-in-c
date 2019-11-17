#include <stdio.h> 
#include <dirent.h> 
 #include<string.h>
int main(void) 
{ 
    struct dirent *de;  
  
    DIR *dr = opendir("."); 
  
    if (dr == NULL)  
    { 
        printf("Could not open current directory" ); 
        return 0; 
    } 
    while ((de = readdir(dr)) != NULL)
	{		
		if(strcmp(de->d_name,".")!=0&&strcmp(de->d_name,"..")!=0&&de->d_type==DT_REG)
		printf("%s\n", de->d_name); 
	}
    closedir(dr);     
    return 0; 
} 