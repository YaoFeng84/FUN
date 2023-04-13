#ifndef FUN1_HTTPServer_h
#define FUN1_HTTPServer_h

#include "TypeMro.h"
#define HttpURLMaxLength      100  //URL����ֽ�������\0

/*
��Ӧ����������ͣ������ǵĵ�һλ���ֱ�ʾ��
1xx����Ϣ�������յ�����������
2xx���ɹ�����Ϊ���ɹ��ؽ��ܡ����Ͳ���
3xx���ض���Ϊ��������󣬱����һ��ִ�еĶ���
4xx���ͻ��˴�����������﷨������������޷�ʵ��
5xx�����������󣬷���������ʵ��һ��������Ч������

�±���ʾÿ����Ӧ�뼰�京�壺
100 ����101 ���齻��Э
200 OK  201 ������ 202 ������ 203 ����Ȩ��Ϣ 204 ������ 205 �������� 206 ��������
300 ��ѡ�� 301 ���õش���  302 �ҵ�3 03 �μ����� 304 δ�Ķ� 305 ʹ�ô��� 307 ��ʱ�ض���
400 �������� 401 δ��Ȩ 402 Ҫ�󸶷� 403 ��ֹ 404 δ�ҵ� 405 ������ķ��� 406 �������� 407 Ҫ�������Ȩ 408 ����ʱ 409  ��ͻ 410 ���ڵ� 411 Ҫ��ĳ��� 412 ǰ�᲻���� 413 ����ʵ��̫�� 414 ����URI̫�� 415 ��֧�ֵ�ý������ 416 �޷����������Χ 417 ʧ�ܵ�Ԥ��
500 �ڲ�����������  501 δ��ʹ��  502 ���ش���  503 �����õķ���  504 ���س�ʱ  505 HTTP�汾δ��֧��
*/

/*
get���͵���������С�����ܴ���2KB��
post���͵��������ϴ�һ�㱻Ĭ��Ϊ�������ơ��������ϣ�IIS4�������Ϊ80KB��IIS5��Ϊ100KB��
*/

// typedef enum
// {
//      OprResult_OprErr = -1,   //����ʧ��
//      OprResult_OprReset = 0,  //��λ
//      OprResult_DecodeOprOK,   //����������
//      OprResult_EncodeOprOK,   //����������
//      OprResult_OprIng         //����������
// }Enum_OprResult;
//OprResult�궨��
#define OprResult_OprErr                -1   //����ʧ��
#define OprResult_OprReset              0    //��λ
#define OprResult_DecodeOprIng          1    //����������
#define OprResult_DecodeOprOK           2    //����������
#define OprResult_EncodeOprIng          3    //����������
#define OprResult_EncodeOprOK           4    //����������

//�汾�궨��
#define HttpVersion_unknow              0    //�汾δ֪
#define HttpVersion_1d0                 1    //V1.0�汾
#define HttpVersion_1d1                 2    //V1.1�汾                 
//��Ӧ���������ͺ궨��----�����
#define HttpContentType_unknow          0    //δ֪
#define HttpContentType_TextHtml        1    //Text/Html//GET����
#define HttpContentType_TextCss         2    //Text/Css//GET����
#define HttpContentType_TextJavaScript  3    //text/javascript//GET����
#define HttpContentType_Image           4    //Image/*//GET����
#define HttpContentType_urlencoded      7    //application/x-www-form-urlencoded//POST���
#define HttpContentType_json            8    //application/json//POST���
#define HttpContentType_multipart       9    //multipart/form-data//POST���
//
// typedef enum
// {
//      Modth_Unknow = 0,   //����δ֪
//      Modth_GET,          //GET����
//      Modth_POST          //POST����
// }Enum_Modth;
#define Modth_Unknow                    0//����δ֪
#define Modth_GET                       1//GET����
#define Modth_POST                      2//POST����


typedef struct
{
     s8 OprResult;       //ģ��������������
//---------- ���½��벿�֣�����Ϊֻ�����ԣ��û���ֹ�޸� -----------------
     u8 Modth;           //����(0:δ֪ 1:Get 2:Post)
     u8 Url[HttpURLMaxLength]; //���URL
     u8 Version;         //http�汾��(ֵ�ο��汾�궨��)
     u8 Connect;         //����״̬(0:δ֪ 1:������ 2:������)     
     u8 QContentType;     //����������(ֵ�ο���Ӧ���������ͺ궨��)     
     u32 QContentLength;  //�������ֽ���

//---------- ���±��벿�֣�����Ϊ��д���� ----------------------------
     u16 AckCode;        // ��Ӧ��(200,404)
     u8 *AckCodeScriptSP;//��Ӧ�����ַ���ָ��
     u8 AContentType;     //��Ӧ������(ֵ�ο���Ӧ���������ͺ궨��)  
     s32 AContentLength;  //��Ӧ���ֽ���(С��0Ϊchunkedģʽ)
     
//----------���²�����ģ���ڲ�ʹ�ã��û���ֹ���ʺ͸Ķ�-------------------
     u8 StateMachine;    //��������״̬��
     u8 EncodeStateMachine;//����ͷ����״̬��
     MatchOPRType motv;  //����ͷ��������

}HttpServerOPRType;

typedef struct
{
     //MatchTableType *mttp;//����ͷ��
     HttpServerOPRType *hotp;
}HttpServerCNFType;

extern s8 FUN1_HTTPServer_Config(HttpServerCNFType *hsctp);
extern s8 FUN1_HTTPServer_Decode(HttpServerOPRType *hotp,u8 *dp,u16 *dl);
extern s8 FUN1_HTTPServer_Encode(HttpServerOPRType *hotp,u8 *dp,u16 *dl);
extern s8 FUN1_HTTPServer_ChunkedEncode(u8 *dp,u16 dl,u8 *sp,u16 *sl);

#endif
