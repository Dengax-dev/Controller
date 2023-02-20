#include "stm32f10x.h"
#include "control.h"

u8 cmd2[]  = {0x66, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x99}; //����ģʽ
u8 cmd3[]  = {0x66, 0x80, 0x80, 0x00, 0x80, 0x00, 0x80, 0x99}; //�����½�
u8 cmd4[]  = {0x66, 0x80, 0x80, 0xff, 0x80, 0x00, 0x7f, 0x99}; //��������
u8 cmd5[]  = {0x66, 0x80, 0x80, 0x80, 0x00, 0x00, 0x80, 0x99}; //ƫ���ǣ���ת
u8 cmd6[]  = {0x66, 0x80, 0x80, 0x80, 0xff, 0x00, 0x7f, 0x99}; //ƫ���ǣ���ת
u8 cmd7[]  = {0x66, 0x80, 0x00, 0x80, 0x80, 0x00, 0x80, 0x99}; //�����ǣ�����
u8 cmd8[]  = {0x66, 0x80, 0xff, 0x80, 0x80, 0x00, 0x7f, 0x99}; //�����ǣ�ǰ��
u8 cmd9[]  = {0x66, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x99}; //����ǣ�����
u8 cmd10[] = {0x66, 0xff, 0x80, 0x80, 0x80, 0x00, 0x7f, 0x99}; //����ǣ��Ҳ��
u8 cmd11[] = {0x66, 0x80, 0x80, 0x80, 0x80, 0x04, 0x04, 0x99}; //���ͣת

void control_uart2_init(u32 bound)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	// NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
	//USART2_TX   GPIOA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.2
   
    //USART1_RX	  GPIOA.3��ʼ��
    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    // GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.3  

    //Usart1 NVIC ����
	// NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	// NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
    //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	// USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_InitStructure.USART_Mode = USART_Mode_Tx;

    USART_Init(USART2, &USART_InitStructure); //��ʼ����2
    // USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
    USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2
}

//��������
void Send_Cmd(u8 *cmd_array)
{
    u8 i;
    for(i = 0; i < 8; i++)
    {
        while(USART_GetFlagStatus(CONTROL_USART,USART_FLAG_TC)==RESET);
        USART_SendData(CONTROL_USART,*cmd_array++);
    }
}

//����У��λ
// u8 Cal_Cmd(u8 data1, u8 data2, u8 data3, u8 data4, u8 data5)
// {
//     u8 data6;
//     data6 = data1^data2^data3^data4^data5;

//     return data6;
// }

//���ɿ�������
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
    array[6] = data1^data2^data3^data4^(0x00); //У��λ
    array[7] = 0x99;

    for(i = 0; i < 8; i++)
    {
        while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
        USART_SendData(USART1, array[i]);
    }

    return array;
}

// ʮ����toʮ������
int DEC_HEX(uint32_t Dec)
{ 
   int ram = 0;//��
    int ray = 0;//��
    uint32_t Hex = 0x0;
    int i =0;
do{   
    ram = Dec /16;
    ray = Dec %16;    
    Dec = ram;
    switch(ray)           //������Ϊ����
    {
        case 0:
        {
            Hex |= (0x0<<(i*4));   //Ȼ��������λ������һ����λ����һ��ѭ����������
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

//���ſ���  ��������ֵ��128ƽ��  0-127�½�  129-255����



 //����2������ģʽ

