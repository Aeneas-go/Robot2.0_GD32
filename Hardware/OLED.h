#ifndef __OLED_H
#define __OLED_H
#include<stdint.h>
#ifdef __cplusplus
extern  "C"{
#endif
#define SDA_PIN GPIO_PIN_9
#define SCL_PIN GPIO_PIN_8

#define GPIO_PORT GPIOB


extern uint8_t debug_Line_Number;
void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line,uint8_t Column,char Char);
void OLED_ShowString(uint8_t Line,uint8_t Column,char *string);
void OLED_ShowNum(uint8_t Line,uint8_t Column,uint32_t Number,uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column,int32_t Number,uint8_t Length);
void Show_HexNum(uint8_t Line,uint8_t Column,uint32_t Number,uint8_t Length);
void OLED_ShowBinNum(uint8_t Line,uint8_t Column,uint32_t Number,uint8_t Length);
void debug(char* string);




#ifdef __cplusplus
}
#endif
#endif
