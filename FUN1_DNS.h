#ifndef FUN1_DNS_h
#define FUN1_DNS_h

#include "TypeMro.h"

//����ӳ�������
#define MaxDonameNum                    (300)  //ӳ��������������������Ԫ��
#define MaxDonameSize                   (100)  //ÿ��������ֽ�������\0
#define MaxDonameSecNum                 (20)   //ÿ���������������� xxx.xxx.xxx.xxx.xxx ��ʾ5������


//����ӳ���ṹ
typedef struct 
{
     u8 *dnp;//������ָ��
     u8 ipp[4];//IP��ַ----�̶�4���ֽڣ���{192,168,1,1}
}DNSMap;
/*
DNSMap NIP[] = {//����3��Ԫ�ص�DNSӳ���
     {"www.123.com",{192,168,1,1}},
     {"xyj.abc.net",{192,168,1,2}},
     {0,{192,168,1,3}}//�����ַ���ָ��Ϊ0��ʾ�������IP��ַΪ����IP
};
*/

typedef struct
{
     DNSMap *dnsmp;//DNS������IPӳ���ָ��
     u8 EnableEndIP;//ʹ�ܽ���IP��־(0:��ֹʹ�ý���IP ��0:ʹ�ý���IP )����Ҫ��������û��ƥ�䵽ʱ�����ʹ�ܸñ�־����ͳһ���ؽ���IP��������ܸñ�־������Ӧ
     void (*DonameBackFun)(u8 *);//������Ϣ�ص�����ָ�룬����ͨ���ص����������û���ǰ�����ѯ������
                                 //�ص�����ֵ(0:��ʾ�պ���ָ�룬�������ص�������ֵΪ�ص�����ָ�룬�����ص�)
                                 //��1������Ϊ��ǰ�����ѯ�����ַ������׵�ַ(��\0��β)
}DNSServerOPRType;

typedef struct
{
     DNSServerOPRType *op;
     DNSMap *dnsmp;//DNS������IPӳ���ָ��
     u8 EnableEndIP;//ʹ�ܽ���IP��־(0:��ֹʹ�ý���IP ��0:ʹ�ý���IP )����Ҫ��������û��ƥ�䵽ʱ�����ʹ�ܸñ�־����ͳһ���ؽ���IP��������ܸñ�־������Ӧ
     void (*DonameBackFun)(u8 *);//������Ϣ�ص�����
}DNSServerCNFType;

extern s8 FUN1_DNS_Config(DNSServerCNFType *tp);
extern s8 FUN1_DNS_Process(DNSServerOPRType *bp,u8 *dp,u16 dl,u16 *pn,u8 *rp,u16 *rl);

#endif
