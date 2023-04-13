#ifndef FUN1_SNMP_h
#define FUN1_SNMP_h

#include "TypeMro.h"

//
#define SNMP_MaxOIDSize            100       //����OID����ֽ�������\0
#define SNMP_MaxValueSize          200       //����Value����ֽ����������ַ�������\0
#define SNMP_SignelMaxOIDNum       4         //����SNMP���ݱ������?�е�OID Valuec�Եĸ���
#define SNMP_MaxCommunitySize      20        //����������ֽ�������\0
//
#define BER_TYPE_BOOLEAN		     0x01
#define BER_TYPE_INTEGER		     0x02
#define BER_TYPE_BIT_STRING	     0x03
#define BER_TYPE_OCTET_STRING	     0x04
#define BER_TYPE_NULL		     0x05
#define BER_TYPE_OID		     0x06
#define BER_TYPE_SEQUENCE          0x30
#define BER_TYPE_COUNTER		     0x41
#define BER_TYPE_GAUGE		     0x42
#define BER_TYPE_TIME_TICKS	     0x43
#define BER_TYPE_NO_SUCH_OBJECT    0x80
#define BER_TYPE_NO_SUCH_INSTANCE  0x81
#define BER_TYPE_END_OF_MIB_VIEW   0x82
#define BER_TYPE_SNMP_GET	     0xA0
#define BER_TYPE_SNMP_GETNEXT	     0xA1
#define BER_TYPE_SNMP_RESPONSE     0xA2
#define BER_TYPE_SNMP_SET	     0xA3
#define BER_TYPE_SNMP_GETBULK	     0xA5
#define BER_TYPE_SNMP_INFORM	     0xA6
#define BER_TYPE_SNMP_TRAP	     0xA7
#define BER_TYPE_SNMP_REPORT	     0xA8
//
#if SNMP_MaxValueSize >= SNMP_MaxOIDSize
#define SNMP_MaxBerSize            (SNMP_MaxValueSize + SNMP_MaxCommunitySize + 20)//����BER���뵥Ԫ(TLV)����ֽ���?
#else
#define SNMP_MaxBerSize            (SNMP_MaxOIDSize + SNMP_MaxCommunitySize + 20)//����BER���뵥Ԫ(TLV)����ֽ���?
#endif
#define SNMP_MaxMessageSize        (SNMP_MaxBerSize * SNMP_SignelMaxOIDNum + SNMP_MaxCommunitySize + 50)//(���PDU�ֽ���)SNMP��������ֽ���?

extern s32 FUN1_SNMP_Config(u8 *cp,MatchTableType *mttp);

extern s32 FUN1_SNMP_Process(u8 *dp,u16 dl,u16 *pn,u8 *rp,u16 *rl);
extern s32 FUN1_SNMP_AcceptAck(u8 *AckOIDp,u8 AckDataType,u8 *AckDatap,u16 AckDatal);



#endif
