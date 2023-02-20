#ifndef __CONTROL_H
#define __CONTROL_H

#define CONTROL_USART           USART2

// #define CMD2_ARRAY     cmd2
// #define CMD3_ARRAY     cmd3
// #define CMD4_ARRAY     cmd4
// #define CMD5_ARRAY     cmd5
// #define CMD6_ARRAY     cmd6
// #define CMD7_ARRAY     cmd7
// #define CMD8_ARRAY     cmd8
// #define CMD9_ARRAY     cmd9
// #define CMD10_ARRAY    cmd10
// #define CMD11_ARRAY    cmd11

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


void control_uart2_init(u32 bound);
void Send_Cmd(u8 *cmd_array);
u8* mk_CmdArray_Dec(u8 data1, u8 data2, u8 data3, u8 data4);
u8* mk_CmdArray_Hex(u8 data1, u8 data2, u8 data3, u8 data4);
int DEC_HEX(uint32_t Dec);

#endif
