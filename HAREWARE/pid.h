#ifndef __PID_H_
#define __PID_H_

typedef struct{
    float speed_x;
    float speed_y;
}Gl9306_kalman_Data;

typedef struct PID{
    float P;
    float I;
    float D;
    float Error;            //比例项
    float Integral;         //积分项
    float Differ;           //微分项
    float PreError;         //前一次误差
    float Ilimit;           //积分分离
    float Irang;            //积分限幅
    unsigned char Ilimit_flag;    //积分分离标志
    float Pout;             //比例项输出
    float Iout;             //积分项输出
    float Dout;             //微分项输出
    float OutPut;           //总输出
}PID_TYPE;

extern Gl9306_kalman_Data Flow_kalman_Data;
extern PID_TYPE PID_Posi_x;
extern PID_TYPE PID_Posi_y;
extern PID_TYPE PID_Posi_High;

float kalmanFilter_A(float inData);
void PID_Cal(PID_TYPE *PID, float target, float measure);
void PidPara_Init(void);

#endif
