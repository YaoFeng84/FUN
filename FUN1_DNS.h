#ifndef FUN1_DNS_h
#define FUN1_DNS_h

#include "TypeMro.h"

//域名映射表属性
#define MaxDonameNum                    (300)  //映射表域名个数，含表结束元素
#define MaxDonameSize                   (100)  //每个域名最长字节数，含\0
#define MaxDonameSecNum                 (20)   //每个域名最大段数，如 xxx.xxx.xxx.xxx.xxx 表示5段域名


//域名映射表结构
typedef struct 
{
     u8 *dnp;//域名串指针
     u8 ipp[4];//IP地址----固定4个字节，如{192,168,1,1}
}DNSMap;
/*
DNSMap NIP[] = {//定义3个元素的DNS映射表
     {"www.123.com",{192,168,1,1}},
     {"xyj.abc.net",{192,168,1,2}},
     {0,{192,168,1,3}}//域名字符串指针为0表示表结束，IP地址为结束IP
};
*/

typedef struct
{
     DNSMap *dnsmp;//DNS域名和IP映射表指针
     u8 EnableEndIP;//使能结束IP标志(0:禁止使用结束IP 非0:使用结束IP )，主要用于域名没有匹配到时，如果使能该标志，则统一返回结束IP。如果禁能该标志，则不响应
     void (*DonameBackFun)(u8 *);//域名信息回调函数指针，用于通过回调函数告诉用户当前请求查询的域名
                                 //回调函数值(0:表示空函数指针，不产生回调，其他值为回调函数指针，产生回调)
                                 //第1个参数为当前请求查询域名字符串的首地址(以\0结尾)
}DNSServerOPRType;

typedef struct
{
     DNSServerOPRType *op;
     DNSMap *dnsmp;//DNS域名和IP映射表指针
     u8 EnableEndIP;//使能结束IP标志(0:禁止使用结束IP 非0:使用结束IP )，主要用于域名没有匹配到时，如果使能该标志，则统一返回结束IP。如果禁能该标志，则不响应
     void (*DonameBackFun)(u8 *);//域名信息回调函数
}DNSServerCNFType;

extern s8 FUN1_DNS_Config(DNSServerCNFType *tp);
extern s8 FUN1_DNS_Process(DNSServerOPRType *bp,u8 *dp,u16 dl,u16 *pn,u8 *rp,u16 *rl);

#endif
