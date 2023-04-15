#ifndef FUN_ParaOPR_h
#define FUN_ParaOPR_h

#include "TypeMro.h"

#define ParaTableMaxSize      255//ÿ�Ų���������ֽ���

typedef struct
{
//-----��Ҫ���õ�
     //�������     
     u32 ParaAreaStarAdd;//������������ʼ��ַ
     u32 ParaAreaSize;   //�����������ֽ���  
     void *TabP;         //���������ռ��׵�ַ
     u32 ParaLabSize;    //���������ֽ���
     u8 VerNum;          //���������汾��
     u8 *CheckSumP;      //��������У�����ŵ�ַ
     //�ص��������
     void (*DefaultFunP)(void);         //������Ĭ��ֵ�ص�����
     s32 (*ReadFunP)(u32,u8 *,u16);     //�����Ͷ��ص�����(����ʼ��ַ,���ݴ���׵�ַ,�����ֽ���)��(����ֵ С��0:��ʧ�� ����ֵ:ʵ�ʶ������ֽ���)
     s32 (*WriteFunP)(u32,u8 *,u16);    //������д�ص�����(д��ʼ��ַ,��д�����׵�ַ,��д�ֽ���)��(����ֵ С��0:дʧ�� ����ֵ:ʵ��д����ֽ���)
     s32 (*EraseFunP)(u32,u16);         //�����Ͳ����ص�����(������ʼ��ַ,�����ֽ���)��(����ֵ С��0:����ʧ�� ����ֵ:ʵ�ʲ������ֽ���)
//-----�ڲ�ʹ�õ�
     u16 Intex;     
}ParaOPRType;

extern s8 FUN_ParaOPR_Read(ParaOPRType *tp);
extern s8 FUN_ParaOPR_Write(ParaOPRType *tp);

#endif

