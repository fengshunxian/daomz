#include "sim800c.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"   	 
#include "key.h"	 	 	 	 	 	  	  	 
#include "malloc.h"
#include "string.h"    
#include "text.h"		
#include "usart3.h" 
#include "ff.h"
#include "timer.h"
#include "gps.h"
#include "beep.h"
 	
u8 Scan_Wtime = 0;//��������ɨ����Ҫ��ʱ��  
u8 BT_Scan_mode=0;//����ɨ���豸ģʽ��־

//usmart֧�ֲ���
//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������USART3_RX_STA;
//     1,����USART3_RX_STA;
void sim_at_response(u8 mode)
{
	if(USART3_RX_STA&0X8000)		          //���յ�һ��������
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
//		printf("%s",USART3_RX_BUF);	          //���͵�����
		if(mode)USART3_RX_STA=0;
	} 
}
//////////////////////////////////////////////////////////////////////////////////////////////////// 
//ATK-SIM800C �������(���Ų��ԡ����Ų��ԡ�GPRS���ԡ���������)���ô���
//SIM800C���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//����,�ڴ�Ӧ������λ��(str��λ��)
u8* sim800c_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART3_RX_STA&0X8000)  //���յ�һ��������
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//��SIM800C��������
//cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 sim800c_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while((USART3->SR&0X40)==0);//�ȴ���һ�����ݷ������  
		USART3->DR=(u32)cmd;
	}else u3_printf("%s\r\n",cmd);  //��������
	
	if(waittime==1100)              //11s����ش�������(���������õ�)
	{
		Scan_Wtime = 11;            //��Ҫ��ʱ��ʱ��
		TIM7_SetARR(10000-1);       //����1S��ʱ�ж�
	}
	if(ack&&waittime)		        //��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	        //�ȴ�����ʱ
		{ 
			if(BT_Scan_mode)        //����ɨ��ģʽ��
			{
				res=KEY_Scan(0);    //������һ��
				if(res==KEY1) return 2;
			}
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(sim800c_check_cmd(ack))
					break;//�õ���Ч���� 
				USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}
//����SIM800C�������ݣ���������ģʽ��ʹ�ã�
//request:�ڴ����������ַ���
//waittimg:�ȴ�ʱ��(��λ��10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 sim800c_wait_request(u8 *request ,u16 waittime)
{
	 u8 res = 1;
	 u8 key;
	 if(request && waittime)
	 {
	    while(--waittime)
		{   
		   key=KEY_Scan(0);
		   if(key==KEY1) return 2;//������һ��
		   delay_ms(10);
		   if(USART3_RX_STA &0x8000)//���յ��ڴ���Ӧ����
		   {
			   if(sim800c_check_cmd(request)) break;//�õ���Ч����
			   USART3_RX_STA=0;
		   }
		}
		if(waittime==0)res=0;
	 }
	 return res;
}

//��1���ַ�ת��Ϊ16��������
//chr:�ַ�,0~9/A~F/a~F
//����ֵ:chr��Ӧ��16������ֵ
u8 sim800c_chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}
//��1��16��������ת��Ϊ�ַ�
//hex:16��������,0~15;
//����ֵ:�ַ�
u8 sim800c_hex2chr(u8 hex)
{
	if(hex<=9)return hex+'0';
	if(hex>=10&&hex<=15)return (hex-10+'A'); 
	return '0';
}

u8** kbd_tbl;
u8* kbd_fn_tbl[2];

//���Զ��ŷ�������(70����[UCS2��ʱ��,1���ַ�/���ֶ���1����])

extern u8  phonenum[50][12];
extern char  gpsbuff[5][15];

//SIM800C�����Ų��� 
void sim800c_sms_send_test(void)
{

	u8 *p;
	u8 i;
	u8 smssendsta=0;		//���ŷ���״̬,0,�ȴ�����;1���ͳɹ� 
	while(sim800c_send_cmd("AT+CSCS=\"GSM\"","OK",200));
	while(sim800c_send_cmd("AT+CMGF=1","OK",300));

	sprintf((char*)p,"AT+CMGS=\"13361299920\"");
	while(smssendsta==0&&i<5)   //���Ͳ��ɹ� ���ѭ�����Է���5��
	{
		if(sim800c_send_cmd(p,">",400)==0)					//���Ͷ�������+�绰����
		{ 						
			u3_printf("Blind location %s N %s E",gpsbuff[0],gpsbuff[1]);		 						//���Ͷ������ݵ�GSMģ�� 
 			if(sim800c_send_cmd((u8*)0X1A,"+CMGS:",1000)==0)//���ͽ�����,�ȴ��������(��ȴ�10����,��Ϊ���ų��˵Ļ�,�ȴ�ʱ��᳤һЩ)
			smssendsta++;
		}
		i++;
	}
	USART3_RX_STA=0;

} 



////GSM��Ϣ��ʾ(�ź�����,��ص���,����ʱ��)
////����ֵ:0,����
////����,�������
u8 sim800c_gsminfo_show(u16 x,u16 y)
{
	u8 *p,*p1,*p2;
	u8 res=0;
//	POINT_COLOR=BLUE; 	
	USART3_RX_STA=0;
	if(sim800c_send_cmd("AT+CPIN?","OK",200))res|=1<<0;	//��ѯSIM���Ƿ���λ 
	USART3_RX_STA=0;  
	if(sim800c_send_cmd("AT+COPS?","OK",200)==0)		//��ѯ��Ӫ������
	{ 
		p1=(u8*)strstr((const char*)(USART3_RX_BUF),"\""); 
		if(p1)//����Ч����
		{
			p2=(u8*)strstr((const char*)(p1+1),"\"");
			p2[0]=0;//���������			
			sprintf((char*)p,"��Ӫ��:%s",p1+1);
		} 
		USART3_RX_STA=0;		
	}else res|=1<<1;
	if(sim800c_send_cmd("AT+CSQ","+CSQ:",200)==0)		//��ѯ�ź�����
	{ 
		p1=(u8*)strstr((const char*)(USART3_RX_BUF),":");
		p2=(u8*)strstr((const char*)(p1),",");
		p2[0]=0;//���������
		sprintf((char*)p,"�ź�����:%s",p1+2);
		USART3_RX_STA=0;		
	}else res|=1<<2;
	if(sim800c_send_cmd("AT+CBC","+CBC:",200)==0)		//��ѯ��ص���
	{ 
		p1=(u8*)strstr((const char*)(USART3_RX_BUF),",");
		p2=(u8*)strstr((const char*)(p1+1),",");
		p2[0]=0;p2[5]=0;//���������
		sprintf((char*)p,"��ص���:%s%%  %smV",p1+1,p2+1);
		USART3_RX_STA=0;		
	}else res|=1<<3; 
	if(sim800c_send_cmd("AT+CCLK?","+CCLK:",200)==0)	//��ѯ��ص���
	{ 
		p1=(u8*)strstr((const char*)(USART3_RX_BUF),"\"");
		p2=(u8*)strstr((const char*)(p1+1),":");
		p2[3]=0;//���������
		sprintf((char*)p,"����ʱ��:%s",p1+1);
		USART3_RX_STA=0;		
	}else res|=1<<4;  
	return res;
}
//NTP����ͬ��ʱ��
void ntp_update(void)
{  
	 sim800c_send_cmd("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK",200);//���ó��س���1
	 sim800c_send_cmd("AT+SAPBR=3,1,\"APN\",\"CMNET\"","OK",200);
	 sim800c_send_cmd("AT+SAPBR=1,1",0,200);//����һ��GPRS������
   delay_ms(5);
   sim800c_send_cmd("AT+CNTPCID=1","OK",200);//����CNTPʹ�õ�CID
	 sim800c_send_cmd("AT+CNTP=\"202.120.2.101\",32","OK",200);//����NTP�������ͱ���ʱ��(32ʱ�� ʱ����׼ȷ)
   sim800c_send_cmd("AT+CNTP","+CNTP: 1",600);//ͬ������ʱ��

}
extern	u8 sim_ready;
//SIM800C�����Գ���
void sim800c_test(void)
{

	while(sim800c_send_cmd("AT","OK",200))//����Ƿ�Ӧ��ATָ�� 
	{
		delay_ms(400);
		sim_ready++;
		if(sim_ready==5) return;
	} 	
  sim_ready = 0;
	SHAKE = 1;
	delay_ms(300); 
	SHAKE =0;
	sim800c_send_cmd("ATE0","OK",200);//������
	ntp_update();//����ͬ��ʱ��	
}












