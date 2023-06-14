#ifndef FUN2_ParaWebServer_h
#define FUN2_ParaWebServer_h

#include "TypeMro.h"
#include "FUN_BufferOPR.h"
#include "FUN_Match.h"
#include "FUN1_HTTPServer.h"

typedef struct
{     
//---------- 以下是配置时，需要提供数据的对象变量 ----------
     BufferOPRType ReceBuffCtrl;   //接收缓存控制器  (接收缓存大小要大等于操作缓存大小，取值标准要保证能放下请求头中最大的一个键值对，如cookie键值对等，建议1K或以上)
     BufferOPRType SendBuffCtrl;   //发送缓存控制器  (发送缓存大小要大等于操作缓存大小，取值标准要保证能放下请求头中最大的一个键值对，如cookie键值对等，建议1K或以上)
     u8 *OperationBufferP;         //操作缓存指针
     u32 OperationBufferSize;      //操作缓存字节数  (空间大小要保证能放下请求头中最大的一个键值对，如cookie键值对等，建议1K或以上)
     //成功页信息回调函数附加信息数据固定为OK
     void (*SuccessPageInfFunP)(void *,u8 *,u16);//成功页面信息回调函数(参数: ParaWebServerOPRType型指针,附带信息数据首地址，附带信息数据字节数)
     //失败页信息回调函数的附加信息数据(ERR：表示未知参数   其他：表示指定参数值越界了)
     void (*FailPageInfFunP)(void *,u8 *,u16);//失败页面信息回调函数(参数: ParaWebServerOPRType型指针 ,附带信息数据首地址，附带信息数据字节数)
     //路径错误(404页)信息回调函数
     void (*PathErrInfFunP)(void *,u8 *,u16);//路径错误(404页面)信息回调函数(参数: ParaWebServerOPRType型指针 ,附带信息数据首地址，附带信息数据字节数)
//---------- 以下是运行时，根据情况在回调时进行修改 ----------
     s32(*GetDataFunP)(u8 *,u16,u8);//读数据到响应体的函数指针(参数:存放空间首地址,希望读取字节数,读取模式(0:从头读 非0:继续读))(返回值：小于-1:出错   -1:等待数据   0:无数据(结束)   大于0:具体读到字节数)
     //参数设置回调函数数据格式(键名=键值)
     s8 (*ParaSetFunP)(u8 *,u16);  //参数设置回调函数指针(参数:数据首地址，数据字节数)(返回值：返回小于-2:未知参数   -1:参数值越界   0:参数OK)
     HttpServerOPRType HttpOpr;    //Http服务器操作对象
//---------- 以下是内部的对象变量，禁止操作!!! ----------
     MatchOPRType WebPathMatch;    //路径表匹配对象
     MatchOPRType ParaTableMatch;  //参数表匹配对象     
     u8 QuestProcessFailFlag;      //请求处理失败标志     
     u8 WebStateMachine;           //操作状态机     
     u16 UrlSize,UrlIntex;         //Url字节数(不含\0),Url起始处理索引号     
     u32 AckOprCount,QuestOprCount;//响应操作计数，请求操作计数
}ParaWebServerOPRType;//参数Web服务模块操作类型

typedef struct
{
     u8 *rp;//接收空间首地址
     u32 rl;//接收空间字节数
     u8 *sp;//发送空间首地址
     u32 sl;//发送空间字节数
     u8 *op;//操作空间首地址
     u32 ol;//操作空间字节数
     MatchTableType *PathTableP;//路径表指针
     MatchTableType *ParaTableP;//参数表指针
     ParaWebServerOPRType *pwsotp;//操作对象指针
     void (*SuccessPageInfFunP)(void *,u8 *,u16);//成功页面信息回调函数
     void (*FailPageInfFunP)(void *,u8 *,u16);//失败页面信息回调函数
     void (*PathErrInfFunP)(void *,u8 *,u16);//路径错误(404页面)信息回调函数
}ParaWebServerCNFType;//参数Web服务模块配置类型

extern s8 FUN2_ParaWebServer_Config(ParaWebServerCNFType *tp);
extern void FUN2_ParaWebServer_Process(ParaWebServerOPRType *tp);

#endif
