#include "stm32f10x.h"
#include "sys.h"
#include "usart.h"
#include "gl9306.h"
	
u8 USART_RX_BUF[USART_REC_LEN];     //USART3���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���

Gl9306_Data Flow_Data = {0,0,0,0,0};

void FlowData_uart3_init(u32 bound)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//ʹ��USART3
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
	//USART2_TX   GPIOA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA.2
   
    // USART1_RX	��ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA.3  

    //Usart3 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
    //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART3, &USART_InitStructure); //��ʼ����2
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
    USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���2
}

void USART3_IRQHandler(void)                	//����3�жϷ������
{
	u8 Res;
	// extern int count;

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�
	{
		// printf("finsih\r\n");
		Res =USART_ReceiveData(USART3);	//��ȡ���յ�������
		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
			USART_RX_STA++;

			if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����

			if(Res==0xAA){USART_RX_STA|=0x8000; //������ɱ�־
			}
		}

		// if((USART_RX_STA&0x8000)!=0)//�������
		// {
		// 	printf("finsih\r\n");
		// 	printf("%c%c%c%c%c%c%c%c%c",USART_RX_BUF[0],USART_RX_BUF[1],USART_RX_BUF[2],USART_RX_BUF[3],USART_RX_BUF[4],USART_RX_BUF[5],USART_RX_BUF[6],USART_RX_BUF[7],USART_RX_BUF[8]);
		// 	USART_RX_STA = 0;
		// }
     } 
} 

//��������
void flow_decode(u8 *UartRxOpticalFlow)
{
	static uint8_t Check_sum = 0;
	if(UartRxOpticalFlow[0] == 0xfe)//У��ͷ
	{
		Check_sum=(uint8_t)(UartRxOpticalFlow[2] + UartRxOpticalFlow[3] + UartRxOpticalFlow[4] + UartRxOpticalFlow[5]);
		if(Check_sum == UartRxOpticalFlow[6])//У�����ȷ
		{
			Flow_Data.speed_x = UartRxOpticalFlow[2] + (UartRxOpticalFlow[3] << 8);
			Flow_Data.speed_y = UartRxOpticalFlow[4] + (UartRxOpticalFlow[5] << 8);
			Flow_Data.quality = UartRxOpticalFlow[7];
		}
	}
}

//����λ�Ƽ���
void move_cal(uint16_t opticalflow_high)
{
	Flow_Data.move_x+=(Flow_Data.speed_x*opticalflow_high)/272.0f;//��λ:mm   ���Ը߶ȵ�λmm
    Flow_Data.move_y+=(Flow_Data.speed_y*opticalflow_high)/272.0f;//��λ:mm   ���Ը߶ȵ�λmm
}
