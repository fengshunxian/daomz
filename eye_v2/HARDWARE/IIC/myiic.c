#include "myiic.h"
#include "delay.h"
#include "lcd.h"
#include "beep.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//IIC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//��ʼ��IIC
void IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOBʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOBʱ��

  //GPIOB8,B9��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	
	  //GPIOA8��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
 // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
		  //GPIOC9��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
 // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��
	
	IIC_SCL=1;		//MPU6050SCL
	IIC_SDA=1;			//MPU6050SDA
	IIC_SCL1=1;			//������SCL
	IIC_SDA1=1;			//������SDA
}
//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//����I2C���߽����ź�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA=1;delay_us(6);	   
	IIC_SCL=1;delay_us(6);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(10);
	IIC_SCL=1;
	delay_us(10);
	IIC_SCL=0;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(10);
	IIC_SCL=1;
	delay_us(10);
	IIC_SCL=0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(10);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL=1;
		delay_us(10); 
		IIC_SCL=0;	
		delay_us(10);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(10);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(5); 
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}
void KS102_WriteOneByte(u8 address,u8 reg,u8 command)
{				   	  	    																 
    IIC_Start();   
	IIC_Send_Byte(address);	    //����д����
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);//���͸ߵ�ַ	  
	IIC_Wait_Ack();	   
  IIC_Send_Byte(command);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
    IIC_Stop();//����һ��ֹͣ����  
}
u8 KS102_ReadOneByte(u8 address, u8 reg)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  
    IIC_Send_Byte(address);   //���͵͵�ַ
	IIC_Wait_Ack();	 
	IIC_Send_Byte(reg);   //���͵͵�ַ
	IIC_Wait_Ack();	   
	IIC_Start();  	 	   
	IIC_Send_Byte(address + 1);           //�������ģʽ			   
	IIC_Wait_Ack();	 

	delay_us(50);	   //���Ӵ˴���ͨ�ųɹ�������
    temp=IIC_Read_Byte(0);	  //���Ĵ���3	   
    IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}
extern u8  gpsbuff[5][15];
u16 avoidance_task(u16 num)
{
	u16 range=0;
//	u16 *rangebuff;
	//GPIO_SetBits(GPIOE,GPIO_Pin_8);//GPIOF9,F10���øߣ�����
		delay_ms(300);
		KS102_WriteOneByte1(0XE8,0X02,0XB0);
		delay_ms(100); 
		range = KS102_ReadOneByte1(0XE8, 0X02);
		range <<= 8;
		range += KS102_ReadOneByte1(0XE8, 0X03);
		Beep_warning(range); 
//	Date_Sorting(rangebuff,num-1);
	return 0;	
}


//����IIC��ʼ�ź�
void IIC_Start1(void)
{
	SDA_OUT1();     //sda�����
	IIC_SDA1=1;	  	  
	IIC_SCL1=1;
	delay_us(4);
 	IIC_SDA1=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL1=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
	delay_us(4);
}	  
//����IICֹͣ�ź�
void IIC_Stop1(void)
{
	SDA_OUT1();//sda�����
	IIC_SCL1=0;
	IIC_SDA1=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL1=1; 
	delay_us(4);
	IIC_SDA1=1;//����I2C���߽����ź�
								   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack1(void)
{
	u8 ucErrTime=0;
	SDA_IN1();      //SDA����Ϊ����  
	IIC_SDA1=1;delay_us(6);;	   
	IIC_SCL1=1;delay_us(6);;	 
	while(READ_SDA1)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop1();
			return 1;
		}
	}
	IIC_SCL1=0;//ʱ�����0 
	return 0;  
} 
//����ACKӦ��
void IIC_Ack1(void)
{
	IIC_SCL1=0;
	SDA_OUT1();
	IIC_SDA1=0;
	delay_us(10);
	IIC_SCL1=1;
	delay_us(10);
	IIC_SCL1=0;
}
//������ACKӦ��		    
void IIC_NAck1(void)
{
	IIC_SCL1=0;
	SDA_OUT1();
	IIC_SDA1=1;
	delay_us(10);
	IIC_SCL1=1;
	delay_us(10);
	IIC_SCL1=0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte1(u8 txd)
{                        
    u8 t;   
	SDA_OUT1(); 	    
    IIC_SCL1=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        IIC_SDA1=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(10);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL1=1;
		delay_us(10); 
		IIC_SCL1=0;	
		delay_us(10);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte1(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN1();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCL1=0; 
        delay_us(10);
		IIC_SCL1=1;
        receive<<=1;
        if(READ_SDA1)receive++;   
		delay_us(5); 
    }					 
    if (!ack)
        IIC_NAck1();//����nACK
    else
        IIC_Ack1(); //����ACK   
    return receive;
}
void KS102_WriteOneByte1(u8 address,u8 reg,u8 command)
{				   	  	    																 
		IIC_Start1();  
	IIC_Send_Byte1(address);	    //����д����
	IIC_Wait_Ack1();
	IIC_Send_Byte1(reg);//���͸ߵ�ַ	  
	IIC_Wait_Ack1();	   
  IIC_Send_Byte1(command);   //���͵͵�ַ
	IIC_Wait_Ack1(); 	 										  		   
    IIC_Stop1();//����һ��ֹͣ����  
}
u8 KS102_ReadOneByte1(u8 address, u8 reg)
{				  
	u8 temp=0;		  	    																 
    IIC_Start1();  
    IIC_Send_Byte1(address);   //���͵͵�ַ
	IIC_Wait_Ack1();	 
	IIC_Send_Byte1(reg);   //���͵͵�ַ
	IIC_Wait_Ack1();	   
	IIC_Start1();  	 	   
	IIC_Send_Byte1(address + 1);           //�������ģʽ			   
	IIC_Wait_Ack1();	 

	delay_us(50);	   //���Ӵ˴���ͨ�ųɹ�������
    temp=IIC_Read_Byte1(0);	  //���Ĵ���3	   
    IIC_Stop1();//����һ��ֹͣ����	    
	return temp;
}
extern u8  gpsbuff[5][15];
u16 avoidance_task1(u16 num)
{
	u16 range=0;
//	u16 *rangebuff;
	//GPIO_SetBits(GPIOE,GPIO_Pin_8);//GPIOF9,F10���øߣ�����
//		delay_ms(300);
		KS102_WriteOneByte1(0XE8,0X02,0XB0);
		delay_ms(100); 
		range = KS102_ReadOneByte1(0XE8, 0X02);
		range <<= 8;
		range += KS102_ReadOneByte1(0XE8, 0X03);
		Beep_warning(range); 
//	Date_Sorting(rangebuff,num-1);
	return 0;	
}


























