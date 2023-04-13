#ifndef FUN_Timer_h
#define FUN_Timer_h

#include "TypeMro.h"

typedef struct
{
     vu8 Enable;//��ʱ��ʹ��λ
     vu32 PreSysBaseTime;//���ڱ��浱ǰ��ϵͳ��׼ʱ��
     vu32 DelayTime;//���ڱ�����Ҫ��ʱ��ʱ��
}TIMERTYPE;//����һ����ʱ�����ƵĹؼ���

extern void FUN_Timer_INT(void);
extern s8 FUN_Timer_SetDelay(TIMERTYPE *Systimer,vu32 delaynumber);
extern s8 FUN_Timer_DetectDelay(TIMERTYPE *Systimer);
extern void FUN_Timer_CloseDelay(TIMERTYPE *Systimer);

#endif

