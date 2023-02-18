#ifndef __GL9306_H_
#define __GL9306_H_

#define USART_REC_LEN  			200  	//�����������ֽ��� 200

extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���

typedef struct{
    int16_t speed_x;
    int16_t speed_y;
    int16_t quality;

    float move_x; //x����λ��
    float move_y; //y����λ��

}Gl9306_Data;

extern Gl9306_Data Flow_Data;

void FlowData_uart3_init(u32 bound);
void flow_decode(u8 *UartRxOpticalFlow);
void move_cal(uint16_t opticalflow_high);

#endif
