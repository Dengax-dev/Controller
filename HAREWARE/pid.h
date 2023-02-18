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
    float Error;            //������
    float Integral;         //������
    float Differ;           //΢����
    float PreError;         //ǰһ�����
    float Ilimit;           //���ַ���
    float Irang;            //�����޷�
    unsigned char Ilimit_flag;    //���ַ����־
    float Pout;             //���������
    float Iout;             //���������
    float Dout;             //΢�������
    float OutPut;           //�����
}PID_TYPE;

extern Gl9306_kalman_Data Flow_kalman_Data;
extern PID_TYPE PID_Posi_x;
extern PID_TYPE PID_Posi_y;
extern PID_TYPE PID_Posi_High;

float kalmanFilter_A(float inData);
void PID_Cal(PID_TYPE *PID, float target, float measure);
void PidPara_Init(void);

#endif
