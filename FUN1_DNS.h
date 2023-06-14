#ifndef FUN1_DNS_h
#define FUN1_DNS_h

#include "TypeMro.h"

#define MaxDonameSize                   (100)  //������ֽ�������\0
#define MaxDonameSecNum                 (10)    //������������ xxx.xxx.xxx.xxx.xxx ��ʾ5������

//������ṹ
typedef struct 
{
     u8 *dnp;//������ָ��
     u8 ipp[4];//IP��ַ----�̶�4���ֽڣ�{0xc0,0xa8,0x01,0x01}��192.168.1.1��
}DNSMap;
/*
DNSMap NIP[] = {//����3��Ԫ�ص�DNSӳ���
     {"www.123.com",{0xc0,0xa8,0x01,0x01}},//192.168.1.1
     {"xyj.abc.net",{0xc0,0xa8,0x01,0x02}},//192.168.1.2
     {0,{0xc0,0xa8,0x01,0x03}}//����ǽ�����Ҳ����������ڱ���û���ҵ�����ͳһ�������IP��
};
*/
typedef struct
{
     DNSMap *dnsmp;//DNS������IPӳ���ָ��
}DNSServerOPRType;

typedef struct
{
     DNSServerOPRType *op;
     DNSMap *dnsmp;//DNS������IPӳ���ָ��
}DNSServerCNFType;

extern s8 FUN1_DNS_Config(DNSServerCNFType *tp);

extern s8 FUN1_DNS_Process(DNSServerOPRType *bp,u8 *dp,u16 dl,u16 *pn,u8 *rp,u16 *rl);


#endif
