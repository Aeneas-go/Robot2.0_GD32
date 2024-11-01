/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : mymath.c
  * @brief          : 我的计算函数
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
#include "mymath.h"
void initStack(struct Stack* s)
{
	s->top=-1;
}
uint8_t isEmpty(struct Stack* s)
{
	return s->top;
}
uint8_t isFull(struct Stack* s)
{
	return s->top==STACK_MAX-1;
}
void push(struct Stack* s,uint16_t item)
{
	if(isFull(s))
	{
		return;
	}
	s->num[++s->top]=item;
}
uint16_t pop(struct Stack*s)
{
	if(isEmpty(s))
	{
		return -1;
	}
	return s->num[s->top--];
}
uint16_t peek(struct Stack* s)
{
	if(isEmpty(s))
	{
		return -1;
	}
	return s->num[s->top];
}
struct Stack createStack()
{
	struct Stack*s;
	s->init=initStack;
	s->isEmpty=isEmpty;
	s->isFull=isFull;
	s->push=push;
	s->pop=pop;
	s->peek=peek;
	s->init(s);
	return (*s);
}
