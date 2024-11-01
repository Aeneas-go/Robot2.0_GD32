 /******************************************************************************
   * 测试硬件：立创开发板·GD32E230C8T6    使用主频72Mhz    晶振8Mhz
   * 版 本 号: V1.0
   * 修改作者: www.lckfb.com
   * 修改日期: 2023年11月02日
   * 功能介绍:      
   *****************************************************************************
   * 梁山派软硬件资料与相关扩展板软硬件资料官网全部开源  
   * 开发板官网：www.lckfb.com   
   * 技术支持常驻论坛，任何技术问题欢迎随时交流学习  
   * 立创论坛：club.szlcsc.com   
   * 其余模块移植手册：【立创·GD32E230C8T6开发板】模块移植手册
   * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
   * 不靠卖板赚钱，以培养中国工程师为己任
  ******************************************************************************/

#ifndef _BSP_ULTRASONIC_H_
#define _BSP_ULTRASONIC_H_

#include "gd32e23x.h"

#define RCU_TRIG RCU_GPIOA
#define PORT_TRIG GPIOA
#define GPIO_TRIG GPIO_PIN_6

#define RCU_ECHO RCU_GPIOA
#define PORT_ECHO GPIOA
#define GPIO_ECHO GPIO_PIN_7

void Ultrasonic_Init(void);//超声波初始化
float Hcsr04GetLength(void );//获取超声波测距的距离
float Get_distance(void);//获取上一次测距的距离

#endif

