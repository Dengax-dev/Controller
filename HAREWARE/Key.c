#include "stm32f10x.h"
#include "Delay.h"
#include "Key.h"

void Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//GPIOA

    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
}

unsigned char Key_GetNum(void)
{
    unsigned char KeyNum=0;
    if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0)
    {
        delay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0);
        delay_ms(20);
        KeyNum=1;
    }
    
    return KeyNum;
}

