#ifndef __GL9306_H_
#define __GL9306_H_

#define USART_REC_LEN  			200  	//定义最大接收字节数 200

extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记

typedef struct{
    int16_t speed_x;
    int16_t speed_y;
    int16_t quality;

    float move_x; //x方向位移
    float move_y; //y方向位移

}Gl9306_Data;

extern Gl9306_Data Flow_Data;

void FlowData_uart3_init(u32 bound);
void flow_decode(u8 *UartRxOpticalFlow);
void move_cal(uint16_t opticalflow_high);

#endif
