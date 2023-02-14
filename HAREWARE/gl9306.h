#ifndef __GL9306_H_
#define __GL9306_H_

typedef struct{
    int16_t speed_x;
    int16_t speed_y;
    int16_t quality;
}Gl9306_Data;

extern Gl9306_Data Flow_Data;

void flow_decode(u8 *UartRxOpticalFlow);

#endif
