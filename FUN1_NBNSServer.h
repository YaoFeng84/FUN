#ifndef FUN1_NBNSServer_h
#define FUN1_NBNSServer_h

#include "TypeMro.h"

#define DomainNameMaxSize     (15)   //域名最长字节数(协议规定)
#define DomainNameMaxIP       (5)    //最多一个域名有5个IP地址

typedef struct
{
     u8 DomainNameCode[DomainNameMaxSize * 2 + 3];     //域名编码(加3，其中2个字节是用于存放域名的属性，1个字节是用于存放\0)
     u8 DomainNameSize;                                //域名编码有效字节数，不含\0
     void (*GetIP)(u8 *,u16 *);                         //获取IP地址的回调函数(参数: u8*为存放空间首地址 u8*(输入)为存放空间字节数 (输出)为实际有效字节数)（存放格式：0xc0 0xa8 0x01 0x07表示192.168.1.7）
}NbnsServerOPRType;

typedef struct
{
     NbnsServerOPRType *op;
     u8 *dnp;//域名字符串指针(要以\0结尾，内容不超过15字节)
     void (*GetIP)(u8 *,u16 *);//获取IP地址的回调函数
}NbnsServerCNFType;

extern s8 FUN1_NBNSServer_Config(NbnsServerCNFType *tp);
extern s8 FUN1_NBNSServer_Process(NbnsServerOPRType *tp,u8 *dp,u16 dl,u16 *pn,u8 *rp,u16 *rl);

#endif
