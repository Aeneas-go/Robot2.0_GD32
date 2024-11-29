#ifndef _BSP_MQ2_H_
#define _BSP_MQ2_H_
 
#include "gd32e23x.h"


#define RCU_MQ2_GPIO_AO    	RCU_GPIOA
#define RCU_MQ2_GPIO_DO    	RCU_GPIOB

#define RCU_MQ2_ADC     	RCU_ADC0
#define RCU_MQ2_DMA     	RCU_DMA1

#define PORT_DMA        	DMA1
#define CHANNEL_DMA     	DMA_CH0

#define PORT_ADC     		ADC0
#define CHANNEL_ADC     	ADC_CHANNEL_1

#define PORT_MQ2_AO     	GPIOA
#define GPIO_MQ2_AO     	GPIO_PIN_1

#define PORT_MQ2_DO     	GPIOB
#define GPIO_MQ2_DO     	GPIO_PIN_1

 //采样次数
#define SAMPLES         30
//采样通道数 
#define CHANNEL_NUM     1


extern uint16_t gt_adc_val[ SAMPLES ][ CHANNEL_NUM ];  //DMA缓冲区
 
/************************
 
//之前的单路采集
void ADC_Init(void);
unsigned int Get_ADC_Value(void);
 
**************************/
void ADC_DMA_Init(void);
unsigned int Get_Adc_Dma_Value(char CHx);
 unsigned int Get_MQ2_Percentage_value(void);
unsigned int Get_MQ2_Percentage_value(void);

#endif