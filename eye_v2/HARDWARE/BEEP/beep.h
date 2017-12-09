#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//��������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

#define ZERO		0X00H
#define ONE 		0X01H
#define TWO 		0X02H
#define THREE 	0X03H
#define FOUR 		0X04H
#define FIVE 		0X05H
#define SIX 		0X06H
#define SEVEN 	0X07H
#define EIGHTE 	0X08H
#define NINE 		0X09H
#define TEN 		0X0AH
#define BAI 		0X0BH
#define DIAN 		0X0CH
#define NIAN 		0X0DH
#define YUE 		0X0EH
#define RI 			0X0FH
#define SHI 		0X10H
#define FEN 		0X11H
#define MIAO	 	0X12H
#define XINGQI 	0X13H
#define DU 			0X14H
#define ZHENG 	0X15H
#define SAHNGWU 0X16H
#define XIAWU 	0X17H
#define WANSHANG 0X18H
#define FU 			0X19H
#define DONG 		0X1AH
#define BEIJINGSHIJIAN 0X1BH
#define WENDU 	0X1CH
#define SHIDU 	0X1DH
#define QIAN 		0X2DH
#define YUAN 		0X3DH


//LED�˿ڶ���
#define BEEP PAout(6)	// ����������IO 
#define SHAKE PAout(7)	// ����������IO 
#define TIMEK PBout(15)	// ����������IO 



void BEEP_Init(void);//��ʼ��		
void Beep_warning(u16 temp);
void VOICE_TEST(u8 num);
void Date_Sorting(u16 *databuff,u8 sum);
void Timer_get(void);
#endif

















