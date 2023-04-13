#ifndef TypeMro_h
#define TypeMro_h

//���ļ�ΪоƬ���ı��������ض����ͷ�ļ�
//���ݲ�ͬ�ı�������������ı������������ͬ��

//---8λ����������ض���---
typedef signed char                     s8;
typedef unsigned char                   u8;
typedef volatile signed char            vs8;
typedef volatile unsigned char          vu8;
//---16λ����������ض���---
typedef signed short int                s16;
typedef volatile signed short int       vs16;
typedef unsigned short int              u16;
typedef volatile unsigned short int    vu16;
//---32λ����������ض���---
typedef signed int                      s32;
typedef volatile signed int             vs32;
typedef unsigned int                    u32;
typedef volatile unsigned int           vu32;
//---64λ����������ض���---
typedef signed long long                s64;
typedef volatile signed long long       vs64;
typedef unsigned long long              u64;
typedef volatile unsigned long long     vu64;

//---��������ľ��Ե�ַ---
//�� u8 Flag ABSADDRESS(0x50);��ʾ���������Ե�ַ������0x50��
#define ABSADDRESS(add)                 __attribute__((at(add)))
//
#define U16MAZVALUE                     65535     //u16��ֵ��
//
#define NULL_VP                         (void *)0
#define NULL_U8P                        (u8 *)0
#endif

