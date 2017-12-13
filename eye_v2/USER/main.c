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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);                               //��ʼ����ʱ����
	LED_Init();					                   //��ʼ��LED   
 	KEY_Init();					                   //������ʼ��  
	BEEP_Init();													 //��ʼ��������
	IIC_Init();														 //IIC��ʼ�� 
	usart4_init(9600);		                 //��ʼ������4 	
	usart3_init(115200);		               //��ʼ������3 	
	EXTIX_Init();       									 //��ʼ���ⲿ�ж����� 
	sim800c_test();                        //GSM����+��ȡ����
	while(1)
	{	
	  KEY_PRESS();												//��⵽�������·��Ͷ�λ����
		avoidance_task1(safe_distance);		//�������һ��500ms
		if(timex%10000==0)
			while(USART4_GPS())
			{
				i++;
				delay_ms(100);
				if(i==5)
					break;
			};
		delay_ms(200);
		if((timex%2)==0) LED0=!LED0;      //200ms��˸ 	
		timex++;	
	} 	
}















