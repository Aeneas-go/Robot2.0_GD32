/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : BlueSerial.h
  * @brief          : Bluetooch Serial 
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024-aeneas
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE BEGIN Header */

#ifndef _BSP_BLUETOOTH_H_
#define _BSP_BLUETOOTH_H_
 
#include "gd32e23x.h" 
#include "string.h"  
#include "systick.h"

//是否开启串口0调试     1开始  0关闭
#define     DEBUG   1


#define  BLERX_LEN_MAX  200 

//蓝牙的TXD端要接的线
#define BLUETOOTH_TXD_RCU                	RCU_GPIOA
#define BLUETOOTH_TXD_PORT                	GPIOA
#define BLUETOOTH_TXD_GPIO                	GPIO_PIN_3

//蓝牙的RXD端要接的线
#define BLUETOOTH_RXD_RCU                	RCU_GPIOA
#define BLUETOOTH_RXD_PORT                	GPIOA
#define BLUETOOTH_RXD_GPIO                	GPIO_PIN_2
	
#define BLUETOOTH_RXD_RCU                	RCU_GPIOA
#define BLUETOOTH_RXD_PORT                	GPIOA
#define BLUETOOTH_RXD_GPIO                	GPIO_PIN_2

//串口配置
#define BLE_USART_RCU           			RCU_USART1
#define BLE_USART                        	USART1                                     
#define BLE_USART_AF                    	GPIO_AF_1  
#define BLE_USART_IRQ                   	USART1_IRQn                                                         
#define BLE_USART_IRQHandler    			USART1_IRQHandler                                                

//连接成功指示引脚
#define BLUETOOTH_LINK_RCU                RCU_GPIOC
#define BLUETOOTH_LINK_PORT                GPIOC
#define BLUETOOTH_LINK_GPIO                GPIO_PIN_2

#define BLUETOOTH_LINK  gpio_input_bit_get(BLUETOOTH_LINK_PORT, BLUETOOTH_LINK_GPIO)        

#define CONNECT                 1       //蓝牙连接成功
#define DISCONNECT          0                //蓝牙连接断开


void Bluetooth_Init(void);
void Clear_BLERX_BUFF(void);
unsigned char Get_Bluetooth_ConnectFlag(void);
void Bluetooth_Mode(void);
void Receive_Bluetooth_Data(void);
void BLE_send_String(unsigned char *str);
#endif

