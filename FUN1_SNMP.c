/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------������ģ����޸ļ�¼��-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * ���ݣ�
 * ���ڣ�2023-02-03
 * ���ߣ�YJX
 * �汾�ţ�V1.0�����棩
 ***********************************************
 * �޸����ݣ�
 * �޸����ڣ�
 * �޸����ߣ�
 * �汾�ţ�
 ***********************************************
*/
/********************************************************************************************************************************************
*                                                                                                                                           *
*                ---------------------------------������ģ���ʹ��˵����-----------------------------------------                           *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*                                       �ص�˵����
          
*/
#include "ProHeadFile.h"
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------����ģ���ĶԽӺ�����-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
#define SNMP_S32ToStr(N,P,L)                 FUN_String_S32ToStr(N,P,L)
#define SNMP_StrLen(SP)                      FUN_String_StrLen(SP)
#define SNMP_StrIsRame(P1,P2)                FUN_String_STRIsSame(P1,P2)
#define SNMP_StrToS32(SP,NP)                 FUN_String_StrToS32(SP,NP)
//
#define SNMP_MIBConfig(T)                    FUN_Match_Config(T)
#define SNMP_MIBMatch(T,P,L,PP)              FUN_Match_Process(T,P,L,PP)
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ı���������--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ڵĺ궨����------------------------------------                                  *
*                                                                                                                                           *
********************************************************************************************************************************************/
//
#define SNMP_VersionNum                 1    //SNMP�汾��(1:V1)
//�������ֵ�궨��
#define SNMP_DecodeErr_DataType         -40  //���������쳣
#define SNMP_DecodeErr_DataSizeMax      -41  //����������
#define SNMP_DecodeErr_DataSizeLess     -42  //����������
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ڵı������Ͷ�����------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
typedef struct
{
     u8 datatype;//��������,���ڱ�����Ӧ����������
     u8 oid[SNMP_MaxOIDSize];
     u16 oidl; //oid�����ֽ���������󷵻���Ч�ֽ���������\0
     u8 value[SNMP_MaxValueSize];
     u16 valuel;//value�����ֽ���������󷵻���Ч�ֽ���������\0
}OIDV;

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����ڲ�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static s32 SNMP_Decode(u8 *dp,u16 dl,u8 *qcmd,u32 *qid,OIDV *op,u16 *processsize);
static s32 SNMP_Encode(u32 qin,OIDV *op,u8 *dp,u16 *dl);
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ڵ� ���� �� �궨�� ������--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static MatchOPRType Snmp_Mot;
static u8 DataTypeList[] = {
     BER_TYPE_BOOLEAN,
     BER_TYPE_INTEGER,
     BER_TYPE_BIT_STRING,
     BER_TYPE_OCTET_STRING,
     BER_TYPE_NULL,
     BER_TYPE_OID,
     BER_TYPE_SEQUENCE,
     BER_TYPE_COUNTER,
     BER_TYPE_GAUGE,
     BER_TYPE_TIME_TICKS,
     BER_TYPE_NO_SUCH_OBJECT,
     BER_TYPE_NO_SUCH_INSTANCE,
     BER_TYPE_END_OF_MIB_VIEW,
     BER_TYPE_SNMP_GET,
     BER_TYPE_SNMP_GETNEXT,
     BER_TYPE_SNMP_RESPONSE,
     BER_TYPE_SNMP_SET,
     BER_TYPE_SNMP_GETBULK,
     BER_TYPE_SNMP_INFORM,
     BER_TYPE_SNMP_TRAP,
     BER_TYPE_SNMP_REPORT
};
static OIDV OidOprList[SNMP_SignelMaxOIDNum];//OidValue������
static u8 OidValueListOprIntex;//OidValue�����������
static s8 AckErrFlag;//Ӧ������־

static u8 *SaveDataP;//��������ݴ�ſռ��׵�ַ
static u16 *SaveDataL;//��������ݴ�ſռ䳤�ȼ�����ֵ
static u32 QuestNum;//���������
static s32 EncodeErrv;//�������״̬
static u8 *CommunityNameP;//�������ַ���ָ��
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ϵͳ����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
������: FUN1_SNMP_Config
��   ��: SNMP����
��   ��:
       u8 *cp: �������ַ����׵�ַ����\0��β
����ֵ:
       ����0:���óɹ�
       С��0:����ʧ��
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-02-07   
-------------------------------------------------*/
s32 FUN1_SNMP_Config(u8 *cp,MatchTableType *mttp)
{
     MatchCNFType mct;

     mct.motp = &Snmp_Mot;
     mct.mttp = mttp;
     mct.SetValueNum = 0;
     mct.EndValueNum = 3;
     mct.EndValue[0] = 0x00;
     mct.EndValue[1] = 0x0d;
     mct.EndValue[2] = 0x0a;
     SNMP_MIBConfig(&mct);
     CommunityNameP = cp;
     return 0;
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����û�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
������: FUN1_SNMP_Process
��   ��: SNMP���ݴ���
��   ��:
       u8 *dp: �����������׵�ַ
       u16 dl: �����������ֽ���
       u16 *pn���Ѵ����ֽ���
       u8 *rp: ��Ӧ���ݿռ��׵�ַ
       u16 *rl: ��Ӧ���ݿռ��ֽ�����������Ӧ������Ч�ֽ���
����ֵ:
       ����0:����ɹ�
       С��0:����ʧ��
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-02-07   
-------------------------------------------------*/
s32 FUN1_SNMP_Process(u8 *dp,u16 dl,u16 *pn,u8 *rp,u16 *rl)
{
     u8 qcmd,u8temp;     
     u8 setopr[SNMP_MaxOIDSize + SNMP_MaxValueSize + 2];
     u16 u16setintex,u16temp;
     u32 qidn;

     *pn = 0;

     //��OidValue����г�ʼ��
     for(u8temp = 0;u8temp < SNMP_SignelMaxOIDNum;u8temp++)
     {
          OidOprList[u8temp].oidl = SNMP_MaxOIDSize;
          OidOprList[u8temp].valuel = SNMP_MaxValueSize;
     }

     //���н��룬�����N��OID���ֱ���N��(OID + VALUE�Ľṹ�建��)
     if(SNMP_Decode(dp,dl,&qcmd,&qidn,OidOprList,pn) < 0)
     {//����ʧ��
          return -100;
     }     

     //��N��OID������� ��ȡ���� ���ֱ����
     //���Get/Setָ������Setָ���OID��VALUE�������������"OID"="VALUE"��ʽ���ٽ����������
     //�����Getָ���������ֱ����Ի�ȡ
     AckErrFlag = 0;//Ӧ������־��0
     for(u8temp = 0;u8temp < SNMP_SignelMaxOIDNum;u8temp++)
     {
          OidValueListOprIntex = u8temp;
          if(OidOprList[u8temp].oidl == 0)
          {//������
               if(u8temp == 0)
               {//�׸�OIDV����û������
                    return -101;
               }
               else
               {
                    break;//�˳�ѭ��
               }
          }
          else
          {//
               if((qcmd == BER_TYPE_SNMP_GET) || (qcmd == BER_TYPE_SNMP_GETNEXT))
               {//GETָ��
                    if(SNMP_MIBMatch(&Snmp_Mot,OidOprList[u8temp].oid,OidOprList[u8temp].oidl + 1,NULL_VP) <= 0)//��Ϊoidl����\0������Ҫ����1��\0���ֽڣ������ж�
                    {//MIB���ʧ�ܣ���ЧOIDS     
                         // DP = OidOprList[u8temp].oid;
                         // DL = OidOprList[u8temp].oidl;
                         return -102;
                    } 
               }
               else if(qcmd == BER_TYPE_SNMP_SET)
               {//SETָ��
                    u16setintex = 0;
                    for(u16temp = 0;u16temp < OidOprList[u8temp].oidl;u16temp++)
                    {
                         setopr[u16setintex++] = OidOprList[u8temp].oid[u16temp];
                    }
                    setopr[u16setintex++] = '=';
                    //�˴�����Setָ���ֵ����������ַ���������ܻ�������⣬��Ϊֵ�п��ܻẬ��0����ǰ������!!!!
                    for(u16temp = 0;u16temp < OidOprList[u8temp].valuel;u16temp++)
                    {
                         setopr[u16setintex++] = OidOprList[u8temp].value[u16temp];
                    }
                    setopr[u16setintex++] = '\0';
                    if(SNMP_MIBMatch(&Snmp_Mot,setopr,u16setintex,NULL_VP) <= 0)
                    {//MIB���ʧ�ܣ���ЧOIDS          
                         return -103;
                    } 
               }
               else
               {
                    return -104;
               }
          }
     }

     //����N��OIDV����
     if(AckErrFlag < 0)
     {//Ӧ�����
          return -105;
     }
     
     if(SNMP_Encode(qidn,OidOprList,rp,rl) < 0)
     {//����ʧ��
          return -106;
     }
     
     return 0;
}


/*-------------------------------------------------
������: FUN1_SNMP_AcceptAck
��   ��: SNMP����Ӧ��
��   ��:
       u8 *AckOIDp: Ӧ��OID�ַ����׵�ַ��".1.3.6.1.2.1.43"��
       u8 AckDataType: Ӧ����������
       u8 *AckDatap��Ӧ�������׵�ַ
       u16 AckDatal: Ӧ�������ֽ���
����ֵ:
       ����0:���ܳɹ�
       С��0:����ʧ��
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-02-03   
-------------------------------------------------*/
s32 FUN1_SNMP_AcceptAck(u8 *AckOIDp,u8 AckDataType,u8 *AckDatap,u16 AckDatal)
{
     u16 u16temp;
     OidOprList[OidValueListOprIntex].datatype = AckDataType;
     for(u16temp = 0;u16temp < 65535;u16temp++)
     {
          if(u16temp >= SNMP_MaxOIDSize)
          {//Խ����
               OidOprList[OidValueListOprIntex].oidl = 0;
               AckErrFlag = -1;
               return -1;
          }
          OidOprList[OidValueListOprIntex].oid[u16temp] = AckOIDp[u16temp];
          if(AckOIDp[u16temp] == '\0')
          {
               if(u16temp == 0)
               {
                    OidOprList[OidValueListOprIntex].oidl = 0;
               }
               else
               {
                    OidOprList[OidValueListOprIntex].oidl = (u16temp - 1);
               }               
               break;
          }
     }

     if(AckDatal >= SNMP_MaxValueSize)
     {//Խ����
          OidOprList[OidValueListOprIntex].datatype = BER_TYPE_NULL;
          OidOprList[OidValueListOprIntex].valuel = 0;
          AckErrFlag = -2;
          return -2;
     }

     for(u16temp = 0;u16temp < AckDatal;u16temp++)
     {     
          OidOprList[OidValueListOprIntex].value[u16temp] = AckDatap[u16temp];
     }
     OidOprList[OidValueListOprIntex].valuel = AckDatal;
     return 0;
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����ڲ�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
//�ֽ����� ת OID�ַ���
//u8 *dp:��ת�������׵�ַ
//u16 dl:��ת�������ֽ���
//u8 *oidsp:ת�����ſռ���׵�ַ
//u16 *oidsl:��ſռ���ֽ�����������Ч���ֽ���
//����ֵС��0��ת��ʧ�� ����0��ת���ɹ� 
static s8 SNMP_ByteToOIDStr(u8 *dp,u16 dl,u8 *oidsp,u16 *oidsl)
{
     u8 oidstr[SNMP_MaxOIDSize];
     s8 s8temp;
     u16 wintex = 0,rintex;
     u32 u32value;

     if(dp[0] != 0x2B)
     {
          return -1;
     }
     else
     {
          //oidstr[wintex++] = '.';
          oidstr[wintex++] = '1';
          oidstr[wintex++] = '.';
          oidstr[wintex++] = '3';
     }

     u32value = 0;
     for(rintex = 1;rintex < dl;rintex++)
     {          
          u32value += (dp[rintex] & 0x7F);
          if(dp[rintex] & 0x80)
          {
               u32value *= 128;
          }
          else
          {
               oidstr[wintex++] = '.';
               s8temp = SNMP_S32ToStr(u32value,&oidstr[wintex],(SNMP_MaxOIDSize - wintex));
               if(s8temp < 0)
               {
                    return -2;
               }
               u32value = 0;
               wintex += s8temp;
          }
     }
     if((wintex + 1) > *oidsl)
     {//��ſռ䲻��
          return -3;
     }
     for(rintex = 0;rintex < wintex;rintex++)
     {
          oidsp[rintex] = oidstr[rintex];
     }
     oidsp[rintex] = '\0';
     *oidsl = rintex;//���Ȳ���\0
     return 0;
}


//u8 *dp:�����������׵�ַ
//u16 dl:�����������ֽ���
//u8 *dtype:�����������ͣ�BER_TYPE�궨�壩
//u16 *dstartintex:������Ч�����ڴ����������е���ʼ������
//u16 *dsize:������Ч���ݵ��ֽ���
//����ֵС��0:����ʧ�� ����0:����ɹ�
static s8 SNMP_BerDecode(u8 *dp,u16 dl,u8 *dtype,u16 *dstartintex,u16 *dsize)
{
     u8 u8temp,u8num;
     u16 u16datalength;

//-----Tag�����жϴ���
     if(dl < 1)
     {//����������
          return SNMP_DecodeErr_DataSizeLess;
     }
     //��1���������ж�Tag���ֵ�����
     // u8temp = (dp[0] & 0xC0);//ȡ��2λ
     // if(u8temp == 0x00)
     // {//��ʾ�� ͨ��������

     // }
     // else if(u8temp == 0x40)
     // {//��ʾ�� Ӧ��������

     // }
     // else if(u8temp == 0x80)
     // {//��ʾ�� ������������

     // }
     // else
     // {//��ʾ�� ˽��������

     // }
     // //��2�����ж�value�����Ƿ�Ƕ��
     // if(dp[0] & 0x20)
     // {//��ʾvalue������Ƕ�׵�----�������ݲ����ﻹ��Ƕ��BER��������

     // }
     // else
     // {//��ʾvalue���ֲ���Ƕ�׵�

     // }
     
     //�ж������Ƿ���ȷ
     u8num = sizeof(DataTypeList);
     for(u8temp = 0;u8temp < u8num;u8temp++)
     {
          if(dp[0] == DataTypeList[u8temp])
          {
               break;
          }
     }
     if(u8temp >= u8num)
     {//���Ͳ���
          return SNMP_DecodeErr_DataType;
     }
     *dtype = dp[0];

//-----Length�����жϴ�����ȡ���ݲ����ֽ�����
     if(dl < 2)
     {//����������
          return SNMP_DecodeErr_DataSizeLess;
     }
     
     if(dp[1] & 0x80)
     {//���Ȳ�����
          if((dp[1] & 0x7F) > 2)
          {//��ʾ���ݳ��ȵ���Ϣ����2���ֽڣ���ʾ�������ݲ��ֹ���������0xFFFF����-------�쳣
               return SNMP_DecodeErr_DataSizeMax;
          }

          if(dl < ((dp[1] & 0x7F) + 2))
          {//�������ݳ��ȵ��ֽ�������
               return SNMP_DecodeErr_DataSizeLess;
          }
          
          //���ݳ��Ȱ���˸�ʽ����
          u16datalength = 0;
          for(u8temp = 0;u8temp < (dp[1] & 0x7F);u8temp++)
          {
               u16datalength <<= 8;
               u16datalength += dp[2 + u8temp];
          }
          *dsize = u16datalength;
          *dstartintex = ((dp[1] & 0x7F) + 2);//��ʼ������          

          if(*dsize > SNMP_MaxMessageSize)
          {//�����ֽ��� ���� ���SNMP���ĳ���-------�쳣
               return SNMP_DecodeErr_DataSizeMax;
          }

          if(dl < (2 + (dp[1] & 0x7F) + *dsize))
          {//�����ֽ�������
               return SNMP_DecodeErr_DataSizeLess;
          }
     }
     else
     {//���ݶ���
          *dsize = dp[1];//��Ч�ֽ���
          *dstartintex = 2;//��Ч������ʼ������

          if(*dsize > SNMP_MaxMessageSize)
          {//�����ֽ��� ���� ���SNMP���ĳ���-------�쳣
               return SNMP_DecodeErr_DataSizeMax;
          }

          if(dl < (2 + *dsize))
          {//�����ֽ�������
               return SNMP_DecodeErr_DataSizeLess;
          }
     }

     return 0;
}

/*-------------------------------------------------
������: FUN_SNMP_Decode
��   ��: SNMP����
��   ��:
       u8 *dp: �����������׵�ַ
       u16 dl: �����������ֽ���
       u8 *qcmd����������������
       u32 *qid����������ID��
       OIDV *op��OIDV����ָ��
       u16 *processsize�������Ѵ����ֽ���
����ֵ:
       ����0:����OK
       С��0:����ʧ��
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-02-03   
-------------------------------------------------*/
static s32 SNMP_Decode(u8 *dp,u16 dl,u8 *qcmd,u32 *qid,OIDV *op,u16 *processsize)
{
     u8 datatype = 0,u8temp,u8oidintex;
     u8 cname[SNMP_MaxCommunitySize];
     s8 s8temp;
     u16 datastartintex = 0,datasize = 0,u16temp,u16intex;
     u16 oidl[SNMP_SignelMaxOIDNum],valuesl[SNMP_SignelMaxOIDNum];//����ֵ����ʱ����
     //u16 u16oidsl = *oidsl,u16valuel = *valuel;
     //
     u32 u32temp;
     s32 errv;
     //
     u8 *ndp = dp;
     s32 ndl = dl;

     //������ʼ��
     *qcmd = 0;
     *qid = 0;
     *processsize = 0;
     //OIDV����г�ʼ��
     u8oidintex = 0;//oidv�����������
     for(u8temp = 0;u8temp < SNMP_SignelMaxOIDNum;u8temp++)
     {//OIDV����г�ʼ��
          oidl[u8temp] = op[u8temp].oidl;//�ݴ�
          op[u8temp].oidl = 0;//��0
          valuesl[u8temp] = op[u8temp].valuel;//�ݴ�
          op[u8temp].valuel = 0;//��0
     }
     
     //��ʼ����
     for(u8temp = 1;ndl > 0;u8temp++)
     {
          errv = (0 - u8temp);

          ndp += datastartintex;
          if((datatype & 0x20) == 0)
          {//datatype��b5=0��ʾvalue��Ƕ�ף���valueΪ�����ݣ�ndp��Ҫ��ƫ�� �����ݵ��ֽ�����
               ndp += datasize;
          }
          ndl = (dl - (ndp - dp));

          if(ndl == 0)
          {
               return 0;
          }
          else if(u8temp == 12)
          {//��ʾ�ж��OID
               u8temp = 9;//�ӵ�9�����¿�ʼ����
               if(u8oidintex >= SNMP_SignelMaxOIDNum)
               {//����������OID�ĸ�������SNMP_SignelMaxOIDNum
                    return errv;
               }
          }

          s8temp = SNMP_BerDecode(ndp,(u16)ndl,&datatype,&datastartintex,&datasize);
          if(s8temp < 0)
          {//����ʧ��
               if((*processsize == 0) && ((s8temp == SNMP_DecodeErr_DataType) || (s8temp == SNMP_DecodeErr_DataSizeMax)))
               {//��*processsize����Ϊ0 �� ������ �� ���������� ����ʱ
                    *processsize = 1;//��1�����ϲ�ɾ�������1���ֽ����������ٳ��Խ���
               }
               return errv;
          }

          switch(u8temp)
          {
               case 1://��1����루Ƕ�ף����ж��������Ƿ��㹻
                    *processsize = (datastartintex + datasize);//�õ����ֽ���
                    if(datatype != BER_TYPE_SEQUENCE)
                    {//�����쳣
                         return errv;
                    }
                    break;
               case 2://��2����룬�õ�SNMP�汾�� ndp[datastartintex] + 1
                    if((datatype != BER_TYPE_INTEGER) || (datasize != 0x01) || (ndp[datastartintex] != (SNMP_VersionNum - 1)))
                    {//�����쳣 �� �汾�Ų���
                         return errv;
                    }                    
                    break;
               case 3://��3����룬�õ������� �� ��������һ��Ϊpublic��
                    if((datatype != BER_TYPE_OCTET_STRING) || (datasize >= SNMP_MaxCommunitySize))
                    {//�����쳣
                         return errv;
                    }
                    for(u16intex = 0;u16intex < datasize;u16intex++)
                    {
                         cname[u16intex] = ndp[datastartintex + u16intex];
                    }
                    cname[u16intex] = '\0';
                    if(SNMP_StrIsRame(CommunityNameP,cname) < 0)
                    {//��������һ��
                         return errv;
                    }
                    break;
               case 4://��4����루Ƕ�ף����õ�SNMPָ��
                    if((datatype != BER_TYPE_SNMP_GETNEXT) && (datatype != BER_TYPE_SNMP_GET) && (datatype != BER_TYPE_SNMP_SET))
                    {//��֧�ֵ�SNMPָ��
                         return errv;
                    }
                    *qcmd = datatype;
                    break;
               case 5://��5����룬�õ�����ID��
                    if((datatype != BER_TYPE_INTEGER))// || (datasize > 0x02))
                    {//�����쳣
                         return errv;
                    }
                    //��˸�ʽ
                    u32temp = 0;
                    for(u16intex = 0;u16intex < datasize;u16intex++)
                    {
                         u32temp <<= 8;
                         u32temp += ndp[datastartintex + u16intex];
                    }
                    *qid = u32temp;
                    break;
               case 6://��6����룬�õ��õ�����״̬�� ndp[0]Ϊ����״̬��
                    if((datatype != BER_TYPE_INTEGER) || (datasize != 0x01) || (ndp[datastartintex] != 0x00))
                    {//�����쳣
                         return errv;
                    }
                    break;
               case 7://��7����룬�õ��õ�����״̬�������� ndp[0]Ϊ����״̬��
                    if((datatype != BER_TYPE_INTEGER) || (datasize != 0x01) || (ndp[datastartintex] != 0x00))
                    {//�����쳣
                         return errv;
                    }
                    break;
               case 8://��8����루Ƕ�ף�----N��OidValue��Ƕ��
                    if((datatype != BER_TYPE_SEQUENCE) )
                    {//�����쳣
                         return errv;
                    }
                    break;
               case 9://��9����루Ƕ�ף�----����OidValueǶ��
                    if((datatype != BER_TYPE_SEQUENCE) )
                    {//�����쳣
                         return errv;
                    }
                    break;
               case 10://��10����룬�õ�OID�ַ���----����OID
                    if((datatype != BER_TYPE_OID) )
                    {//�����쳣
                         return errv;
                    }
                    if(SNMP_ByteToOIDStr(&ndp[datastartintex],datasize,op[u8oidintex].oid,&oidl[u8oidintex]) < 0)
                    {//�ֽ� ת OID�ַ���ʧ��
                         return errv;
                    }
                    op[u8oidintex].oidl = oidl[u8oidintex];
                    break;
               case 11://��11����룬�õ�OID��ֵ----����Value
                    if((*qcmd == BER_TYPE_SNMP_GETNEXT) || (*qcmd == BER_TYPE_SNMP_GET))
                    {//�����Get����
                         if((datatype != BER_TYPE_NULL) )
                         {//�����쳣
                              return errv;
                         }
                         op[u8oidintex].valuel = 0;
                    }
                    else if(*qcmd == BER_TYPE_SNMP_SET)
                    {//�����Set����
                         //�жϻ����Ƿ�
                         if(valuesl[u8oidintex] < datasize)
                         {//����ռ䲻��
                              return errv;
                         }
                         for(u16intex = 0;u16intex < datasize;u16intex++)
                         {
                              op[u8oidintex].value[u16intex] = ndp[datastartintex + u16intex];
                         }
                         op[u8oidintex].valuel = datasize;
                    }
                    else
                    {
                         op[u8oidintex].valuel = 0;
                         return errv;
                    }

                    u8oidintex++;
                    break;
               
               default:
                    break;
          }
     }

     return 0;
}


//u8 *oidsp:oid�ַ����׵�ַ��ע��Ҫ��\0��β
//u8 *dp:ת�����ſռ��׵�ַ
//u16 *dl:��ſռ�����ֽ���������ʵ����Ч���ֽ���
//����ֵС��0:ת��ʧ�� ����0ת���ɹ�
static s8 SNMP_OIDStrToBytes(u8 *oidsp,u8 *dp,u16 *dl)
{
     s8 s8dintex,s8temp;
     u8 numstr[12],numintex;
     u16 maxsize,u16wintex,u16i;
     s32 s32num,s32temp;
     u32 Darray[5];

     maxsize = *dl;
     *dl = 0;
     u16wintex = 0;
     
     numstr[0] = '\0';
     numintex = 0;

     //if((oidsp[0] == '.') && (oidsp[1] == '1') && (oidsp[2] == '.') && (oidsp[3] == '3'))
     if((oidsp[0] == '1') && (oidsp[1] == '.') && (oidsp[2] == '3'))
     {
          dp[u16wintex++] = 0x2B;
     }
     else
     {//�Ƿ�
          return -1;
     }

     //for(u16i = 4;;u16i++)
     for(u16i = 3;;u16i++)
     {
          if((oidsp[u16i] != '.') && (oidsp[u16i] != '\0'))
          {
               numstr[numintex++] = oidsp[u16i];
          }
          else if(u16i > 4)//�����1���ͳ���.
          {               
               numstr[numintex] = '\0';
               numintex = 0;
               s32num = -1;
               s8dintex = 0;
               if(SNMP_StrToS32(numstr,&s32num) < 0)
               {//ת��ʧ��
                    return -2;
               }

               if(s32num < 0)
               {
                    return -3;
               }
               else if(s32num < 128)
               {
                    if(maxsize < (u16wintex + 1))
                    {//�ռ䲻��
                         return -4;
                    }
                    else
                    {
                         dp[u16wintex++] = (u8)s32num;
                    }
               }
               else
               {
                    s32temp = s32num;//�ݴ�
                    //-----��������128�������棬ֱ����С��128
                    for(s8dintex = 0;s32temp > 127;s8dintex++)
                    {
                         Darray[s8dintex] = s32temp;
                         s32temp /= 128;
                    }
                    Darray[s8dintex++] = s32temp;//�������һ����������С��128����
                    //-----�жϿռ�
                    if(maxsize < (u16wintex + s8dintex))
                    {//�ռ䲻��
                         return -4;
                    }
                    //
                    s8dintex--;//�����ż�1��ָ��Darray��������һ������
                    s8temp = s8dintex;//�ݴ����һ��������
                    for(;s8dintex > -1;s8dintex--)
                    {//�˴���Ҫ���з������ݴ��������һ����ʼ����
                         if(s8dintex == s8temp)
                         {//���һ������,�������0x80
                              dp[u16wintex++] = (0x80 + Darray[s8dintex]);
                         }
                         else if(s8dintex == 0)
                         {//��1�����ݣ�����󲻼�0x80
                              dp[u16wintex++] = (Darray[s8dintex] % 128);
                         }
                         else
                         {//�������ݣ�������ټ�0x80
                              dp[u16wintex++] = ((Darray[s8dintex] % 128) + 0x80);
                         }
                    }
               }
          }
          //
          if(oidsp[u16i] == '\0')
          {
               break;//�˳�ѭ��
          }
     }
     *dl = u16wintex;//����dp�����е���Ч�ֽ���
     return 0;
}

//BER����
//u8 dtype:�������ͣ�BER_TYPE�궨�壩
//u8 *vp:�����������׵�ַ
//u16 vl:�����������ֽ���
//u16 en:�Ѵ����ֽ���
//u16 upen:����֮ǰ���ֽ������������Ե�OID��VALUE��
//u8 *oidvflag:  1��ʾ����OIDVǶ��   0��ʾ����Ƕ��
//u8 *sp:������ſռ���ַ
//u16 *sl:��ſռ䳤���ֽ�����������Ч���ֽ���
//����ֵ��С��0����ʧ�� ����0����ɹ�
static s8 SNMP_BerEncode(u8 dtype,u8 *vp,u16 vl,u16 en,u16 upen,u8 *oidvflag,u8 *sp,u16 *sl)
{
     u16 u16temp;
     u16 maxsize = *sl;
     u16 wintex;//��������
     u16 bltemp;//�ֽڳ���

     *sl = 0;
     wintex = 0;

     if(maxsize < 1)
     {//�ռ䲻��
          return -1;
     }
     //�����������
     sp[wintex++] = dtype;

     bltemp = vl;
     if(dtype & 0x20)
     {//Ƕ��
          bltemp = en;//��Ƕ��          
          //����ǵ���OIDSǶ��
          if(*oidvflag)
          {//����ǵ���OIDVǶ��
               *oidvflag = 0;
               bltemp = (en - upen);
          }
     }
     
     //�������ݳ���
     if(bltemp > 127)
     {
          if(bltemp <= 255)
          {
               if(maxsize < 3)
               {//�ռ䲻��
                    return -1;
               }
               sp[wintex++] = 0x81;
               sp[wintex++] = (u8)bltemp;
          }
          else if(bltemp <= 65535)
          {
               if(maxsize < 4)
               {//�ռ䲻��
                    return -1;
               }
               sp[wintex++] = 0x82;
               sp[wintex++] = (u8)(bltemp >> 8);//��˸�ʽ
               sp[wintex++] = (u8)bltemp;
          }
          else
          {//Խ��
               return -2;
          }
     }
     else
     {
          if(maxsize < 2)
          {//�ռ䲻��
               return -1;
          }
          sp[wintex++] = (u8)bltemp;
     }

     if((dtype & 0x20) == 0)
     {//��Ƕ��
          //�������
          if(maxsize < (wintex + vl))
          {//�ռ䲻��
               return -1;
          }
          for(u16temp = 0;u16temp < vl;u16temp++)
          {
               sp[wintex++] = vp[u16temp];
          }
     }     
     *sl = wintex;
     
     return 0;
}

//u8 *oidsp:Ӧ��OID�ַ���ָ��
//u32 qin:������
//u8 valuet:��Ӧֵ����
//u8 *valuep:��Ӧֵָ��
//u8 valuel:��Ӧֵ�ֽ���
//u8 *dp:�����ſռ��׵�ַ
//u16 *dl:�����ſռ䳤���ֽ���������ʵ����Ч���ֽ���
//����С��0:����ʧ�� ����0:����ɹ�
//static s32 SNMP_Encode(u8 *oidsp,u32 qin,u8 valuet,u8 *valuep,u8 valuel,u8 *dp,u16 *dl)
static s32 SNMP_Encode(u32 qin,OIDV *op,u8 *dp,u16 *dl)
{
     s32 errv;
     s16 s16oidvintex;
     u8 u8temp,oidb[SNMP_MaxOIDSize],buffertemp[SNMP_MaxBerSize];
     u8 oidvsflag;//����OIDVǶ�ױ�־��1������OIDVǶ��  0������Ƕ�ף�
     u8 ndt;//��������������
     u8 *ndp;//�����������׵�ַ
     u16 ndl;//�����������ֽ���
     u16 wintex;
     u16 maxsize = *dl;
     u16 u16i,upen,en;//֮ǰOIDV�Ѵ��ڵ��ֽ��������Ѵ��ڵ��ֽ���
     u16 oidbl = SNMP_MaxOIDSize,buffertempl = SNMP_MaxBerSize;

     *dl = 0;

     for(s16oidvintex = SNMP_SignelMaxOIDNum - 1;s16oidvintex >= 0;s16oidvintex--)
     {
          if(OidOprList[s16oidvintex].oidl > 0)
          {
               break;//�˳�ѭ��
          }
     }
     if(s16oidvintex < 0)
     {//û�д���Ӧ������
          return -1;
     }

     en = 0;
     wintex = (maxsize - 1);
     for(u8temp = 11;u8temp > 0;u8temp--)
     {
          errv = (0 - u8temp);
          switch(u8temp)
          {
               case 11://��Ӧֵ
                    upen = en;//��¼��OIDV֮ǰ�Ѵ��ڵ��ֽ���
                    ndt = OidOprList[s16oidvintex].datatype;
                    ndp = OidOprList[s16oidvintex].value;
                    ndl = OidOprList[s16oidvintex].valuel;
                    break;
               case 10://��ӦOID�ַ���
                    if(SNMP_OIDStrToBytes(OidOprList[s16oidvintex].oid,oidb,&oidbl) < 0)
                    {//ת��ʧ��
                         return -10;
                    }
                    ndt = BER_TYPE_OID;
                    ndp = oidb;
                    ndl = oidbl;
                    break;
               case 9://Ƕ��----ÿ��OIDVֵ��Ƕ��
                    ndt = BER_TYPE_SEQUENCE;
                    ndp = NULL_U8P;
                    ndl = 0;
                    //
                    oidvsflag = 1;//����OIDVǶ�ױ�־
                    s16oidvintex--;
                    if(s16oidvintex >= 0)
                    {//���ǻ���OIDV��
                         u8temp = 12;
                         //oidvsflag = 1;//����oidv��
                    }
                    break;
               case 8://Ƕ��----N��OIDVֵ��Ƕ��
                    ndt = BER_TYPE_SEQUENCE;
                    ndp = NULL_U8P;
                    ndl = 0;
                    break;
               case 7://����������
                    oidb[0] = 0;
                    ndt = BER_TYPE_INTEGER;
                    ndp = oidb;
                    ndl = 1;
                    break;
               case 6://����״̬
                    oidb[0] = 0;
                    ndt = BER_TYPE_INTEGER;
                    ndp = oidb;
                    ndl = 1;
                    break;
               case 5://����IDn
                    ndt = BER_TYPE_INTEGER;
                    ndp = oidb;
                    //ת�ɴ�˸�ʽ��ע�⣺�˴�֮����ͳһ��4λ����Ϊ�˱�֤���λΪ0�������ڳ��ָ���!!!��
                    //�������127��С��256ʱ��ʹ��1�ֽڣ���1�ֽڵĸ�λΪ1���������յ��������ɸ���!!!
                    oidb[0] = (u8)(qin >> 24);
                    oidb[1] = (u8)(qin >> 16);
                    oidb[2] = (u8)(qin >> 8);
                    oidb[3] = (u8)(qin);
                    ndl = 4;
                    break;
               case 4://Ӧ������
                    ndt = BER_TYPE_SNMP_RESPONSE;
                    ndp = NULL_U8P;
                    ndl = 0;
                    break;
               case 3://������
                    ndt = BER_TYPE_OCTET_STRING;
                    ndp = CommunityNameP;
                    ndl = SNMP_StrLen(CommunityNameP);
                    break;
               case 2://�汾��
                    oidb[0] = 0;
                    ndt = BER_TYPE_INTEGER;
                    ndp = oidb;
                    ndl = 1;
                    break;
               case 1://Ƕ��
                    ndt = BER_TYPE_SEQUENCE;
                    ndp = NULL_U8P;
                    ndl = 0;
                    break;               
               default:
                    ndl = 0;
                    break;
          }

          //����
          buffertempl = SNMP_MaxBerSize;
          if(SNMP_BerEncode(ndt,ndp,ndl,en,upen,&oidvsflag,buffertemp,&buffertempl) < 0)
          {
               return (errv);
          }

          //�жϿռ��Ƿ��㹻
          if(maxsize < (en + buffertempl))
          {//�ռ䲻��
               return (errv - 10);
          }
          //�Ӻ���ǰ���
          for(u16i = 0;u16i < buffertempl;u16i++)
          {
               dp[wintex--] = buffertemp[buffertempl - u16i - 1];
               en++;
          }
     }
     
     //���ݴ������ �Ƶ� ǰ��
     wintex++;//��λ��������ʼ������
     for(u16i = 0;u16i < en;u16i++)
     {
          dp[u16i] = dp[wintex + u16i];
     }
     *dl = en;//������Ч�ֽ���

     return 0;
}
