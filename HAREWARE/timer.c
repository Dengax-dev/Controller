#include "timer.h"
#include "stm32f10x.h"

void Timer_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    // GPIO_InitTypeDef GPIO_InitStructure;

    TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;//预分频
    TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;//计数模式，选择向上计数
    TIM_TimeBaseInitStruct.TIM_Period=100-1;//自动重装器值
    TIM_TimeBaseInitStruct.TIM_Prescaler=7200-1;//psc预分频器值
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;//重复计数器值，此处不需要开启

    NVIC_InitStruct.NVIC_IRQChannel=TIM2_IRQn; //定时器2通道
    NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;

    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
    // GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
    // GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    // GPIO_Init(GPIOA,&GPIO_InitStructure);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); //开启时钟

    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    

    // TIM_ETRClockMode2Config(TIM2,TIM_ExtTRGPSC_OFF,TIM_ExtTRGPolarity_NonInverted,0x0f); //选择外部时钟
    TIM_InternalClockConfig(TIM2); //选择内部时钟，可以不写，定时器上电默认选择内部时钟

    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct); //时基单元配置

    TIM_ClearFlag(TIM2,TIM_FLAG_Update); //解决完成初始化后立即进入中断的问题

    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //中断输出控制，使能外设中断输出

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //优先级分组

    NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM2,ENABLE);//使能计数器

}

uint16_t Timer_GetCounter(void)
{
    return TIM_GetCounter(TIM2);
}

//中断函数模板
/* void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
    {
        // Num++;
        TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //清除标志位
    }
} */

