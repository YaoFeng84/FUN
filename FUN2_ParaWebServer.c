/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------������ģ����޸ļ�¼��-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * ��  �ݣ�
 * ��  �ڣ�2023-03-16
 * ��  �ߣ�YJX
 * �汾�ţ�V1.0�����棩
 ***********************************************
 * �޸����ݣ�
 * �޸����ڣ�
 * �޸����ߣ�
 * �� �� �ţ�
 ***********************************************
*/
/********************************************************************************************************************************************
*                                                                                                                                           *
*                ---------------------------------������ģ���ʹ��˵����-----------------------------------------                           *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*                                       �ص�˵����

*/
#include "FUN_Match.h"
#include "FUN_String.h"
#include "FUN_BufferOPR.h"
#include "FUN1_HTTPServer.h"
#include "FUN2_ParaWebServer.h"
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------����ģ���ĶԽӺ�����-----------------------------------------                             *
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
*               ----------------------------------������ģ���ı���������--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ڵĺ궨����------------------------------------                                  *
*                                                                                                                                           *
********************************************************************************************************************************************/
#define WebServerStatu_QuestHeadDecode            1    //����ͷ����
#define WebServerStatu_QuestUrlProcess            2    //����Url����
#define WebServerStatu_QuestDataProcess           3    //���������ݴ���
#define WebServerStatu_WaitQuestDataProcessOK     4    //�ȴ����������ݴ������
#define WebServerStatu_RemoveQuestData            5    //��ȡ���������ݲ�����
#define WebServerStatu_AckHeadEncode              6    //��Ӧͷ����
#define WebServerStatu_AckDataProcess             7    //��Ӧ���ݴ���

//��������
// #define WebServerParaSecName_WIFI                 "wifi"    //Wifi��������
// #define WebServerParaSecName_Printer              "printer" //Printer��������
//
// #define QuestObject_Unknow                        0
// #define QuestObject_PathHtml                      1    //·������(����Html��Դ)
// #define QuestObject_ParaForm                      2    //������(��������ʾ)
// #define QuestObject_Other                         3    //��������(����������Դ)

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ڵı������Ͷ�����------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����ڲ�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
// static void ParaWebServer_SetFail(void);
// static void ParaWebServer_SetOK(void);
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ڵ� ���� �� �궨�� ������--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ϵͳ����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
�� �� ��: FUN2_ParaWebServer_Config
��   ��: ����Web������ ����
��   ��:
       ��
�� �� ֵ:
       0:���óɹ�
   С��0:����ʧ��
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-03-07
-------------------------------------------------*/
s8 FUN2_ParaWebServer_Config(ParaWebServerCNFType *tp)
{
     MatchCNFType mctp;
     HttpServerCNFType hsctv;
     BufferCNFType bcf;                           //����һ��BufferCNFType���͵ı���bcf
     s8 s8result = 0;

     //���ջ������������
     if(s8result == 0)
     {
          bcf.BufferCP = &(tp->pwsotp->ReceBuffCtrl);
          bcf.BufferLen = tp->rl;
          bcf.BufferStartP = tp->rp;
          s8result = ParaWebServer_BufferOPR_Config(&bcf);
     }
     //���ͻ������������
     if(s8result == 0)
     {
          bcf.BufferCP = &(tp->pwsotp->SendBuffCtrl);
          bcf.BufferLen = tp->sl;
          bcf.BufferStartP = tp->sp;
          s8result = ParaWebServer_BufferOPR_Config(&bcf);
     }
     //������������
     if(s8result == 0)
     {
          tp->pwsotp->OperationBufferP = tp->op;
          tp->pwsotp->OperationBufferSize = tp->ol;
     }
     //·��������
     if(s8result == 0)
     {
          mctp.motp = &(tp->pwsotp->WebPathMatch);
          mctp.mttp = tp->PathTableP;
          mctp.SetValueNum = 0;
          mctp.EndValueNum = 1;
          mctp.EndValue[0] = '\0';
          s8result = ParaWebServer_Match_Config(&mctp);
     }
     //����������
     if(s8result == 0)
     {
          mctp.motp = &(tp->pwsotp->ParaTableMatch);
          mctp.mttp = tp->ParaTableP;
          mctp.SetValueNum = 0;
          mctp.EndValueNum = 2;
          mctp.EndValue[0] = '\0';//���������POST�ύ�����������Url������\0��β��
          mctp.EndValue[1] = '?';//�����ʹ��GET�ύ�����������?��β�ġ�
          s8result = ParaWebServer_Match_Config(&mctp);
     }
     //HTTP_Server����
     if(s8result == 0)
     {
          hsctv.hotp = &(tp->pwsotp->HttpOpr);
          s8result = ParaWebServer_HTTPServer_Config(&hsctv);
     }
     tp->pwsotp->FailPageInfFunP = tp->FailPageInfFunP;
     tp->pwsotp->SuccessPageInfFunP = tp->SuccessPageInfFunP;
     tp->pwsotp->WebStateMachine = WebServerStatu_QuestHeadDecode;//����ͷ����

     return s8result;
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����û�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
�� �� ��: FUN2_ParaWebServer_Process
��   ��: ����Web������ ����
��   ��:
       ParaWebServerOPRType *tp����������ָ��
�� �� ֵ:
       ��
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-03-07
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
//---------- ����ͷ���� ----------
          case WebServerStatu_QuestHeadDecode:
               rn = ParaWebServer_BufferOPR_ReadData(&(tp->ReceBuffCtrl),0,tp->OperationBufferP,tp->OperationBufferSize);//Ԥ��
               u16temp = rn;//�ݴ�
               ParaWebServer_HTTPServer_Decode(&(tp->HttpOpr),tp->OperationBufferP,&rn);
               if(rn)
               {
                    ParaWebServer_BufferOPR_MoveRP(&(tp->ReceBuffCtrl),rn);//�Ƴ��Ѵ�����ֽ�
               }
               else if(u16temp)
               {//��Ҫ���Ӵ���ʱ��ʩ

               }
          //--------
               if(tp->HttpOpr.OprResult == OprResult_DecodeOprOK)
               {//�������ݵ� ��ʼ�� ����ͷ �ѽ������
                    tp->QuestOprCount = tp->HttpOpr.QContentLength;
                    tp->UrlSize = ParaWebServer_String_StrLen(tp->HttpOpr.Url);//URL�ֽ���
                    tp->QuestProcessFailFlag = 1;
                    tp->WebStateMachine = WebServerStatu_RemoveQuestData;//�Ƴ�����������

                    //�˴�URLֻƥ��·���������������                    
                    if(ParaWebServer_Match_Process(&(tp->WebPathMatch),tp->HttpOpr.Url,tp->UrlSize + 1,(void *)tp) > 0)
                    {//·��ƥ����ȷ
                         tp->QuestProcessFailFlag = 0;                       
                    }     
                    else
                    {//�Ƿ�·�� 
                         if(tp->HttpOpr.Modth == Modth_GET)
                         {//�����GET�����ж��Ƿ��� ?
                              if(ParaWebServer_String_Split2(tp->HttpOpr.Url,tp->UrlSize,"?",1,fi,fil))
                              {//���ҵ� ?
                                   if(ParaWebServer_Match_Process(&(tp->ParaTableMatch),tp->HttpOpr.Url,tp->UrlSize,(void *)tp) > 0)
                                   {//�ҵ�������
                                        tp->UrlIntex = (fi[0] + 1);
                                        tp->QuestProcessFailFlag = 0;
                                        tp->WebStateMachine = WebServerStatu_QuestUrlProcess;//����Url����
                                   }
                              }
                         }
                         else if(tp->HttpOpr.Modth == Modth_POST)
                         {//���POST����URL��Ϊ������
                              if(ParaWebServer_Match_Process(&(tp->ParaTableMatch),tp->HttpOpr.Url,tp->UrlSize + 1,(void *)tp) > 0)
                              {//�ҵ�������
                                   tp->QuestProcessFailFlag = 0;
                                   tp->WebStateMachine = WebServerStatu_QuestDataProcess;//���������ݴ���
                              }
                         }
                    }               
               }
               break;
//---------- Url���ݴ��� ----------                 
          case WebServerStatu_QuestUrlProcess://GET������
               rn = (tp->UrlSize - tp->UrlIntex);//δ�����ֽ���
               if(rn)
               {
                    u8p = &(tp->HttpOpr.Url[tp->UrlIntex]);
                    if(ParaWebServer_String_Split2(u8p,rn,"&",1,fi,fil))
                    {//���ҵ� &
                        s8temp = tp->ParaSetFunP(u8p,fi[0]);
                         if(s8temp == 0)
                         {//����OK
                              tp->UrlIntex += (fi[0] + 1);//����UrlIntex
                         }
                         else
                         {//�����쳣
                              if(s8temp == -1)
                              {//����ֵԽ��
                                   tp->FailPageInfFunP(tp,u8p,fi[0]);//����ʧ����ʾҳ��
                              }
                              else
                              {//δ֪����
                                   tp->FailPageInfFunP(tp,"ERR",3);//����ʧ����ʾҳ��
                              }                              
                              tp->WebStateMachine = WebServerStatu_RemoveQuestData;//��ȡ���������ݲ�����
                         }
                    }
                    else 
                    {//���������һ��
                        s8temp = tp->ParaSetFunP(u8p,rn);
                         if(s8temp == 0)
                         {//����OK
                              tp->UrlIntex += rn;//����UrlIntex
                         }
                         else
                         {//�����쳣
                              if(s8temp == -1)
                              {//����ֵԽ��
                                   tp->FailPageInfFunP(tp,u8p,rn);//����ʧ����ʾҳ��
                              }
                              else
                              {//δ֪����
                                   tp->FailPageInfFunP(tp,"ERR",3);//����ʧ����ʾҳ��
                              }  
                              tp->WebStateMachine = WebServerStatu_RemoveQuestData;//��ȡ���������ݲ�����
                         }
                    }
               }
               else
               {
                    tp->SuccessPageInfFunP(tp,"OK",2);//���óɹ���ʾҳ��
                    tp->WebStateMachine = WebServerStatu_RemoveQuestData;//��ȡ���������ݲ�����
               }
               break;
//---------- ���������ݴ��� ----------               
          case WebServerStatu_QuestDataProcess://POST������
               if(tp->QuestOprCount)//�����������
               {
                    if(tp->HttpOpr.QContentType == HttpContentType_urlencoded)
                    {//ʹ�� =�������÷� ��ʹ�� &���������
                         rn = ParaWebServer_BufferOPR_ReadData(&(tp->ReceBuffCtrl),0,tp->OperationBufferP,tp->OperationBufferSize);//Ԥ��
                         if(rn)
                         {//�ж�������
                              if(ParaWebServer_String_Split2(tp->OperationBufferP,rn,"&",1,fi,fil))
                              {//���ҵ� &
                                   tp->OperationBufferP[fi[0]] = '\0';
                                   s8temp = tp->ParaSetFunP(tp->OperationBufferP,fi[0]);
                                   if(s8temp == 0)
                                   {//����OK
                                        ParaWebServer_BufferOPR_MoveRP(&(tp->ReceBuffCtrl),fi[0] + 1);//ɾ����������
                                        tp->QuestOprCount -= (fi[0] + 1);//���������ֽ���
                                   }
                                   else
                                   {//�����쳣
                                        if(s8temp == -1)
                                        {//����ֵԽ��                                             
                                             tp->FailPageInfFunP(tp,tp->OperationBufferP,fi[0]);//����ʧ����ʾҳ��
                                        }
                                        else
                                        {//δ֪����
                                             tp->FailPageInfFunP(tp,"ERR",3);//����ʧ����ʾҳ��
                                        }  
                                        tp->WebStateMachine = WebServerStatu_RemoveQuestData;//��ȡ���������ݲ�����
                                   }
                              }
                              else 
                              {//���������һ��
                                   if(tp->QuestOprCount < tp->OperationBufferSize)
                                   {
                                        tp->OperationBufferP[tp->QuestOprCount] = '\0';
                                   }

                                   s8temp = tp->ParaSetFunP(tp->OperationBufferP,tp->QuestOprCount);
                                   if(s8temp == 0)
                                   {//����OK
                                        ParaWebServer_BufferOPR_MoveRP(&(tp->ReceBuffCtrl),tp->QuestOprCount);//ɾ����������
                                        tp->QuestOprCount -= (tp->QuestOprCount);//���������ֽ���
                                   }
                                   else
                                   {//�����쳣
                                        if(s8temp == -1)
                                        {//����ֵԽ��
                                             tp->FailPageInfFunP(tp,tp->OperationBufferP,tp->QuestOprCount);//����ʧ����ʾҳ��
                                        }
                                        else
                                        {//δ֪����
                                             tp->FailPageInfFunP(tp,"ERR",3);//����ʧ����ʾҳ��
                                        } 
                                        tp->WebStateMachine = WebServerStatu_RemoveQuestData;//��ȡ���������ݲ�����
                                   }
                              }
                         }
                         else
                         {//�˴����ӵȴ���ʱ

                         }
                    }
                    else
                    {//δ֪��������
                         tp->QuestProcessFailFlag = 1;
                         tp->WebStateMachine = WebServerStatu_RemoveQuestData;//��ȡ���������ݲ�����
                    }
               }
               else
               {//�������ֽ���Ϊ0���������  
                    tp->SuccessPageInfFunP(tp,"OK",2);//���óɹ���ʾҳ��
                    tp->WebStateMachine = WebServerStatu_RemoveQuestData;//��ȡ���������ݲ�����
               }
               break;  
//---------- �Ƴ����������� ----------               
          case WebServerStatu_RemoveQuestData://�Ƴ�����������
               if(tp->QuestOprCount)
               {
                    rn = tp->QuestOprCount;
                    if(rn > tp->OperationBufferSize)
                    {
                         rn = tp->OperationBufferSize;
                    }
                    tp->QuestOprCount -= ParaWebServer_BufferOPR_ReadData(&(tp->ReceBuffCtrl),1,tp->OperationBufferP,rn);//ֱ��                    
               }
               else
               {
                    tp->HttpOpr.AckCode = 200;
                    tp->HttpOpr.AckCodeScriptSP = "OK";
                    tp->AckOprCount = 0;//�ѷ����ֽ���
                    tp->WebStateMachine = WebServerStatu_AckHeadEncode;//�л�Ϊ����
                    if(tp->QuestProcessFailFlag)
                    {
                         tp->WebStateMachine = WebServerStatu_QuestHeadDecode;//�л�Ϊ����
                    }
               }
               break;
//***************************************************************************************************************************************               
//---------- ��Ӧͷ���� ----------                
          case WebServerStatu_AckHeadEncode:
               rn = ParaWebServer_BufferOPR_GetFreeSize(&(tp->SendBuffCtrl));//��ȡ���ͻ���ռ�
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
               {//��Ӧ���ݵ� ��ʼ�� ��Ӧͷ �ѱ������
                    tp->WebStateMachine = WebServerStatu_QuestHeadDecode;//�л�Ϊ����
                    if(tp->HttpOpr.AContentLength)
                    {//������Ҫ����
                         tp->WebStateMachine = WebServerStatu_AckDataProcess;
                    }
               }
               else
               {//��Ҫ���ӳ�ʱ����

               }
               break;
//---------- ��֯��Ӧ������ ----------               
          case WebServerStatu_AckDataProcess:
               if(tp->HttpOpr.AContentLength > 0)
               {//���ֽ�����������
                    s32temp = (tp->HttpOpr.AContentLength - tp->AckOprCount);//ʣ���ֽ���
                    if(s32temp > 0)
                    {
                         u32temp = (u32)s32temp;//ʣ���ֽ���
                         if(u32temp > tp->OperationBufferSize)
                         {
                              u32temp = tp->OperationBufferSize;
                         }
                         u32intex = ParaWebServer_BufferOPR_GetFreeSize(&(tp->SendBuffCtrl));
                         if(u32temp > u32intex)
                         {
                              u32temp = u32intex;
                         }
                         //�����ݵ��м仺��
                         s32temp = tp->GetDataFunP(tp->OperationBufferP,u32temp,1);
                         u32temp = 0;
                         if(s32temp >= 0)
                         {//��������
                              u32temp = (u32)s32temp;
                         }
                         else
                         {//�ȴ� �� �����������ӳ�ʱ

                         }
                         tp->AckOprCount += ParaWebServer_BufferOPR_WriteData(&(tp->SendBuffCtrl),tp->OperationBufferP,u32temp);
                    }
                    else
                    {//�������
                         tp->WebStateMachine = WebServerStatu_QuestHeadDecode;//�л�Ϊ����
                    }
               }
               else if(tp->HttpOpr.AContentLength < 0)
               {//��chunkedģʽ��������               
                    u32temp = tp->OperationBufferSize;
                    u32intex = ParaWebServer_BufferOPR_GetFreeSize(&(tp->SendBuffCtrl));
                    if(u32temp > u32intex)
                    {
                         u32temp = u32intex;
                    }
                    if(u32temp > 10)
                    {//���Զ�ȡ����ΪԤ��10���ֽ����ڴ��chunked��Ϣ
                         u32temp -= 10;
                         s32temp = tp->GetDataFunP(tp->OperationBufferP,u32temp,1);
                         if(s32temp >= 0)
                         {//��������
                              u32temp = (u32)s32temp;
                              rn = tp->OperationBufferSize;
                              s8temp = ParaWebServer_HTTPServer_ChunkedEncode(tp->OperationBufferP,u32temp,tp->OperationBufferP,&rn);
                              if(s8temp == 0)
                              {//����ɹ�
                                   ParaWebServer_BufferOPR_WriteData(&(tp->SendBuffCtrl),tp->OperationBufferP,rn);
                                   if(u32temp == 0)
                                   {//��������
                                        tp->WebStateMachine = WebServerStatu_QuestHeadDecode;//�л�Ϊ����
                                   }
                              }
                              else
                              {//�������
                                   
                              }
                         }
                         else
                         {//���� �� �ȴ�����
                              
                         }                       
                    }
               }
               else
               {
                    tp->WebStateMachine = WebServerStatu_QuestHeadDecode;//�л�Ϊ����
               }
               break;
//---------- ���� ----------               
          default:
               tp->HttpOpr.OprResult = OprResult_OprReset;
               tp->WebStateMachine = WebServerStatu_QuestHeadDecode;
               break;
     }
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����ڲ�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/


