/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : Frame.c
  * @brief          : 框架定义文件
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


#include "Frame.h"
#include <string.h>
#include "bsp_usart.h"
#include "bluetooth.h"
#include <stdio.h>
#include <stdlib.h>
#include "RobotServoController.h"
#include "Action.h"
#include "OLED.h"
#include "bsp_ultrasonic.h"

extern unsigned char Bluetooth_ConnectFlag; //蓝牙连接状态 =0没有手机连接                =1有手机连接
extern char BLERX_BUFF[BLERX_LEN_MAX];
extern unsigned char BLERX_FLAG;
uint8_t lengthflag=0;
static uint8_t actnumber=0;
enum SelectAction{
	Stop=0,
	Forward=1,
	Retreat=2,
	Rotation=3,
	Speed_High=4,
	Speed_Midium=5,
	Speed_Low=6,
	Posture_Midium=7,
	Posture_Low=8,
	Posture_High=9,
	Turn_Left=10,
	Turn_Right=11,
	Side_Left=12,
	Side_Right=13,
	test=14
	
}SelectAction;
enum CtrlMode
{
	BlueTooch=0,
	WiFi,
	Hander
}CtrlMode;

void testfunc(void)
{
    /* enable the LED1 GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* configure LED1 GPIO port */ 
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
	
	
	
    /* reset LED1 GPIO pin */
	gpio_bit_set(GPIOA,GPIO_PIN_5);
}
/**
  * @brief  action callback 函数指针枚举
  * @param  none
  * @retval none
  */


Action actionenum[15]=
{
	Move_Stop,Move_Advance,Move_Retreat,Move_Rotation,
	Set_Speed_High,Set_Speed_Midium,Set_Speed_Low,
	Set_Posture_Midium,Set_Posture_Low,Set_Posture_High,
	Move_Left_Turn,Move_Right_Turn,Move_Side_Left,Move_Side_Right,
	testfunc,
};
/**
  * @brief  HexapodRobot 初始化函数 包括模块的初始化函数
  * @param  Robot_Positure_Init   动作初始化函数，传入需要时初始化是表现得动作
  * @retval  none
  */
void HexapodRobot_Init(InitFunc Robot_Positure_Init)
{ 
#define refresh
	for(uint8_t i=0;i<18;i++)
	{
		Robot_Posture_Begin[i]=Robot_Low_Start[i];
	}
	OLED_Init();
	systick_config();
	usart_gpio_config(9600);
	Ultrasonic_Init();
	Robot_Positure_Init();
}
/**
  * @brief  框架入口函数
  * @param  none
  * @retval  none
  */
void FrameEntry()
{
	HexapodRobot_Init(Robot_Action);
	delay_ms(2);
	debug("Frame");
	while(1)
	{
		
		CtrlEntry(BluetoochMode);
		//test();
	}
}
/**
  * @brief  控制模式入口函数
  * @param  传入控制的模式函数
  * @retval  none
  */
void CtrlEntry(CallBackFunc CtrlMode)
{
	CtrlMode();
}
/**
  * @brief  Action
* @param  action:函数数组索引
  * @retval  none
  */
void ActionCallBack(uint8_t action)
{
	actionenum[action]();
}
/**
  * @brief  蓝牙模式
  * @param  none
  * @retval  none
  */
void BluetoochMode(void)
{
	Bluetooth_Init();
	//debug("Bluetooch");
	uint8_t i=0;
	
	while(1)
	{
		Bluetooth_Mode();
		//if(lengthflag!=1)
			ActionCallBack(actnumber);
        OLED_ShowNum(3,1,actnumber,4);     
		if( BLERX_FLAG == 1 )//接收到蓝牙数据    
		{
                    
			BLERX_FLAG = 0;
					//显示蓝牙发送过来的数据
					//BLERX_BUFF为数据包
					//usart_send_string(BLERX_BUFF);
					//printf("data = %s\r\n",BLERX_BUFF);
					//strcpy(PACKET,BLERX_BUFF);
			actnumber=atoi(BLERX_BUFF);		
			Clear_BLERX_BUFF();//清除接收缓存		
         }
	}
}
void WIFIMode(void)
{
	
}
/**
  * @brief  步态选择函数
  * @param  none
  * @retval  none
  */
void SwitchGait()
{
	
	return;
}

