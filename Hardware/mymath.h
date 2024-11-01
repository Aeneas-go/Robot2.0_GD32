#ifndef __MYMATH_H__
#define __MYMATH_H__
#include<stdint.h>

#define STACK_MAX 50

typedef struct Stack
{
	uint8_t num[STACK_MAX];
	uint8_t top;
	
	void(*init)(struct Stack*);
	uint8_t(*isEmpty)(struct Stack*);
	uint8_t(*isFull)(struct Stack*);
	void (*push)(struct Stack*,uint16_t);
	uint16_t(*pop)(struct Stack*);
	uint16_t(*peek)(struct Stack*);
	
}Sta;

uint16_t HexadecimalToBinary(uint16_t hexa);
uint16_t BinaryToDecimalism();


#endif
