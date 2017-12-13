#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "key.h"  
#include "usmart.h"		
#include "usart3.h"
#include "sim800c.h" 
#include "myiic.h" 
#include "beep.h" 
#include "gps.h" 
#include "usart4.h"
#include "exti.h"



char  phonenum[50][12];
char  gpsbuff[5][15];
u8 phonedel[50][12];
u8 msgnum=0;
u8 flag_msg=0;
u16 safe_distance=1500;
u8 sim_ready=0;
u8 msglive=0;
u8 msgdel=0;
u8 msgmax=0;

int main(void)
{ 	
  u16 timex=0;
	u8 i;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);                               //初始化延时函数
	LED_Init();					                   //初始化LED   
 	KEY_Init();					                   //按键初始化  
	BEEP_Init();													 //初始化蜂鸣器
	IIC_Init();														 //IIC初始化 
	usart4_init(9600);		                 //初始化串口4 	
	usart3_init(115200);		               //初始化串口3 	
	EXTIX_Init();       									 //初始化外部中断输入 
	sim800c_test();                        //GSM测试+读取短信
	while(1)
	{	
	  KEY_PRESS();												//检测到按键按下发送定位短信
		avoidance_task1(safe_distance);		//避障完成一次500ms
		if(timex%10000==0)
			while(USART4_GPS())
			{
				i++;
				delay_ms(100);
				if(i==5)
					break;
			};
		delay_ms(200);
		if((timex%2)==0) LED0=!LED0;      //200ms闪烁 	
		timex++;	
	} 	
}















