#ifndef __CONTROL_H
#define __CONTROL_H

#define CONTROL_USART           USART2
#define CONTROL_INTERVAL        500 // 控制间隔500ms

typedef struct 
{
    u8 fly_state; //飞机状态 0落地 1飞起
    uint16_t height;
}PLANESTATE;


extern u8 cmd2[] ;
extern u8 cmd3[] ;
extern u8 cmd4[] ;
extern u8 cmd5[] ;
extern u8 cmd6[] ;
extern u8 cmd7[] ;
extern u8 cmd8[] ;
extern u8 cmd9[] ;
extern u8 cmd10[];
extern u8 cmd11[];
extern PLANESTATE PlaneState;


void control_uart2_init(u32 bound);
void Send_Cmd(u8 *cmd_array);
u8* mk_CmdArray_Dec(u8 data1, u8 data2, u8 data3, u8 data4);
u8* mk_CmdArray_Hex(u8 data1, u8 data2, u8 data3, u8 data4);
int DEC_HEX(uint32_t Dec);

#endif
