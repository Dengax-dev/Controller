#include "stm32f10x.h"
#include "pwm.h"

void Pwm_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP; //选用复用推挽输出：断开输出数据寄存器，改用片上外设tim2的ch1通道控制输出
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;


    TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;// 时钟分割 0
    TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;//计数模式，选择向上计数
    TIM_TimeBaseInitStruct.TIM_Period=100-1;//自动重装器值                            ARR
    TIM_TimeBaseInitStruct.TIM_Prescaler=720-1;//PSC预分频器值                         PSC
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;//重复计数器值，此处不需要开启

    TIM_OCStructInit(&TIM_OCInitStructure); //给输出比较配置结构体赋初始值，防止使用高级定时器时出现不确定性
    TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1; //输出模式
    TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCPolarity_High; //比较输出极性
    TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable; //输出使能
    TIM_OCInitStructure.TIM_Pulse=1; //CCR的值                                           CCR
    /*
    ARR,PSC,CCR共同决定pwm的周期和占空比
    频率：  CK_PSC/(PSC+1)/(ARR+1)=计数器更新频率
    占空比：CCP/(ARR+1)
    分辨率：1/(ARR+1)
    */

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); //开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //打开AFIO时钟
    // GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2,ENABLE); //引脚重映射PA1》PA15
    // GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE); //解除调试端口复用重映射

    GPIO_Init(GPIOA,&GPIO_InitStructure);

    // TIM_InternalClockConfig(TIM2); //选择内部时钟，可以不写，定时器上电默认选择内部时钟

    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct); //初始化定时器

    TIM_OC1Init(TIM2,&TIM_OCInitStructure); //初始化输出比较配置部分参数


    TIM_Cmd(TIM2,ENABLE);//使能定时器

}

void PWM_SetCompare1(uint16_t Compare)
{
    TIM_SetCompare3(TIM2,Compare);
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

