/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------以下是模块的修改记录区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * 内  容：
 * 日  期：2023-03-16
 * 作  者：YJX
 * 版本号：V1.0（初版）
 ***********************************************
 * 修改内容：
 * 修改日期：
 * 修改作者：
 * 版 本 号：
 ***********************************************
*/
/********************************************************************************************************************************************
*                                                                                                                                           *
*                ---------------------------------以下是模块的使用说明区-----------------------------------------                           *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*                                       特点说明：

*/
#include "FUN_Match.h"
#include "FUN_String.h"
#include "FUN_BufferOPR.h"
#include "FUN1_HTTPServer.h"
#include "FUN2_ParaWebServer.h"
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下模块间的对接函数区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
#define ParaWebServer_Match_Config(TP)                      FUN_Match_Config(TP)
#define ParaWebServer_Match_Process(TP,DP,DL,PP)            FUN_Match_Process(TP,DP,DL,PP)

//#define ParaWebServer_String_StrLink(P,L,SP)                FUN_String_StrLink(P,L,SP)
#define ParaWebServer_String_STRIsSame2(P1,P2,N)            FUN_String_STRIsSame2(P1,P2,N)
#define ParaWebServer_String_STRIsSame(DP1,DP2)             FUN_String_STRIsSame(DP1,DP2)
#define ParaWebServer_String_Split2(DP,DL,FP,FL,IP,IL)      FUN_String_Split2(DP,DL,FP,FL,IP,IL)
#define ParaWebServer_String_StrLen(SP)                     FUN_String_StrLen(SP)

#define ParaWebServer_BufferOPR_WriteData(MP,DP,DL)         FUN_BufferOPR_WriteData(MP,DP,DL)
#define ParaWebServer_BufferOPR_GetFreeSize(MP)             FUN_BufferOPR_GetFreeSize(MP)
#define ParaWebServer_BufferOPR_ReadData(MP,M,DP,DL)        FUN_BufferOPR_ReadData(MP,M,DP,DL)
#define ParaWebServer_BufferOPR_MoveRP(MP,N)                FUN_BufferOPR_MoveRP(MP,N)
#define ParaWebServer_BufferOPR_Config(TP)                  FUN_BufferOPR_Config(TP)

#define ParaWebServer_HTTPServer_Config(TP)                 FUN1_HTTPServer_Config(TP)
#define ParaWebServer_HTTPServer_ChunkedEncode(DP,DL,SP,SL) FUN1_HTTPServer_ChunkedEncode(DP,DL,SP,SL)
#define ParaWebServer_HTTPServer_Encode(TP,DP,DL)           FUN1_HTTPServer_Encode(TP,DP,DL)
#define ParaWebServer_HTTPServer_Decode(TP,DP,DL)           FUN1_HTTPServer_Decode(TP,DP,DL)
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
#define WebServerStatu_QuestHeadDecode            1    //请求头解码
#define WebServerStatu_QuestUrlProcess            2    //请求Url处理
#define WebServerStatu_QuestDataProcess           3    //请求体数据处理
#define WebServerStatu_WaitQuestDataProcessOK     4    //等待请求体数据处理完成
#define WebServerStatu_RemoveQuestData            5    //读取请求体数据不处理
#define WebServerStatu_AckHeadEncode              6    //响应头编码
#define WebServerStatu_AckDataProcess             7    //响应数据处理

//参数表单名
// #define WebServerParaSecName_WIFI                 "wifi"    //Wifi表单参数名
// #define WebServerParaSecName_Printer              "printer" //Printer表单参数名
//
// #define QuestObject_Unknow                        0
// #define QuestObject_PathHtml                      1    //路径请求(请求Html资源)
// #define QuestObject_ParaForm                      2    //表单请求(参数表单提示)
// #define QuestObject_Other                         3    //其他请求(请求其他资源)

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
// static void ParaWebServer_SetFail(void);
// static void ParaWebServer_SetOK(void);
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块内的 变量 与 宏定义 申明区--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的系统函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
函 数 名: FUN2_ParaWebServer_Config
功   能: 参数Web服务器 配置
参   数:
       无
返 回 值:
       0:配置成功
   小于0:配置失败
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2023-03-07
-------------------------------------------------*/
s8 FUN2_ParaWebServer_Config(ParaWebServerCNFType *tp)
{
     MatchCNFType mctp;
     HttpServerCNFType hsctv;
     BufferCNFType bcf;                           //申明一个BufferCNFType类型的变量bcf
     s8 s8result = 0;

     //接收缓存控制器配置
     if(s8result == 0)
     {
          bcf.BufferCP = &(tp->pwsotp->ReceBuffCtrl);
          bcf.BufferLen = tp->rl;
          bcf.BufferStartP = tp->rp;
          s8result = ParaWebServer_BufferOPR_Config(&bcf);
     }
     //发送缓存控制器配置
     if(s8result == 0)
     {
          bcf.BufferCP = &(tp->pwsotp->SendBuffCtrl);
          bcf.BufferLen = tp->sl;
          bcf.BufferStartP = tp->sp;
          s8result = ParaWebServer_BufferOPR_Config(&bcf);
     }
     //操作缓存配置
     if(s8result == 0)
     {
          tp->pwsotp->OperationBufferP = tp->op;
          tp->pwsotp->OperationBufferSize = tp->ol;
     }
     //路径表配置
     if(s8result == 0)
     {
          mctp.motp = &(tp->pwsotp->WebPathMatch);
          mctp.mttp = tp->PathTableP;
          mctp.SetValueNum = 0;
          mctp.EndValueNum = 1;
          mctp.EndValue[0] = '\0';
          s8result = ParaWebServer_Match_Config(&mctp);
     }
     //参数表配置
     if(s8result == 0)
     {
          mctp.motp = &(tp->pwsotp->ParaTableMatch);
          mctp.mttp = tp->ParaTableP;
          mctp.SetValueNum = 0;
          mctp.EndValueNum = 2;
          mctp.EndValue[0] = '\0';//如果表单是用POST提交，则表单名就是Url，是以\0结尾的
          mctp.EndValue[1] = '?';//如果表单使用GET提交，则表单名是以?结尾的。
          s8result = ParaWebServer_Match_Config(&mctp);
     }
     //HTTP_Server配置
     if(s8result == 0)
     {
          hsctv.hotp = &(tp->pwsotp->HttpOpr);
          s8result = ParaWebServer_HTTPServer_Config(&hsctv);
     }
     tp->pwsotp->FailPageInfFunP = tp->FailPageInfFunP;
     tp->pwsotp->SuccessPageInfFunP = tp->SuccessPageInfFunP;
     tp->pwsotp->WebStateMachine = WebServerStatu_QuestHeadDecode;//请求头解码

     return s8result;
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的用户函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
函 数 名: FUN2_ParaWebServer_Process
功   能: 参数Web服务器 处理
参   数:
       ParaWebServerOPRType *tp：操作对象指针
返 回 值:
       无
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2023-03-07
-------------------------------------------------*/
void FUN2_ParaWebServer_Process(ParaWebServerOPRType *tp)
{
     s8 s8temp;
     u8 *u8p;
     u16 rn,u16temp;
     u16 fi[10],fil = 10;
     s32 s32temp;
     u32 u32intex,u32temp;


     switch(tp->WebStateMachine)
     {
//---------- 请求头解码 ----------
          case WebServerStatu_QuestHeadDecode:
               rn = ParaWebServer_BufferOPR_ReadData(&(tp->ReceBuffCtrl),0,tp->OperationBufferP,tp->OperationBufferSize);//预读
               u16temp = rn;//暂存
               ParaWebServer_HTTPServer_Decode(&(tp->HttpOpr),tp->OperationBufferP,&rn);
               if(rn)
               {
                    ParaWebServer_BufferOPR_MoveRP(&(tp->ReceBuffCtrl),rn);//移除已处理的字节
               }
               else if(u16temp)
               {//需要增加处理超时措施

               }
          //--------
               if(tp->HttpOpr.OprResult == OprResult_DecodeOprOK)
               {//请求数据的 起始行 请求头 已解析完成
                    tp->QuestOprCount = tp->HttpOpr.QContentLength;
                    tp->UrlSize = ParaWebServer_String_StrLen(tp->HttpOpr.Url);//URL字节数
                    tp->QuestProcessFailFlag = 1;
                    tp->WebStateMachine = WebServerStatu_RemoveQuestData;//移除请求体数据

                    //此处URL只匹配路径，不处理表单数据                    
                    if(ParaWebServer_Match_Process(&(tp->WebPathMatch),tp->HttpOpr.Url,tp->UrlSize + 1,(void *)tp) > 0)
                    {//路径匹配正确
                         tp->QuestProcessFailFlag = 0;                       
                    }     
                    else
                    {//非法路径 
                         if(tp->HttpOpr.Modth == Modth_GET)
                         {//如果是GET请求，判断是否有 ?
                              if(ParaWebServer_String_Split2(tp->HttpOpr.Url,tp->UrlSize,"?",1,fi,fil))
                              {//有找到 ?
                                   if(ParaWebServer_Match_Process(&(tp->ParaTableMatch),tp->HttpOpr.Url,tp->UrlSize,(void *)tp) > 0)
                                   {//找到参数表
                                        tp->UrlIntex = (fi[0] + 1);
                                        tp->QuestProcessFailFlag = 0;
                                        tp->WebStateMachine = WebServerStatu_QuestUrlProcess;//请求Url处理
                                   }
                              }
                         }
                         else if(tp->HttpOpr.Modth == Modth_POST)
                         {//如果POST请求，URL作为表单对象
                              if(ParaWebServer_Match_Process(&(tp->ParaTableMatch),tp->HttpOpr.Url,tp->UrlSize + 1,(void *)tp) > 0)
                              {//找到参数表
                                   tp->QuestProcessFailFlag = 0;
                                   tp->WebStateMachine = WebServerStatu_QuestDataProcess;//请求体数据处理
                              }
                         }
                    }               
               }
               break;
//---------- Url数据处理 ----------                 
          case WebServerStatu_QuestUrlProcess://GET表单参数
               rn = (tp->UrlSize - tp->UrlIntex);//未处理字节数
               if(rn)
               {
                    u8p = &(tp->HttpOpr.Url[tp->UrlIntex]);
                    if(ParaWebServer_String_Split2(u8p,rn,"&",1,fi,fil))
                    {//有找到 &
                        s8temp = tp->ParaSetFunP(u8p,fi[0]);
                         if(s8temp == 0)
                         {//参数OK
                              tp->UrlIntex += (fi[0] + 1);//更新UrlIntex
                         }
                         else
                         {//参数异常
                              if(s8temp == -1)
                              {//参数值越界
                                   tp->FailPageInfFunP(tp,u8p,fi[0]);//设置失败提示页面
                              }
                              else
                              {//未知参数
                                   tp->FailPageInfFunP(tp,"ERR",3);//设置失败提示页面
                              }                              
                              tp->WebStateMachine = WebServerStatu_RemoveQuestData;//读取请求体数据不处理
                         }
                    }
                    else 
                    {//可能是最后一对
                        s8temp = tp->ParaSetFunP(u8p,rn);
                         if(s8temp == 0)
                         {//参数OK
                              tp->UrlIntex += rn;//更新UrlIntex
                         }
                         else
                         {//参数异常
                              if(s8temp == -1)
                              {//参数值越界
                                   tp->FailPageInfFunP(tp,u8p,rn);//设置失败提示页面
                              }
                              else
                              {//未知参数
                                   tp->FailPageInfFunP(tp,"ERR",3);//设置失败提示页面
                              }  
                              tp->WebStateMachine = WebServerStatu_RemoveQuestData;//读取请求体数据不处理
                         }
                    }
               }
               else
               {
                    tp->SuccessPageInfFunP(tp,"OK",2);//设置成功提示页面
                    tp->WebStateMachine = WebServerStatu_RemoveQuestData;//读取请求体数据不处理
               }
               break;
//---------- 请求体数据处理 ----------               
          case WebServerStatu_QuestDataProcess://POST表单参数
               if(tp->QuestOprCount)//请求体带数据
               {
                    if(tp->HttpOpr.QContentType == HttpContentType_urlencoded)
                    {//使用 =符做设置符 ，使用 &符做隔离符
                         rn = ParaWebServer_BufferOPR_ReadData(&(tp->ReceBuffCtrl),0,tp->OperationBufferP,tp->OperationBufferSize);//预读
                         if(rn)
                         {//有读到数据
                              if(ParaWebServer_String_Split2(tp->OperationBufferP,rn,"&",1,fi,fil))
                              {//有找到 &
                                   tp->OperationBufferP[fi[0]] = '\0';
                                   s8temp = tp->ParaSetFunP(tp->OperationBufferP,fi[0]);
                                   if(s8temp == 0)
                                   {//参数OK
                                        ParaWebServer_BufferOPR_MoveRP(&(tp->ReceBuffCtrl),fi[0] + 1);//删除缓存数据
                                        tp->QuestOprCount -= (fi[0] + 1);//更新内容字节数
                                   }
                                   else
                                   {//参数异常
                                        if(s8temp == -1)
                                        {//参数值越界                                             
                                             tp->FailPageInfFunP(tp,tp->OperationBufferP,fi[0]);//设置失败提示页面
                                        }
                                        else
                                        {//未知参数
                                             tp->FailPageInfFunP(tp,"ERR",3);//设置失败提示页面
                                        }  
                                        tp->WebStateMachine = WebServerStatu_RemoveQuestData;//读取请求体数据不处理
                                   }
                              }
                              else 
                              {//可能是最后一对
                                   if(tp->QuestOprCount < tp->OperationBufferSize)
                                   {
                                        tp->OperationBufferP[tp->QuestOprCount] = '\0';
                                   }

                                   s8temp = tp->ParaSetFunP(tp->OperationBufferP,tp->QuestOprCount);
                                   if(s8temp == 0)
                                   {//参数OK
                                        ParaWebServer_BufferOPR_MoveRP(&(tp->ReceBuffCtrl),tp->QuestOprCount);//删除缓存数据
                                        tp->QuestOprCount -= (tp->QuestOprCount);//更新内容字节数
                                   }
                                   else
                                   {//参数异常
                                        if(s8temp == -1)
                                        {//参数值越界
                                             tp->FailPageInfFunP(tp,tp->OperationBufferP,tp->QuestOprCount);//设置失败提示页面
                                        }
                                        else
                                        {//未知参数
                                             tp->FailPageInfFunP(tp,"ERR",3);//设置失败提示页面
                                        } 
                                        tp->WebStateMachine = WebServerStatu_RemoveQuestData;//读取请求体数据不处理
                                   }
                              }
                         }
                         else
                         {//此处增加等待超时

                         }
                    }
                    else
                    {//未知数据类型
                         tp->QuestProcessFailFlag = 1;
                         tp->WebStateMachine = WebServerStatu_RemoveQuestData;//读取请求体数据不处理
                    }
               }
               else
               {//待处理字节数为0，处理完成  
                    tp->SuccessPageInfFunP(tp,"OK",2);//设置成功提示页面
                    tp->WebStateMachine = WebServerStatu_RemoveQuestData;//读取请求体数据不处理
               }
               break;  
//---------- 移除请求体数据 ----------               
          case WebServerStatu_RemoveQuestData://移除请求体数据
               if(tp->QuestOprCount)
               {
                    rn = tp->QuestOprCount;
                    if(rn > tp->OperationBufferSize)
                    {
                         rn = tp->OperationBufferSize;
                    }
                    tp->QuestOprCount -= ParaWebServer_BufferOPR_ReadData(&(tp->ReceBuffCtrl),1,tp->OperationBufferP,rn);//直读                    
               }
               else
               {
                    tp->HttpOpr.AckCode = 200;
                    tp->HttpOpr.AckCodeScriptSP = "OK";
                    tp->AckOprCount = 0;//已发送字节数
                    tp->WebStateMachine = WebServerStatu_AckHeadEncode;//切换为编码
                    if(tp->QuestProcessFailFlag)
                    {
                         tp->WebStateMachine = WebServerStatu_QuestHeadDecode;//切换为解码
                    }
               }
               break;
//***************************************************************************************************************************************               
//---------- 响应头编码 ----------                
          case WebServerStatu_AckHeadEncode:
               rn = ParaWebServer_BufferOPR_GetFreeSize(&(tp->SendBuffCtrl));//获取发送缓存空间
               if(rn)
               {
                    if(rn > tp->OperationBufferSize)
                    {
                         rn = tp->OperationBufferSize;
                    }
                    ParaWebServer_HTTPServer_Encode(&(tp->HttpOpr),tp->OperationBufferP,&rn);
                    if(rn)
                    {
                         ParaWebServer_BufferOPR_WriteData(&(tp->SendBuffCtrl),tp->OperationBufferP,rn);
                    }
               } 
          //-----
               if(tp->HttpOpr.OprResult == OprResult_EncodeOprOK)
               {//响应数据的 起始行 响应头 已编码完成
                    tp->WebStateMachine = WebServerStatu_QuestHeadDecode;//切换为解码
                    if(tp->HttpOpr.AContentLength)
                    {//有数据要发送
                         tp->WebStateMachine = WebServerStatu_AckDataProcess;
                    }
               }
               else
               {//需要增加超时机制

               }
               break;
//---------- 组织响应体数据 ----------               
          case WebServerStatu_AckDataProcess:
               if(tp->HttpOpr.AContentLength > 0)
               {//按字节量发送数据
                    s32temp = (tp->HttpOpr.AContentLength - tp->AckOprCount);//剩余字节量
                    if(s32temp > 0)
                    {
                         u32temp = (u32)s32temp;//剩余字节量
                         if(u32temp > tp->OperationBufferSize)
                         {
                              u32temp = tp->OperationBufferSize;
                         }
                         u32intex = ParaWebServer_BufferOPR_GetFreeSize(&(tp->SendBuffCtrl));
                         if(u32temp > u32intex)
                         {
                              u32temp = u32intex;
                         }
                         //读数据到中间缓存
                         s32temp = tp->GetDataFunP(tp->OperationBufferP,u32temp,1);
                         u32temp = 0;
                         if(s32temp >= 0)
                         {//读到数据
                              u32temp = (u32)s32temp;
                         }
                         else
                         {//等待 或 出错，考虑增加超时

                         }
                         tp->AckOprCount += ParaWebServer_BufferOPR_WriteData(&(tp->SendBuffCtrl),tp->OperationBufferP,u32temp);
                    }
                    else
                    {//发送完成
                         tp->WebStateMachine = WebServerStatu_QuestHeadDecode;//切换为解码
                    }
               }
               else if(tp->HttpOpr.AContentLength < 0)
               {//按chunked模式发送数据               
                    u32temp = tp->OperationBufferSize;
                    u32intex = ParaWebServer_BufferOPR_GetFreeSize(&(tp->SendBuffCtrl));
                    if(u32temp > u32intex)
                    {
                         u32temp = u32intex;
                    }
                    if(u32temp > 10)
                    {//可以读取，因为预留10个字节用于存放chunked信息
                         u32temp -= 10;
                         s32temp = tp->GetDataFunP(tp->OperationBufferP,u32temp,1);
                         if(s32temp >= 0)
                         {//读到数据
                              u32temp = (u32)s32temp;
                              rn = tp->OperationBufferSize;
                              s8temp = ParaWebServer_HTTPServer_ChunkedEncode(tp->OperationBufferP,u32temp,tp->OperationBufferP,&rn);
                              if(s8temp == 0)
                              {//编码成功
                                   ParaWebServer_BufferOPR_WriteData(&(tp->SendBuffCtrl),tp->OperationBufferP,rn);
                                   if(u32temp == 0)
                                   {//无数据了
                                        tp->WebStateMachine = WebServerStatu_QuestHeadDecode;//切换为解码
                                   }
                              }
                              else
                              {//编码错误
                                   
                              }
                         }
                         else
                         {//出错 或 等待数据
                              
                         }                       
                    }
               }
               else
               {
                    tp->WebStateMachine = WebServerStatu_QuestHeadDecode;//切换为解码
               }
               break;
//---------- 其他 ----------               
          default:
               tp->HttpOpr.OprResult = OprResult_OprReset;
               tp->WebStateMachine = WebServerStatu_QuestHeadDecode;
               break;
     }
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/


