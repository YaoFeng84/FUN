#ifndef FUN_Match_h
#define FUN_Match_h

#include "TypeMro.h"

#define EndSetValueMaxNum     5  //����/���� ֵ������
//#define MatchMaxLong          255//ƥ��������ֽ������ַ���ʱ����\0

typedef struct
{
     u8 *cmdp;                //��������ָ��
     u16 cmdl;                //�����ֽ������ַ���ʱ����\0
     void (*Fp)(u8,u8 *,u16,void *);  //ִ�к���ָ��,Ϊ0��ʾ�����(����1:Ϊ0ʱ��ʾû�����÷�������Ĳ���2�Ͳ���3������
                                      //                         ����1:��0ʱ��ʾ�������÷�������2Ϊ���������׵�ַ������3Ϊ���������ֽ���)
                                      //              ����4Ϊ�ص�������Ԥ������ָ��(һ��������Ҫ�ص��������޸�ĳ������ʱ�����Խ��������ͨ�����ָ�����޸�)
}MatchTableType;//ƥ�������

typedef struct
{
     u8 EnableFlag;                     //���ñ�־(0:������  ��0:����)
     MatchTableType *mttp;              //ƥ���ָ��
     u16 cmdnum;                        //ƥ������
     u8 SetValueNum;                    //����ֵ����(����Ϊ0)
     u8 SetValue[EndSetValueMaxNum];    //����ֵ
     u8 EndValueNum;                    //����ֵ����(����Ϊ0)
     u8 EndValue[EndSetValueMaxNum];    //����ֵ
}MatchOPRType;//ƥ���������-----����ƥ���Ĳ��� ���� ����״̬


typedef struct 
{
     MatchTableType *mttp;
     u8 SetValueNum;                    //����ֵ����
     u8 SetValue[EndSetValueMaxNum];    //����ֵ
     u8 EndValueNum;                    //����ֵ����
     u8 EndValue[EndSetValueMaxNum];    //����ֵ
     MatchOPRType *motp;
}MatchCNFType;//ƥ����������-----����ʱʹ��


extern s8 FUN_Match_Config(MatchCNFType *mctp);

extern s16 FUN_Match_Process(MatchOPRType *motp,u8 *cdp,u16 cdl,void *pp);


#endif
