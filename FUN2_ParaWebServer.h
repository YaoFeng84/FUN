#ifndef FUN2_ParaWebServer_h
#define FUN2_ParaWebServer_h

#include "TypeMro.h"
#include "FUN_BufferOPR.h"
#include "FUN_Match.h"
#include "FUN1_HTTPServer.h"

typedef struct
{     
//---------- ����������ʱ����Ҫ�ṩ���ݵĶ������ ----------
     BufferOPRType ReceBuffCtrl;   //���ջ��������  (���ջ����СҪ����ڲ��������С��ȡֵ��׼Ҫ��֤�ܷ�������ͷ������һ����ֵ�ԣ���cookie��ֵ�Եȣ�����1K������)
     BufferOPRType SendBuffCtrl;   //���ͻ��������  (���ͻ����СҪ����ڲ��������С��ȡֵ��׼Ҫ��֤�ܷ�������ͷ������һ����ֵ�ԣ���cookie��ֵ�Եȣ�����1K������)
     u8 *OperationBufferP;         //��������ָ��
     u32 OperationBufferSize;      //���������ֽ���  (�ռ��СҪ��֤�ܷ�������ͷ������һ����ֵ�ԣ���cookie��ֵ�Եȣ�����1K������)
     //�ɹ�ҳ��Ϣ�ص�����������Ϣ���ݹ̶�ΪOK
     void (*SuccessPageInfFunP)(void *,u8 *,u16);//�ɹ�ҳ����Ϣ�ص�����(����: ParaWebServerOPRType��ָ��,������Ϣ�����׵�ַ��������Ϣ�����ֽ���)
     //ʧ��ҳ��Ϣ�ص������ĸ�����Ϣ����(ERR����ʾδ֪����   ��������ʾָ������ֵԽ����)
     void (*FailPageInfFunP)(void *,u8 *,u16);//ʧ��ҳ����Ϣ�ص�����(����: ParaWebServerOPRType��ָ�� ,������Ϣ�����׵�ַ��������Ϣ�����ֽ���)
     //·������(404ҳ)��Ϣ�ص�����
     void (*PathErrInfFunP)(void *,u8 *,u16);//·������(404ҳ��)��Ϣ�ص�����(����: ParaWebServerOPRType��ָ�� ,������Ϣ�����׵�ַ��������Ϣ�����ֽ���)
//---------- ����������ʱ����������ڻص�ʱ�����޸� ----------
     s32(*GetDataFunP)(u8 *,u16,u8);//�����ݵ���Ӧ��ĺ���ָ��(����:��ſռ��׵�ַ,ϣ����ȡ�ֽ���,��ȡģʽ(0:��ͷ�� ��0:������))(����ֵ��С��-1:����   -1:�ȴ�����   0:������(����)   ����0:��������ֽ���)
     //�������ûص��������ݸ�ʽ(����=��ֵ)
     s8 (*ParaSetFunP)(u8 *,u16);  //�������ûص�����ָ��(����:�����׵�ַ�������ֽ���)(����ֵ������С��-2:δ֪����   -1:����ֵԽ��   0:����OK)
     HttpServerOPRType HttpOpr;    //Http��������������
//---------- �������ڲ��Ķ����������ֹ����!!! ----------
     MatchOPRType WebPathMatch;    //·����ƥ�����
     MatchOPRType ParaTableMatch;  //������ƥ�����     
     u8 QuestProcessFailFlag;      //������ʧ�ܱ�־     
     u8 WebStateMachine;           //����״̬��     
     u16 UrlSize,UrlIntex;         //Url�ֽ���(����\0),Url��ʼ����������     
     u32 AckOprCount,QuestOprCount;//��Ӧ���������������������
}ParaWebServerOPRType;//����Web����ģ���������

typedef struct
{
     u8 *rp;//���տռ��׵�ַ
     u32 rl;//���տռ��ֽ���
     u8 *sp;//���Ϳռ��׵�ַ
     u32 sl;//���Ϳռ��ֽ���
     u8 *op;//�����ռ��׵�ַ
     u32 ol;//�����ռ��ֽ���
     MatchTableType *PathTableP;//·����ָ��
     MatchTableType *ParaTableP;//������ָ��
     ParaWebServerOPRType *pwsotp;//��������ָ��
     void (*SuccessPageInfFunP)(void *,u8 *,u16);//�ɹ�ҳ����Ϣ�ص�����
     void (*FailPageInfFunP)(void *,u8 *,u16);//ʧ��ҳ����Ϣ�ص�����
     void (*PathErrInfFunP)(void *,u8 *,u16);//·������(404ҳ��)��Ϣ�ص�����
}ParaWebServerCNFType;//����Web����ģ����������

extern s8 FUN2_ParaWebServer_Config(ParaWebServerCNFType *tp);
extern void FUN2_ParaWebServer_Process(ParaWebServerOPRType *tp);

#endif
