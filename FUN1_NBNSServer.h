#ifndef FUN1_NBNSServer_h
#define FUN1_NBNSServer_h

#include "TypeMro.h"

#define DomainNameMaxSize     (15)   //������ֽ���(Э��涨)
#define DomainNameMaxIP       (5)    //���һ��������5��IP��ַ

typedef struct
{
     u8 DomainNameCode[DomainNameMaxSize * 2 + 3];     //��������(��3������2���ֽ������ڴ�����������ԣ�1���ֽ������ڴ��\0)
     u8 DomainNameSize;                                //����������Ч�ֽ���������\0
     void (*GetIP)(u8 *,u16 *);                         //��ȡIP��ַ�Ļص�����(����: u8*Ϊ��ſռ��׵�ַ u8*(����)Ϊ��ſռ��ֽ��� (���)Ϊʵ����Ч�ֽ���)����Ÿ�ʽ��0xc0 0xa8 0x01 0x07��ʾ192.168.1.7��
}NbnsServerOPRType;

typedef struct
{
     NbnsServerOPRType *op;
     u8 *dnp;//�����ַ���ָ��(Ҫ��\0��β�����ݲ�����15�ֽ�)
     void (*GetIP)(u8 *,u16 *);//��ȡIP��ַ�Ļص�����
}NbnsServerCNFType;

extern s8 FUN1_NBNSServer_Config(NbnsServerCNFType *tp);
extern s8 FUN1_NBNSServer_Process(NbnsServerOPRType *tp,u8 *dp,u16 dl,u16 *pn,u8 *rp,u16 *rl);

#endif
