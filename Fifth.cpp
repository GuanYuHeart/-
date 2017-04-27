#include<iostream>               /* 基本输入输出流 */
#include<stack>                  /* 运用栈，省去自己再写栈 */
using namespace std;
/***************************************
*                 数据结构             *
*       逆波兰式==> 目标代码           *
***************************************/
/*********************************************
*   目标代码指令：LD，ST，ADD，SUB，MUL，DIV *
*   相应的数值  ：1，  2，  3，  4，  5，  6 *
*   数据段开始：设置为a-z；单个寄存器        *
* acc为寄存器标志：为0表示为空，非0，被占用*
*********************************************/
char temp='a'-1;       /* 临时变量a-z   */
stack<char> SEM;       /* 语义栈        */
int s;                 /* 栈指针        */
typedef struct
{
 int op;            /* 操作符对应的数值 */
 char rt;           /* 单个寄存器       */
 char num;          /* 操作数           */
}ObjType;
ObjType OB[40];        /* 目标代码区       */
int o_pt=0;            /* 区指针           */
int acc;               /* 寄存器标志       */
char blexp[40];        /* 逆波兰式区       */

/*************************************
*              代码区                *
*************************************/
/*************************************
*           函数声明                 *
*************************************/
int isop(char);      /* 判断操作符是否是+-/* */
void build(char);    /* 根据操作符生成目标代码函数 */
void B_O();    /* 生成算法 */
char* OpString(int); /* 操作符转化成字符显示 */
void display();   /* 显示目标代码 */

/*************************************
*    判断当前操作符是否是运算符      *
*       如果是返回相应的正数(3-6)    *
*       否则返回零                   *
*************************************/
int isop(char ch)
{
 if(ch=='+')
  return 3;
 else if(ch=='-')
  return 4;
 else if(ch=='*')
  return 5;
 else if(ch=='/')
  return 6;
 else
  return 0;
}
/*********************************************
*        目标代码生成表生成目标代码         *
*********************************************/
/***************************目标代码生成表****************************************************************
* 操作符W  SEM[s-1]即x1 SEM[s]即x2  acc   OBJ                   *
* +-/*    X1         X2        0     LD R,X1;W R,X2;        *
* +-/*    X1         X2       K!=0   T=NEW T;ST R,T;LD R,X1;W R,X2;*
* +-/*    R           X        s-1   W R,X;       *
* +*      X           R         s    W R,X;       *
* /-      X           R          s   T=NEW T;ST R,T;LD R,X;W R,T; *
*******************************************************************/
void build(char ch)
{
 char x1,x2;                   /* 从语义栈中弹出俩个操作数，用于判断与运算 */
 x2=SEM.top();                 /* SEM.top 记得后面要加(),否则没有值!!!     */
 cout<<x2<<endl;      /* 用于调试用的，看栈是怎样变化的 */
 SEM.pop();
 x1=SEM.top();
 cout<<x1<<endl;
 SEM.pop();
 if(acc==0)                    /* 如果寄存器没被占用，直接生成运算指令     */
 {
  OB[o_pt].op=1;            /* 装载SEM[S-1]即x1 到寄存器R               */
  OB[o_pt].rt='R';
  OB[o_pt].num=x1;
  o_pt++;
  OB[o_pt].op=isop(ch);     /* 生成对应的运算指令                       */
  OB[o_pt].rt='R';
  OB[o_pt].num=x2;
  o_pt++;
 }
 else if(x1!='R'&&x2!='R'&&acc!=0) /* x1和x2都不是寄存器的值，且寄存器被占用 */
 {                               
  temp++;                       /* 申请一个临时变量，在执行代码生成过程中，由操作系统生成，这个是模拟 */
  OB[o_pt].op=2;      /*  将寄存器的值移出到临时变量 */
  OB[o_pt].rt='R';          
  OB[o_pt].num=temp;
  SEM.pop();                    /* 将临时变量压入语义栈    */
  SEM.push(temp);
  o_pt++;
  OB[o_pt].op=1;                /* 装载操作数 */
  OB[o_pt].rt='R';
  OB[o_pt].num=x1;
  o_pt++;
  OB[o_pt].op=isop(ch);         /* 生成运算指令 */
  OB[o_pt].rt='R';
  OB[o_pt].num=x2;
  o_pt++;
 }
 else if(x1=='R'&&x2!='R'&&acc==(s-1))/* SEM[s-1]即x1为寄存器的值而x2是操作数且acc为s-1，说明寄存器现在的值为x1 */
 {
  OB[o_pt].op=isop(ch);            /* 直接生成运算指令，因为此时的寄存器中已经存在一个操作数 */
  OB[o_pt].rt='R';
  OB[o_pt].num=x2;
  o_pt++;
 }
 else if(x1!='R'&&x2=='R'&&acc==s&&(isop(ch)==3||isop(ch)==5))/* 寄存器的值在SEM[s]即x2，如果操作符为+*  */
 {
  OB[o_pt].op=isop(ch);                                    /* 直接生成运算指令 */
  OB[o_pt].rt='R';
  OB[o_pt].num=x1;
  o_pt++;
 }
 else if(x1!='R'&&x2=='R'&&acc==s&&(isop(ch)==4||isop(ch)==6))/* 如果是- /，则需要调换一下位置 */
 {
  temp++;              /* 生成临时变量，用于存储下寄存器值 */
  OB[o_pt].op=2;
  OB[o_pt].rt='R';           /* 不需要压栈，因为马上就要用到 */
  OB[o_pt].num=temp;
  o_pt++;
  OB[o_pt].op=1;            /* 装载x1如寄存器  */
  OB[o_pt].rt='R';
  OB[o_pt].num=x1;
  o_pt++;
  OB[o_pt].op=isop(ch);          /* 利用临时变量生成运算指令 */
  OB[o_pt].rt='R';
  OB[o_pt].num=temp;
  o_pt++;
 }
 else               /* 出错处理 */
 {
  cout<<"不符合目标代码生成条件，出错!"<<endl;
  exit(0);
 }
 SEM.push('R');  /* 将寄存器'R'压栈，用于下次判断，而不是寄存器的值 */
 s--;
 return; 
}
/******************************
*  生成算法     *
******************************/
void B_O()
{
 int i=0;    /* 置初始值s=0，acc=0，i=0 */
 char ch;    /* 当前处理字符 */
 s=0;     /* s 为栈顶指示 */
 acc=0;
 while(blexp[i]!='\0')   /* 逆波兰式区还没处理完 */
 {
  ch=blexp[i++];  /* 取当前处理字符  */
  if(isalnum(ch))  /* 是操作数，入栈 */
  {
   SEM.push(ch);
   s++;
  }
  else if(isop(ch)) /* 是操作符，生成指令 */
  {
   build(ch);
   acc=s;   /* acc标志到栈顶，因为R入栈了，指示R的位置用，是个技巧 */
  }
  else              /* 字符不符合要求，出错处理 */
  {
   cout<<"逆波兰式内字符有错!"<<endl;
   exit(0);
  }
 } 
}
/*****************************************
*    将操作指令的数值转换成相应的字符串  *
*****************************************/
char* OpString(int i)
{
 switch(i)
 {
 case 1:
  return "LD";
 case 2:
  return "ST";
 case 3:
  return "ADD";
 case 4:
  return "SUB";
 case 5:
  return "MUL";
 case 6:
  return "DIV";
 default:
  cout<<"操作符有错!"<<endl;
  exit(0);
 }
}
/*******************************
*        显示目标代码          *
*******************************/
void display()
{
 int i;
 cout<<"==============目标代码=========="<<endl;
 for(i=0;i<o_pt;i++)
 {
  cout<<OpString(OB[i].op)<<"\t"<<OB[i].rt<<","<<OB[i].num<<endl;
  
 }
}

int main()
{
 cout<<"逆波兰式生成目标代码"<<endl;
 cout<<"请输入逆波兰式："<<endl;
 gets(blexp);
 B_O();
 display();
 return 1;
}
