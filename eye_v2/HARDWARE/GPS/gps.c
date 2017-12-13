#include "gps.h" 
#include "led.h" 
#include "delay.h" 								   
#include "usart3.h" 								   
#include "stdio.h"	 
#include "stdarg.h"	 
#include "string.h"	 
#include "math.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
//#include "includes.h"

extern char  gpsbuff[5][15];
//分析GNGGA信息
//gpsx:nmea信息结构体
//buf:接收到的GPS数据缓冲区首地址
u8 NMEA_GNGGA_Analysis(u8 *buf)
{			  
	u8 five=buf[5];
	double rev1=0;
	double rev2=0;
	if(five =='A')
	{
		rev1=get_double_number1(&buf[Getcomma(2,buf)]);
		rev2=get_double_number2(&buf[Getcomma(4,buf)]);
		return 0;
	}
	else return 1;
}


double get_double_number1(u8 * buf)
{
	char  buff[32]="0";
	int i;
	u8 num=0;
	double rev1=0;
	i=Getcomma(1,buf);
	strncpy((char*)buff,(char*)buf,i-1);
	buff[i-1]=0;
	//strcpy(gpsbuff[2],buff);
	num=strlen(buff);
	if(num<32&&num>3) data_change1(buff);
	return rev1;
}

double get_double_number2(u8 * buf)
{
	char  buff[32]="0";
	int i;
	u8 num=0;
	double rev2;
	i=Getcomma(1,buf);
	strncpy((char*)buff,(char*)buf,i-1);
	buff[i-1]=0;
	//strcpy(gpsbuff[3],buff);
	num=strlen(buff);
	if(num<32&&num>3) data_change2(buff);
	return rev2;
}
int Getcomma(int num,u8 * buf)
{
	int i,j=0;
	int len=strlen((char *)buf);
	for(i=0;i<len;i++)
	{
		if(buf[i]==',')
			j++;
		if(j==num)
			return i+1;
	}
	return 0;
}
  

void data_change1(char * bufn)
{
    int xlen1,xlen2,xlen3;
		
    double num;
    char * p=NULL;
    char *p3=NULL;
    char p1[32]="0";
    char p2[32]="0";
	
    p=strstr(bufn,".");
    xlen1 = strlen(bufn);
    xlen2 = strlen(p);
    xlen2 =xlen1-xlen2;
    strncpy(p1,bufn,xlen2-2);
    p1[xlen2-2]=0;

    strcpy(p,p+1);

    num = (double)atoi(p-2);
    num=num/60;

    sprintf(p2,"%lf",num);

    p3 =strstr(p2,".");
    xlen3=strlen(p2)-strlen(p3);
    strcpy(p2+xlen3,p3+1);

    strcat(p1,".");
    strcat(p1,p2);
    p1[strlen(p1)-1]=0;
		strcpy(gpsbuff[0],p1);
		//gpsbuff[0][13]=0;

}

void data_change2(char * bufn)
{
    int xlen1,xlen2,xlen3;
		
    double num;
    char * p=NULL;
    char *p3=NULL;
    char p1[32]="0";
    char p2[32]="0";
	
    p=strstr(bufn,".");
    xlen1 = strlen(bufn);
    xlen2 = strlen(p);
    xlen2 =xlen1-xlen2;
    strncpy(p1,bufn,xlen2-2);
    p1[xlen2-2]=0;

    strcpy(p,p+1);

    num = (double)atoi(p-2);
    num=num/60;

    sprintf(p2,"%lf",num);

    p3 =strstr(p2,".");
    xlen3=strlen(p2)-strlen(p3);
    strcpy(p2+xlen3,p3+1);

    strcat(p1,".");
    strcat(p1,p2);
   p1[strlen(p1)-1]=0;;
		strcpy(gpsbuff[1],p1);	
	//	gpsbuff[1][13]=0;	
}


