#include "pid.h"

Gl9306_kalman_Data Flow_kalman_Data = {0,0};

//�������˲�
float kalmanFilter_A(float inData) 
{
    static float prevData=0; //ǰһ������
    //p,k��������  Q��R����������������������Э������ǵ�ֵ����Ҫ�����Գ����ġ�rԽСԽ�ӽ�ԭ���ݣ�rԽ����Ȼ�˲������ƽ�������˲������ò����У������ͺ�r���������˲����������ʵ�����ߵ�����̶ȣ�rԽСԽ�ӽ���q�������˲��������ƽ���̶ȣ�qԽСԽƽ����
    static float p=10, q=0.001, r=0.001, kGain=0;
    
    p = p+q; 
    kGain = p/(p+r);
    inData = prevData+(kGain*(inData-prevData)); 
    p = (1-kGain)*p;
    prevData = inData;

    return inData; 
}

///*��ͣ����*/
//static void PositionPID(float sum_x, float sum_y)
//{
//        static float lastVxErro,lastVyErro;
//        static float pidVx_pOut,pidVx_dOut,pidVx_iOut;
//        static float pidVy_pOut,pidVy_dOut,pidVy_iOut;
//        static unsigned char flag_Y,flag_X;
//        /***************X��PID����**ROLL************/
//        float Vxkp=0.087f;//
//        float Vxki=0.00052f;//0.001f;
//        float Vxkd=0.0252f;//-0.000531;

//        /***************Y��PID����*PITCH*************/
//        float Vykp=0.088f;
//        float Vyki=0.0005f;//0.001f;
//        float Vykd=0.025f;

//        /*X��λ���ٶȵ���*/
//        //float vxErro=(float)(0.0f-(-pixX*hight/100));
//        float vxErro= sum_x;
//        float vxErroDelta=(vxErro-lastVxErro)/0.016f;
//        lastVxErro=vxErro;
//        /*X����ַ��봦��*/
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

//         /*** ��һ�� ***/
//        pidVx_value = pidVx_value * 22.0 /500.0 ;
//        //pidVx_value = pidVx_value * 0.08f;
//        pidVx_value = LIMIT(pidVx_value, -1.0f, 1.0f);
//}
