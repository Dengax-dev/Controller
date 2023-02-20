#include "stm32f10x.h"
#include "control.h"

u8 cmd2[]  = {0x66, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x99}; //定高模式
u8 cmd3[]  = {0x66, 0x80, 0x80, 0x00, 0x80, 0x00, 0x80, 0x99}; //极速下降
u8 cmd4[]  = {0x66, 0x80, 0x80, 0xff, 0x80, 0x00, 0x7f, 0x99}; //极速上升
u8 cmd5[]  = {0x66, 0x80, 0x80, 0x80, 0x00, 0x00, 0x80, 0x99}; //偏航角：左转
u8 cmd6[]  = {0x66, 0x80, 0x80, 0x80, 0xff, 0x00, 0x7f, 0x99}; //偏航角：右转
u8 cmd7[]  = {0x66, 0x80, 0x00, 0x80, 0x80, 0x00, 0x80, 0x99}; //俯仰角：后退
u8 cmd8[]  = {0x66, 0x80, 0xff, 0x80, 0x80, 0x00, 0x7f, 0x99}; //俯仰角：前进
u8 cmd9[]  = {0x66, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x99}; //横滚角：左侧飞
u8 cmd10[] = {0x66, 0xff, 0x80, 0x80, 0x80, 0x00, 0x7f, 0x99}; //横滚角：右侧飞
u8 cmd11[] = {0x66, 0x80, 0x80, 0x80, 0x80, 0x04, 0x04, 0x99}; //电机停转

void control_uart2_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	// NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
	//USART2_TX   GPIOA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2
   
    //USART1_RX	  GPIOA.3初始化
    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    // GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3  

    //Usart1 NVIC 配置
	// NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	// NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
    //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	// USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_InitStructure.USART_Mode = USART_Mode_Tx;

    USART_Init(USART2, &USART_InitStructure); //初始化串2
    // USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(USART2, ENABLE);                    //使能串口2
}

//发送命令
void Send_Cmd(u8 *cmd_array)
{
    u8 i;
    for(i = 0; i < 8; i++)
    {
        while(USART_GetFlagStatus(CONTROL_USART,USART_FLAG_TC)==RESET);
        USART_SendData(CONTROL_USART,*cmd_array++);
    }
}

//生成校验位
// u8 Cal_Cmd(u8 data1, u8 data2, u8 data3, u8 data4, u8 data5)
// {
//     u8 data6;
//     data6 = data1^data2^data3^data4^data5;

//     return data6;
// }

//生成控制数组
u8* mk_CmdArray_Dec(u8 data1, u8 data2, u8 data3, u8 data4)
{
    return mk_CmdArray_Hex(DEC_HEX(data1), DEC_HEX(data2), DEC_HEX(data3), DEC_HEX(data4));
}


u8* mk_CmdArray_Hex(u8 data1, u8 data2, u8 data3, u8 data4)
{
    static u8 array[8];
    u8 i;
    array[0] = 0x66; 
    array[1] = data1;
    array[2] = data2;
    array[3] = data3;
    array[4] = data4;
    array[5] = 0x00;
    array[6] = data1^data2^data3^data4^(0x00); //校验位
    array[7] = 0x99;

    for(i = 0; i < 8; i++)
    {
        while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
        USART_SendData(USART1, array[i]);
    }

    return array;
}

// 十进制to十六进制
int DEC_HEX(uint32_t Dec)
{ 
   int ram = 0;//整
    int ray = 0;//余
    uint32_t Hex = 0x0;
    int i =0;
do{   
    ram = Dec /16;
    ray = Dec %16;    
    Dec = ram;
    switch(ray)           //看余数为多少
    {
        case 0:
        {
            Hex |= (0x0<<(i*4));   //然后右移四位，让下一个四位在下一个循环进行运算
            break;
        }
        case 1:
        {
            Hex |= (0x1<<(i*4));
            break;
        }
        case 2:
        {
        Hex |= (0x2<<(i*4));
            break;
        }
        case 3:
        {
            Hex |= (0x3<<(i*4));
            break;
        }
        case 4:
        {
        Hex |= (0x4<<(i*4));
            break;
        }
        case 5:
        {
            Hex |= (0x5<<(i*4));
            break;
        }
        case 6:
        {
            Hex |= (0x6<<(i*4));
            break;
        }
        case 7:
        {
            Hex |= (0x7<<(i*4));
            break;
        }
        case 8:
        {
            Hex |= (0x8<<(i*4));
            break;
        }
        case 9:
        {
            Hex |= (0x9<<(i*4));
            break;
        }
        case 10:
        {
                Hex |= (0xA<<(i*4));
            break;
        }
        case 11:
        {
            Hex |= (0xb<<(i*4));
            break;
        }
        case 12:
        {
            Hex |= (0xC<<(i*4));
            break;
        }
        case 13:
        {
            Hex |= (0xd<<(i*4));
        
            break;
        }
        case 14:
        {
                Hex |= (0xe<<(i*4));
            break;
        }
        case 15:
        {
                Hex |= (0xf<<(i*4));
            break;
        }
    }
    Hex &= 0xFFFFFFFF;
    i++;
} while(ram/16 >= 0 && i <= 8);
    return Hex;
}

//油门控制  传入油门值：128平衡  0-127下降  129-255上升



 //命令2：定高模式

