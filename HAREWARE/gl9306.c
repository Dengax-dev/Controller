#include "stm32f10x.h"
#include "sys.h"
#include "usart.h"
#include "gl9306.h"
	
u8 USART_RX_BUF[USART_REC_LEN];     //USART3接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记

Gl9306_Data Flow_Data = {0,0,0,0,0};

void FlowData_uart3_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
	//USART2_TX   GPIOA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.2
   
    // USART1_RX	初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.3  

    //Usart3 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
    //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART3, &USART_InitStructure); //初始化串2
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(USART3, ENABLE);                    //使能串口2
}

void USART3_IRQHandler(void)                	//串口3中断服务程序
{
	u8 Res;
	// extern int count;

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断
	{
		// printf("finsih\r\n");
		Res =USART_ReceiveData(USART3);	//读取接收到的数据
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
			USART_RX_STA++;

			if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收

			if(Res==0xAA){USART_RX_STA|=0x8000; //接收完成标志
			}
		}

		// if((USART_RX_STA&0x8000)!=0)//接收完成
		// {
		// 	printf("finsih\r\n");
		// 	printf("%c%c%c%c%c%c%c%c%c",USART_RX_BUF[0],USART_RX_BUF[1],USART_RX_BUF[2],USART_RX_BUF[3],USART_RX_BUF[4],USART_RX_BUF[5],USART_RX_BUF[6],USART_RX_BUF[7],USART_RX_BUF[8]);
		// 	USART_RX_STA = 0;
		// }
     } 
} 

//光流解码
void flow_decode(u8 *UartRxOpticalFlow)
{
	static uint8_t Check_sum = 0;
	if(UartRxOpticalFlow[0] == 0xfe)//校验头
	{
		Check_sum=(uint8_t)(UartRxOpticalFlow[2] + UartRxOpticalFlow[3] + UartRxOpticalFlow[4] + UartRxOpticalFlow[5]);
		if(Check_sum == UartRxOpticalFlow[6])//校验和正确
		{
			Flow_Data.speed_x = UartRxOpticalFlow[2] + (UartRxOpticalFlow[3] << 8);
			Flow_Data.speed_y = UartRxOpticalFlow[4] + (UartRxOpticalFlow[5] << 8);
			Flow_Data.quality = UartRxOpticalFlow[7];
		}
	}
}

//光流位移计算
void move_cal(uint16_t opticalflow_high)
{
	Flow_Data.move_x+=(Flow_Data.speed_x*opticalflow_high)/272.0f;//单位:mm   乘以高度单位mm
    Flow_Data.move_y+=(Flow_Data.speed_y*opticalflow_high)/272.0f;//单位:mm   乘以高度单位mm
}
