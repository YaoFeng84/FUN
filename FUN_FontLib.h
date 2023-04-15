#ifndef FUN_FontLib_h
#define FUN_FontLib_h

//以下值无特殊要求不可更改，目前只定义项目部分使用的内容
//其他内容的宏定义值可自行参考技术文件进行增加
#include "TypeMro.h"

//阻塞型读数据函数指针
//u32:读取地址 u8*:数据存放地址 u16:读取字节数
//s32返回值:小于0读取失败 其他值:实际读到的字节数
typedef s32(*ReadFunP)(u32,u8 *,u16);

//字号参数宏定义 m*n(m为宽度 n为高度)，单位点
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

//字体参数宏定义
#define FontLib_Type_Roman         0    //罗马字体
#define FontLib_Type_Sanserif      1    //
#define FontLib_Type_OCR_A         2
#define FontLib_Type_OCR_B         3
#define FontLib_Type_SongTi        32   //宋体
#define FontLib_Type_HeiTi         33   //黑体

//CodePage参数宏定义
#define FontLib_CodePage_ASCII     0    //标准ASCii
#define FontLib_CodePage_ICS       1    //国际字符集
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
#define FontLib_CodePage_GB18030   54936//中文

//国际字符集参数宏定义
#define FontLib_ICSN_USA                     0    //美国
#define FontLib_ICSN_France                  1    //法国
#define FontLib_ICSN_Germany                 2    //德国
#define FontLib_ICSN_UK                      3    //英国
#define FontLib_ICSN_DenmarkI                4    //丹麦I
#define FontLib_ICSN_Sweden                  5    //瑞典
#define FontLib_ICSN_Italy                   6    //意大利
#define FontLib_ICSN_SpainI                  7    //西班牙I
#define FontLib_ICSN_Japan                   8    //日本
#define FontLib_ICSN_Norway                  9    //挪威
#define FontLib_ICSN_DenmarkII               10   //丹麦II
#define FontLib_ICSN_SpainII                 11   //西班牙II
#define FontLib_ICSN_LatinAmerica            12   //拉丁美洲
#define FontLib_ICSN_Korea                   13   //朝鲜
#define FontLib_ICSN_Slovenia                14   //斯洛文尼亚
#define FontLib_ICSN_Chinese                 15   //中国

//点阵取模方式宏定义
#define FontLib_ModeType_LRUD      0//(水平取模式)左->右，上->下
#define FontLib_ModeType_RLUD      1//(水平取模式)右->左，上->下
#define FontLib_ModeType_LRDU      2//(水平取模式)左->右，下->上
#define FontLib_ModeType_RLDU      3//(水平取模式)右->左，下->上
#define FontLib_ModeType_UDLR      4//(垂直取模式)上->下，左->右
#define FontLib_ModeType_UDRL      5//(垂直取模式)上->下，右->左
#define FontLib_ModeType_DULR      6//(垂直取模式)下->上，左->右
#define FontLib_ModeType_DURL      7//(垂直取模式)下->上，右->左


typedef struct
{
     u8 mode;       // 取模方式，取值参考点阵取模方式宏定义
     u8 col;        // 点阵列宽，单位点 
     u8 raw;        // 点阵行高，单位点 
     u16 size;      // 字符点阵总字节数 
     u32 address;   // 字符点阵起始地址 
}FUN_FontLib_DEF_CharaINF;//字符信息结构体



extern s8 FUN_FontLib_Config(ReadFunP rp,u32 fadd);

extern s8 FUN_FontLib_Set(u8 fontflag,u8 fontsize, u8 fonttype, u32 fontobj);

extern s8 FUN_FontLib_OutCharaINF(u8 *dp,u8 dl,FUN_FontLib_DEF_CharaINF *infp);

extern s8 FUN_FontLib_BaseSize_OutCharaINF(u8 fintsize,u8 *dp,u8 dl,FUN_FontLib_DEF_CharaINF *infp);

extern s8 FUN_FontLib_OutLibVersion(u8 *vp,u8 vl);



#endif

