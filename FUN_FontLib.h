#ifndef FUN_FontLib_h
#define FUN_FontLib_h

//����ֵ������Ҫ�󲻿ɸ��ģ�Ŀǰֻ������Ŀ����ʹ�õ�����
//�������ݵĺ궨��ֵ�����вο������ļ���������
#include "TypeMro.h"

//�����Ͷ����ݺ���ָ��
//u32:��ȡ��ַ u8*:���ݴ�ŵ�ַ u16:��ȡ�ֽ���
//s32����ֵ:С��0��ȡʧ�� ����ֵ:ʵ�ʶ������ֽ���
typedef s32(*ReadFunP)(u32,u8 *,u16);

//�ֺŲ����궨�� m*n(mΪ��� nΪ�߶�)����λ��
#define FontLib_Size_8X12          1
#define FontLib_Size_12X20         2
#define FontLib_Size_16X24         3
#define FontLib_Size_24X32         4
#define FontLib_Size_32X48         5
#define FontLib_Size_14X19         6
#define FontLib_Size_14X25         7
#define FontLib_Size_21X27         8
#define FontLib_Size_5X7           20
#define FontLib_Size_7X9           21
#define FontLib_Size_9X17          22
#define FontLib_Size_12X24         23
#define FontLib_Size_48X48         24
#define FontLib_Size_6X24          25
#define FontLib_Size_8X24          26
#define FontLib_Size_20X24         27
#define FontLib_Size_36X24         28
#define FontLib_Size_16X16         29
#define FontLib_Size_24X24         30
#define FontLib_Size_32X32         31

//��������궨��
#define FontLib_Type_Roman         0    //��������
#define FontLib_Type_Sanserif      1    //
#define FontLib_Type_OCR_A         2
#define FontLib_Type_OCR_B         3
#define FontLib_Type_SongTi        32   //����
#define FontLib_Type_HeiTi         33   //����

//CodePage�����궨��
#define FontLib_CodePage_ASCII     0    //��׼ASCii
#define FontLib_CodePage_ICS       1    //�����ַ���
#define FontLib_CodePage_CP437     437
#define FontLib_CodePage_Katakana  20290
#define FontLib_CodePage_CP850     850
#define FontLib_CodePage_CP860     860
#define FontLib_CodePage_CP863     863
#define FontLib_CodePage_CP865     865
#define FontLib_CodePage_WCP1251   1251
#define FontLib_CodePage_CP866     866
#define FontLib_CodePage_CP862     862
#define FontLib_CodePage_WCP1252   1252
#define FontLib_CodePage_WCP1253   1253
#define FontLib_CodePage_CP852     852
#define FontLib_CodePage_CP858     858
#define FontLib_CodePage_CP737     737
#define FontLib_CodePage_ISO8859_1 28591//
#define FontLib_CodePage_GB18030   54936//����

//�����ַ��������궨��
#define FontLib_ICSN_USA                     0    //����
#define FontLib_ICSN_France                  1    //����
#define FontLib_ICSN_Germany                 2    //�¹�
#define FontLib_ICSN_UK                      3    //Ӣ��
#define FontLib_ICSN_DenmarkI                4    //����I
#define FontLib_ICSN_Sweden                  5    //���
#define FontLib_ICSN_Italy                   6    //�����
#define FontLib_ICSN_SpainI                  7    //������I
#define FontLib_ICSN_Japan                   8    //�ձ�
#define FontLib_ICSN_Norway                  9    //Ų��
#define FontLib_ICSN_DenmarkII               10   //����II
#define FontLib_ICSN_SpainII                 11   //������II
#define FontLib_ICSN_LatinAmerica            12   //��������
#define FontLib_ICSN_Korea                   13   //����
#define FontLib_ICSN_Slovenia                14   //˹��������
#define FontLib_ICSN_Chinese                 15   //�й�

//����ȡģ��ʽ�궨��
#define FontLib_ModeType_LRUD      0//(ˮƽȡģʽ)��->�ң���->��
#define FontLib_ModeType_RLUD      1//(ˮƽȡģʽ)��->����->��
#define FontLib_ModeType_LRDU      2//(ˮƽȡģʽ)��->�ң���->��
#define FontLib_ModeType_RLDU      3//(ˮƽȡģʽ)��->����->��
#define FontLib_ModeType_UDLR      4//(��ֱȡģʽ)��->�£���->��
#define FontLib_ModeType_UDRL      5//(��ֱȡģʽ)��->�£���->��
#define FontLib_ModeType_DULR      6//(��ֱȡģʽ)��->�ϣ���->��
#define FontLib_ModeType_DURL      7//(��ֱȡģʽ)��->�ϣ���->��


typedef struct
{
     u8 mode;       // ȡģ��ʽ��ȡֵ�ο�����ȡģ��ʽ�궨��
     u8 col;        // �����п���λ�� 
     u8 raw;        // �����иߣ���λ�� 
     u16 size;      // �ַ��������ֽ��� 
     u32 address;   // �ַ�������ʼ��ַ 
}FUN_FontLib_DEF_CharaINF;//�ַ���Ϣ�ṹ��



extern s8 FUN_FontLib_Config(ReadFunP rp,u32 fadd);

extern s8 FUN_FontLib_Set(u8 fontflag,u8 fontsize, u8 fonttype, u32 fontobj);

extern s8 FUN_FontLib_OutCharaINF(u8 *dp,u8 dl,FUN_FontLib_DEF_CharaINF *infp);

extern s8 FUN_FontLib_BaseSize_OutCharaINF(u8 fintsize,u8 *dp,u8 dl,FUN_FontLib_DEF_CharaINF *infp);

extern s8 FUN_FontLib_OutLibVersion(u8 *vp,u8 vl);



#endif

