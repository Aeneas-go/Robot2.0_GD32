/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : Frame.h
  * @brief          : 框架头文件
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
#ifndef __FRAME_H__
#define __FRAME_H__
#include <stdint.h>

typedef void (*CallBackFunc)(void);
typedef void (*InitFunc)(void);
typedef void (*Action)(void);


void FrameEntry(void);
void CtrlEntry(CallBackFunc CtrlMode);
void BluetoochMode(void);
void BalanceMode(void);



#endif
