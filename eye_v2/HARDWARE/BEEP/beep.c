#include "beep.h" 
#include "delay.h"
#include "led.h"
#include "usart3.h"
#include "sim800c.h"


//初始化PF8为输出口		    
//BEEP IO初始化
void BEEP_Init(void)
{   
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOA/GPIOB时钟
  
  //初始化蜂鸣器对应引脚GPIOA6和马达GPIOA7
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
	
	  //初始化报时
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//下拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
		
  BEEP=0;
	SHAKE=0;
	TIMEK=1;
}
extern u16 safe_distance;
void Beep_warning(u16 temp)
{	
	if(temp <safe_distance&temp>0)
	{
		BEEP=1;
		delay_ms(100);
	}
	BEEP=0;
}

void Timer_get(void)
{
	u8 hour,hour1,min,min1;
	char p1[32],p2[32];
	char *p;
	if(sim800c_send_cmd("AT+CCLK?","+CCLK",200)==0)
		p=(char*)strstr((char*)(USART3_RX_BUF),",");
		//800c_gprs_test();//GPRS测试
		strncpy((char*)p1,(const char*)(p+1),2);
		strncpy((char *)p2,( char*)(p+1+3),2);
			p1[2]=0;
			p2[2]=0;
	hour=(p1[0]-'0');
	hour1=(p1[1]-'0');
	min=(p2[0]-'0');
	min1=(p2[1]-'0');
	if(hour!=0)
		{
		VOICE_TEST(0x0A);
			delay_ms(500);
			if(hour1>0)
				VOICE_TEST(hour1);
			delay_ms(500);
		VOICE_TEST(0x0C);
			delay_ms(500);
		}
	else if(hour==0)
	{
	VOICE_TEST(hour1);
		delay_ms(500);
	VOICE_TEST(0x0C);
		delay_ms(500);
	}
	if(min>0)
	{
			VOICE_TEST(min);
				delay_ms(500);
			VOICE_TEST(0x0A);
				delay_ms(500);
				if(min1>0)
				VOICE_TEST(min1);
				delay_ms(500);
			VOICE_TEST(0x11);
				delay_ms(500);
		}	
	else if(min==0)	
	{
		if(min1==0)
		{
		VOICE_TEST(0x15);
		delay_ms(500);
		}
		else{
			VOICE_TEST(0x00);
			delay_ms(500);
			VOICE_TEST(min1);
			delay_ms(500);
			VOICE_TEST(0x11);
		}
	}
		delay_ms(500);				

}


//num 为十六进制声音指令
void VOICE_TEST(u8 num)
{
		u8 i;
		TIMEK=0;
		delay_ms(5);
	for(i=0;i<8;i++)
		{
			
			TIMEK=1;
			if(num&0x01)
			{
				delay_ms(3);
				TIMEK=0;
				delay_ms(1);	
			}
			else{
			delay_ms(1);
			TIMEK=0;	
			delay_ms(3);
			}
			num>>=1;
		}
		TIMEK=1;
}

void Date_Sorting(u16 *databuff,u8 sum)
{
	int i,j;
	u16 temp =0;
	for ( i = 0; i < sum+1; i++)
  {
		for (j = i+1; j < sum+1; j++)
    {
			if (databuff[i] > databuff[j])
      {
			  temp = databuff[i];
        databuff[i] = databuff[j];
        databuff[j] = temp;
      }
			}
   }
}





