#include "pid.h"

Gl9306_kalman_Data Flow_kalman_Data = {0,0};

PID_TYPE PID_Posi_x;
PID_TYPE PID_Posi_y;
PID_TYPE PID_Posi_High;

//卡尔曼滤波
float kalmanFilter_A(float inData) 
{
    static float prevData=0; //前一次数据
    //p,k可以随意  Q和R他俩的物理意义是噪声的协方差，r越小越接近原数据，r越大，虽然滤波结果更平滑，但滤波器会变得不敏感，会有滞后，r参数调整滤波后的曲线与实测曲线的相近程度，r越小越接近。q参数调滤波后的曲线平滑程度，q越小越平滑。
    static float p=10, q=0.001, r=0.001, kGain=0;
    
    p = p+q; 
    kGain = p/(p+r);
    inData = prevData+(kGain*(inData-prevData)); 
    p = (1-kGain)*p;
    prevData = inData;

    return inData; 
}

void PID_Cal(PID_TYPE *PID, float target, float measure)
{
    PID->Error = target - measure; //误差
    PID->Differ = PID->Error - PID->PreError; //微分量
    PID->Integral += PID->Error; //累计误差

    if(PID->Integral > PID->Ilimit)  PID->Integral = PID->Ilimit; //积分限幅
    if(PID->Integral < -PID->Ilimit) PID->Integral = -PID->Ilimit;

    PID->Pout = PID->P * PID->Error;
    PID->Iout = PID->I * PID->Integral;
    PID->Dout = PID->D * PID->Differ;

    PID->OutPut = PID->Pout + PID->Iout + PID->Dout;

    PID->PreError = PID->Error; //更新前误差
}

void PidPara_Init(void)
{
    PID_Posi_x.P = 0;
    PID_Posi_x.I = 0;
    PID_Posi_x.D = 0;
    PID_Posi_x.Error = 0;            //比例项
    PID_Posi_x.Integral = 0;         //积分项
    PID_Posi_x.Differ = 0;           //微分项
    PID_Posi_x.PreError = 0;         //前一次误差
    PID_Posi_x.Ilimit = 0;           //积分分离
    PID_Posi_x.Irang = 0;            //积分限幅
    PID_Posi_x.Ilimit_flag = 0;    //积分分离标志
    PID_Posi_x.Pout = 0;             //比例项输出
    PID_Posi_x.Iout = 0;             //积分项输出
    PID_Posi_x.Dout = 0;             //微分项输出
    PID_Posi_x.OutPut = 0;           //总输出

    PID_Posi_y.P = 0;
    PID_Posi_y.I = 0;
    PID_Posi_y.D = 0;
    PID_Posi_y.Error = 0;            //比例项
    PID_Posi_y.Integral = 0;         //积分项
    PID_Posi_y.Differ = 0;           //微分项
    PID_Posi_y.PreError = 0;         //前一次误差
    PID_Posi_y.Ilimit = 0;           //积分分离
    PID_Posi_y.Irang = 0;            //积分限幅
    PID_Posi_y.Ilimit_flag = 0;    //积分分离标志
    PID_Posi_y.Pout = 0;             //比例项输出
    PID_Posi_y.Iout = 0;             //积分项输出
    PID_Posi_y.Dout = 0;             //微分项输出
    PID_Posi_y.OutPut = 0;           //总输出

    PID_Posi_High.P = 1;
    PID_Posi_High.I = 0.001;
    PID_Posi_High.D = 0.005;
    PID_Posi_High.Error = 0;            //比例项
    PID_Posi_High.Integral = 0;         //积分项
    PID_Posi_High.Differ = 0;           //微分项
    PID_Posi_High.PreError = 0;         //前一次误差
    PID_Posi_High.Ilimit = 0;           //积分分离
    PID_Posi_High.Irang = 0;            //积分限幅
    PID_Posi_High.Ilimit_flag = 0;    //积分分离标志
    PID_Posi_High.Pout = 0;             //比例项输出
    PID_Posi_High.Iout = 0;             //积分项输出
    PID_Posi_High.Dout = 0;             //微分项输出
    PID_Posi_High.OutPut = 0;           //总输出
}

///*悬停控制*/
//static void PositionPID(float sum_x, float sum_y)
//{
//        static float lastVxErro,lastVyErro;
//        static float pidVx_pOut,pidVx_dOut,pidVx_iOut;
//        static float pidVy_pOut,pidVy_dOut,pidVy_iOut;
//        static unsigned char flag_Y,flag_X;
//        /***************X轴PID参数**ROLL************/
//        float Vxkp=0.087f;//
//        float Vxki=0.00052f;//0.001f;
//        float Vxkd=0.0252f;//-0.000531;

//        /***************Y轴PID参数*PITCH*************/
//        float Vykp=0.088f;
//        float Vyki=0.0005f;//0.001f;
//        float Vykd=0.025f;

//        /*X轴位移速度调整*/
//        //float vxErro=(float)(0.0f-(-pixX*hight/100));
//        float vxErro= sum_x;
//        float vxErroDelta=(vxErro-lastVxErro)/0.016f;
//        lastVxErro=vxErro;
//        /*X轴积分分离处理*/
//        if(vxErro <= 50.0f&&vxErro >= -50.0f)
//        {
//                flag_X = 0;
//        }
//        else
//        {
//                flag_X = 1;
//        }
//        pidVx_pOut=Vxkp * vxErro;
//        pidVx_dOut=Vxkd * vxErroDelta;
//        pidVx_iOut+=Vxki * vxErro;
//        if(pidVx_iOut>2.5f)//1.5
//                pidVx_iOut=2.5f;
//        if(pidVx_iOut<-2.5f)
//                pidVx_iOut=-2.5f;

//        pidVx_value=pidVx_pOut+pidVx_dOut+flag_X*pidVx_iOut;

//         /*** 归一化 ***/
//        pidVx_value = pidVx_value * 22.0 /500.0 ;
//        //pidVx_value = pidVx_value * 0.08f;
//        pidVx_value = LIMIT(pidVx_value, -1.0f, 1.0f);
//}
