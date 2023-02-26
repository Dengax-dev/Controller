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

#define LENGTH   70

// int count = 0;
// u8 *a;
// int i,j ;
u8 Ctrl_Interval_Count;
u8 Step = 0; //����״̬�� 0��ʼ�� 1-��� 2-���1s 3-�½� 4�ȴ�10s 5������� 6��ǰ1m 7�����ȴ�5s 8����10m 9����
u8 task2 = 0; //����2
u16 forward = 0; //0������2 1����2ǰ�� 2����2����
u8 circle = 0;


void delay_s(u16 s)
{
    int i = 0;
    for(i = 0; i < s; i++)
    {
        delay_ms(1000);
    }
}
//��������ȡ��
int16_t change(int16_t num)
{
    if(num < 0) num = ~(num - 1);
    // else if(num > 0) num = ~num + 1;
    return num;
}

void Take_Off(PLANESTATE *StateStruct)
{
    if((StateStruct->fly_state == 0)&&(StateStruct->pre_landing != 1))
    {
        delay_ms(1500);
        delay_ms(1500);
        delay_ms(1500);
        delay_ms(1500);
        Send_Cmd(cmd2); //stay high
        printf("128\r\n");
        delay_ms(1000);
        Send_Cmd(cmd2); //stay high
        printf("128\r\n");
        delay_ms(1000    );
        Send_Cmd(cmd2); //stay high
        printf("128\r\n");
        delay_ms(1000    );
        printf("128\r\n");

        LED1_ON();

        Send_Cmd(mk_CmdArray_Dec(128, 128, 255, 128));
        printf("255\r\n");
        delay_ms(300);

        Send_Cmd(cmd2); //stay high
        printf("128\r\n");
        delay_ms(300);

        // Send_Cmd(mk_CmdArray_Dec(128, 128, 255, 128));
        // printf("255\r\n");
        // delay_ms(300);

        StateStruct->fly_state = 1; //�ɻ���ɱ�־
        LED1_OFF();
    }
}

void Landing(PLANESTATE *StateStruct)
{
    int a = 128;

    StateStruct->pre_landing = 1;

    for(a = 128; a > 1; a-=1)
    {
        // Send_Cmd(mk_CmdArray_Dec((128 + PID_Posi_y.OutPut), (128 + PID_Posi_x.OutPut), a, 128)); //������һ����Բ���ʧ�أ������ʹ�û�Բ����Ϳ���ʹ�ø�����
        Send_Cmd(mk_CmdArray_Dec(128, 128, a, 128));
        delay_ms(50);
        if(StateStruct->height <= 100)
        {
            Send_Cmd(cmd11);
        
            StateStruct->pre_landing = 0;
            StateStruct->fly_state = 0; //�ɻ���ر�־
            return;
        }
    }
    Send_Cmd(cmd11);
    
    StateStruct->pre_landing = 0;
    StateStruct->fly_state = 0; //�ɻ���ر�־
}

int main(void)
{
    delay_init();
    LED_Init();
    // OLED_Init();
    // Key_Init();
    uart1_init(115200);
    control_uart2_init(19200);
    FlowData_uart3_init(19200);
    // PidPara_Init();
    Timer_Init();
		
	// OLED_Fill_Fast(0xFF); //����ɫ
	// delay_ms(200);
	// OLED_Fill_Fast(0x00); //����

    // OLED_ShowStr(0,0,"speed_x:000",1);
    // OLED_ShowStr(0,1,"speed_y:000",1);
    // OLED_ShowStr(0,2,"quality:000",1);
    
    // OLED_ShowStr(0,4,"distance:0",1);

    while(1)
    {
        // if(Flow_Data.speed_x<0)OLED_ShowStr(48,0,"-",1);
        // else OLED_ShowStr(48,0," ",1);
        // if(Flow_Data.speed_y<0)OLED_ShowStr(48,1,"-",1);
        // else OLED_ShowStr(48,1," ",1);
        // OLED_ShowNum(54,0,change(Flow_Data.speed_x),2,12);
        // OLED_ShowNum(54,1,change(Flow_Data.speed_y),2,12);
        // OLED_ShowNum(48,2,Flow_Data.quality,3,12);

        // OLED_ShowNum(54,4,height,4,12);
        switch (Step)
        {
        case 0:
            Step = 1;
            break;
        case 1: //����һ
            Flow_Data.move_x = 0;
            Flow_Data.move_y = 0;
            PidPara_Init();
            Take_Off(&PlaneState);
            delay_s(10);
            Landing(&PlaneState);
            Step = 5;
            break;
        case 5: //����2
            Flow_Data.move_x = 0;
            Flow_Data.move_y = 0;
            PidPara_Init();
            Take_Off(&PlaneState);
            forward = 0;
            task2 = 1;
            delay_s(20);
            forward = LENGTH;
            task2 = 2;
            delay_s(20);
            Landing(&PlaneState);
            Step = 10;
            break;
        case 10:
            Flow_Data.move_x = 0;
            Flow_Data.move_y = 0;
            PidPara_Init();
            Take_Off(&PlaneState);
            circle = 1;
            delay_s(15);
            // Send_Cmd(cmd11);
            
            Landing(&PlaneState);
            Step =  11;
            break;
        
        default:
            break;
        }
        
        // Send_Cmd(cmd11);
    }
}

void TIM2_IRQHandler(void) //10ms
{
    if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
    {
        uint16_t distance = 0;
        uint16_t RangeStatus = 1;
        Ctrl_Interval_Count++;

        if((USART_RX_STA&0x8000)!=0)//flow�������
		{
			flow_decode(USART_RX_BUF); //��������
			USART_RX_STA = 0; //gl9306������ɱ�־

            move_cal(PlaneState.height); //����λ��У׼,����߶�(mm)�������mm��
		}

        if(USART1_RX_STA == 1) //vl53l1x�������
        {
            distance=USART1_RX_BUF[4]<<8|USART1_RX_BUF[5];
            RangeStatus=(USART1_RX_BUF[6]>>4)&0x0f; //0:����ֵ�ɿ�
            if(RangeStatus == 0) PlaneState.height = distance + 18;//printf("distance:%d\r\n",distance);

			USART1_RX_STA = 0;//����������ϱ�־
        }

        //PID��������
        PID_Cal(&PID_Posi_High, 1000+30, PlaneState.height);

        if(task2 == 1) {
            if(forward < LENGTH) forward+=1;
            PID_Cal(&PID_Posi_x, forward, 0.1*Flow_Data.move_x);
        }
        else if(task2 ==2){
            if(forward > 0) forward-=1;
            PID_Cal(&PID_Posi_x, forward, 0.1*Flow_Data.move_x);
        }
        else PID_Cal(&PID_Posi_x, 0, 0.1*Flow_Data.move_x);

        PID_Cal(&PID_Posi_y, 0, 0.1*Flow_Data.move_y);

        printf("target Height ThrottleOut FlowSPx FlowSPy flowDMx flowDMy :1000,%d,%f,%f,%d,%d,%f,%f,%f,%f,%f,%f\r\n",
        PlaneState.height,
        (128+PID_Posi_High.OutPut),
        PID_Posi_High.Integral,
        Flow_Data.speed_x, 
        Flow_Data.speed_y, 
        Flow_Data.move_x, 
        Flow_Data.move_y, 
        (128+PID_Posi_x.OutPut), 
        (128+PID_Posi_y.OutPut),
        PID_Posi_x.Integral,
        PID_Posi_y.Integral
        );

        //���ߴ��ڴ�ӡ״̬��Ϣ
        // printf("m_x m_y sp_x sp_y h:%f,%f,%d,%d,%d,%d\r\n",Flow_Data.move_x, Flow_Data.move_y, Flow_Data.speed_x, Flow_Data.speed_y, height, Flow_Data.quality);
        
        if((Ctrl_Interval_Count*20 >= CONTROL_INTERVAL)&&(PlaneState.fly_state == 1)&&(PlaneState.pre_landing == 0)&&(circle !=1))
        {
            if(task2 == 1)
            {
                Send_Cmd(mk_CmdArray_Dec((128 + PID_Posi_y.OutPut), (128 + PID_Posi_x.OutPut), (128 + (int)(PID_Posi_High.OutPut)), 128)); //300ms����ָ��
            }
            else Send_Cmd(mk_CmdArray_Dec((128 + PID_Posi_y.OutPut), (128 + PID_Posi_x.OutPut), (128 + (int)(PID_Posi_High.OutPut)), 128)); //300ms����ָ��

            Ctrl_Interval_Count = 0;
        }
        else if((Ctrl_Interval_Count*20 >= CONTROL_INTERVAL)&&(PlaneState.fly_state == 1)&&(PlaneState.pre_landing == 0)&&(circle == 1))
        {
                Send_Cmd(mk_CmdArray_Dec(128, 140, (128 + (int)(PID_Posi_High.OutPut)), 160));
        }
        TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //�����־λ
    }
}
