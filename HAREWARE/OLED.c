#include "stm32f10x.h"
#include "OLED.h"
#include "OLED_Font.h"
// #include "OLED_Codetab.h"

/*引脚配置*/
#define OLED_W_SCL(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_10, (BitAction)(x))
#define OLED_W_SDA(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_11, (BitAction)(x))

/*引脚初始化*/
void OLED_I2C_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C开始
  * @param  无
  * @retval 无
  */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);
	OLED_W_SCL(1);
	OLED_W_SDA(0);
	OLED_W_SCL(0);
}

/**
  * @brief  I2C停止
  * @param  无
  * @retval 无
  */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(Byte & (0x80 >> i));
		OLED_W_SCL(1);
		OLED_W_SCL(0);
	}
	OLED_W_SCL(1);	//额外的一个时钟，不处理应答信号
	OLED_W_SCL(0);
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x00);		//写命令
	OLED_I2C_SendByte(Command); 
	OLED_I2C_Stop();
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x40);		//写数据
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
}

/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)			//上电延时
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();			//端口初始化
	
	OLED_WriteCommand(0xAE);	//关闭显示
	
	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0xf0);	//原来是80
	
	OLED_WriteCommand(0xA8);	//设置多路复用率
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);	//设置显示开始行
	
	OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	//设置对比度控制
	OLED_WriteCommand(0xFF);

	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xff);	//原来0xF1

	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭

	OLED_WriteCommand(0xA6);	//设置正常/倒转显示

	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//开启显示

	OLED_Clear();				//OLED清屏
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置低4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置高4位
}

void OLED_SetPos(unsigned char x,unsigned char y)  //设置起点坐标
{
	OLED_WriteCommand(0xb0+y);
	OLED_WriteCommand((x&0xf0)>>4|0x10); //取高位
	OLED_WriteCommand((x&0x0f)|0x01);    //取低位
}

//全屏填充
void OLED_Fill(unsigned char Fill_Data)  
{
	unsigned char m,n;
	
	for(m=0;m<8;m++){
		OLED_WriteCommand(0xb0+m);
		OLED_WriteCommand(0x00);
		OLED_WriteCommand(0x10);
	
	  for(n=0;n<128;n++){
		OLED_WriteData(Fill_Data);
		}
	}
}

//全屏填充
void OLED_Fill_Fast(unsigned char Fill_Data)  
{
	unsigned char m,n;
	
	for(m=0;m<8;m++){
		OLED_WriteCommand(0xb0+m);
		OLED_WriteCommand(0x00);
		OLED_WriteCommand(0x10);
		
		OLED_I2C_Start();
		OLED_I2C_SendByte(0x78);		//从机地址
		OLED_I2C_SendByte(0x40);		//写数据
		
		for(n=0;n<128;n++){
			// OLED_WriteData(Fill_Data);
			OLED_I2C_SendByte(Fill_Data);
		}
		OLED_I2C_Stop();
	}
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{  
	OLED_Fill(0x00);
	/* uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	} */
}


void OLED_ON(void)
{
	OLED_WriteCommand(0X8D);  //设置电荷泵
	OLED_WriteCommand(0X14);  //开启电荷泵
	OLED_WriteCommand(0XAF);  //OLED唤醒
}
 
void OLED_OFF(void)
{
	OLED_WriteCommand(0X8D);  //设置电荷泵
	OLED_WriteCommand(0X10);  //关闭电荷泵
	OLED_WriteCommand(0XAE);  //OLED休眠
}

/**
  * @brief  OLED显示一个字符
  * @param  y 行位置，范围：1~8
  * @param  x 列位置，范围：0~128
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @param  Char_Size 字符尺寸，必须是12(6x8字符)或16（8x16字符)
  * @retval 无
  */
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//???????			
		if(x>128-1){x=0;y=y+2;}
		if(Char_Size == 16)
		{
			OLED_SetPos(x,y);	
			for(i=0;i<8;i++)
				OLED_WriteData(OLED_F8X16[c*16+i]);
			OLED_SetPos(x,y+1);
			for(i=0;i<8;i++)
				OLED_WriteData(OLED_F8X16[c*16+i+8]);
		}
		else if(Char_Size == 12)
		{	
			OLED_SetPos(x,y);
			for(i=0;i<6;i++)
				OLED_WriteData(OLED_F6X8[c][i]);
			
		}
}

/**
  * @brief  OLED显示字符串
  * @param  x 起始行位置，范围：0~127
  * @param  Column 起始列位置，范围：1~7
  * @param  ch[] 要显示的字符串，范围：ASCII可见字符
  * @param  TextSize 字符大小(1:6*8 ; 2:8*16)
  * @retval 无
  */
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char *ch, unsigned char TextSize)
{
	unsigned char c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<6;i++)
					OLED_WriteData(OLED_F6X8[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<8;i++)
					OLED_WriteData(OLED_F8X16[c*16+i]);
				OLED_SetPos(x,y+1);
				for(i=0;i<8;i++)
					OLED_WriteData(OLED_F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED显示数字
  * @param  x 起始行位置，范围：0~127
  * @param  Column 起始列位置，范围：1~7
  * @param  num 要显示的数字，范围：0~4294967295
  * @param  Size2 字符大小(12:6*8 ; 16:8*16)
  * @retval 无
  */
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2)
{         	
	u8 t,temp;
	u8 enshow=0; //判断高位是否为0的标志
	for(t=0;t<len;t++)
	{
		temp=(num/OLED_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}else enshow=1; 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
} 

//显示汉字 （x：横坐标  y：纵坐标  N：字数）
void OLED_ShowCN(unsigned char x,unsigned char y,unsigned char N)
{
    unsigned char wm=0;
	unsigned int addr = 32*N;
	OLED_SetPos(x,y);
    for(wm=0;wm<16;wm++)
	{
		OLED_WriteData( F16X16[addr]);
		addr +=1;
	}
	OLED_SetPos(x,y+1);
	for(wm=0;wm<16;wm++)
	{
		OLED_WriteData( F16X16[addr]);
		addr +=1;
	}
 
}

//显示汉字串
//这是自己写的显示中文字符串的函数，要先把中文字符串“共阴——列行式——逆向输出”取字模后存入asc.h相应的位置(连续存入)
//传入参数分别为：x:起始横坐标
//y：纵坐标(填入0-7)
//begin:填入的中文字符串的第一个字在我们asc.c字库里面的序号  
//num:我们要填写几个字
//比如要填“测试”，取完字模存入后这两个字在字库中序号为0,1,横坐标0，纵坐标第二行，就填：x：0，y：2，begin：0，num：2
void OLED_ShowCN_Str(u8 x , u8 y , u8 begin , u8 num)
{
	u8 i;
	for(i=0;i<num;i++){OLED_ShowCN(i*16+x,y,i+begin);}
}

// Parameters     : x0,y0 起始点坐标(x0:0~127, y0:0~7); x1,y1 图片终点坐标(x1:1~128,y1:1~8)
// Description    : 选择图片
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
    for(x=x0;x<x1;x++)
		{
			OLED_WriteData(BMP[j++]);
		}
	}
}

// Parameters     : x0,y0 起始点坐标(x0:0~127, y0:0~7); x1,y1 图片终点坐标(x1:1~128,y1:1~8)
// Description    : 选择图片
void OLED_DrawBMP_Fast(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);

		OLED_I2C_Start();
		OLED_I2C_SendByte(0x78);		//从机地址
		OLED_I2C_SendByte(0x40);		//写数据
		
		for(x=x0;x<x1;x++)
		{
			// OLED_WriteData(BMP[j++]);
			OLED_I2C_SendByte(BMP[j++]);
		}
		OLED_I2C_Stop();
	}
}

/*
	@brief			????
	@param			x0:?????
				y0:?????
				x1:?????
				y1:?????
				k: ???
				m: ??????
				BMP[][m]:?????????
	@retval			?
 */
void OLED_DrawGIF(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1, unsigned char k, int m, unsigned char GIF[][294])
{
	unsigned int j=0; //????
 	unsigned char x,y,i; //????
  
 	if(y1%8==0) y=y1/8;   //????????8????
 	 else y=y1/8+1;
	for (i=0;i<k;i++) //???????
	{
		j = 0;
		for(y=y0;y<y1;y++) //??????,?????
		{
			OLED_SetPos(x0,y); //?????????
   			
			for(x=x0;x<x1;x++) //?x1 - x0 ?
	    		{
						
	    			OLED_WriteData(GIF[i][j++]);	//?????    	
	    		}
		}
		//delay_ms(80);//?????????

	}
}

