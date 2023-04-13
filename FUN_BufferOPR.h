#ifndef FUN_BufferOPR_h
#define FUN_BufferOPR_h

#include "TypeMro.h"
//#define BufferFlowControl

typedef struct
{
     u8 *BufferStartP;//�����׵�ַָ��
     u8 *BufferEndP;//����ĩ��ַָ��
     u8 *BufferRP;//�����ָ��(�ܶ������ݵĵ�ַ)
     u8 *BufferWP;//����дָ��(��д�����ݵĵ�ַ)
     u32 BufferLen;//���泤�ȣ���λ�ֽ�
#ifdef BufferFlowControl     
     //����������ר��
     u32 MinStopReceSize;//��Сֹͣ���յĿ����ֽ���(�����ֽ���С�ڸ�ֵʱִ��ֹͣ���ջص�����)
     u32 MaxStartReceSize;//���ʼ���յĿ����ֽ���(�����ֽ������ڸ�ֵʱִ�п�ʼ���ջص�����)
     void(*StopReceF)(void);//ֹͣ���ջص�����
     void(*StartReceF)(void);//ʹ�ܽ��ջص�����
#endif     
}BufferOPRType;//��������������

typedef struct
{
     u8 *BufferStartP;//�����׵�ַָ��
     u32 BufferLen;//���泤�ȣ���λ�ֽ�
     BufferOPRType *BufferCP;//����������ָ��
#ifdef BufferFlowControl  
     //����������ר��
     u32 MinStopReceSize;//��Сֹͣ���յĿ����ֽ���(�����ֽ���С�ڸ�ֵʱִ��ֹͣ���ջص�����)
     u32 MaxStartReceSize;//���ʼ���յĿ����ֽ���(�����ֽ������ڸ�ֵʱִ�п�ʼ���ջص�����)
     void(*StopReceF)(void);//ֹͣ���ջص�����
     void(*StartReceF)(void);//ʹ�ܽ��ջص����� 
#endif     
}BufferCNFType;//���������������


extern s8 FUN_BufferOPR_Config(BufferCNFType *Bot);

extern u32 FUN_BufferOPR_WriteData(BufferOPRType *Bot,u8 *wdp,u32 wdl);

extern u32 FUN_BufferOPR_ReadData(BufferOPRType *Bot,u8 rm,u8 *rdp,u32 rdl);

extern u32 FUN_BufferOPR_MoveRP(BufferOPRType *Bot,u32 mbn);

extern void FUN_BufferOPR_SetWP(BufferOPRType *Bot,u32 nwp);

extern void FUN_BufferOPR_Reset(BufferOPRType *Bot);

extern u32 FUN_BufferOPR_GetFreeSize(BufferOPRType *Bot);

extern u32 FUN_BufferOPR_GetUseSize(BufferOPRType *Bot);


#endif

