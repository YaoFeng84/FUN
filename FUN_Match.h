#ifndef FUN_Match_h
#define FUN_Match_h

#include "TypeMro.h"

#define EndSetValueMaxNum     5  //结束/设置 值最多个数
//#define MatchMaxLong          255//匹配数据最长字节数，字符串时不含\0

typedef struct
{
     u8 *cmdp;                //命令数据指针
     u16 cmdl;                //命令字节数，字符串时不含\0
     void (*Fp)(u8,u8 *,u16,void *);  //执行函数指针,为0表示表结束(参数1:为0时表示没有设置符，后面的参数2和参数3无意义
                                      //                         参数1:非0时表示存在设置符，参数2为设置数据首地址，参数3为设置数据字节数)
                                      //              参数4为回调函数的预留参数指针(一般是在需要回调函数里修改某个参数时，可以将这个参数通过这个指针来修改)
}MatchTableType;//匹配表类型

typedef struct
{
     u8 EnableFlag;                     //可用标志(0:不可用  非0:可用)
     MatchTableType *mttp;              //匹配表指针
     u16 cmdnum;                        //匹配条数
     u8 SetValueNum;                    //设置值个数(可以为0)
     u8 SetValue[EndSetValueMaxNum];    //设置值
     u8 EndValueNum;                    //结束值个数(不可为0)
     u8 EndValue[EndSetValueMaxNum];    //结束值
}MatchOPRType;//匹配操作类型-----描述匹配表的参数 及其 工作状态


typedef struct 
{
     MatchTableType *mttp;
     u8 SetValueNum;                    //设置值个数
     u8 SetValue[EndSetValueMaxNum];    //设置值
     u8 EndValueNum;                    //结束值个数
     u8 EndValue[EndSetValueMaxNum];    //结束值
     MatchOPRType *motp;
}MatchCNFType;//匹配配置类型-----配置时使用


extern s8 FUN_Match_Config(MatchCNFType *mctp);

extern s16 FUN_Match_Process(MatchOPRType *motp,u8 *cdp,u16 cdl,void *pp);


#endif
