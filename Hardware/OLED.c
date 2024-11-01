#include "OLED.h"
#include "gd32e23x.h"                   // Device header
#include "OLED_Font.h"
typedef struct Str{
	char * str_data[6];
	uint8_t str_size;
}Str;
Str oled;

#define OLED_W_SCL(x) gpio_bit_write(GPIOB,SCL_PIN,(x))
#define OLED_W_SDA(x) gpio_bit_write(GPIOB,SDA_PIN,(x))

void OLED_I2C_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_mode_set(GPIO_PORT,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,SDA_PIN);
	gpio_mode_set(GPIO_PORT,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,SCL_PIN);
}

void iic_delay(uint32_t delay)
{
	for(uint32_t i=0;i<delay;i++)
	{
		__ASM("nop");
	}
}

void iic_start(void)
{
	OLED_W_SDA(1);
	OLED_W_SCL(1);
	OLED_W_SDA(0);
	OLED_W_SCL(0);
}
void iic_stop(void)
{
	OLED_W_SDA(0);
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

void iic_send_byte(uint8_t data) {
  uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(data & (0x80 >> i));
		OLED_W_SCL(1);
		OLED_W_SCL(0);
	}
	OLED_W_SCL(1);	//额外的一个时钟，不处理应答信号
	OLED_W_SCL(0);
}

uint8_t iic_read_byte(uint8_t ack) {
    uint8_t data = 0;
    gpio_bit_set(GPIO_PORT, SDA_PIN); // 释放SDA为输入
    for (int i = 0; i < 8; i++) {
        gpio_bit_reset(GPIO_PORT, SCL_PIN); // SCL置低
        iic_delay(10);
        gpio_bit_set(GPIO_PORT, SCL_PIN); // SCL置高
        if (gpio_input_bit_get(GPIO_PORT, SDA_PIN)) {
            data |= (1 << (7 - i)); // 读取数据
        }
        iic_delay(10);
    }
    if (ack) {
        gpio_bit_reset(GPIO_PORT, SDA_PIN); // 发送ACK
    } else {
        gpio_bit_set(GPIO_PORT, SDA_PIN);   // 发送NACK
    }
    gpio_bit_set(GPIO_PORT, SCL_PIN); // SCL置高
    iic_delay(10);
    gpio_bit_reset(GPIO_PORT, SCL_PIN); // SCL置低
    gpio_bit_reset(GPIO_PORT, SDA_PIN); // SDA准备为下一个数据
    return data;
	
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command)
{
	iic_start();
	iic_send_byte(0x78);
	iic_send_byte(0x00);
	iic_send_byte(Command);
	iic_stop();
}


void OLED_WriteData(uint8_t Data)
{
	iic_start();
	iic_send_byte(0x78);
	iic_send_byte(0x40);
	iic_send_byte(Data);
	iic_stop();
}

void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置低4位
}

void OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
}
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}
void debug(char* string)
{
	OLED_Clear();
	if(oled.str_size==5)
	{
		for(uint8_t i=0;i<4;i++)
		{
			oled.str_data[i]=oled.str_data[i+1];
		}
	}
	else
	{
		oled.str_size++;
	}
	oled.str_data[oled.str_size-1]=string;
	for(uint8_t i=0;i<oled.str_size;i++)
	{
		OLED_ShowString(i, 1, oled.str_data[i]);
	}
}

void OLED_Init(void)
{
	uint32_t i, j;
	
	iic_delay(1000);
	OLED_I2C_Init();			//端口初始化
	
	OLED_WriteCommand(0xAE);	//关闭显示
	
	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);
	
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
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭

	OLED_WriteCommand(0xA6);	//设置正常/倒转显示

	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//开启显示
		
	OLED_Clear();				//OLED清屏
}
