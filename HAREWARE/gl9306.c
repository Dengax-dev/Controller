#include "stm32f10x.h"
#include "usart.h"
#include "gl9306.h"

Gl9306_Data Flow_Data = {0,0,0,0,0};

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
	Flow_Data.move_x+=(Flow_Data.speed_x*opticalflow_high)/1000.0f;//单位:mm   乘以高度单位mm
    Flow_Data.move_y+=(Flow_Data.speed_y*opticalflow_high)/1000.0f;//单位:mm   乘以高度单位mm
}
