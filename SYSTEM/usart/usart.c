#include "sys.h"
#include "usart.h"
#include "string.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

#if EN_USART1_RX   //如果使能了接收

void uart1_init(u32 bound){
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 

}

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART1_RX_BUF[USART1_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART1_RX_STA=0;       //接收状态标记

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	static uint8_t i=0,rebuf[20]={0};
	// static uint16_t sum, distance, RangeStatus;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//判断接收标志
	{
		rebuf[i++]=USART_ReceiveData(USART1);//读取串口数据，同时清接收标志

		if (rebuf[0]!=0x5a) i=0;//帧头不对
		if ((i==2)&&(rebuf[1]!=0x5a)) i=0;//帧头不对
	
		if(i>3)//i等于4时，已经接收到数据量字节rebuf[3]
		{
			if(i!=(rebuf[3]+5)) return;//判断是否接收一帧数据完毕
			switch(rebuf[2])//接收完毕后处理
			{
				case 0x15:
					if(!USART1_RX_STA)//当数据处理完成后才接收新的数据
					{
						memcpy(USART1_RX_BUF,rebuf,8);//拷贝保存接收到的数据
						USART1_RX_STA=1;//接收完成标志

						// for(sum=0,i=0;i<(USART1_RX_BUF[3]+4);i++) sum+=USART1_RX_BUF[i];//rgb_data[3]=3

						// if(sum==USART1_RX_BUF[i])//校验和判断
						// {
						// 	distance=USART1_RX_BUF[4]<<8|USART1_RX_BUF[5];
						// 	RangeStatus=(USART1_RX_BUF[6]>>4)&0x0f; //0:距离值可靠
						// 	// Time=(USART1_RX_BUF[6]>>2)&0x03;
						// 	// Mode=USART1_RX_BUF[6]&0x03;
						// 	// send_3out(&USART1_RX_BUF[4],3,0x15);//上传给上位机
						// }
						// if(RangeStatus == 0) printf("distance:%d\r\n",distance);

						// USART1_RX_STA = 0;//处理数据完毕标志
						// USART1_RX_STA = 0; //清除标志
					}
					break;
			}
			i=0;//缓存清0
		}
	}
	// u8 Res;
	// if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
	// 	Res =USART_ReceiveData(USART1);	//读取接收到的数据
	// 	if((USART1_RX_STA&0x8000)==0)//接收未完成
	// 	{
	// 		if(USART1_RX_STA&0x4000)//接收到了0x0d
	// 			{
	// 			if(Res!=0x0a)USART1_RX_STA=0;//接收错误,重新开始
	// 			else USART1_RX_STA|=0x8000;	//接收完成了 
	// 			}
	// 		else //还没收到0X0D
	// 			{	
	// 			if(Res==0x0d)USART1_RX_STA|=0x4000;
	// 			else
	// 				{
	// 				USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
	// 				USART1_RX_STA++;
	// 				if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;//接收数据错误,重新开始接收	  
	// 				}		 
	// 			}
	// 	}
	// }
} 

// void USART1_IRQHandler(void)                	//串口1中断服务程序
// {
// 	u8 Res;
// 	extern int count;
// #if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
// 	OSIntEnter();    
// #endif
// 	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
// 	{
// 		Res =USART_ReceiveData(USART1);	//读取接收到的数据
// 		if((USART_RX_STA&0x8000)==0)//接收未完成
// 		{
// 			USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
// 			USART_RX_STA++;

// 			if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收

// 			if(Res==0xAA){USART_RX_STA|=0x8000; //接收完成标志
// 			}

// 		}

// 		// if((USART_RX_STA&0x8000)!=0)//接收完成
// 		// {
// 		// 	// printf("finsih\r\n");
// 		// 	// printf("%c%c%c%c%c%c%c%c%c",USART_RX_BUF[0],USART_RX_BUF[1],USART_RX_BUF[2],USART_RX_BUF[3],USART_RX_BUF[4],USART_RX_BUF[5],USART_RX_BUF[6],USART_RX_BUF[7],USART_RX_BUF[8]);
// 		// 	USART_RX_STA = 0;
// 		// }
//      } 
// #if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
// 	OSIntExit();  											 
// #endif
// } 

#endif	
