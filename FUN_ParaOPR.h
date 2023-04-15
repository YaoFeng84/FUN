#ifndef FUN_ParaOPR_h
#define FUN_ParaOPR_h

#include "TypeMro.h"

#define ParaTableMaxSize      255//每张参数表最大字节数

typedef struct
{
//-----需要配置的
     //变量相关     
     u32 ParaAreaStarAdd;//参数【区域】起始地址
     u32 ParaAreaSize;   //参数【区域】字节数  
     void *TabP;         //参数【表】空间首地址
     u32 ParaLabSize;    //参数【表】字节数
     u8 VerNum;          //参数【表】版本号
     u8 *CheckSumP;      //参数【表】校验码存放地址
     //回调函数相关
     void (*DefaultFunP)(void);         //阻塞型默认值回调函数
     s32 (*ReadFunP)(u32,u8 *,u16);     //阻塞型读回调函数(读起始地址,数据存放首地址,待读字节数)，(返回值 小于0:读失败 其他值:实际读到的字节数)
     s32 (*WriteFunP)(u32,u8 *,u16);    //阻塞型写回调函数(写起始地址,待写数据首地址,待写字节数)，(返回值 小于0:写失败 其他值:实际写入的字节数)
     s32 (*EraseFunP)(u32,u16);         //阻塞型擦除回调函数(擦除起始地址,擦除字节数)，(返回值 小于0:擦除失败 其他值:实际擦除的字节数)
//-----内部使用的
     u16 Intex;     
}ParaOPRType;

extern s8 FUN_ParaOPR_Read(ParaOPRType *tp);
extern s8 FUN_ParaOPR_Write(ParaOPRType *tp);

#endif

