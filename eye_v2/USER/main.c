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
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 


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
	u8 fall_ret=0;
	short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);                               //��ʼ����ʱ����
	LED_Init();					                   //��ʼ��LED   
 	KEY_Init();					                   //������ʼ��  
	BEEP_Init();													 //��ʼ��������
	IIC_Init();														 //IIC��ʼ�� 
	MPU_Init();
	usart4_init(9600);		                 //��ʼ������4 	
	usart3_init(115200);		               //��ʼ������3 	
	EXTIX_Init();       									 //��ʼ���ⲿ�ж����� 
//	sim800c_test();                        //GSM����+��ȡ����
	while(1)
	{	
//		if(USART4_RX_STA&0X8000)		//GPS���յ�һ��������
//		{	
//			rxlen=USART4_RX_STA&0X7FFF;	//�õ����ݳ���
//			for(i=0;i<rxlen;i++)USART1_TX_BUF[i]=USART4_RX_BUF[i];
//			USART1_TX_BUF[i]=0;			//�Զ���ӽ�����	
//			if(strlen((char*)USART1_TX_BUF)>5) NMEA_GNGGA_Analysis(USART1_TX_BUF);  //����GGA�ַ�������ȡ��γ����Ϣ
//			USART4_RX_STA=0;		   	//������һ�ν���
//			}
//		if(timex%6==0)USART4_GPS();				//3s����һ��GPS��λ��Ϣ
	  KEY_PRESS();												//��⵽�������·��Ͷ�λ����
		fall_ret = Fall_down(aacx,aacy,aacz);
		if(fall_ret==0)avoidance_task1(safe_distance);		//�������һ��500ms
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















