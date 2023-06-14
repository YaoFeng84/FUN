#ifndef FUN1_DNS_h
#define FUN1_DNS_h

#include "TypeMro.h"

#define MaxDonameSize                   (100)  //域名最长字节数，含\0
#define MaxDonameSecNum                 (10)    //域名段数，如 xxx.xxx.xxx.xxx.xxx 表示5段域名

//域名表结构
typedef struct 
{
     u8 *dnp;//域名串指针
     u8 ipp[4];//IP地址----固定4个字节（{0xc0,0xa8,0x01,0x01}：192.168.1.1）
}DNSMap;
/*
DNSMap NIP[] = {//定义3个元素的DNS映射表
     {"www.123.com",{0xc0,0xa8,0x01,0x01}},//192.168.1.1
     {"xyj.abc.net",{0xc0,0xa8,0x01,0x02}},//192.168.1.2
     {0,{0xc0,0xa8,0x01,0x03}}//这个是结束，也是如果域名在表中没有找到，则统一返回这个IP串
};
*/
typedef struct
{
     DNSMap *dnsmp;//DNS域名和IP映射表指针
}DNSServerOPRType;

typedef struct
{
     DNSServerOPRType *op;
     DNSMap *dnsmp;//DNS域名和IP映射表指针
}DNSServerCNFType;

extern s8 FUN1_DNS_Config(DNSServerCNFType *tp);

extern s8 FUN1_DNS_Process(DNSServerOPRType *bp,u8 *dp,u16 dl,u16 *pn,u8 *rp,u16 *rl);


#endif
