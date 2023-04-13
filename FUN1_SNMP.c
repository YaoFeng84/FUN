/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------以下是模块的修改记录区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * 内容：
 * 日期：2023-02-03
 * 作者：YJX
 * 版本号：V1.0（初版）
 ***********************************************
 * 修改内容：
 * 修改日期：
 * 修改作者：
 * 版本号：
 ***********************************************
*/
/********************************************************************************************************************************************
*                                                                                                                                           *
*                ---------------------------------以下是模块的使用说明区-----------------------------------------                           *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*                                       特点说明：
          
*/
#include "ProHeadFile.h"
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下模块间的对接函数区-----------------------------------------                             *
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
*               ----------------------------------以下是模块间的变量申明区--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块内的宏定义区------------------------------------                                  *
*                                                                                                                                           *
********************************************************************************************************************************************/
//
#define SNMP_VersionNum                 1    //SNMP版本号(1:V1)
//解码错误值宏定义
#define SNMP_DecodeErr_DataType         -40  //数据类型异常
#define SNMP_DecodeErr_DataSizeMax      -41  //数据量过大
#define SNMP_DecodeErr_DataSizeLess     -42  //数据量不够
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块内的变量类型定义区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
typedef struct
{
     u8 datatype;//数据类型,用于保存响应的数据类型
     u8 oid[SNMP_MaxOIDSize];
     u16 oidl; //oid缓存字节数，处理后返回有效字节数，不含\0
     u8 value[SNMP_MaxValueSize];
     u16 valuel;//value缓存字节数，处理后返回有效字节数，不含\0
}OIDV;

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数申明区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static s32 SNMP_Decode(u8 *dp,u16 dl,u8 *qcmd,u32 *qid,OIDV *op,u16 *processsize);
static s32 SNMP_Encode(u32 qin,OIDV *op,u8 *dp,u16 *dl);
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块内的 变量 与 宏定义 申明区--------------------------------                            *
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
static OIDV OidOprList[SNMP_SignelMaxOIDNum];//OidValue操作表
static u8 OidValueListOprIntex;//OidValue表操作索引号
static s8 AckErrFlag;//应答错误标志

static u8 *SaveDataP;//编码后数据存放空间首地址
static u16 *SaveDataL;//编码后数据存放空间长度及返回值
static u32 QuestNum;//请求操作码
static s32 EncodeErrv;//编码错误状态
static u8 *CommunityNameP;//社区名字符串指针
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的系统函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
函数名: FUN1_SNMP_Config
功   能: SNMP配置
参   数:
       u8 *cp: 社区名字符串首地址，以\0结尾
返回值:
       等于0:配置成功
       小于0:配置失败
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2023-02-07   
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
*               ----------------------------------以下是模块的用户函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
函数名: FUN1_SNMP_Process
功   能: SNMP数据处理
参   数:
       u8 *dp: 待处理数据首地址
       u16 dl: 待处理数据字节数
       u16 *pn：已处理字节数
       u8 *rp: 响应数据空间首地址
       u16 *rl: 响应数据空间字节数，返回响应数据有效字节数
返回值:
       等于0:处理成功
       小于0:处理失败
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2023-02-07   
-------------------------------------------------*/
s32 FUN1_SNMP_Process(u8 *dp,u16 dl,u16 *pn,u8 *rp,u16 *rl)
{
     u8 qcmd,u8temp;     
     u8 setopr[SNMP_MaxOIDSize + SNMP_MaxValueSize + 2];
     u16 u16setintex,u16temp;
     u32 qidn;

     *pn = 0;

     //对OidValue表进行初始化
     for(u8temp = 0;u8temp < SNMP_SignelMaxOIDNum;u8temp++)
     {
          OidOprList[u8temp].oidl = SNMP_MaxOIDSize;
          OidOprList[u8temp].valuel = SNMP_MaxValueSize;
     }

     //进行解码，并解出N个OID，分别存放N个(OID + VALUE的结构体缓存)
     if(SNMP_Decode(dp,dl,&qcmd,&qidn,OidOprList,pn) < 0)
     {//解码失败
          return -100;
     }     

     //对N个OID进行配对 获取数据 ，分别存入
     //结合Get/Set指令，如果是Set指令，则将OID和VALUE进行整理，整理成"OID"="VALUE"格式，再进行配对设置
     //如果是Get指令，则不用整理，直接配对获取
     AckErrFlag = 0;//应答错误标志清0
     for(u8temp = 0;u8temp < SNMP_SignelMaxOIDNum;u8temp++)
     {
          OidValueListOprIntex = u8temp;
          if(OidOprList[u8temp].oidl == 0)
          {//结束了
               if(u8temp == 0)
               {//首个OIDV表中没有数据
                    return -101;
               }
               else
               {
                    break;//退出循环
               }
          }
          else
          {//
               if((qcmd == BER_TYPE_SNMP_GET) || (qcmd == BER_TYPE_SNMP_GETNEXT))
               {//GET指令
                    if(SNMP_MIBMatch(&Snmp_Mot,OidOprList[u8temp].oid,OidOprList[u8temp].oidl + 1,NULL_VP) <= 0)//因为oidl不含\0，故需要增加1个\0的字节，用于判断
                    {//MIB配对失败，无效OIDS     
                         // DP = OidOprList[u8temp].oid;
                         // DL = OidOprList[u8temp].oidl;
                         return -102;
                    } 
               }
               else if(qcmd == BER_TYPE_SNMP_SET)
               {//SET指令
                    u16setintex = 0;
                    for(u16temp = 0;u16temp < OidOprList[u8temp].oidl;u16temp++)
                    {
                         setopr[u16setintex++] = OidOprList[u8temp].oid[u16temp];
                    }
                    setopr[u16setintex++] = '=';
                    //此处对于Set指令的值，如果不是字符串，则可能会出现问题，因为值中可能会含有0而提前被结束!!!!
                    for(u16temp = 0;u16temp < OidOprList[u8temp].valuel;u16temp++)
                    {
                         setopr[u16setintex++] = OidOprList[u8temp].value[u16temp];
                    }
                    setopr[u16setintex++] = '\0';
                    if(SNMP_MIBMatch(&Snmp_Mot,setopr,u16setintex,NULL_VP) <= 0)
                    {//MIB配对失败，无效OIDS          
                         return -103;
                    } 
               }
               else
               {
                    return -104;
               }
          }
     }

     //进行N个OIDV编码
     if(AckErrFlag < 0)
     {//应答错误
          return -105;
     }
     
     if(SNMP_Encode(qidn,OidOprList,rp,rl) < 0)
     {//编码失败
          return -106;
     }
     
     return 0;
}


/*-------------------------------------------------
函数名: FUN1_SNMP_AcceptAck
功   能: SNMP接受应答
参   数:
       u8 *AckOIDp: 应答OID字符串首地址（".1.3.6.1.2.1.43"）
       u8 AckDataType: 应答数据类型
       u8 *AckDatap：应答数据首地址
       u16 AckDatal: 应答数据字节数
返回值:
       等于0:接受成功
       小于0:接受失败
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2023-02-03   
-------------------------------------------------*/
s32 FUN1_SNMP_AcceptAck(u8 *AckOIDp,u8 AckDataType,u8 *AckDatap,u16 AckDatal)
{
     u16 u16temp;
     OidOprList[OidValueListOprIntex].datatype = AckDataType;
     for(u16temp = 0;u16temp < 65535;u16temp++)
     {
          if(u16temp >= SNMP_MaxOIDSize)
          {//越界了
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
     {//越界了
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
*               ----------------------------------以下是模块的内部函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
//字节数据 转 OID字符串
//u8 *dp:待转换数据首地址
//u16 dl:待转换数据字节数
//u8 *oidsp:转换后存放空间的首地址
//u16 *oidsl:存放空间的字节数，返回有效的字节数
//返回值小于0：转换失败 等于0：转换成功 
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
     {//存放空间不够
          return -3;
     }
     for(rintex = 0;rintex < wintex;rintex++)
     {
          oidsp[rintex] = oidstr[rintex];
     }
     oidsp[rintex] = '\0';
     *oidsl = rintex;//长度不含\0
     return 0;
}


//u8 *dp:待解码数据首地址
//u16 dl:待解码数据字节数
//u8 *dtype:返回数据类型（BER_TYPE宏定义）
//u16 *dstartintex:返回有效数据在待解码数据中的起始索引号
//u16 *dsize:返回有效数据的字节数
//返回值小于0:解码失败 等于0:解码成功
static s8 SNMP_BerDecode(u8 *dp,u16 dl,u8 *dtype,u16 *dstartintex,u16 *dsize)
{
     u8 u8temp,u8num;
     u16 u16datalength;

//-----Tag部分判断处理
     if(dl < 1)
     {//数据量不够
          return SNMP_DecodeErr_DataSizeLess;
     }
     //【1】、首先判断Tag部分的类型
     // u8temp = (dp[0] & 0xC0);//取高2位
     // if(u8temp == 0x00)
     // {//表示是 通用型数据

     // }
     // else if(u8temp == 0x40)
     // {//表示是 应用型数据

     // }
     // else if(u8temp == 0x80)
     // {//表示是 上下文型数据

     // }
     // else
     // {//表示是 私有型数据

     // }
     // //【2】、判断value部分是否嵌套
     // if(dp[0] & 0x20)
     // {//表示value部分是嵌套的----就是数据部分里还有嵌套BER编码数据

     // }
     // else
     // {//表示value部分不是嵌套的

     // }
     
     //判断类型是否正确
     u8num = sizeof(DataTypeList);
     for(u8temp = 0;u8temp < u8num;u8temp++)
     {
          if(dp[0] == DataTypeList[u8temp])
          {
               break;
          }
     }
     if(u8temp >= u8num)
     {//类型不对
          return SNMP_DecodeErr_DataType;
     }
     *dtype = dp[0];

//-----Length部分判断处理（获取数据部分字节数）
     if(dl < 2)
     {//数据量不够
          return SNMP_DecodeErr_DataSizeLess;
     }
     
     if(dp[1] & 0x80)
     {//长度不定长
          if((dp[1] & 0x7F) > 2)
          {//表示数据长度的信息大于2个字节，表示后面数据部分过长（超过0xFFFF个）-------异常
               return SNMP_DecodeErr_DataSizeMax;
          }

          if(dl < ((dp[1] & 0x7F) + 2))
          {//计算数据长度的字节数不够
               return SNMP_DecodeErr_DataSizeLess;
          }
          
          //数据长度按大端格式计算
          u16datalength = 0;
          for(u8temp = 0;u8temp < (dp[1] & 0x7F);u8temp++)
          {
               u16datalength <<= 8;
               u16datalength += dp[2 + u8temp];
          }
          *dsize = u16datalength;
          *dstartintex = ((dp[1] & 0x7F) + 2);//起始索引号          

          if(*dsize > SNMP_MaxMessageSize)
          {//数据字节量 大于 最大SNMP报文长度-------异常
               return SNMP_DecodeErr_DataSizeMax;
          }

          if(dl < (2 + (dp[1] & 0x7F) + *dsize))
          {//数据字节量不够
               return SNMP_DecodeErr_DataSizeLess;
          }
     }
     else
     {//数据定长
          *dsize = dp[1];//有效字节数
          *dstartintex = 2;//有效数据起始索引号

          if(*dsize > SNMP_MaxMessageSize)
          {//数据字节量 大于 最大SNMP报文长度-------异常
               return SNMP_DecodeErr_DataSizeMax;
          }

          if(dl < (2 + *dsize))
          {//数据字节量不够
               return SNMP_DecodeErr_DataSizeLess;
          }
     }

     return 0;
}

/*-------------------------------------------------
函数名: FUN_SNMP_Decode
功   能: SNMP解码
参   数:
       u8 *dp: 待解码数据首地址
       u16 dl: 待解码数据字节数
       u8 *qcmd：返回请求命令码
       u32 *qid：返回请求ID号
       OIDV *op：OIDV变量指针
       u16 *processsize：返回已处理字节数
返回值:
       等于0:处理OK
       小于0:解码失败
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2023-02-03   
-------------------------------------------------*/
static s32 SNMP_Decode(u8 *dp,u16 dl,u8 *qcmd,u32 *qid,OIDV *op,u16 *processsize)
{
     u8 datatype = 0,u8temp,u8oidintex;
     u8 cname[SNMP_MaxCommunitySize];
     s8 s8temp;
     u16 datastartintex = 0,datasize = 0,u16temp,u16intex;
     u16 oidl[SNMP_SignelMaxOIDNum],valuesl[SNMP_SignelMaxOIDNum];//长度值的暂时保存
     //u16 u16oidsl = *oidsl,u16valuel = *valuel;
     //
     u32 u32temp;
     s32 errv;
     //
     u8 *ndp = dp;
     s32 ndl = dl;

     //变量初始化
     *qcmd = 0;
     *qid = 0;
     *processsize = 0;
     //OIDV表进行初始化
     u8oidintex = 0;//oidv表操作索引号
     for(u8temp = 0;u8temp < SNMP_SignelMaxOIDNum;u8temp++)
     {//OIDV表进行初始化
          oidl[u8temp] = op[u8temp].oidl;//暂存
          op[u8temp].oidl = 0;//清0
          valuesl[u8temp] = op[u8temp].valuel;//暂存
          op[u8temp].valuel = 0;//清0
     }
     
     //开始解码
     for(u8temp = 1;ndl > 0;u8temp++)
     {
          errv = (0 - u8temp);

          ndp += datastartintex;
          if((datatype & 0x20) == 0)
          {//datatype的b5=0表示value非嵌套（即value为纯数据，ndp需要再偏移 纯数据的字节量）
               ndp += datasize;
          }
          ndl = (dl - (ndp - dp));

          if(ndl == 0)
          {
               return 0;
          }
          else if(u8temp == 12)
          {//表示有多个OID
               u8temp = 9;//从第9层重新开始解码
               if(u8oidintex >= SNMP_SignelMaxOIDNum)
               {//单个报文中OID的个数超过SNMP_SignelMaxOIDNum
                    return errv;
               }
          }

          s8temp = SNMP_BerDecode(ndp,(u16)ndl,&datatype,&datastartintex,&datasize);
          if(s8temp < 0)
          {//解码失败
               if((*processsize == 0) && ((s8temp == SNMP_DecodeErr_DataType) || (s8temp == SNMP_DecodeErr_DataSizeMax)))
               {//当*processsize还是为0 且 （类型 或 数据量过大 出错）时
                    *processsize = 1;//置1，让上层删除缓存的1个字节量，重新再偿试解码
               }
               return errv;
          }

          switch(u8temp)
          {
               case 1://第1层解码（嵌套），判断数据量是否足够
                    *processsize = (datastartintex + datasize);//得到总字节数
                    if(datatype != BER_TYPE_SEQUENCE)
                    {//数据异常
                         return errv;
                    }
                    break;
               case 2://第2层解码，得到SNMP版本号 ndp[datastartintex] + 1
                    if((datatype != BER_TYPE_INTEGER) || (datasize != 0x01) || (ndp[datastartintex] != (SNMP_VersionNum - 1)))
                    {//数据异常 或 版本号不对
                         return errv;
                    }                    
                    break;
               case 3://第3层解码，得到团体名 或 社区名（一般为public）
                    if((datatype != BER_TYPE_OCTET_STRING) || (datasize >= SNMP_MaxCommunitySize))
                    {//数据异常
                         return errv;
                    }
                    for(u16intex = 0;u16intex < datasize;u16intex++)
                    {
                         cname[u16intex] = ndp[datastartintex + u16intex];
                    }
                    cname[u16intex] = '\0';
                    if(SNMP_StrIsRame(CommunityNameP,cname) < 0)
                    {//社区名不一样
                         return errv;
                    }
                    break;
               case 4://第4层解码（嵌套），得到SNMP指令
                    if((datatype != BER_TYPE_SNMP_GETNEXT) && (datatype != BER_TYPE_SNMP_GET) && (datatype != BER_TYPE_SNMP_SET))
                    {//不支持的SNMP指令
                         return errv;
                    }
                    *qcmd = datatype;
                    break;
               case 5://第5层解码，得到操作ID码
                    if((datatype != BER_TYPE_INTEGER))// || (datasize > 0x02))
                    {//数据异常
                         return errv;
                    }
                    //大端格式
                    u32temp = 0;
                    for(u16intex = 0;u16intex < datasize;u16intex++)
                    {
                         u32temp <<= 8;
                         u32temp += ndp[datastartintex + u16intex];
                    }
                    *qid = u32temp;
                    break;
               case 6://第6层解码，得到得到错误状态码 ndp[0]为错误状态码
                    if((datatype != BER_TYPE_INTEGER) || (datasize != 0x01) || (ndp[datastartintex] != 0x00))
                    {//数据异常
                         return errv;
                    }
                    break;
               case 7://第7层解码，得到得到错误状态码索引号 ndp[0]为错误状态码
                    if((datatype != BER_TYPE_INTEGER) || (datasize != 0x01) || (ndp[datastartintex] != 0x00))
                    {//数据异常
                         return errv;
                    }
                    break;
               case 8://第8层解码（嵌套）----N对OidValue总嵌套
                    if((datatype != BER_TYPE_SEQUENCE) )
                    {//数据异常
                         return errv;
                    }
                    break;
               case 9://第9层解码（嵌套）----单对OidValue嵌套
                    if((datatype != BER_TYPE_SEQUENCE) )
                    {//数据异常
                         return errv;
                    }
                    break;
               case 10://第10层解码，得到OID字符串----单对OID
                    if((datatype != BER_TYPE_OID) )
                    {//数据异常
                         return errv;
                    }
                    if(SNMP_ByteToOIDStr(&ndp[datastartintex],datasize,op[u8oidintex].oid,&oidl[u8oidintex]) < 0)
                    {//字节 转 OID字符串失败
                         return errv;
                    }
                    op[u8oidintex].oidl = oidl[u8oidintex];
                    break;
               case 11://第11层解码，得到OID的值----单对Value
                    if((*qcmd == BER_TYPE_SNMP_GETNEXT) || (*qcmd == BER_TYPE_SNMP_GET))
                    {//如果是Get操作
                         if((datatype != BER_TYPE_NULL) )
                         {//数据异常
                              return errv;
                         }
                         op[u8oidintex].valuel = 0;
                    }
                    else if(*qcmd == BER_TYPE_SNMP_SET)
                    {//如果是Set操作
                         //判断缓存是否够
                         if(valuesl[u8oidintex] < datasize)
                         {//缓存空间不够
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


//u8 *oidsp:oid字符串首地址，注意要以\0结尾
//u8 *dp:转换后存放空间首地址
//u16 *dl:存放空间最大字节数，返回实际有效的字节数
//返回值小于0:转换失败 等于0转换成功
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
     {//非法
          return -1;
     }

     //for(u16i = 4;;u16i++)
     for(u16i = 3;;u16i++)
     {
          if((oidsp[u16i] != '.') && (oidsp[u16i] != '\0'))
          {
               numstr[numintex++] = oidsp[u16i];
          }
          else if(u16i > 4)//避免第1个就出现.
          {               
               numstr[numintex] = '\0';
               numintex = 0;
               s32num = -1;
               s8dintex = 0;
               if(SNMP_StrToS32(numstr,&s32num) < 0)
               {//转换失败
                    return -2;
               }

               if(s32num < 0)
               {
                    return -3;
               }
               else if(s32num < 128)
               {
                    if(maxsize < (u16wintex + 1))
                    {//空间不够
                         return -4;
                    }
                    else
                    {
                         dp[u16wintex++] = (u8)s32num;
                    }
               }
               else
               {
                    s32temp = s32num;//暂存
                    //-----依次整除128，并保存，直至商小于128
                    for(s8dintex = 0;s32temp > 127;s8dintex++)
                    {
                         Darray[s8dintex] = s32temp;
                         s32temp /= 128;
                    }
                    Darray[s8dintex++] = s32temp;//保存最后一次整除后商小于128的数
                    //-----判断空间
                    if(maxsize < (u16wintex + s8dintex))
                    {//空间不够
                         return -4;
                    }
                    //
                    s8dintex--;//索引号减1，指向Darray数组的最后一个数据
                    s8temp = s8dintex;//暂存最后一个索引号
                    for(;s8dintex > -1;s8dintex--)
                    {//此处需要进行反向数据处理，从最后一个开始处理
                         if(s8dintex == s8temp)
                         {//最后一个数据,不求余加0x80
                              dp[u16wintex++] = (0x80 + Darray[s8dintex]);
                         }
                         else if(s8dintex == 0)
                         {//第1个数据，求余后不加0x80
                              dp[u16wintex++] = (Darray[s8dintex] % 128);
                         }
                         else
                         {//其他数据，求余后再加0x80
                              dp[u16wintex++] = ((Darray[s8dintex] % 128) + 0x80);
                         }
                    }
               }
          }
          //
          if(oidsp[u16i] == '\0')
          {
               break;//退出循环
          }
     }
     *dl = u16wintex;//返回dp数组中的有效字节数
     return 0;
}

//BER编码
//u8 dtype:数据类型（BER_TYPE宏定义）
//u8 *vp:待编码数据首地址
//u16 vl:待编码数据字节数
//u16 en:已存在字节数
//u16 upen:本对之前的字节数（不含本对的OID和VALUE）
//u8 *oidvflag:  1表示单条OIDV嵌套   0表示所有嵌套
//u8 *sp:编码后存放空间首址
//u16 *sl:存放空间长度字节数，返回有效的字节数
//返回值：小于0编码失败 等于0编码成功
static s8 SNMP_BerEncode(u8 dtype,u8 *vp,u16 vl,u16 en,u16 upen,u8 *oidvflag,u8 *sp,u16 *sl)
{
     u16 u16temp;
     u16 maxsize = *sl;
     u16 wintex;//字索引号
     u16 bltemp;//字节长度

     *sl = 0;
     wintex = 0;

     if(maxsize < 1)
     {//空间不够
          return -1;
     }
     //存放数据类型
     sp[wintex++] = dtype;

     bltemp = vl;
     if(dtype & 0x20)
     {//嵌套
          bltemp = en;//总嵌套          
          //如果是单个OIDS嵌套
          if(*oidvflag)
          {//如果是单条OIDV嵌套
               *oidvflag = 0;
               bltemp = (en - upen);
          }
     }
     
     //整理数据长度
     if(bltemp > 127)
     {
          if(bltemp <= 255)
          {
               if(maxsize < 3)
               {//空间不够
                    return -1;
               }
               sp[wintex++] = 0x81;
               sp[wintex++] = (u8)bltemp;
          }
          else if(bltemp <= 65535)
          {
               if(maxsize < 4)
               {//空间不够
                    return -1;
               }
               sp[wintex++] = 0x82;
               sp[wintex++] = (u8)(bltemp >> 8);//大端格式
               sp[wintex++] = (u8)bltemp;
          }
          else
          {//越界
               return -2;
          }
     }
     else
     {
          if(maxsize < 2)
          {//空间不够
               return -1;
          }
          sp[wintex++] = (u8)bltemp;
     }

     if((dtype & 0x20) == 0)
     {//非嵌套
          //存放数据
          if(maxsize < (wintex + vl))
          {//空间不够
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

//u8 *oidsp:应答OID字符串指针
//u32 qin:请求码
//u8 valuet:响应值类型
//u8 *valuep:响应值指针
//u8 valuel:响应值字节数
//u8 *dp:编码存放空间首地址
//u16 *dl:编码存放空间长度字节数，返回实际有效的字节数
//返回小于0:编码失败 等于0:编码成功
//static s32 SNMP_Encode(u8 *oidsp,u32 qin,u8 valuet,u8 *valuep,u8 valuel,u8 *dp,u16 *dl)
static s32 SNMP_Encode(u32 qin,OIDV *op,u8 *dp,u16 *dl)
{
     s32 errv;
     s16 s16oidvintex;
     u8 u8temp,oidb[SNMP_MaxOIDSize],buffertemp[SNMP_MaxBerSize];
     u8 oidvsflag;//单条OIDV嵌套标志（1：单条OIDV嵌套  0：所有嵌套）
     u8 ndt;//待处理数据类型
     u8 *ndp;//待处理数据首地址
     u16 ndl;//待处理数据字节数
     u16 wintex;
     u16 maxsize = *dl;
     u16 u16i,upen,en;//之前OIDV已存在的字节数，总已存在的字节数
     u16 oidbl = SNMP_MaxOIDSize,buffertempl = SNMP_MaxBerSize;

     *dl = 0;

     for(s16oidvintex = SNMP_SignelMaxOIDNum - 1;s16oidvintex >= 0;s16oidvintex--)
     {
          if(OidOprList[s16oidvintex].oidl > 0)
          {
               break;//退出循环
          }
     }
     if(s16oidvintex < 0)
     {//没有待响应的数据
          return -1;
     }

     en = 0;
     wintex = (maxsize - 1);
     for(u8temp = 11;u8temp > 0;u8temp--)
     {
          errv = (0 - u8temp);
          switch(u8temp)
          {
               case 11://响应值
                    upen = en;//记录本OIDV之前已存在的字节数
                    ndt = OidOprList[s16oidvintex].datatype;
                    ndp = OidOprList[s16oidvintex].value;
                    ndl = OidOprList[s16oidvintex].valuel;
                    break;
               case 10://响应OID字符串
                    if(SNMP_OIDStrToBytes(OidOprList[s16oidvintex].oid,oidb,&oidbl) < 0)
                    {//转换失败
                         return -10;
                    }
                    ndt = BER_TYPE_OID;
                    ndp = oidb;
                    ndl = oidbl;
                    break;
               case 9://嵌套----每对OIDV值的嵌套
                    ndt = BER_TYPE_SEQUENCE;
                    ndp = NULL_U8P;
                    ndl = 0;
                    //
                    oidvsflag = 1;//单条OIDV嵌套标志
                    s16oidvintex--;
                    if(s16oidvintex >= 0)
                    {//还是还有OIDV对
                         u8temp = 12;
                         //oidvsflag = 1;//还有oidv对
                    }
                    break;
               case 8://嵌套----N对OIDV值的嵌套
                    ndt = BER_TYPE_SEQUENCE;
                    ndp = NULL_U8P;
                    ndl = 0;
                    break;
               case 7://错误索引号
                    oidb[0] = 0;
                    ndt = BER_TYPE_INTEGER;
                    ndp = oidb;
                    ndl = 1;
                    break;
               case 6://错误状态
                    oidb[0] = 0;
                    ndt = BER_TYPE_INTEGER;
                    ndp = oidb;
                    ndl = 1;
                    break;
               case 5://请求IDn
                    ndt = BER_TYPE_INTEGER;
                    ndp = oidb;
                    //转成大端格式（注意：此处之所以统一用4位，是为了保证最高位为0，不至于出现负数!!!）
                    //如果大于127又小于256时，使用1字节，则1字节的高位为1，服务器收到解码后会变成负数!!!
                    oidb[0] = (u8)(qin >> 24);
                    oidb[1] = (u8)(qin >> 16);
                    oidb[2] = (u8)(qin >> 8);
                    oidb[3] = (u8)(qin);
                    ndl = 4;
                    break;
               case 4://应答命令
                    ndt = BER_TYPE_SNMP_RESPONSE;
                    ndp = NULL_U8P;
                    ndl = 0;
                    break;
               case 3://团体名
                    ndt = BER_TYPE_OCTET_STRING;
                    ndp = CommunityNameP;
                    ndl = SNMP_StrLen(CommunityNameP);
                    break;
               case 2://版本号
                    oidb[0] = 0;
                    ndt = BER_TYPE_INTEGER;
                    ndp = oidb;
                    ndl = 1;
                    break;
               case 1://嵌套
                    ndt = BER_TYPE_SEQUENCE;
                    ndp = NULL_U8P;
                    ndl = 0;
                    break;               
               default:
                    ndl = 0;
                    break;
          }

          //编码
          buffertempl = SNMP_MaxBerSize;
          if(SNMP_BerEncode(ndt,ndp,ndl,en,upen,&oidvsflag,buffertemp,&buffertempl) < 0)
          {
               return (errv);
          }

          //判断空间是否足够
          if(maxsize < (en + buffertempl))
          {//空间不够
               return (errv - 10);
          }
          //从后往前存放
          for(u16i = 0;u16i < buffertempl;u16i++)
          {
               dp[wintex--] = buffertemp[buffertempl - u16i - 1];
               en++;
          }
     }
     
     //数据从数组后部 移到 前部
     wintex++;//定位到数据起始索引号
     for(u16i = 0;u16i < en;u16i++)
     {
          dp[u16i] = dp[wintex + u16i];
     }
     *dl = en;//返回有效字节数

     return 0;
}
