#include "sim800c.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"   	 
#include "key.h"	 	 	 	 	 	  	  	 
#include "malloc.h"
#include "string.h"    
#include "text.h"		
#include "usart3.h" 
#include "ff.h"
#include "timer.h"
#include "gps.h"
#include "beep.h"
 	
u8 Scan_Wtime = 0;//保存蓝牙扫描需要的时间  
u8 BT_Scan_mode=0;//蓝牙扫描设备模式标志

//usmart支持部分
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART3_RX_STA;
//     1,清零USART3_RX_STA;
void sim_at_response(u8 mode)
{
	if(USART3_RX_STA&0X8000)		          //接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
//		printf("%s",USART3_RX_BUF);	          //发送到串口
		if(mode)USART3_RX_STA=0;
	} 
}
//////////////////////////////////////////////////////////////////////////////////////////////////// 
//ATK-SIM800C 各项测试(拨号测试、短信测试、GPRS测试、蓝牙测试)共用代码
//SIM800C发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//其他,期待应答结果的位置(str的位置)
u8* sim800c_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART3_RX_STA&0X8000)  //接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//向SIM800C发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 sim800c_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while((USART3->SR&0X40)==0);//等待上一次数据发送完成  
		USART3->DR=(u32)cmd;
	}else u3_printf("%s\r\n",cmd);  //发送命令
	
	if(waittime==1100)              //11s后读回串口数据(蓝牙测试用到)
	{
		Scan_Wtime = 11;            //需要定时的时间
		TIM7_SetARR(10000-1);       //产生1S定时中断
	}
	if(ack&&waittime)		        //需要等待应答
	{
		while(--waittime)	        //等待倒计时
		{ 
			if(BT_Scan_mode)        //蓝牙扫描模式下
			{
				res=KEY_Scan(0);    //返回上一级
				if(res==KEY1) return 2;
			}
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(sim800c_check_cmd(ack))
					break;//得到有效数据 
				USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}
//接收SIM800C返回数据（蓝牙测试模式下使用）
//request:期待接收命令字符串
//waittimg:等待时间(单位：10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 sim800c_wait_request(u8 *request ,u16 waittime)
{
	 u8 res = 1;
	 u8 key;
	 if(request && waittime)
	 {
	    while(--waittime)
		{   
		   key=KEY_Scan(0);
		   if(key==KEY1) return 2;//返回上一级
		   delay_ms(10);
		   if(USART3_RX_STA &0x8000)//接收到期待的应答结果
		   {
			   if(sim800c_check_cmd(request)) break;//得到有效数据
			   USART3_RX_STA=0;
		   }
		}
		if(waittime==0)res=0;
	 }
	 return res;
}

//将1个字符转换为16进制数字
//chr:字符,0~9/A~F/a~F
//返回值:chr对应的16进制数值
u8 sim800c_chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}
//将1个16进制数字转换为字符
//hex:16进制数字,0~15;
//返回值:字符
u8 sim800c_hex2chr(u8 hex)
{
	if(hex<=9)return hex+'0';
	if(hex>=10&&hex<=15)return (hex-10+'A'); 
	return '0';
}

u8** kbd_tbl;
u8* kbd_fn_tbl[2];

//测试短信发送内容(70个字[UCS2的时候,1个字符/数字都算1个字])

extern u8  phonenum[50][12];
extern char  gpsbuff[5][15];

//SIM800C发短信测试 
void sim800c_sms_send_test(void)
{

	u8 *p;
	u8 i;
	u8 smssendsta=0;		//短信发送状态,0,等待发送;1发送成功 
	while(sim800c_send_cmd("AT+CSCS=\"GSM\"","OK",200));
	while(sim800c_send_cmd("AT+CMGF=1","OK",300));

	sprintf((char*)p,"AT+CMGS=\"13361299920\"");
	while(smssendsta==0&&i<5)   //发送不成功 最多循环尝试发送5次
	{
		if(sim800c_send_cmd(p,">",400)==0)					//发送短信命令+电话号码
		{ 						
			u3_printf("Blind location %s N %s E",gpsbuff[0],gpsbuff[1]);		 						//发送短信内容到GSM模块 
 			if(sim800c_send_cmd((u8*)0X1A,"+CMGS:",1000)==0)//发送结束符,等待发送完成(最长等待10秒钟,因为短信长了的话,等待时间会长一些)
			smssendsta++;
		}
		i++;
	}
	USART3_RX_STA=0;

} 



////GSM信息显示(信号质量,电池电量,日期时间)
////返回值:0,正常
////其他,错误代码
u8 sim800c_gsminfo_show(u16 x,u16 y)
{
	u8 *p,*p1,*p2;
	u8 res=0;
//	POINT_COLOR=BLUE; 	
	USART3_RX_STA=0;
	if(sim800c_send_cmd("AT+CPIN?","OK",200))res|=1<<0;	//查询SIM卡是否在位 
	USART3_RX_STA=0;  
	if(sim800c_send_cmd("AT+COPS?","OK",200)==0)		//查询运营商名字
	{ 
		p1=(u8*)strstr((const char*)(USART3_RX_BUF),"\""); 
		if(p1)//有有效数据
		{
			p2=(u8*)strstr((const char*)(p1+1),"\"");
			p2[0]=0;//加入结束符			
			sprintf((char*)p,"运营商:%s",p1+1);
		} 
		USART3_RX_STA=0;		
	}else res|=1<<1;
	if(sim800c_send_cmd("AT+CSQ","+CSQ:",200)==0)		//查询信号质量
	{ 
		p1=(u8*)strstr((const char*)(USART3_RX_BUF),":");
		p2=(u8*)strstr((const char*)(p1),",");
		p2[0]=0;//加入结束符
		sprintf((char*)p,"信号质量:%s",p1+2);
		USART3_RX_STA=0;		
	}else res|=1<<2;
	if(sim800c_send_cmd("AT+CBC","+CBC:",200)==0)		//查询电池电量
	{ 
		p1=(u8*)strstr((const char*)(USART3_RX_BUF),",");
		p2=(u8*)strstr((const char*)(p1+1),",");
		p2[0]=0;p2[5]=0;//加入结束符
		sprintf((char*)p,"电池电量:%s%%  %smV",p1+1,p2+1);
		USART3_RX_STA=0;		
	}else res|=1<<3; 
	if(sim800c_send_cmd("AT+CCLK?","+CCLK:",200)==0)	//查询电池电量
	{ 
		p1=(u8*)strstr((const char*)(USART3_RX_BUF),"\"");
		p2=(u8*)strstr((const char*)(p1+1),":");
		p2[3]=0;//加入结束符
		sprintf((char*)p,"日期时间:%s",p1+1);
		USART3_RX_STA=0;		
	}else res|=1<<4;  
	return res;
}
//NTP网络同步时间
void ntp_update(void)
{  
	 sim800c_send_cmd("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK",200);//配置承载场景1
	 sim800c_send_cmd("AT+SAPBR=3,1,\"APN\",\"CMNET\"","OK",200);
	 sim800c_send_cmd("AT+SAPBR=1,1",0,200);//激活一个GPRS上下文
   delay_ms(5);
   sim800c_send_cmd("AT+CNTPCID=1","OK",200);//设置CNTP使用的CID
	 sim800c_send_cmd("AT+CNTP=\"202.120.2.101\",32","OK",200);//设置NTP服务器和本地时区(32时区 时间最准确)
   sim800c_send_cmd("AT+CNTP","+CNTP: 1",600);//同步网络时间

}
extern	u8 sim_ready;
//SIM800C主测试程序
void sim800c_test(void)
{

	while(sim800c_send_cmd("AT","OK",200))//检测是否应答AT指令 
	{
		delay_ms(400);
		sim_ready++;
		if(sim_ready==5) return;
	} 	
  sim_ready = 0;
	SHAKE = 1;
	delay_ms(300); 
	SHAKE =0;
	sim800c_send_cmd("ATE0","OK",200);//不回显
	ntp_update();//网络同步时间	
}












