#include "pid.h"

/*悬停控制*/
static void PositionPID(float sum_x, float sum_y)
{
        static float lastVxErro,lastVyErro;
        static float pidVx_pOut,pidVx_dOut,pidVx_iOut;
        static float pidVy_pOut,pidVy_dOut,pidVy_iOut;
        static unsigned char flag_Y,flag_X;
        /***************X轴PID参数**ROLL************/
        float Vxkp=0.087f;//
        float Vxki=0.00052f;//0.001f;
        float Vxkd=0.0252f;//-0.000531;

        /***************Y轴PID参数*PITCH*************/
        float Vykp=0.088f;
        float Vyki=0.0005f;//0.001f;
        float Vykd=0.025f;

        /*X轴位移速度调整*/
        //float vxErro=(float)(0.0f-(-pixX*hight/100));
        float vxErro= sum_x;
        float vxErroDelta=(vxErro-lastVxErro)/0.016f;
        lastVxErro=vxErro;
        /*X轴积分分离处理*/
        if(vxErro <= 50.0f&&vxErro >= -50.0f)
        {
                flag_X = 0;
        }
        else
        {
                flag_X = 1;
        }
        pidVx_pOut=Vxkp * vxErro;
        pidVx_dOut=Vxkd * vxErroDelta;
        pidVx_iOut+=Vxki * vxErro;
        if(pidVx_iOut>2.5f)//1.5
                pidVx_iOut=2.5f;
        if(pidVx_iOut<-2.5f)
                pidVx_iOut=-2.5f;

        pidVx_value=pidVx_pOut+pidVx_dOut+flag_X*pidVx_iOut;

         /*** 归一化 ***/
        pidVx_value = pidVx_value * 22.0 /500.0 ;
        //pidVx_value = pidVx_value * 0.08f;
        pidVx_value = LIMIT(pidVx_value, -1.0f, 1.0f);
}
