#include "myiic.h"
#include "delay.h"
#include "lcd.h"
#include "beep.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//IIC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//初始化IIC
void IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOB时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOB时钟

  //GPIOB8,B9初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
	  //GPIOA8初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
 // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
		  //GPIOC9初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
 // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化
	
	IIC_SCL=1;		//MPU6050SCL
	IIC_SDA=1;			//MPU6050SDA
	IIC_SCL1=1;			//超声波SCL
	IIC_SDA1=1;			//超声波SDA
}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
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
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
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
//不产生ACK应答		    
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
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(10);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(10); 
		IIC_SCL=0;	
		delay_us(10);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
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
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}
void KS102_WriteOneByte(u8 address,u8 reg,u8 command)
{				   	  	    																 
    IIC_Start();   
	IIC_Send_Byte(address);	    //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);//发送高地址	  
	IIC_Wait_Ack();	   
  IIC_Send_Byte(command);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
    IIC_Stop();//产生一个停止条件  
}
u8 KS102_ReadOneByte(u8 address, u8 reg)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  
    IIC_Send_Byte(address);   //发送低地址
	IIC_Wait_Ack();	 
	IIC_Send_Byte(reg);   //发送低地址
	IIC_Wait_Ack();	   
	IIC_Start();  	 	   
	IIC_Send_Byte(address + 1);           //进入接收模式			   
	IIC_Wait_Ack();	 

	delay_us(50);	   //增加此代码通信成功！！！
    temp=IIC_Read_Byte(0);	  //读寄存器3	   
    IIC_Stop();//产生一个停止条件	    
	return temp;
}
extern u8  gpsbuff[5][15];
u16 avoidance_task(u16 num)
{
	u16 range=0;
//	u16 *rangebuff;
	//GPIO_SetBits(GPIOE,GPIO_Pin_8);//GPIOF9,F10设置高，灯灭
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


//产生IIC起始信号
void IIC_Start1(void)
{
	SDA_OUT1();     //sda线输出
	IIC_SDA1=1;	  	  
	IIC_SCL1=1;
	delay_us(4);
 	IIC_SDA1=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL1=0;//钳住I2C总线，准备发送或接收数据 
	delay_us(4);
}	  
//产生IIC停止信号
void IIC_Stop1(void)
{
	SDA_OUT1();//sda线输出
	IIC_SCL1=0;
	IIC_SDA1=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL1=1; 
	delay_us(4);
	IIC_SDA1=1;//发送I2C总线结束信号
								   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack1(void)
{
	u8 ucErrTime=0;
	SDA_IN1();      //SDA设置为输入  
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
	IIC_SCL1=0;//时钟输出0 
	return 0;  
} 
//产生ACK应答
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
//不产生ACK应答		    
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
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte1(u8 txd)
{                        
    u8 t;   
	SDA_OUT1(); 	    
    IIC_SCL1=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA1=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(10);   //对TEA5767这三个延时都是必须的
		IIC_SCL1=1;
		delay_us(10); 
		IIC_SCL1=0;	
		delay_us(10);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte1(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN1();//SDA设置为输入
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
        IIC_NAck1();//发送nACK
    else
        IIC_Ack1(); //发送ACK   
    return receive;
}
void KS102_WriteOneByte1(u8 address,u8 reg,u8 command)
{				   	  	    																 
		IIC_Start1();  
	IIC_Send_Byte1(address);	    //发送写命令
	IIC_Wait_Ack1();
	IIC_Send_Byte1(reg);//发送高地址	  
	IIC_Wait_Ack1();	   
  IIC_Send_Byte1(command);   //发送低地址
	IIC_Wait_Ack1(); 	 										  		   
    IIC_Stop1();//产生一个停止条件  
}
u8 KS102_ReadOneByte1(u8 address, u8 reg)
{				  
	u8 temp=0;		  	    																 
    IIC_Start1();  
    IIC_Send_Byte1(address);   //发送低地址
	IIC_Wait_Ack1();	 
	IIC_Send_Byte1(reg);   //发送低地址
	IIC_Wait_Ack1();	   
	IIC_Start1();  	 	   
	IIC_Send_Byte1(address + 1);           //进入接收模式			   
	IIC_Wait_Ack1();	 

	delay_us(50);	   //增加此代码通信成功！！！
    temp=IIC_Read_Byte1(0);	  //读寄存器3	   
    IIC_Stop1();//产生一个停止条件	    
	return temp;
}
extern u8  gpsbuff[5][15];
u16 avoidance_task1(u16 num)
{
	u16 range=0;
//	u16 *rangebuff;
	//GPIO_SetBits(GPIOE,GPIO_Pin_8);//GPIOF9,F10设置高，灯灭
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


























