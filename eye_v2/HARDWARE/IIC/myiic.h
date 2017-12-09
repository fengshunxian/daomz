#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h" 
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
   	   		   
//IO方向设置
#define SDA_IN()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	//PB9输入模式
#define SDA_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} //PB9输出模式

#define SDA_IN1()  {GPIOC->MODER&=~(3<<(9*2));GPIOC->MODER|=0<<9*2;}	//PB9输入模式
#define SDA_OUT1() {GPIOC->MODER&=~(3<<(9*2));GPIOC->MODER|=1<<9*2;} //PB9输出模式


//IO操作函数	 
#define IIC_SCL    PBout(8) //SCL
#define IIC_SDA    PBout(9) //SDA	 
#define READ_SDA   PBin(9)  //输入SDA 

#define IIC_SCL1    PAout(8) //SCL
#define IIC_SDA1    PCout(9) //SDA	 
#define READ_SDA1   PCin(9)  //输入SDA 



//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  
u8 KS102_ReadOneByte(u8 address, u8 reg);
void KS102_WriteOneByte(u8 address,u8 reg,u8 command);
u16 avoidance_task(u16 num);
	 
void IIC_Start1(void);				//发送IIC开始信号
void IIC_Stop1(void);	  			//发送IIC停止信号
void IIC_Send_Byte1(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte1(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack1(void); 				//IIC等待ACK信号
void IIC_Ack1(void);					//IIC发送ACK信号
void IIC_NAck1(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte1(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte1(u8 daddr,u8 addr);	  
u8 KS102_ReadOneByte1(u8 address, u8 reg);
void KS102_WriteOneByte1(u8 address,u8 reg,u8 command);
u16 avoidance_task1(u16 num);
#endif
















