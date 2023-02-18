#include "stm32f10x.h"
#include "delay.h"
#include "Key.h"
#include "OLED.h"
#include "pwm.h"
#include "timer.h"
#include "usart.h"
#include "OLED_Codetab.h"
#include "gl9306.h"
#include "pid.h"
#include "control.h"

// u8 cmd1[] = {0x00, 0x01, 0xd3, 0xff,0x00, 0x01, 0xd3, 0xff};

int count = 0;

//数字正负取反
int16_t change(int16_t num)
{
    if(num < 0) num = ~(num - 1);
    // else if(num > 0) num = ~num + 1;
    return num;
}

int main(void)
{
    delay_init();
    OLED_Init();
    // Key_Init();
    uart1_init(19200);
    control_uart2_init(115200);
    PidPara_Init();
    // Timer_Init();
	// Pwm_Init();
		
	OLED_Fill_Fast(0xFF); //填充白色
	delay_ms(500);
	OLED_Fill_Fast(0x00); //清屏

    OLED_ShowStr(0,0,"speed_x:000",1);
    OLED_ShowStr(0,1,"speed_y:000",1);
    OLED_ShowStr(0,2,"quality:000",1);
    
    OLED_ShowStr(0,4,"speed_x:000",1);
    OLED_ShowStr(0,5,"speed_y:000",1);

    OLED_ShowStr(0,6,"move_x:",1);
    OLED_ShowStr(0,7,"move_y:",1);

    Timer_Init();


    // Send_Cmd(mk_CmdArray(0xff, 0x80, 0x80, 0x80, 0x00));
    // delay_ms(100);
    // USART_SendData(CONTROL_USART,DEC_HEX(0));
    // delay_ms(100);
    // USART_SendData(CONTROL_USART,DEC_HEX(255));
    // delay_ms(100);
    // USART_SendData(CONTROL_USART,DEC_HEX(16));
    // delay_ms(100);
    // USART_SendData(CONTROL_USART,DEC_HEX(3));
    // delay_ms(100);
    // USART_SendData(CONTROL_USART,DEC_HEX(127));

    while(1)
    {
        if(Flow_Data.speed_x<0)OLED_ShowStr(48,0,"-",1);
        else OLED_ShowStr(48,0," ",1);
        if(Flow_Data.speed_y<0)OLED_ShowStr(48,1,"-",1);
        else OLED_ShowStr(48,1," ",1);
        OLED_ShowNum(54,0,change(Flow_Data.speed_x),2,12);
        OLED_ShowNum(54,1,change(Flow_Data.speed_y),2,12);
        OLED_ShowNum(48,2,Flow_Data.quality,3,12);


        if(Flow_kalman_Data.speed_x<0)OLED_ShowStr(48,4,"-",1);
        else OLED_ShowStr(48,4," ",1);
        if(Flow_kalman_Data.speed_y<0)OLED_ShowStr(48,5,"-",1);
        else OLED_ShowStr(48,5," ",1);
        OLED_ShowNum(54,4,change(Flow_kalman_Data.speed_x),2,12);
        OLED_ShowNum(54,5,change(Flow_kalman_Data.speed_y),2,12);

        if(Flow_Data.move_x<0)OLED_ShowStr(48,6,"-",1);
        else OLED_ShowStr(48,6," ",1);
        if(Flow_Data.move_y<0)OLED_ShowStr(48,7,"-",1);
        else OLED_ShowStr(48,7," ",1);

        delay_ms(10);
    }
}

void TIM2_IRQHandler(void) //10ms
{
    if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
    {
        // count++;
        
        if((USART_RX_STA&0x8000)!=0)//接收完成
		{
            // printf("count = %d\r\n",count);
            // count = 0;

			flow_decode(USART_RX_BUF);
			USART_RX_STA = 0;

            Flow_kalman_Data.speed_x = (float)(Flow_Data.speed_x);
            Flow_kalman_Data.speed_y = (float)(Flow_Data.speed_y);

            // Flow_kalman_Data.speed_x = kalmanFilter_A(Flow_Data.speed_x);
            // kalmanFilter_A(Flow_kalman_Data.speed_y);
            
            //print flow data
            // printf("ori_x:%4d   ori_y:%4d   klm_x:%.2f   klm_y:%.2f\r\n",Flow_Data.speed_x,Flow_Data.speed_y,Flow_kalman_Data.speed_x,Flow_kalman_Data.speed_y);

            move_cal(760);
            // printf("move_x:%f,%f\r\n",Flow_Data.move_x,Flow_Data.move_y);
            // OLED_ShowNum(54,6,Flow_Data.move_x,5,12);
            // OLED_ShowNum(54,7,Flow_Data.move_y,5,12);
		}
        TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //清除标志位
    }
}
