#ifndef FUN_Timer_h
#define FUN_Timer_h

#include "TypeMro.h"

typedef struct
{
     vu8 Enable;//定时器使能位
     vu32 PreSysBaseTime;//用于保存当前的系统基准时间
     vu32 DelayTime;//用于保存所要定时的时间
}TIMERTYPE;//申明一个定时器名称的关键字

extern void FUN_Timer_INT(void);
extern s8 FUN_Timer_SetDelay(TIMERTYPE *Systimer,vu32 delaynumber);
extern s8 FUN_Timer_DetectDelay(TIMERTYPE *Systimer);
extern void FUN_Timer_CloseDelay(TIMERTYPE *Systimer);

#endif

