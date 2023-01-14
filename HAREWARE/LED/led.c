#include "stm32f10x.h"
#include "led.h"

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//GPIOC

    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOC,&GPIO_InitStructure);
    
    // GPIOC_13初始化

}

void LED1_ON(void)
{
    GPIO_ResetBits(GPIOC,GPIO_Pin_13);
}

void LED1_OFF(void)
{
    GPIO_SetBits(GPIOC,GPIO_Pin_13);
}

void LED1_Turn(void)
{
    if(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_13)==0)
    {
        GPIO_SetBits(GPIOC,GPIO_Pin_13);
    }
    else
    {
        GPIO_ResetBits(GPIOC,GPIO_Pin_13);
    }
}

