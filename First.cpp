# include<stdio.h>
# include<stdlib.h>
# include<conio.h>
int i;
FILE *fp;
FILE *rfp;
char buffer[1000];
int WriteFile(char* filename)
	{
		char a;
		int count=0;
		int i=0;
		if((fp=fopen(filename,"w"))==NULL)
		{   
			printf("THE FILE DOESN'T OPEN！");exit(0); 
		}
		while(i<1000)    
		{
			scanf("%c",&a) ;
			if(a=='0')return i;       
			fprintf(fp,"%c" ,a);        
			i++;     
		}
	fclose(fp);          
	return i;
	}
int readbuffer()   
	{
		int flen=0;       
		int j=0;
		char ch;
		char *filename="zxm.txt";
		printf("文件名：zxm.txt \n");
		if((rfp=fopen("zxm.txt","r"))==NULL)
		{ 
			printf("THE FILE DOESN'T OPEN！");exit(0); 
		}
		while(j<1000)    
		{ 
			ch=fgetc(rfp);       
			if (feof(rfp))      
			{
				i=j;                 
				break; 
			}	
		buffer[j]=ch;       
		++j;
		}
		while(flen<j)         
		{
			printf("%c",buffer[flen]);  
			flen++;
		}                      
		printf("\n");             
		return 1;
	}  
main()    
	{
		char *filename={"zxm.txt"};
		int j;
		j=0;
		char creatflie='0';
		printf("1、输入c创建文件\n");
		printf("2、输入o打开文件\n");
		scanf("%c",&creatflie) ;
		if(creatflie=='c')
		   WriteFile(filename);
		if(creatflie=='o')
		   readbuffer();
	}  
