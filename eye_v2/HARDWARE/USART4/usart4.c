#include "delay.h"
#include "usart4.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	  
#include "timer.h"
#include "gps.h"
 

//���ڷ��ͻ����� 	
__align(8) u8 USART4_TX_BUF[USART4_MAX_SEND_LEN]; 	//���ͻ���,���USART4_MAX_SEND_LEN�ֽ�
								//���ʹ���˽���   	  
//���ڽ��ջ����� 	
u8 USART4_RX_BUF[USART4_MAX_RECV_LEN]; 				//���ջ���,���USART4_MAX_RECV_LEN���ֽ�.


//����ɨ���£�   timer=1S
//������ɨ���£� timer=10ms
//ͨ���жϽ�������2���ַ�֮���ʱ������timer�������ǲ���һ������������.
//���2���ַ����ռ������timer,����Ϊ����1����������.Ҳ���ǳ���timerû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
u16 USART4_RX_STA=0;   	 
void UART4_IRQHandler(void)
{
	u8 res;	    
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//���յ�����
	{	 
 
	res =USART_ReceiveData(UART4);		
	if((USART4_RX_STA&(1<<15))==0)                  //�������һ������,��û�б�����,���ٽ�����������
	{ 
		if(USART4_RX_STA<USART4_MAX_RECV_LEN)		//�����Խ�������
		{
			TIM_SetCounter(TIM2,0);                 //���������        				 
			if(USART4_RX_STA==0)		
				TIM_Cmd(TIM2, ENABLE);              //ʹ�ܶ�ʱ��7 
			USART4_RX_BUF[USART4_RX_STA++]=res;		//��¼���յ���ֵ	 
		}else 
		{
			USART4_RX_STA|=1<<15;					//ǿ�Ʊ�ǽ������
		} 
	}  	
 }										 
}  

//��ʼ��IO ����4
//bound:������	  
void usart4_init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);      //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);     //ʹ��USART4ʱ��

 	USART_DeInit(UART4);                                     //��λ����3
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_UART4);  //GPIOA0����ΪUSART4
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_UART4);  //GPIOA1����ΪUSART4	
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;    //GPIOA0/A1��ʼ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;              //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	      //�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;            //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;              //����
	GPIO_Init(GPIOA,&GPIO_InitStructure);                     //��ʼ��
	
	USART_InitStructure.USART_BaudRate = bound;                                    //������ 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;                            //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx ;       	       //ֻ����ģʽ
  
	USART_Init(UART4, &USART_InitStructure);                                      //��ʼ������4
 
	USART_Cmd(UART4, ENABLE);                                                     //ʹ�ܴ��� 
	
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);            //�����ж�   
	
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	                        //����ָ���Ĳ�����ʼ��VIC�Ĵ���
	TIM2_Int_Init(100-1,8400-1);		                    //10ms�ж�
	USART4_RX_STA=0;		                                //����
	TIM_Cmd(TIM2, DISABLE);                                 //�رն�ʱ��2
  	

}

//����2,printf ����
//ȷ��һ�η������ݲ�����USART4_MAX_SEND_LEN�ֽ�
void u4_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART4_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART4_TX_BUF);                     //�˴η������ݵĳ���
	for(j=0;j<i;j++)                                          //ѭ����������
	{
	  while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET);//�ȴ��ϴδ������ 
		USART_SendData(UART4,(uint8_t)USART4_TX_BUF[j]); 	  //�������ݵ�����3 
	}
}
u8 USART1_TX_BUF[USART4_MAX_RECV_LEN];
u8 USART4_GPS(void)
{
	u16 rxlen,i,res=1;
	if(USART4_RX_STA&0X8000)		//GPS���յ�һ��������
		{	
			rxlen=USART4_RX_STA&0X7FFF;	//�õ����ݳ���
			for(i=0;i<rxlen;i++)USART1_TX_BUF[i]=USART4_RX_BUF[i];
			USART1_TX_BUF[i]=0;			//�Զ���ӽ�����	
			if(strlen((char*)USART1_TX_BUF)>5) res = NMEA_GNGGA_Analysis(USART1_TX_BUF);  //����GGA�ַ�������ȡ��γ����Ϣ
			USART4_RX_BUF[0]=0;
			USART4_RX_STA=0;		   	//������һ�ν���
		}
		return res;
}
 
 
 











