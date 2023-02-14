#ifndef __PID_H_
#define __PID_H_

typedef struct{
    float speed_x;
    float speed_y;
}Gl9306_kalman_Data;

extern Gl9306_kalman_Data Flow_kalman_Data;

float kalmanFilter_A(float inData);

#endif
