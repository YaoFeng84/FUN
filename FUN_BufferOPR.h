#ifndef FUN_BufferOPR_h
#define FUN_BufferOPR_h

#include "TypeMro.h"
//#define BufferFlowControl

typedef struct
{
     u8 *BufferStartP;//缓存首地址指针
     u8 *BufferEndP;//缓存末地址指针
     u8 *BufferRP;//缓存读指针(能读出数据的地址)
     u8 *BufferWP;//缓存写指针(能写入数据的地址)
     u32 BufferLen;//缓存长度，单位字节
#ifdef BufferFlowControl     
     //以下是流控专用
     u32 MinStopReceSize;//最小停止接收的空闲字节数(空闲字节数小于该值时执行停止接收回调函数)
     u32 MaxStartReceSize;//最大开始接收的空闲字节数(空闲字节数大于该值时执行开始接收回调函数)
     void(*StopReceF)(void);//停止接收回调函数
     void(*StartReceF)(void);//使能接收回调函数
#endif     
}BufferOPRType;//缓存管理操作类型

typedef struct
{
     u8 *BufferStartP;//缓存首地址指针
     u32 BufferLen;//缓存长度，单位字节
     BufferOPRType *BufferCP;//缓存管理变量指针
#ifdef BufferFlowControl  
     //以下是流控专用
     u32 MinStopReceSize;//最小停止接收的空闲字节数(空闲字节数小于该值时执行停止接收回调函数)
     u32 MaxStartReceSize;//最大开始接收的空闲字节数(空闲字节数大于该值时执行开始接收回调函数)
     void(*StopReceF)(void);//停止接收回调函数
     void(*StartReceF)(void);//使能接收回调函数 
#endif     
}BufferCNFType;//缓存管理配置类型


extern s8 FUN_BufferOPR_Config(BufferCNFType *Bot);

extern u32 FUN_BufferOPR_WriteData(BufferOPRType *Bot,u8 *wdp,u32 wdl);

extern u32 FUN_BufferOPR_ReadData(BufferOPRType *Bot,u8 rm,u8 *rdp,u32 rdl);

extern u32 FUN_BufferOPR_MoveRP(BufferOPRType *Bot,u32 mbn);

extern void FUN_BufferOPR_SetWP(BufferOPRType *Bot,u32 nwp);

extern void FUN_BufferOPR_Reset(BufferOPRType *Bot);

extern u32 FUN_BufferOPR_GetFreeSize(BufferOPRType *Bot);

extern u32 FUN_BufferOPR_GetUseSize(BufferOPRType *Bot);


#endif

