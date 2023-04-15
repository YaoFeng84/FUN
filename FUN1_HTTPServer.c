/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------以下是模块的修改记录区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * 内容：
 * 日期：2023-03-05
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
#include "FUN_String.h"
#include "FUN_Match.h"
#include "FUN1_HTTPServer.h"
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下模块间的对接函数区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
//#define Http_StrIsSame(SP1,SP2)                   FUN_String_STRIsSame(SP1,SP2)
#define Http_String_STRIsSame2(SP1,SP2,N)         FUN_String_STRIsSame2(SP1,SP2,N)
#define Http_String_Split2(DP,DL,FP,FL,IP,IL)     FUN_String_Split2(DP,DL,FP,FL,IP,IL)
#define Http_String_StrLink3(SP,SL,DP,DLP)        FUN_String_StrLink3(SP,SL,DP,DLP)
#define Http_String_Ltrim(P)                      FUN_String_Ltrim(P)
#define Http_String_S32ToStr(N,SP,SL)             FUN_String_S32ToStr(N,SP,SL)
#define Http_String_StrLen(SP)                    FUN_String_StrLen(SP)
#define Http_String_StrToS32(SP,NP)               FUN_String_StrToS32(SP,NP)
#define Http_String_CharToHexStr(N,P)             FUN_String_CharToHexStr(N,P)
//
#define HttpServer_Match_Config(P)                FUN_Match_Config(P)
#define HttpServer_Match_Process(MP,DP,DL,PP)     FUN_Match_Process(MP,DP,DL,PP)

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


#define HttpStateMachine_Decode_Start       1    //解码开始
#define HttpStateMachine_Decode_QuestFirst   2    //解码请求首行
#define HttpStateMachine_Decode_QuestHead   3    //解码请求头
#define HttpSataeMachine_Decode_IDE         4    //空闲

#define HttpStateMachine_Encode_Start       11   //编码开始
#define HttpStateMachine_Encode_AckFirst    12   //编码响应首行
#define HttpStateMachine_Encode_AckHead     13   //编码响应头
#define HttpSataeMachine_Encode_IDE         14    //空闲
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块内的变量类型定义区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/


/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数申明区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static void HttpServer_QuestHead_Connection(u8 flag,u8 *dp,u16 dl,void *pp);
static void HttpServer_QuestHead_Host(u8 flag,u8 *dp,u16 dl,void *pp);
static void HttpServer_QuestHead_UserAgent(u8 flag,u8 *dp,u16 dl,void *pp);
static void HttpServer_QuestHead_ContentLength(u8 flag,u8 *dp,u16 dl,void *pp);
static void HttpServer_QuestHead_ContentType(u8 flag,u8 *dp,u16 dl,void *pp);
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块内的 变量 与 宏定义 申明区--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static MatchTableType QuestHeadTable[] = //请求头表
{     
     {"Host",                      0,HttpServer_QuestHead_Host                       },//Post/Get请求
     {"User-Agent",                0,HttpServer_QuestHead_UserAgent                  },//Post/Get请求
     {"Connection",                0,HttpServer_QuestHead_Connection                 },//Post/Get请求
     {"Content-Length",            0,HttpServer_QuestHead_ContentLength              },//Post请求
     {"Content-Type",              0,HttpServer_QuestHead_ContentType                },//Post请求
     {0,0,0}
};
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的系统函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
函数名: FUN1_HTTPServer_Config
功   能: HTTPServerServer配置
参   数:
       
返回值:
       等于0:配置成功
       小于0:配置失败
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2023-02-07   
-------------------------------------------------*/
s8 FUN1_HTTPServer_Config(HttpServerCNFType *hsctp)
{
     MatchCNFType mctv;

     mctv.mttp = QuestHeadTable;//hctp->mttp;//初始化匹配表指针
     mctv.motp = &(hsctp->hotp->motv);//初始化匹配操作对象
     mctv.SetValueNum = 1;
     mctv.SetValue[0] = ':';
     mctv.EndValueNum = 3;
     mctv.EndValue[0] = 0x0d;
     mctv.EndValue[1] = 0x0a;
     mctv.EndValue[2] = 0x00;

     //
     hsctp->hotp->StateMachine = HttpStateMachine_Decode_Start;//默认开始解码

     return HttpServer_Match_Config(&mctv);
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的用户函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
函数名: FUN1_HTTPServer_Decode
功   能: HTTP请求头数据解码
参   数:
       HttpServerOPRType *hotp：对象结构体首地址
       u8  *dp：待解码数据首地址
       u16 *dl：(输入)待解码数据字节数 (输出)实际处理数据字节数
返回值:
       0:未出错
   小于0:出错
注   意:
       以一行数据+0d0a为最小解码单元
示   例:
作   者:YJX
版   本:V1.0
时   间:2023-03-07   
-------------------------------------------------*/
s8 FUN1_HTTPServer_Decode(HttpServerOPRType *hotp,u8 *dp,u16 *dl)
{
     u8 fl = 2,fd[] = {0x0d,0x0a};//回车换行//{'\r','\n'};
     u8 f1l =1,f1d[] = {' '};//空格
     u8 f2l =4,f2d[] = {0x0d,0x0a,0x0d,0x0a};//回车换行//{'\r','\n'};
     u16 ip[20],il = 20;
     s16 s16temp;
     s8 s8result = 0;
     u16 maxsize = *dl;
     u16 u16intex;


     *dl = 0;

     if(hotp->OprResult == OprResult_OprReset)
     {          
          hotp->StateMachine = HttpStateMachine_Decode_Start;
     }
     switch(hotp->StateMachine)
     {     
          case HttpSataeMachine_Decode_IDE:
               break;
          case HttpStateMachine_Decode_Start:
               hotp->OprResult = OprResult_DecodeOprIng;
               hotp->Connect = 0;
               hotp->QContentLength = 0;//0字节
               hotp->QContentType = HttpContentType_unknow;
               hotp->Modth = HttpVersion_unknow;//未知
               hotp->Url[0] = 0;
               hotp->StateMachine = HttpStateMachine_Decode_QuestFirst;
               hotp->Version = 0;
               break;
          case HttpStateMachine_Decode_QuestFirst://解析请求首行，结构：方式 URL 版本
               s16temp = Http_String_Split2(dp,maxsize,fd,fl,ip,il);//寻找\r\n----请求首行结束
               if(s16temp)
               {//有找到\r\n
                    *dl = (ip[0] + fl);//置已处理字节数
                    s16temp = Http_String_Split2(dp,ip[0],f1d,f1l,ip,il);//从第1组中寻找空格
                    if(s16temp == 2)
                    {//有找到 2 个空格
                    //-----获取 请求方法
                         if(ip[0] <= 4)
                         {
                              if(Http_String_STRIsSame2(dp,"GET",3) == 0)
                              {//GET方法
                                   hotp->Modth = Modth_GET;
                              }
                              else if(Http_String_STRIsSame2(dp,"POST",4) == 0)
                              {//POST方法
                                   hotp->Modth = Modth_POST;
                              }  
                              else
                              {
                                   s8result = -1;
                              }
                         }
                         else
                         {
                              s8result = -2;
                         }
                    //-----获取 URL
                         if(s8result == 0)
                         {//可以继续解析URL
                              s16temp = (ip[1] - ip[0] - f1l);//URL长度
                              if(s16temp <= HttpURLMaxLength)
                              {
                                  for(u16intex = 0;u16intex < s16temp;u16intex++)
                                   {
                                        hotp->Url[u16intex] = dp[ip[0] + f1l + u16intex];
                                   }
                                   hotp->Url[u16intex] = '\0'; 
                              }
                              else
                              {
                                   s8result = -3;
                              }
                         }
                    //-----获取 版本号
                         if(s8result == 0)
                         {//可以继续解析版本号
                              //
                              if(Http_String_STRIsSame2(&dp[ip[1] + f1l],"HTTP/1.0",8) == 0)
                              {
                                   hotp->Version = HttpVersion_1d0;
                                   hotp->StateMachine = HttpStateMachine_Decode_QuestHead;
                              }
                              else if(Http_String_STRIsSame2(&dp[ip[1] + f1l],"HTTP/1.1",8) == 0)
                              {
                                   hotp->Version = HttpVersion_1d1;
                                   hotp->StateMachine = HttpStateMachine_Decode_QuestHead;
                              }  
                              else
                              {
                                   s8result = -4;
                              }                            
                         }
                    }
                    else
                    {
                         s8result = -5;
                    }
               }
               break;
          case HttpStateMachine_Decode_QuestHead:          
               s16temp = Http_String_Split2(dp,maxsize,f2d,f2l,ip,il);//寻找\r\n\r\n----请求头结束
               if(s16temp)
               {//找到请求头结束                    
                    *dl = (ip[0] + f2l);//置已处理字节数
               }
               s16temp = HttpServer_Match_Process(&(hotp->motv),dp,maxsize,(void *)hotp);
               if(s16temp)
               {
                    if(s16temp < 0)
                    {
                         s16temp = (0 - s16temp);
                         s8result = -6;
                    }
                    if((s16temp >= *dl) || ((*dl - s16temp) < f2l))
                    {//认为结束了    
                         hotp->OprResult = OprResult_DecodeOprOK;                
                         hotp->StateMachine = HttpSataeMachine_Decode_IDE;
                    }
                    else
                    {//还有数据未处理
                         *dl = s16temp;
                    }                                 
               }
               break;

          default:
               hotp->StateMachine = HttpStateMachine_Decode_Start;
               break;
     }     
     return s8result;
}

/*-------------------------------------------------
函数名: FUN1_HTTPServer_Encode
功   能: HTTP响应头编码
参   数:
       HttpServerOPRType *hotp：对象结构体首地址
       u8  *sp：存放空间的首地址
       u16 *sl：(输入)存放空间的最大字节数 (输出)存放空间的编码数据的有效字节数
返回值:
       0:未出错
   小于0:出错
注   意:
       以一行数据+0d0a为最小编码码单元
示   例:
作   者:YJX
版   本:V1.0
时   间:2023-03-08   
-------------------------------------------------*/
s8 FUN1_HTTPServer_Encode(HttpServerOPRType *hotp,u8 *sp,u16 *sl)
{
     u8 datatemp[20],dtl = 20;
     s8 s8result = 0;
     u16 u16wi = 0;//当前wi
     u16 maxsize = *sl;

     *sl = 0;
     sp[0] = '\0';

     if(hotp->OprResult == OprResult_OprReset)
     {          
          hotp->StateMachine = HttpStateMachine_Encode_Start;
     }
     switch(hotp->StateMachine)
     {
          case HttpSataeMachine_Encode_IDE:
               break;
          case HttpStateMachine_Encode_Start:
               hotp->OprResult = OprResult_EncodeOprIng;               
               hotp->StateMachine = HttpStateMachine_Encode_AckFirst;
               break;
          case HttpStateMachine_Encode_AckFirst:               
               //版本号        
               // ok++;   
               s8result = Http_String_StrLink3(sp,maxsize,"HTTP/",&u16wi);
               if(hotp->Version == HttpVersion_1d0)
               {
                    s8result = Http_String_StrLink3(sp,maxsize,"1.0 ",&u16wi);
               }
               else if(hotp->Version == HttpVersion_1d1)
               {
                    s8result = Http_String_StrLink3(sp,maxsize,"1.1 ",&u16wi);
               }
               else
               {
                    s8result = -1;
               }
               //响应码转换
               Http_String_S32ToStr(hotp->AckCode,datatemp,dtl);
               s8result = Http_String_StrLink3(sp,maxsize,datatemp,&u16wi);
               s8result = Http_String_StrLink3(sp,maxsize," ",&u16wi);
               s8result = Http_String_StrLink3(sp,maxsize,hotp->AckCodeScriptSP,&u16wi);
               s8result = Http_String_StrLink3(sp,maxsize,"\r\n",&u16wi);     

               if(s8result == 0)
               {
                    hotp->EncodeStateMachine = 0;
                    hotp->StateMachine = HttpStateMachine_Encode_AckHead;  
               }                    
               break;
          case HttpStateMachine_Encode_AckHead:      
               switch(hotp->EncodeStateMachine)
               {
                    case 0:
                         s8result = Http_String_StrLink3(sp,maxsize,"Pragma: no-cache\r\n",&u16wi);
                         break;
                    case 1:
                         s8result = Http_String_StrLink3(sp,maxsize,"Cache-Control: no-store\r\n",&u16wi);
                         break;
                    case 2:
                         s8result = Http_String_StrLink3(sp,maxsize,"Cache-Control: no-cache\r\n",&u16wi);
                         break;
                    case 3:
                         s8result = Http_String_StrLink3(sp,maxsize,"Connection: close\r\n",&u16wi);
                         break;
                    case 4:
                         s8result = Http_String_StrLink3(sp,maxsize,"Content-Type: ",&u16wi);
                         if(hotp->AContentType == HttpContentType_TextHtml)
                         {
                              s8result = Http_String_StrLink3(sp,maxsize,"text/html\r\n",&u16wi);
                         }
                         else if(hotp->AContentType == HttpContentType_TextCss)
                         {
                              s8result = Http_String_StrLink3(sp,maxsize,"text/css\r\n",&u16wi);
                         }
                         else if(hotp->AContentType == HttpContentType_TextJavaScript)
                         {
                              s8result = Http_String_StrLink3(sp,maxsize,"text/javascript\r\n",&u16wi);
                         }
                         else if(hotp->AContentType == HttpContentType_Image)
                         {
                              s8result = Http_String_StrLink3(sp,maxsize,"image/*\r\n",&u16wi);
                         }
                         else
                         {
                              s8result = -2;
                         }
                         break;
                    case 5:
                         if(hotp->AContentLength > 0)
                         {//长度采用字节量定义
                              s8result = Http_String_StrLink3(sp,maxsize,"Accept-Ranges: bytes\r\n",&u16wi);
                              s8result = Http_String_StrLink3(sp,maxsize,"Content-Length: ",&u16wi);
                              Http_String_S32ToStr(hotp->AContentLength,datatemp,dtl);
                              s8result = Http_String_StrLink3(sp,maxsize,datatemp,&u16wi);
                              s8result = Http_String_StrLink3(sp,maxsize,"\r\n",&u16wi);
                         }
                         else if(hotp->AContentLength < 0)
                         {//长度采用chunked模式定义
                              s8result = Http_String_StrLink3(sp,maxsize,"Transfer-Encoding: chunked\r\n",&u16wi);
                         }
                         break;
                    case 6:
                         s8result = Http_String_StrLink3(sp,maxsize,"\r\n",&u16wi);//响应头结束                         
                         break;
                    case 7:
                         hotp->StateMachine = HttpSataeMachine_Encode_IDE;
                         hotp->OprResult = OprResult_EncodeOprOK;
                         break;
                    default:
                         hotp->EncodeStateMachine = 0;
                         break;
               }
               if(s8result == 0)
               {
                    hotp->EncodeStateMachine += 1;
               }
               break;

          default:
               hotp->StateMachine = HttpStateMachine_Encode_Start;
               break;
     }

     if(s8result == 0)
     {
          *sl = u16wi;
     }     
     return s8result;
}

/*-------------------------------------------------
函数名: FUN1_HTTPServer_ChunkedEncode
功   能: HTTP的Chunked编码
参   数:
       u8 *dp：待编码数据首地址
       u16 dl：待编码数据字节数(允许为0，表示结束)
       u8  *sp：存放空间的首地址
       u16 *sl：(输入)存放空间的最大字节数 (输出)存放空间的编码数据的有效字节数
返回值:
       0:编码成功
   小于0:编码出错
注   意:
       支持dp和sp相同缓存
示   例:
作   者:YJX
版   本:V1.0
时   间:2023-03-15  
-------------------------------------------------*/
s8 FUN1_HTTPServer_ChunkedEncode(u8 *dp,u16 dl,u8 *sp,u16 *sl)
{
     u8 hexs[4],u8i;
     u16 u16temp = *sl,u16i,u16r;

     *sl = 0;
     Http_String_CharToHexStr((u8)(dl >> 8),hexs);//高8位转Hex字符
     Http_String_CharToHexStr((u8)(dl),&hexs[2]);//低8位转Hex字符
//-----寻找首个不为0的索引号
     for(u8i = 0;u8i < 4;u8i++)//去除前导为'0'
     {
          if(hexs[u8i] != '0')
          {
               break;
          }
     }
     if(u8i >= 4)
     {//如果都为0，表示dl=0，索引号指到最后一个字符上
          u8i = 3;
     }
//-----判断存放空间是否够
     //第1个4表示有2个\r\n，第2个4是指hexs的字节数
     u16i = (dl + 4 + (4 - u8i));//计算编码后所需的空间字节数
     if(u16i > u16temp)
     {//空间不够
          return -1;
     }
     *sl = u16i;//保存返回有效字节数
//-----此处从后往前放置数据-----支持输入输出缓存相同
     u16i--;
     //存放chunked数据的结束符\r\n
     sp[u16i--] = '\n';
     sp[u16i--] = '\r';
     //存放chunked数据
     if(dl)
     {
          u16r = (dl - 1);
          for(u16temp = 0;u16temp < dl;u16temp++)
          {
               sp[u16i--] = dp[u16r--];
          }
     }
     //存放chunked数据长度的结束符\r\n
     sp[u16i--] = '\n';
     sp[u16i--] = '\r';
     //存放chunked数据长度
     u16r = 3;
     for(u16temp = 0;u16temp < (4 - u8i);u16temp++)
     {
          sp[u16i--] = hexs[u16r--];
     }
     return 0;
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static void HttpServer_QuestHead_Connection(u8 flag,u8 *dp,u16 dl,void *pp)
{
     HttpServerOPRType *hsotp = (HttpServerOPRType *)pp;
     if(flag)
     {
          hsotp->Connect = 1;//短连接
          dp = Http_String_Ltrim(dp);//去除左侧空格          
          if(Http_String_STRIsSame2(dp,"keep-alive",10) == 0)
          {//相同
               hsotp->Connect = 2;//长连接
          }
     }
}

static void HttpServer_QuestHead_Host(u8 flag,u8 *dp,u16 dl,void *pp)
{
     
}

static void HttpServer_QuestHead_UserAgent(u8 flag,u8 *dp,u16 dl,void *pp)
{
     
}

static void HttpServer_QuestHead_ContentLength(u8 flag,u8 *dp,u16 dl,void *pp)
{//Post请求中的内容字节数
     HttpServerOPRType *hsotp = (HttpServerOPRType *)pp;
     u8 dtemp[32],di,*u8p;
     s32 s32temp;
     // ok = 123;
     if(flag)
     {
          // ok = 124;
          hsotp->QContentLength = 0;
          if(dl < 32)
          {
               for(di = 0;di < dl;di++)
               {
                    dtemp[di] = dp[di];
               }
               dtemp[di] = '\0';

               u8p = Http_String_Ltrim(dtemp);//去除左侧空格  
               if(Http_String_StrToS32(u8p,&s32temp) == 0)
               {
                    // ok = 125;
                    hsotp->QContentLength = (u16)s32temp;
               }
          }
     }
}

static void HttpServer_QuestHead_ContentType(u8 flag,u8 *dp,u16 dl,void *pp)
{//Post请求中的内容类型
     HttpServerOPRType *hsotp = (HttpServerOPRType *)pp;
     u8 *ctp1 = "application/x-www-form-urlencoded";
     u8 *ctp2 = "application/json";
     u8 *ctp3 = "multipart/form-data";
     if(flag)
     {
          dp = Http_String_Ltrim(dp);//去除左侧空格          
          if(Http_String_STRIsSame2(dp,ctp1,Http_String_StrLen(ctp1)) == 0)
          {//相同
               hsotp->QContentType = HttpContentType_urlencoded;
          }
          else if(Http_String_STRIsSame2(dp,ctp2,Http_String_StrLen(ctp2)) == 0)
          {//相同
               hsotp->QContentType = HttpContentType_json;
          }
          else if(Http_String_STRIsSame2(dp,ctp3,Http_String_StrLen(ctp3)) == 0)
          {//相同
               hsotp->QContentType = HttpContentType_multipart;
          }
          else
          {
               hsotp->QContentType = HttpContentType_unknow;
          }
     }
}
