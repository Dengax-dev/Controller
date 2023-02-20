#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
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

// int count = 0;
u8 *a;
int i,j ;
float heigh = 760;

//数字正负取反
int16_t change(int16_t num)
{
    if(num < 0) num = ~(num - 1);
    // else if(num > 0) num = ~num + 1;
    return num;
}

void Cmd_Test(void)
{
        LED1_ON();

    Send_Cmd(cmd2); //stay high
    printf("128\r\n");
    delay_ms(1000);

    Send_Cmd(cmd2); //stay high
    printf("128\r\n");
    delay_ms(1000);

    Send_Cmd(cmd2); //stay high
    printf("128\r\n");
    delay_ms(1000);

    Send_Cmd(cmd2); //stay high
    printf("128\r\n");
    delay_ms(1000);

    Send_Cmd(cmd2); //stay high
    printf("128\r\n");
    delay_ms(1000);

    Send_Cmd(mk_CmdArray_Dec(128, 128, 250, 128));
    printf("250\r\n");
    delay_ms(10);

    Send_Cmd(cmd2); //stay high
    printf("180\r\n");
    delay_ms(10);

    Send_Cmd(mk_CmdArray_Dec(128, 128, 230, 128));
    printf("180\r\n");
    delay_ms(1500);

    Send_Cmd(mk_CmdArray_Dec(128, 128, 230, 128));
    printf("230\r\n");
    delay_ms(1500);

    // Send_Cmd(mk_CmdArray_Dec(128, 128, 255, 128));
    // printf("255\r\n");
    // delay_ms(1500);

    Send_Cmd(mk_CmdArray_Dec(128, 128, 230, 128));
    printf("230\r\n");
    delay_ms(1000);

    Send_Cmd(mk_CmdArray_Dec(128, 128, 255, 128));
    printf("230\r\n");
    delay_ms(1000);

    Send_Cmd(mk_CmdArray_Dec(128, 128, 255, 128));
    printf("230\r\n");
    delay_ms(1000);

    Send_Cmd(mk_CmdArray_Dec(128, 128, 255, 128));
    printf("230\r\n");
    delay_ms(1000);

    Send_Cmd(mk_CmdArray_Dec(128, 128, 255, 128));
    printf("230\r\n");
    delay_ms(1000);

    Send_Cmd(mk_CmdArray_Dec(128, 128, 255, 128));
    printf("230\r\n");
    delay_ms(1000);

    Send_Cmd(mk_CmdArray_Dec(128, 128, 255, 128));
    printf("230\r\n");
    delay_ms(1000);

    Send_Cmd(mk_CmdArray_Dec(128, 128, 128, 128));
    printf("128\r\n");
    delay_ms(10);
    // Send_Cmd(cmd2); //stay high
    // delay_ms(1000);
    printf("0\r\n");

    LED1_OFF();

    // Send_Cmd(cmd4); //up
    // delay_ms(10);
    // Send_Cmd(cmd2); //stay high
    // delay_ms(10);

    // for(i = 128; i < 255; i++)
    // {
    //     a = mk_CmdArray_Dec(128, 128, i, 128);
    //     Send_Cmd(a);
    //     delay_ms(50);

    //     for(j = 0; j < 8; j++)
    //     {
    //         while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
    //         USART_SendData(USART1, *a++);
    //     }
    //     // Send_Cmd(cmd2);
    //     // delay_ms(10);
    // }
    // Send_Cmd(cmd3); //down
    // delay_ms(5000);
    Send_Cmd(cmd11); //stop
}

int main(void)
{
    delay_init();
    LED_Init();
    OLED_Init();
    // Key_Init();
    uart1_init(115200);
    control_uart2_init(19200);
    FlowData_uart3_init(19200);
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

    // 测试命令
    // Send_Cmd(mk_CmdArray(0xff, 0x80, 0x80, 0x80, 0x00));
    // Cmd_Test();

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
        Send_Cmd(cmd11);
    }
}

void TIM2_IRQHandler(void) //10ms
{
    if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
    {
        // count++;
        
        if((USART_RX_STA&0x8000)!=0)//flow接收完成
		{
            // printf("count = %d\r\n",count);
            // count = 0;

			flow_decode(USART_RX_BUF);
			USART_RX_STA = 0;

            // Flow_kalman_Data.speed_x = (float)(Flow_Data.speed_x);
            // Flow_kalman_Data.speed_y = (float)(Flow_Data.speed_y);

            // Flow_kalman_Data.speed_x = kalmanFilter_A(Flow_Data.speed_x);
            // kalmanFilter_A(Flow_kalman_Data.speed_y);
            
            //print flow data
            // printf("ori_x:%4d   ori_y:%4d   klm_x:%.2f   klm_y:%.2f\r\n",Flow_Data.speed_x,Flow_Data.speed_y,Flow_kalman_Data.speed_x,Flow_kalman_Data.speed_y);

            move_cal(heigh); //光流位移校准
            // printf("move_x:%f,%f\r\n",Flow_Data.move_x,Flow_Data.move_y);
            // OLED_ShowNum(54,6,Flow_Data.move_x,5,12);
            // OLED_ShowNum(54,7,Flow_Data.move_y,5,12);
		}

        if(USART1_RX_STA == 1)
        {
            static u8 sum, distance;

            for(sum=0,i=0;i<(USART1_RX_BUF[3]+4);i++)//rgb_data[3]=3
			sum+=USART1_RX_BUF[i];
			if(sum==USART1_RX_BUF[i])//校验和判断
			{
				distance=USART1_RX_BUF[4]<<8|USART1_RX_BUF[5];
				// RangeStatus=(USART1_RX_BUF[6]>>4)&0x0f;
				// Time=(USART1_RX_BUF[6]>>2)&0x03;
				// Mode=USART1_RX_BUF[6]&0x03;
				// send_3out(&USART1_RX_BUF[4],3,0x15);//上传给上位机
			}
            printf("distance:%d\r\n",distance);
			USART1_RX_STA = 0;//处理数据完毕标志
        }

        //PID参数计算
        // PID_Cal(&PID_Posi_High, 1000, heigh);
        // PID_Cal(&PID_Posi_x, 0, Flow_Data.move_x);
        // PID_Cal(&PID_Posi_y, 0, Flow_Data.move_y);
        TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //清除标志位
    }
}
