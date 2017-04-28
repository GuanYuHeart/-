#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#define  LENGTH 46  
#define  N 100  

/*-----------------*/
/* Thank Smile_7x  */
/*copy http://blog.csdn.net/smile_7x/article/details/8912279*/
/*-----------------*/

/***    定义结构体    ***/  
typedef struct token  
{  
    char name[30];  
    int code;  
    int addr;  
} token;  
  
typedef struct KeyWord  
{  
    char name[30];  
    int code;  
} KeyWord;  
  
typedef struct symble  
{  
    char name[30];  //字符名字  
    int number;    //字符编码  
    int type;  
} symble;  
  
/****  文件及变量  ****/  
char ch;  
int error_count; //错误出现的个数  
int var_count;  
int num_count;  
int label_count;  
int code_count;  
int addr_count;  //内码编址  
int LineOfPro;   //错误出现的行号  
char filename[30];  
FILE *SourceFin;    //源文件  
FILE *TokenFout;    //输出文件  
FILE *SymbleFout;   //符号表  
FILE *NumFout;   //常量表  
  
KeyWord key[14]= {{"void",1},{"main",2},{"int",3},{"float",4},{"const",5},{"for",6},  
    {"if",7},{"else",8},{"then",9},{"while",10},{"switch",11},{"break",12},{"begin",13},{"end",14}  
};  
  
token CurrentToken;  
token zancun;  
symble CurrentSimble;  
symble SymbleList[N];  
symble NumList[N];  
  
//***********************************  
void Scanner();       //主程序  
void IsAlpha();       //关键字  
void IsNumber();      //数字  
void IsAnotation();   //注释  
void IsOther();       //其它  
void OutPut();        //输出  
void Error(int a);    //错误类型  
int Flag_WordHave();  //查添符号表  
int Flag_NumHave();   //查添常量表  
//************************************  
int main()  
{  
    int i = 0 ;  
    code_count = 0;  
    var_count = 1;  
    label_count = 1;  
    addr_count = 0;  
    num_count = 0;  
    LineOfPro = 0; //行号  
    Scanner();  
    printf("输出标识符表：\n");  
    for(i=0; i<var_count-1; i++)  
        printf("<%s,%d>",SymbleList[i].name,i+1);  
    printf("\n");  
    printf("输出数字表：\n");  
    for(i=0; i<num_count; i++)  
        printf("<%s,%d>",NumList[i-1].name,i+1);  
    return 0;  
}  
//*************主程序***************  
void Scanner()  
{  
    int i =0;  
    if((SourceFin=fopen("shugang.txt","r"))==NULL)  
    {  
        printf("无法打开文件%s!\n",filename);  
        exit(1);  
    }  
  
    if((TokenFout=fopen("输出文件.txt","w"))==NULL)  
    {  
        printf("无法打开文件输出文件.txt！\n");  
        exit(1);  
    }  
    if((SymbleFout=fopen("符号文件.txt","w"))==NULL)  
    {  
        printf("无法打开符号文件.txt！\n");  
        exit(1);  
    }  
    if((NumFout=fopen("常量文件.txt","w"))==NULL)  
    {  
        printf("无法打开文件常量.txt！\n");  
        exit(1);  
    }  
    ch=fgetc(SourceFin);  
    while(ch!=EOF)  
    {  
        for(i=0; i<30; i++)  
            CurrentToken.name[i]='\0';//将单词缓冲区初始化  
        if((ch>47)&&(ch<58))  
            IsNumber();  
        if(((ch>='a')&&(ch<='z'))||((ch>='A')&&(ch<='Z'))||(ch=='_'))  
        {  
            IsAlpha();  
        }  
        if(ch=='/')  
            IsAnotation();  
        else  
            IsOther();  
    }  
    fclose(TokenFout);  
    fclose(SymbleFout);  
    fclose(NumFout);  
    printf("词法分析完毕。\n");  
}  
  
//************数字处理****************  
void IsNumber()  
{  
    int k=0;  
    while(((ch>='0')&&(ch<='9')))  
    {  
        CurrentToken.name[k++]=ch;//将数字放入单词缓冲区  
        ch=fgetc(SourceFin);  
  
    }  
    CurrentToken.code=18;//数字的机内码是18  
    OutPut();  
}  
//************是否为关键字****************  
void IsAlpha()  
{  
    int i , h;  
    h=0;  
    i=0;  
    while(((ch>='a')&&(ch<='z'))||((ch>='A')&&(ch<='Z'))||ch=='_')//将完整的单词放入单词缓冲区  
    {  
        CurrentToken.name[i++]=ch;  
        ch=fgetc(SourceFin);  
    }  
    zancun=CurrentToken;  
    for(i=0; i<14; i++) //将单词缓冲区中的词和关键字数组中的词比较，看是不是关键字  
    {  
        for(int j=0; j<30; j++)  
        {  
            if(CurrentToken.name[j]==key[i].name[j])  
                h=0;  
            else  
            {  
                h=1;  
                break;  
            }  
  
        }  
        if(h==0)  
            break;  
    }  
    if(h==0)  
    {  
        CurrentToken.code=key[i].code;//将第i个关键字的机内码给单词缓冲区中现有单词的机内码  
        CurrentToken.addr=-1;//关键字地址为-1  
        OutPut();  
    }  
    else  
    {  
        CurrentToken.code=17;  
        CurrentToken.addr=addr_count++; //如果不是关键字就是普通标识符，地址加１  
        OutPut();  
    }  
}  
//**************处理注释*****************  
void IsAnotation()  
{  
    char ch1;  
    ch1=ch;  
    ch=fgetc(SourceFin);  
    if(ch=='*')  
    {  
        for(;;)  
        {  
            ch=fgetc(SourceFin);  
            if(ch==EOF)  
            {  
                Error(2);    //到最后没有*说明注释不完全，有错误  
                break;  
            }  
            if(ch=='*')  
            {  
                ch1=ch;  
                ch=fgetc(SourceFin);  
                if(ch=='/')//如果最后有*/说明注释完整  
                {  
                    ch=fgetc(SourceFin);  
                    break;  
                }  
            }  
        }  
    }  
    else  
    {  
        error_count++;  
        Error(2);  
        CurrentToken.name[0]='/';//如果注释不完整，将第一个字母看成/  
        CurrentToken.code=22;  
        CurrentToken.addr=-1;//符号的地址是-1  
        OutPut();  
    }  
}  
  
//***************其它 ************  
void IsOther()  
{  
    char ch1;  
    int i;  
    for(i=0; i<30; i++)  
    {  
        CurrentToken.name[i]='\0';    //将缓冲区初始化  
    }  
    switch(ch)  
    {  
    case'+':  
    {  
        ch1=fgetc(SourceFin);  
        if(ch1=='=')  
        {  
            CurrentToken.name[0]='+';  
            CurrentToken.name[1]='=';  
            CurrentToken.code=38;  
            CurrentToken.addr=-1;  
            OutPut();  
            ch=fgetc(SourceFin);  
            break;  
        }  
        else  
        {  
            CurrentToken.name[0]='+';  
            CurrentToken.code=19;  
            CurrentToken.addr=-1;  
            OutPut();  
            ch=fgetc(SourceFin);  
            break;  
        }  
    }  
    case'-':  
    {  
        ch1=fgetc(SourceFin);  
        if(ch1=='=')  
        {  
            CurrentToken.name[0]='-';  
            CurrentToken.name[1]='=';  
            CurrentToken.code=39;  
            CurrentToken.addr=-1;  
            OutPut();  
            ch=fgetc(SourceFin);  
            break;  
        }  
        else  
        {  
            CurrentToken.name[0]='-';  
            CurrentToken.code=20;  
            CurrentToken.addr=-1;  
            OutPut();  
            ch=fgetc(SourceFin);  
            break;  
        }  
    }  
    case'*':  
    {  
        ch1=fgetc(SourceFin);  
        if(ch1=='=')  
        {  
            CurrentToken.name[0]='*';  
            CurrentToken.name[1]='=';  
            CurrentToken.code=40;  
            CurrentToken.addr=-1;  
            OutPut();  
            ch=fgetc(SourceFin);  
            break;  
        }  
        else  
        {  
            CurrentToken.name[0]='*';  
            CurrentToken.code=21;  
            CurrentToken.addr=-1;  
            OutPut();  
            ch=fgetc(SourceFin);  
            break;  
        }  
    }  
    case'%':  
    {  
        if(ch1 == '=')  
        {  
            CurrentToken.name[0]='%';  
            CurrentToken.name[1]='=';  
            CurrentToken.code=41;  
            CurrentToken.addr=-1;  
            OutPut();  
            ch=fgetc(SourceFin);  
            break;  
        }  
        else  
        {  
            CurrentToken.name[0]='%';  
            CurrentToken.code=23;  
            CurrentToken.addr=-1;  
            OutPut();  
            ch=fgetc(SourceFin);  
            break;  
        }  
    }  
    case'(':  
    {  
        CurrentToken.name[0]='(';  
        CurrentToken.code=24;  
        CurrentToken.addr=-1;  
        OutPut();  
        ch=fgetc(SourceFin);  
        break;  
    }  
    case')':  
    {  
        CurrentToken.name[0]=')';  
        CurrentToken.code=25;  
        CurrentToken.addr=-1;  
        OutPut();  
        ch=fgetc(SourceFin);  
        break;  
    }  
    case'[':  
    {  
        CurrentToken.name[0]='[';  
        CurrentToken.code=26;  
        CurrentToken.addr=-1;  
        OutPut();  
        ch=fgetc(SourceFin);  
        break;  
    }  
    case']':  
    {  
        CurrentToken.name[0]=']';  
        CurrentToken.code=27;  
        CurrentToken.addr=-1;  
        OutPut();  
        ch=fgetc(SourceFin);  
        break;  
    }  
    case'<':  
    {  
        ch1=fgetc(SourceFin);  
        if(ch1=='=')  
        {  
            CurrentToken.name[0]='<';  
            CurrentToken.name[1]='=';  
            CurrentToken.code=31;  
            CurrentToken.addr=-1;  
            OutPut();  
            ch=fgetc(SourceFin);  
            break;  
        }  
        else  
        {  
            CurrentToken.name[0]='<';  
            CurrentToken.code=29;  
            CurrentToken.addr=-1;  
            OutPut();  
            ch=fgetc(SourceFin);  
            break;  
        }  
    }  
    case'>':  
    {  
        ch1=fgetc(SourceFin);  
        if(ch1=='=')  
        {  
            CurrentToken.name[0]='>';  
            CurrentToken.name[1]='=';  
            CurrentToken.code=32;  
            CurrentToken.addr=-1;  
            OutPut();  
            ch=fgetc(SourceFin);  
            break;  
        }  
        else  
        {  
            CurrentToken.name[0]='>';  
            CurrentToken.code=30;  
            CurrentToken.addr=-1;  
            OutPut();  
            ch=fgetc(SourceFin);  
            break;  
        }  
    }  
    case'=':  
    {  
        CurrentToken.name[0]='=';  
        CurrentToken.code=33;  
        CurrentToken.addr=-1;  
        OutPut();  
        ch=fgetc(SourceFin);  
        break;  
    }  
    case'!':  
    {  
        ch1=fgetc(SourceFin);  
        if(ch1=='=')  
        {  
            CurrentToken.name[0]='!';  
            CurrentToken.name[1]='=';  
            CurrentToken.code=34;  
            CurrentToken.addr=-1;  
            OutPut();  
            ch=fgetc(SourceFin);  
            break;  
        }  
        else  
        {  
            CurrentToken.name[0]='n';  
            CurrentToken.name[1]='o';  
            CurrentToken.name[2]='t';  
            CurrentToken.code=44;  
            CurrentToken.addr=-1;  
            OutPut();  
            ch=fgetc(SourceFin);  
            break;  
        }  
    }  
    case';':  
    {  
        CurrentToken.name[0]=';';  
        CurrentToken.code=35;  
        CurrentToken.addr=-1;  
        OutPut();  
        ch=fgetc(SourceFin);  
        break;  
    }  
    case'|':  
    {  
        ch1=fgetc(SourceFin);  
        if(ch1=='|')  
        {  
            CurrentToken.name[0]='O';  
            CurrentToken.name[1]='R';  
            CurrentToken.code=42;  
            CurrentToken.addr=-1;  
            OutPut();  
            ch=fgetc(SourceFin);  
            break;  
        }  
    }  
    case'&':  
    {  
        ch1=fgetc(SourceFin);  
        if(ch1=='&')  
        {  
            CurrentToken.name[0]='A';  
            CurrentToken.name[1]='N';  
            CurrentToken.name[2]='D';  
            CurrentToken.code=43;  
            CurrentToken.addr=-1;  
            OutPut();  
            ch=fgetc(SourceFin);  
            break;  
        }  
    }  
    case 10:                //    /n换行  
    {  
        LineOfPro++;  
        ch=fgetc(SourceFin);  
        break;  
    }  
    case 13:                 //   回车换行  
    {  
        LineOfPro++;  
        ch=fgetc(SourceFin);  
        break;  
    }  
    case' ':                 //     空格  
        CurrentToken.code=60;  
        ch=fgetc(SourceFin);  
        break;  
  
    default:  
        Error(1);  
        ch=fgetc(SourceFin);  
        break;  
    }  
}  
//************错误类型************  
void Error(int a)  
{  
    error_count++;  
    switch(a)  
    {  
    case 1:  
    {  
        printf("error %2d 非法字符 %3d行.\n",error_count,LineOfPro+1);  
        break;  
    }  
  
    case 2:  
    {  
        printf("error %2d 没有匹配的注释符 %3d行.\n",error_count,LineOfPro+1);  
        break;  
    }  
  
    }  
    return;  
}  
void OutPut()  
{  
    //输出形式  
    if(CurrentToken.code==17)         //标志符输出  
    {  
        CurrentSimble.number=CurrentToken.addr;  
        CurrentSimble.type=CurrentToken.code;  
        strcpy(CurrentSimble.name,CurrentToken.name);  
        Flag_WordHave();  
        fprintf(TokenFout,"<id,%d>",addr_count);  
        printf("<id,%d>",addr_count);  
    }  
    else if(CurrentToken.code==18)        //   数字输出  
    {  
        CurrentSimble.number=CurrentToken.addr;  
        CurrentSimble.type=CurrentToken.code;  
        strcpy(CurrentSimble.name,CurrentToken.name);  
        Flag_NumHave();  
        fprintf(TokenFout,"<num , %s>",CurrentToken.name);  
        printf("<num , %d>",num_count);  
  
    }  
    else if((CurrentToken.code>=1)&&(CurrentToken.code<=14)) //关键字的输出  
    {  
        fprintf(TokenFout,"<%s,_>",zancun.name);  
        printf("<%s ,_>",zancun.name);  
    }  
    else//符号的输出  
    {  
        fprintf(TokenFout,"<%s,_>",CurrentToken.name);  
        printf("<%s ,_>",CurrentToken.name);  
    }  
}  
//查添符号  
int Flag_WordHave()  
{  
    int flag,i=0;  
    //用缓冲符号表中的符号和符号数组中的比较  
    for(i=0; i<(var_count-1); i++)  
    {  
        flag=strcmp(CurrentSimble.name,SymbleList[i].name);  
        if(flag==0)  
        {  
            CurrentToken.addr=var_count;//如果存在，将符号数组的地址返回  
            return 0;  
        }  
    }  
    SymbleList[var_count-1].number=CurrentToken.addr;  
    SymbleList[var_count-1].type=CurrentToken.code;  
    strcpy(SymbleList[var_count-1].name,CurrentToken.name);//不存在写入  
    fprintf(SymbleFout,"<%s ,%3d >",SymbleList[var_count-1].name,var_count);//符号的输出模式  
//  printf("<%s ,%3d>",SymbleList[var_count-1].name,var_count);  
    var_count=var_count+1;  
    return 1;  
}  
  
//常量  
int Flag_NumHave()  
{  
    int flag,i=0;  
    //用缓冲常量表中的常量和常量数组中的比较  
    for(i=0; i<(num_count-1); i++)  
    {  
        flag=strcmp(CurrentSimble.name,NumList[i].name);  
        if(flag==0)  
        {  
            CurrentToken.addr=num_count;//如果存在，将符号数组的地址返回  
            return 0;  
        }  
    }  
    NumList[num_count-1].number=CurrentToken.addr;  
    NumList[num_count-1].type=CurrentToken.code;  
    strcpy(NumList[num_count-1].name,CurrentToken.name);//不存在写入  
    fprintf(NumFout,"<%s ,%3d >",NumList[num_count-1].name,num_count);//符号的输出模式  
    num_count=num_count+1;  
    return 1;  
}  