#include "stm32f10x.h"
#include "delay.h"
#include "Key.h"
#include "OLED.h"
#include "pwm.h"
#include "timer.h"
#include "usart.h"
#include "OLED_Codetab.h"

// unsigned char KeyNum;
uint16_t refresh_count = 0;
uint16_t refresh_count_last1 = 0;
uint16_t refresh_count_last2 = 0;

uint16_t time_count = 0;

int main(void)
{
    // int a = 0;
	// int i;

    delay_init();
    OLED_Init();
    Key_Init();
    uart_init(115200);
    Timer_Init();
	// Pwm_Init();
		
	OLED_Fill(0xFF);
	delay_ms(500);
	OLED_Fill(0x00);

    while(1)
    {
        // OLED_ShowStr(20,0,"Hello world!",1);			//显示字符串
        // OLED_ShowNum(3,4,4,1,1);
        // OLED_ShowCN_Str(20 , 1 , 0 , 3);
        OLED_DrawBMP(0,0,127,8,BMP3);						//显示图片
        refresh_count+=1;

        OLED_DrawBMP(0,0,127,8,BMP4);						//显示图片
        refresh_count+=1;

        // OLED_DrawGIF(30,2,78,8,12,294,BMP2);			//显示动态图片

        /* 
        for(i=0;i<3;i++){
            OLED_ShowCN(0 + 16*i,6,i);
        } */

        /* 
			// PWM_SetCompare1(100);
        
			
       for(a = 0;a<10000;a++)
        {
            
            //OLED_ShowChar(1,1,'A');
        // OLED_ShowString(2,1,"Hello World!");
            // OLED_ShowNum(3,1,a,3);
        }
        for(a =10000;a>0;a--)
        {
            //OLED_ShowChar(1,1,'A');
            // OLED_ShowString(2,1,"Hello World!");
            // OLED_ShowNum(3,1,a,3);
        }
 */


    }
}

//中断函数模板
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
    {
        time_count++;
        printf("time_count:%d   fps:%.2f   refresh_count_list:%d %d %d\r\n",time_count,(float)(refresh_count+refresh_count_last1+refresh_count_last2)/3,refresh_count,refresh_count_last1,refresh_count_last2);

        refresh_count_last2 = refresh_count_last1;
        refresh_count_last1 = refresh_count;
        refresh_count = 0;

        TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //清除标志位
    }
}
