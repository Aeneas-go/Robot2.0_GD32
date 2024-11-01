/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : Action.c
  * @brief          : 运动规则函数文件
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

#include "gd32e23x.h"                   // Device header

#include "common.h"
#include "string.h"

#include "RobotServoController.h"
#include "Action.h"

uint16_t SPEED=SPEED_HIGH;
uint16_t* Robot_Posture=Robot_Posture_Begin;

const uint8_t Robot_ModuleArray[18]=
{
	rear_Left_joint,rear_Left_leg,rear_Left_foot,
	middle_Left_joint,middle_Left_leg,middle_Left_foot,
	front_Left_joint,front_Left_leg,front_Left_foot,
	
	rear_Right_joint,rear_Right_leg,rear_Right_foot,
	middle_Right_joint,middle_Right_leg,middle_Right_foot,
	front_Right_joint,front_Right_leg,front_Right_foot
};

uint16_t Robot_Low_Start[18]=
{
	522,321,//1，2
	206,
	525,351,//4，5
	255,
	487,359,//7，8
	191,
	500,663,//10，11
	805,
	499,706,//13，14
	808,
	483,662,//16，17
	793
};
uint16_t Robot_Posture_Begin[18];
uint8_t RobotjointArray[6]=//joint enum array
{
	rear_Left_joint,middle_Left_joint,front_Left_joint,
	rear_Right_joint,middle_Right_joint,front_Right_joint
};

int16_t Rotation_Param[6]=//rotation param
{
	80,80,80,80,80,80
};

uint8_t LeftJoint_Module[3] = {rear_Left_joint,middle_Right_joint,front_Left_joint};
uint8_t LeftLeg_Module[3] = {rear_Left_leg,middle_Right_leg,front_Left_leg};

uint8_t RightJoint_Module[3] = { rear_Right_joint,middle_Left_joint,front_Right_joint };
uint8_t RightLeg_Module[3] = { rear_Right_leg,middle_Left_leg,front_Right_leg };

int16_t Universal_Param[3]={0,0,0};//通用leg放下 angle数组

int16_t LeftLegUP_Param[3]={FrontLeftLeg_UP,MiddleRightLeg_UP,RearLeftLeg_UP};
int16_t RightLeg_UP_Param[3]={FrontRightLeg_UP,MiddleLeftLeg_UP,RearRightLeg_UP};

int16_t AdvanceLeftJointfront_Param[3]={FrontLeftJoint_front,MiddleRightJoint_front,RearLeftJoint_front};//前进 左边 joine 抬起 数组
int16_t AdvanceLeftJointrear_Param[3]={FrontLeftJoint_rear,MiddleRightJoint_rear,RearLeftJoint_rear};
int16_t AdvanceRightJointfront_Param[3]={FrontRightJoint_front,MiddleLeftJoint_front,RearRightJoint_front};
int16_t AdvanceRightJointrear_Param[3]={FrontRightJoint_rear,MiddleLeftJoint_rear,RearRightJoint_rear};

int16_t RetreatLeftJointfront_Param[3]={FrontLeftJoint_rear,MiddleRightJoint_rear,RearLeftJoint_rear};
int16_t RetreatLeftJointrear_Param[3]={FrontLeftJoint_front,MiddleRightJoint_front,RearLeftJoint_front};
int16_t RetreatRightJointfront_Param[3]={FrontRightJoint_rear,MiddleLeftJoint_rear,RearRightJoint_rear};
int16_t RetreatRightJointrear_Param[3]={FrontRightJoint_front,MiddleLeftJoint_front,RearRightJoint_front};

int16_t TurnLeftJointfront_Param[3]={FrontLeftJoint_rear,MiddleRightJoint_front,RearLeftJoint_rear};
int16_t TurnLeftJointrear_Param[3]={FrontLeftJoint_front,MiddleRightJoint_rear,RearLeftJoint_front};
int16_t TurnRightJointfront_Param[3]={FrontRightJoint_front,MiddleLeftJoint_rear,RearRightJoint_front};
int16_t TurnRightJointrear_Param[3]={FrontRightJoint_rear,MiddleLeftJoint_front,RearRightJoint_rear};


/**
  * @brief  Robot_Low_Action
  * @param  void
  * @retval  none
  */

void Robot_Action(void)
{
	for(uint8_t i=0;i<18;i++)
	{
		MoveServo(i+1,Robot_Posture_Begin[i],SPEED);
	}
}

/**
  * @brief  move function
* @param  Robot_Module:零部件枚举数组
* @param  Angle:移动角度数组
* @param  AttitudeMode: 舵机姿态模式
* @param number:移动舵机个数
  * @retval  none
  */
void _Move_Robot(uint8_t* Robot_Module,uint16_t* AttitudeMode,int16_t*Angle,uint8_t number,uint16_t Time)
{
	for(uint8_t i=0;i<number;i++)
	{
		MoveServo(Robot_Module[i],(uint16_t)(AttitudeMode[Robot_Module[i]-1]+Angle[i]),Time);
	}
}
/**
  * @brief  Move run logic
  * @param  dire 存放行动参数结构体
  * @retval  none
  */
void Move_Logic(Dire* dire)
{
	_Move_Robot(LeftJoint_Module,Robot_Posture,dire->LeftJointUPAngle,3,SPEED);//左边joint运行
	_Move_Robot(LeftLeg_Module,Robot_Posture,LeftLegUP_Param,3,SPEED);//左边leg抬起
	delay_ms(SPEED+1);
	_Move_Robot(RightJoint_Module,Robot_Posture,dire->RightJointDOWNAngle,3,SPEED);//右边joint放下
	delay_ms(SPEED);
	_Move_Robot(LeftLeg_Module,Robot_Posture,Universal_Param,3,SPEED);//左边leg放下
	
	_Move_Robot(LeftJoint_Module,Robot_Posture,dire->LeftJointDOWNAngle,3,SPEED);//左边joint放下
	_Move_Robot(RightLeg_Module,Robot_Posture,RightLeg_UP_Param,3,SPEED);//右边leg抬起
	delay_ms(SPEED+1);
	_Move_Robot(RightJoint_Module,Robot_Posture,dire->RightJointUPAngle,3,SPEED);//右边joint运行
	delay_ms(SPEED);
	_Move_Robot(RightLeg_Module,Robot_Posture,Universal_Param,3,SPEED);//右边leg放下
	delay_ms(10);
}

/**
  * @brief  Move advance code
  * @param  void
  * @retval  none
  */

void Move_Advance(void)
{	
	Dire _dire;
	_dire.LeftJointUPAngle=AdvanceLeftJointfront_Param;
	_dire.LeftJointDOWNAngle=AdvanceLeftJointrear_Param;
	_dire.RightJointUPAngle=AdvanceRightJointfront_Param;
	_dire.RightJointDOWNAngle=AdvanceRightJointrear_Param;
	Move_Logic(&_dire);
}

/**
  * @brief  move Retreat function
  * @param  void
  * @retval  none
  */

void Move_Retreat(void)
{
	Dire _dire;
	_dire.LeftJointUPAngle=RetreatLeftJointfront_Param;
	_dire.LeftJointDOWNAngle=RetreatLeftJointrear_Param;
	_dire.RightJointUPAngle=RetreatRightJointfront_Param;
	_dire.RightJointDOWNAngle=RetreatRightJointrear_Param;
	Move_Logic(&_dire);
}

/**
  * @brief  move turn lrft function
  * @param  void
  * @retval  none
  */

void Move_Left_Turn(void)
{
	Dire _dire;
	_dire.LeftJointUPAngle=TurnLeftJointfront_Param;
	_dire.LeftJointDOWNAngle=TurnLeftJointrear_Param;
	_dire.RightJointUPAngle=TurnRightJointfront_Param;
	_dire.RightJointDOWNAngle=TurnRightJointrear_Param;
	Move_Logic(&_dire);
}

/**
  * @brief  move turn right function
  * @param  void
  * @retval  none
  */
void Move_Right_Turn(void)
{
	Dire _dire;
	_dire.LeftJointUPAngle=TurnLeftJointrear_Param;
	_dire.LeftJointDOWNAngle=TurnLeftJointfront_Param;
	_dire.RightJointUPAngle=TurnRightJointrear_Param;
	_dire.RightJointDOWNAngle=TurnRightJointfront_Param;
	Move_Logic(&_dire);
}
/**
  * @brief  side move left
  * @param  void
  * @retval  none
  */
void Move_Side_Left(void)
{
	
}

/**
  * @brief  side move right
  * @param  void
  * @retval  none
  */
void Move_Side_Right(void)
{
	
}

/**
  * @brief  move rotation function
  * @param  void
  * @retval  none
  */
void Move_Rotation(void)
{
	_Move_Robot(RobotjointArray,Robot_Posture,Rotation_Param,6,200);
	delay_ms(202);
	for(uint8_t i=0;i<6;i++)
	{
		Rotation_Param[i]=-Rotation_Param[i];
	}
	delay_ms(200);
	_Move_Robot(RobotjointArray,Robot_Posture,Rotation_Param,6,200);
	delay_ms(202);
	for(uint8_t i=0;i<6;i++)
	{
		Rotation_Param[i]=-Rotation_Param[i];
	}
	delay_ms(200);
}
void Set_Posture_High(void){
	for(uint8_t i=1;i<19;i++)
	{
		if(i==2||i==5||i==3||i==6||i==8||i==9){Robot_Posture_Begin[i-1]=Robot_Low_Start[i-1]+180;}
		else if(i==11||i==12||i==14||i==15||i==17||i==18){Robot_Posture_Begin[i-1]=Robot_Low_Start[i-1]-180;}
		else continue;
	}
	Move_Stop();
	delay_ms(SPEED);
}
void Set_Posture_Midium(void){
	for(uint8_t i=1;i<19;i++)
	{
		if(i==2||i==5||i==3||i==6||i==8||i==9){Robot_Posture_Begin[i-1]=Robot_Low_Start[i-1]+100;}
		else if(i==11||i==12||i==14||i==15||i==17||i==18){Robot_Posture_Begin[i-1]=Robot_Low_Start[i-1]-100;}
		else continue;
	}
	Move_Stop();
	delay_ms(SPEED);
}
void Set_Posture_Low(void){
	for(uint8_t i=0;i<18;i++)
	{Robot_Posture_Begin[i]=Robot_Low_Start[i];}
	Move_Stop();delay_ms(SPEED);
}		
void Set_Speed_High(void){SPEED=SPEED_HIGH;}
void Set_Speed_Midium(void){SPEED=SPEED_MIDIUM;}
void Set_Speed_Low(void){SPEED=SPEED_LOW;}
/**
  * @brief  move stop
  * @param  void
  * @retval none
  */
void Move_Stop(void)
{
	delay_ms(200);
	Robot_Action();
}
