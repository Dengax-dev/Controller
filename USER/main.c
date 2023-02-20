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
uint16_t height = 0;


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
    delay_ms(1500);
    Send_Cmd(cmd2); //stay high
    printf("128\r\n");
    delay_ms(1500);
    Send_Cmd(cmd2); //stay high
    printf("128\r\n");
    delay_ms(1500);
    Send_Cmd(cmd2); //stay high
    printf("128\r\n");
    delay_ms(1500);

    Send_Cmd(mk_CmdArray_Dec(128, 128, 250, 128));
    printf("250\r\n");
    delay_ms(10);

    Send_Cmd(cmd2); //stay high
    printf("128\r\n");
    delay_ms(10);

    Send_Cmd(mk_CmdArray_Dec(128, 128, 250, 128));
    printf("180\r\n");
    delay_ms(10);

    

    LED1_OFF();

    // for(i = 128; i < 255; i++)
    // {
    //     a = mk_CmdArray_Dec(128, 128, i, 128);
    //     Send_Cmd(a);
    //     delay_ms(50);

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
    Timer_Init();
		
	OLED_Fill_Fast(0xFF); //填充白色
	delay_ms(200);
	OLED_Fill_Fast(0x00); //清屏

    OLED_ShowStr(0,0,"speed_x:000",1);
    OLED_ShowStr(0,1,"speed_y:000",1);
    OLED_ShowStr(0,2,"quality:000",1);
    
    OLED_ShowStr(0,4,"distance:0",1);

    // OLED_ShowStr(0,4,"speed_x:000",1);
    // OLED_ShowStr(0,5,"speed_y:000",1);

    // OLED_ShowStr(0,6,"move_x:",1);
    // OLED_ShowStr(0,7,"move_y:",1);

    // 测试命令
    Cmd_Test();

    while(1)
    {
        if(Flow_Data.speed_x<0)OLED_ShowStr(48,0,"-",1);
        else OLED_ShowStr(48,0," ",1);
        if(Flow_Data.speed_y<0)OLED_ShowStr(48,1,"-",1);
        else OLED_ShowStr(48,1," ",1);
        OLED_ShowNum(54,0,change(Flow_Data.speed_x),2,12);
        OLED_ShowNum(54,1,change(Flow_Data.speed_y),2,12);
        OLED_ShowNum(48,2,Flow_Data.quality,3,12);

        OLED_ShowNum(54,4,height,4,12);
        // if(Flow_kalman_Data.speed_x<0)OLED_ShowStr(48,4,"-",1);
        // else OLED_ShowStr(48,4," ",1);
        // if(Flow_kalman_Data.speed_y<0)OLED_ShowStr(48,5,"-",1);
        // else OLED_ShowStr(48,5," ",1);
        // OLED_ShowNum(54,4,change(Flow_kalman_Data.speed_x),2,12);
        // OLED_ShowNum(54,5,change(Flow_kalman_Data.speed_y),2,12);

        // if(Flow_Data.move_x<0)OLED_ShowStr(48,6,"-",1);
        // else OLED_ShowStr(48,6," ",1);
        // if(Flow_Data.move_y<0)OLED_ShowStr(48,7,"-",1);
        // else OLED_ShowStr(48,7," ",1);


        delay_ms(10);
        Send_Cmd(cmd11);
    }
}

void TIM2_IRQHandler(void) //10ms
{
    if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
    {
        // uint16_t sum = 0;
        uint16_t distance = 0;
        uint16_t RangeStatus = 1;

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

            move_cal(distance); //光流位移校准
            // printf("move_x:%f,%f\r\n",Flow_Data.move_x,Flow_Data.move_y);
		}

        if(USART1_RX_STA == 1) //vl53l1x接收完成
        {
            distance=USART1_RX_BUF[4]<<8|USART1_RX_BUF[5];
            RangeStatus=(USART1_RX_BUF[6]>>4)&0x0f; //0:距离值可靠
            if(RangeStatus == 0) height = distance;//printf("distance:%d\r\n",distance);

			USART1_RX_STA = 0;//处理数据完毕标志
        }

        //PID参数计算
        PID_Cal(&PID_Posi_High, 1000, height);
        printf("target height output:1000,%d,%f\r\n",height, PID_Posi_High.OutPut);
        // PID_Cal(&PID_Posi_x, 0, Flow_Data.move_x);
        // PID_Cal(&PID_Posi_y, 0, Flow_Data.move_y);



        TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //清除标志位
    }
}
