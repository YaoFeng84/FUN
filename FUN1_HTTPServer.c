/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------������ģ����޸ļ�¼��-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * ���ݣ�
 * ���ڣ�2023-03-05
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
#include "FUN_String.h"
#include "FUN_Match.h"
#include "FUN1_HTTPServer.h"
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------����ģ���ĶԽӺ�����-----------------------------------------                             *
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
*               ----------------------------------������ģ���ı���������--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ڵĺ궨����------------------------------------                                  *
*                                                                                                                                           *
********************************************************************************************************************************************/


#define HttpStateMachine_Decode_Start       1    //���뿪ʼ
#define HttpStateMachine_Decode_QuestFirst   2    //������������
#define HttpStateMachine_Decode_QuestHead   3    //��������ͷ
#define HttpSataeMachine_Decode_IDE         4    //����

#define HttpStateMachine_Encode_Start       11   //���뿪ʼ
#define HttpStateMachine_Encode_AckFirst    12   //������Ӧ����
#define HttpStateMachine_Encode_AckHead     13   //������Ӧͷ
#define HttpSataeMachine_Encode_IDE         14    //����
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
static void HttpServer_QuestHead_Connection(u8 flag,u8 *dp,u16 dl,void *pp);
static void HttpServer_QuestHead_Host(u8 flag,u8 *dp,u16 dl,void *pp);
static void HttpServer_QuestHead_UserAgent(u8 flag,u8 *dp,u16 dl,void *pp);
static void HttpServer_QuestHead_ContentLength(u8 flag,u8 *dp,u16 dl,void *pp);
static void HttpServer_QuestHead_ContentType(u8 flag,u8 *dp,u16 dl,void *pp);
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ڵ� ���� �� �궨�� ������--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static MatchTableType QuestHeadTable[] = //����ͷ��
{     
     {"Host",                      0,HttpServer_QuestHead_Host                       },//Post/Get����
     {"User-Agent",                0,HttpServer_QuestHead_UserAgent                  },//Post/Get����
     {"Connection",                0,HttpServer_QuestHead_Connection                 },//Post/Get����
     {"Content-Length",            0,HttpServer_QuestHead_ContentLength              },//Post����
     {"Content-Type",              0,HttpServer_QuestHead_ContentType                },//Post����
     {0,0,0}
};
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ϵͳ����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
������: FUN1_HTTPServer_Config
��   ��: HTTPServerServer����
��   ��:
       
����ֵ:
       ����0:���óɹ�
       С��0:����ʧ��
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-02-07   
-------------------------------------------------*/
s8 FUN1_HTTPServer_Config(HttpServerCNFType *hsctp)
{
     MatchCNFType mctv;

     mctv.mttp = QuestHeadTable;//hctp->mttp;//��ʼ��ƥ���ָ��
     mctv.motp = &(hsctp->hotp->motv);//��ʼ��ƥ���������
     mctv.SetValueNum = 1;
     mctv.SetValue[0] = ':';
     mctv.EndValueNum = 3;
     mctv.EndValue[0] = 0x0d;
     mctv.EndValue[1] = 0x0a;
     mctv.EndValue[2] = 0x00;

     //
     hsctp->hotp->StateMachine = HttpStateMachine_Decode_Start;//Ĭ�Ͽ�ʼ����

     return HttpServer_Match_Config(&mctv);
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����û�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
������: FUN1_HTTPServer_Decode
��   ��: HTTP����ͷ���ݽ���
��   ��:
       HttpServerOPRType *hotp������ṹ���׵�ַ
       u8  *dp�������������׵�ַ
       u16 *dl��(����)�����������ֽ��� (���)ʵ�ʴ��������ֽ���
����ֵ:
       0:δ����
   С��0:����
ע   ��:
       ��һ������+0d0aΪ��С���뵥Ԫ
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-03-07   
-------------------------------------------------*/
s8 FUN1_HTTPServer_Decode(HttpServerOPRType *hotp,u8 *dp,u16 *dl)
{
     u8 fl = 2,fd[] = {0x0d,0x0a};//�س�����//{'\r','\n'};
     u8 f1l =1,f1d[] = {' '};//�ո�
     u8 f2l =4,f2d[] = {0x0d,0x0a,0x0d,0x0a};//�س�����//{'\r','\n'};
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
               hotp->QContentLength = 0;//0�ֽ�
               hotp->QContentType = HttpContentType_unknow;
               hotp->Modth = HttpVersion_unknow;//δ֪
               hotp->Url[0] = 0;
               hotp->StateMachine = HttpStateMachine_Decode_QuestFirst;
               hotp->Version = 0;
               break;
          case HttpStateMachine_Decode_QuestFirst://�����������У��ṹ����ʽ URL �汾
               s16temp = Http_String_Split2(dp,maxsize,fd,fl,ip,il);//Ѱ��\r\n----�������н���
               if(s16temp)
               {//���ҵ�\r\n
                    *dl = (ip[0] + fl);//���Ѵ����ֽ���
                    s16temp = Http_String_Split2(dp,ip[0],f1d,f1l,ip,il);//�ӵ�1����Ѱ�ҿո�
                    if(s16temp == 2)
                    {//���ҵ� 2 ���ո�
                    //-----��ȡ ���󷽷�
                         if(ip[0] <= 4)
                         {
                              if(Http_String_STRIsSame2(dp,"GET",3) == 0)
                              {//GET����
                                   hotp->Modth = Modth_GET;
                              }
                              else if(Http_String_STRIsSame2(dp,"POST",4) == 0)
                              {//POST����
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
                    //-----��ȡ URL
                         if(s8result == 0)
                         {//���Լ�������URL
                              s16temp = (ip[1] - ip[0] - f1l);//URL����
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
                    //-----��ȡ �汾��
                         if(s8result == 0)
                         {//���Լ��������汾��
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
               s16temp = Http_String_Split2(dp,maxsize,f2d,f2l,ip,il);//Ѱ��\r\n\r\n----����ͷ����
               if(s16temp)
               {//�ҵ�����ͷ����                    
                    *dl = (ip[0] + f2l);//���Ѵ����ֽ���
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
                    {//��Ϊ������    
                         hotp->OprResult = OprResult_DecodeOprOK;                
                         hotp->StateMachine = HttpSataeMachine_Decode_IDE;
                    }
                    else
                    {//��������δ����
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
������: FUN1_HTTPServer_Encode
��   ��: HTTP��Ӧͷ����
��   ��:
       HttpServerOPRType *hotp������ṹ���׵�ַ
       u8  *sp����ſռ���׵�ַ
       u16 *sl��(����)��ſռ������ֽ��� (���)��ſռ�ı������ݵ���Ч�ֽ���
����ֵ:
       0:δ����
   С��0:����
ע   ��:
       ��һ������+0d0aΪ��С�����뵥Ԫ
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-03-08   
-------------------------------------------------*/
s8 FUN1_HTTPServer_Encode(HttpServerOPRType *hotp,u8 *sp,u16 *sl)
{
     u8 datatemp[20],dtl = 20;
     s8 s8result = 0;
     u16 u16wi = 0;//��ǰwi
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
               //�汾��        
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
               //��Ӧ��ת��
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
                         {//���Ȳ����ֽ�������
                              s8result = Http_String_StrLink3(sp,maxsize,"Accept-Ranges: bytes\r\n",&u16wi);
                              s8result = Http_String_StrLink3(sp,maxsize,"Content-Length: ",&u16wi);
                              Http_String_S32ToStr(hotp->AContentLength,datatemp,dtl);
                              s8result = Http_String_StrLink3(sp,maxsize,datatemp,&u16wi);
                              s8result = Http_String_StrLink3(sp,maxsize,"\r\n",&u16wi);
                         }
                         else if(hotp->AContentLength < 0)
                         {//���Ȳ���chunkedģʽ����
                              s8result = Http_String_StrLink3(sp,maxsize,"Transfer-Encoding: chunked\r\n",&u16wi);
                         }
                         break;
                    case 6:
                         s8result = Http_String_StrLink3(sp,maxsize,"\r\n",&u16wi);//��Ӧͷ����                         
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
������: FUN1_HTTPServer_ChunkedEncode
��   ��: HTTP��Chunked����
��   ��:
       u8 *dp�������������׵�ַ
       u16 dl�������������ֽ���(����Ϊ0����ʾ����)
       u8  *sp����ſռ���׵�ַ
       u16 *sl��(����)��ſռ������ֽ��� (���)��ſռ�ı������ݵ���Ч�ֽ���
����ֵ:
       0:����ɹ�
   С��0:�������
ע   ��:
       ֧��dp��sp��ͬ����
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-03-15  
-------------------------------------------------*/
s8 FUN1_HTTPServer_ChunkedEncode(u8 *dp,u16 dl,u8 *sp,u16 *sl)
{
     u8 hexs[4],u8i;
     u16 u16temp = *sl,u16i,u16r;

     *sl = 0;
     Http_String_CharToHexStr((u8)(dl >> 8),hexs);//��8λתHex�ַ�
     Http_String_CharToHexStr((u8)(dl),&hexs[2]);//��8λתHex�ַ�
//-----Ѱ���׸���Ϊ0��������
     for(u8i = 0;u8i < 4;u8i++)//ȥ��ǰ��Ϊ'0'
     {
          if(hexs[u8i] != '0')
          {
               break;
          }
     }
     if(u8i >= 4)
     {//�����Ϊ0����ʾdl=0��������ָ�����һ���ַ���
          u8i = 3;
     }
//-----�жϴ�ſռ��Ƿ�
     //��1��4��ʾ��2��\r\n����2��4��ָhexs���ֽ���
     u16i = (dl + 4 + (4 - u8i));//������������Ŀռ��ֽ���
     if(u16i > u16temp)
     {//�ռ䲻��
          return -1;
     }
     *sl = u16i;//���淵����Ч�ֽ���
//-----�˴��Ӻ���ǰ��������-----֧���������������ͬ
     u16i--;
     //���chunked���ݵĽ�����\r\n
     sp[u16i--] = '\n';
     sp[u16i--] = '\r';
     //���chunked����
     if(dl)
     {
          u16r = (dl - 1);
          for(u16temp = 0;u16temp < dl;u16temp++)
          {
               sp[u16i--] = dp[u16r--];
          }
     }
     //���chunked���ݳ��ȵĽ�����\r\n
     sp[u16i--] = '\n';
     sp[u16i--] = '\r';
     //���chunked���ݳ���
     u16r = 3;
     for(u16temp = 0;u16temp < (4 - u8i);u16temp++)
     {
          sp[u16i--] = hexs[u16r--];
     }
     return 0;
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����ڲ�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static void HttpServer_QuestHead_Connection(u8 flag,u8 *dp,u16 dl,void *pp)
{
     HttpServerOPRType *hsotp = (HttpServerOPRType *)pp;
     if(flag)
     {
          hsotp->Connect = 1;//������
          dp = Http_String_Ltrim(dp);//ȥ�����ո�          
          if(Http_String_STRIsSame2(dp,"keep-alive",10) == 0)
          {//��ͬ
               hsotp->Connect = 2;//������
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
{//Post�����е������ֽ���
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

               u8p = Http_String_Ltrim(dtemp);//ȥ�����ո�  
               if(Http_String_StrToS32(u8p,&s32temp) == 0)
               {
                    // ok = 125;
                    hsotp->QContentLength = (u16)s32temp;
               }
          }
     }
}

static void HttpServer_QuestHead_ContentType(u8 flag,u8 *dp,u16 dl,void *pp)
{//Post�����е���������
     HttpServerOPRType *hsotp = (HttpServerOPRType *)pp;
     u8 *ctp1 = "application/x-www-form-urlencoded";
     u8 *ctp2 = "application/json";
     u8 *ctp3 = "multipart/form-data";
     if(flag)
     {
          dp = Http_String_Ltrim(dp);//ȥ�����ո�          
          if(Http_String_STRIsSame2(dp,ctp1,Http_String_StrLen(ctp1)) == 0)
          {//��ͬ
               hsotp->QContentType = HttpContentType_urlencoded;
          }
          else if(Http_String_STRIsSame2(dp,ctp2,Http_String_StrLen(ctp2)) == 0)
          {//��ͬ
               hsotp->QContentType = HttpContentType_json;
          }
          else if(Http_String_STRIsSame2(dp,ctp3,Http_String_StrLen(ctp3)) == 0)
          {//��ͬ
               hsotp->QContentType = HttpContentType_multipart;
          }
          else
          {
               hsotp->QContentType = HttpContentType_unknow;
          }
     }
}
