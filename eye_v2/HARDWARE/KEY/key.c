#include "key.h"
#include "delay.h" 
#include "sim800c.h"
#include "usart4.h"
#include "beep.h"

//������ʼ������
void KEY_Init(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOA
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5; //KEY0 KEY1 ��Ӧ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//����
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC4,5
	
	GPIO_ResetBits(GPIOC,GPIO_Pin_4 | GPIO_Pin_5);//GPIOC4,5���øߣ�����
 
} 
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY0����
//2��KEY1����
//3��KEY2���� 
//4��WKUP���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>WK_UP!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY0==1||KEY2==0||WK_UP==1))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY0==1)return 1;
//		else if(KEY1==1)return 2;
//		else if(KEY2==1)return 3;
		else if(WK_UP==1)return 4;
	}else if(KEY0==1&&KEY2==1)key_up=1; 	    
 	return 0;// �ް�������
}
extern u8 flag_msg;
extern u8 sim_ready;
u8 KEY_PRESS(void)
{
//	if((flag_msg==2)&&(sim_ready==0))		//��Ȱ�������
		if(flag_msg ==2)		//��Ȱ�������
		{
			USART4_GPS();
			sim800c_sms_send_test();
			SHAKE=1;
			delay_ms(300); 
			SHAKE=0;
		flag_msg=0;			
		}
		else if(flag_msg==1)			//��ʱ��������
		{
			Timer_get();	
			flag_msg=0;
		}
		
	return 0;
}



















