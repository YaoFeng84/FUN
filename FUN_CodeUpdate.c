/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------������ģ����޸ļ�¼��-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * ���ݣ�
 * ���ڣ�2018-07-26
 * ���ߣ�YJX
 * �汾�ţ�V1.0�����棩
 ***********************************************
 * �޸����ݣ������豸�����ж�
 * �޸����ڣ�2021-11-26
 * �޸����ߣ�YJX
 * �� �� �ţ�V1.1
 ***********************************************
 * �޸����ݣ����ӷ�����ɾ��������д���¼�
 * �޸����ڣ�2022-01-04
 * �޸����ߣ�YJX
 * �� �� �ţ�V1.2
 ***********************************************
*/
/********************************************************************************************************************************************
*                                                                                                                                           *
*                ---------------------------------������ģ���ʹ��˵����-----------------------------------------                           *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*                                       �ص�˵����
          ֧�ֶ����ļ����͵����ظ���
��1��������
��ģ�鹲�ṩ6������������2��ƽ̨������4�����ܺ���
ƽ̨�������£�
FUN_CodeUpdate_Config���ú���
FUN_CodeUpdate_RealTask������
���ܺ������£�
FUN_CodeUpdate_ReceData���պ��������ڽ��������Ĵ�������
FUN_CodeUpdate_Start�������������ú���ִ�к���Զ�ִ����������
FUN_CodeUpdate_Stopֹͣ������



*/
#include "TypeMro_N.h"
#include "FUN_CodeUpdate.h"
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------����ģ���ĶԽӺ�����-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/

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
#define ProjectModeSize            4    //��Ŀ���ͳ���
#define DeviceModeSize             16   //�豸���ͳ���

//------bin�ļ���Ϣ�궨��------
#define CHECKSUM_SIZE	          4    //CheckSum��������ռ���ֽ���
#define CommandSize                16   //bin�ļ�ͷ������Ĵ�С����λ�ֽ�
#define FileHeaderSize             48   //bin�ļ�ͷ��С����λ�ֽ�
#define FileHeaderCheckSum_Size    2    //ͷ�����ļ��ֽ���

//
#define CU_ReadWriteByteNum        500  //���ζ�д�ֽ���
#define CU_EraseByteNum            32768//���β����ֽ���(���ɴ����65536���ܺ����β�����)

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ڵı������Ͷ�����------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
typedef struct 
{
     u8 Command[CommandSize];           //Ԥ��
	u8 machinetype[ProjectModeSize];   //4���ֽ���Ŀ��
	u8 DeviceMode[DeviceModeSize];     //�豸����
	u8 datatype;                       //��������
	u8  downloadcontrol;               //���ؿ���(0:��ʾ��ɺ�ͽ������� ��0:��ʾ��ɺ����µĴ���Ҫ����)
     u32 Start_address;                 //��ʼ��ŵ�ַ
	u32 File_Length;                   //�ֽ���
	u16 Checksum;                      //У����
} FILE_HEADER;

typedef struct
{
     u8 ProjectMode[ProjectModeSize];   //��Ŀ����
     u8 DeviceMode[DeviceModeSize];     //�豸����,'\0'��β
     u8 FileTypeCode;                   //�ļ�������
     u32 Address;                       //��ʼ��ַ
     u32 Size;                          //�ֽ���(Ϊ0ʱ����ʾ�ñ��������������һ�ű�)
}FILETYPETAB;

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����ڲ�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static u8 CodeUpdate_GetSLen(u8 *buf);
static s8 CodeUpdate_FileHeaderDecode(FILE_HEADER *p,vu8 *dp);
static s8 CodeUpdate_CheckHeaderValid(FILE_HEADER *p,vu8 *dp);

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ڵ� ���� �� �궨�� ������--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static FILETYPETAB FileTypeLab[CodeUpdate_CodeTypeNum + 1];//��1��Ҫ��Ϊ�˴�����һ����Ч�Ľ�����
static FILE_HEADER fileheader;//����һ���ļ�ͷ�ṹ�����
static CU_CallBackFunPA CallFpA;//�ص�����ָ����

#define CU_StateMachine_IDE             0//����
#define CU_StateMachine_WaitScanf       1//�ȴ���ɨ������
#define CU_StateMachine_WaitHead        2//�ȴ�ͷ����
#define CU_StateMachine_CheckHeadData   3//У��ͷ����
#define CU_StateMachine_EraseS1         4//������1��
#define CU_StateMachine_EraseS2         5//������2��
#define CU_StateMachine_WriteS1         6//д���1��
#define CU_StateMachine_WriteS2         7//д���2��
#define CU_StateMachine_CheckCodeDataS1 8//У��������ݵ�1��
#define CU_StateMachine_CheckCodeDataS2 9//У��������ݵ�2��
#define CU_StateMachine_SUCCESS         10//�ɹ�
#define CU_StateMachine_Error           20//����״̬
static u8  CU_StateMachine;

static CU_StatuEnum CU_Errflag;//�����
static u32 CU_OprCount;//��������(���ڶ�д�ֽ�������)
static u32 CU_OprMax;//���ڵ��ζ�д���ֵ
static u32 CU_CheckSum;//����У��
static u32 CU_OprAddress;//������ַ
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ϵͳ����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
������: FUN_CodeUpdate_Config
��   ��: ����
��   ��:
       CUCnfType *fttp:��ʼ�����ýṹ��ָ��
����ֵ:
     0�����óɹ�
 С��0������ʧ��

ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2018-08-08
-------------------------------------------------*/
s8 FUN_CodeUpdate_Config(CUCnfType *cucp)
{
     u8 u8temp,u8temp2;
     
     //��ձ�
     for(u8temp = 0;u8temp < CodeUpdate_CodeTypeNum;u8temp++)
     {
          FileTypeLab[u8temp].Size = 0;//��ձ�
     }
     //���ر�
     for(u8temp = 0;u8temp < CodeUpdate_CodeTypeNum;u8temp++)
     {          
          u8 *ps = cucp->FileTabP[u8temp].pmp;//��Ŀ�����׵�ַ
          u8 *ds = cucp->FileTabP[u8temp].dmp;//�豸�����׵�ַ

          if(cucp->FileTabP[u8temp].Size)
          {               
               u8temp2 = CodeUpdate_GetSLen(ps);//�ж���Ŀ�ַ����ֽ���
               if(u8temp2 != ProjectModeSize)
               {
                    return -1;//�Ƿ���Ŀ�ַ���
               }

               u8temp2 = CodeUpdate_GetSLen(ds);//�ж��豸�ͺ��ַ����ֽ���
               if((u8temp2 >= DeviceModeSize) || (u8temp2 == 0))
               {
                    return -2;//�Ƿ��豸�����ַ���
               }

               //������Ŀ���ͱ�
               for(u8temp2 = 0;u8temp2 < ProjectModeSize;u8temp2++)
               {
                    FileTypeLab[u8temp].ProjectMode[u8temp2] = ps[u8temp2];
               }

               //�����豸���ͱ�
               for(u8temp2 = 0;u8temp2 < DeviceModeSize;u8temp2++)
               {
                    FileTypeLab[u8temp].DeviceMode[u8temp2] = ds[u8temp2];
               }
               
               FileTypeLab[u8temp].FileTypeCode = cucp->FileTabP[u8temp].FileTypeCode;
               FileTypeLab[u8temp].Address = cucp->FileTabP[u8temp].Address;
               FileTypeLab[u8temp].Size = cucp->FileTabP[u8temp].Size;
          }
          else
          {
               FileTypeLab[u8temp].Size = 0;
               break;
          }
     }
     
     //���ûص�����ָ����
     CallFpA.NonBlock_StartGetHead = cucp->CallBackFp->NonBlock_StartGetHead;
     CallFpA.NonBlock_CheckGetHead = cucp->CallBackFp->NonBlock_CheckGetHead;
     CallFpA.NonBlock_CheckErase = cucp->CallBackFp->NonBlock_CheckErase;
     CallFpA.NonBlock_CheckRead = cucp->CallBackFp->NonBlock_CheckRead;
     CallFpA.NonBlock_CheckWrite = cucp->CallBackFp->NonBlock_CheckWrite;
     CallFpA.NonBlock_StartErase = cucp->CallBackFp->NonBlock_StartErase;
     CallFpA.NonBlock_StartRead = cucp->CallBackFp->NonBlock_StartRead;
     CallFpA.NonBlock_StartWrite = cucp->CallBackFp->NonBlock_StartWrite;
     CallFpA.StatuFp = cucp->CallBackFp->StatuFp;
     
     
     FUN_CodeUpdate_Start();
     return 0;
}


/*-------------------------------------------------
������: FUN_CodeUpdate_RealTask
��   ��: ����ʵʱ�߳�
��   ��:
       ��
����ֵ:
       ��
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
��   ��:
       ��1���������˲���ʱ���ز����ռ������ YJX_20220713
ʱ   ��:2018-08-08   
-------------------------------------------------*/
void FUN_CodeUpdate_RealTask(void)
{
     s8 s8temp;
     s32 s32temp;
     u16 u16temp;
     u32 u32temp;
     u8 u8temp;
     
     switch(CU_StateMachine)
     {
          case CU_StateMachine_IDE:
               break;
//---------------�ȴ�ͷ���ݲ���
          case CU_StateMachine_WaitScanf://�ȴ�ͷ���� 
               CallFpA.StatuFp(WaitUpdateHead,0);
               s8temp = CallFpA.NonBlock_StartGetHead(&CU_OprAddress,FileHeaderSize);
               if(s8temp < 0)
               {
                    CU_Errflag = HeadDataError;
                    CU_StateMachine = CU_StateMachine_Error;
               }
               else
               {
                    CU_StateMachine = CU_StateMachine_WaitHead;
               }
               break;
          case CU_StateMachine_WaitHead://�ȴ�ͷ���ݻ�ȡ���
               s32temp = CallFpA.NonBlock_CheckGetHead();
               if(s32temp >= FileHeaderSize)
               {
                    fileheader.File_Length = 0;//�����ֽ�����0
                    //ͷ���ݽ���
                    CodeUpdate_FileHeaderDecode(&fileheader,(vu8 *)CU_OprAddress);
                    if(CodeUpdate_CheckHeaderValid(&fileheader,(vu8 *)CU_OprAddress) < 0)
                    {//ͷ������Ч
                         CU_Errflag = HeadDataError;
                         CU_StateMachine = CU_StateMachine_Error;
                    }
                    else
                    {//ͷ������Ч
                         CU_OprCount = 0;
                         CU_OprMax = CU_EraseByteNum;
                         if(CU_OprMax > fileheader.File_Length)
                         {
                              CU_OprMax = fileheader.File_Length;
                         }
                         CU_StateMachine = CU_StateMachine_EraseS1; 
                         CallFpA.StatuFp(StartErase,0);
                    }
               }
               else if(s32temp < -1)
               {
                    CU_Errflag = HeadDataError;
                    CU_StateMachine = CU_StateMachine_Error;
               }
               break;
//---------------��������               
          case CU_StateMachine_EraseS1://����������1��
               s8temp = CallFpA.NonBlock_StartErase(fileheader.datatype,fileheader.Start_address + CU_OprCount,CU_OprMax);
               if(s8temp == 0)
               {//��������OK    
                    CU_StateMachine = CU_StateMachine_EraseS2;
               }
               else
               {//���������쳣
                    CU_Errflag = EraseError;
                    CU_StateMachine = CU_StateMachine_Error;
               }
               break;
          case CU_StateMachine_EraseS2://����������2��
               s32temp = CallFpA.NonBlock_CheckErase();
               if(s32temp >= 0)
               {//�������                    
                    CU_OprCount += s32temp;  
                    if(CU_OprCount < fileheader.File_Length)
                    {//����δ���
                         //������һ�β������ֽ���
                         CU_OprMax = CU_EraseByteNum;
                         if((fileheader.File_Length - CU_OprCount) < CU_EraseByteNum)
                         {
                              CU_OprMax = (fileheader.File_Length - CU_OprCount);
                         }
                         CU_StateMachine = CU_StateMachine_EraseS1;
                    }
                    else
                    {//�������
                         CU_OprCount = 0;
                         CU_OprMax = CU_ReadWriteByteNum;
                         CU_StateMachine = CU_StateMachine_WriteS1;
                    }
               }
               else if(s32temp < -1)
               {//�������� �� ������ʱ   
                    CU_Errflag = EraseError;
                    CU_StateMachine = CU_StateMachine_Error;
               }
               break;

//---------------д���ݲ���
          case CU_StateMachine_WriteS1://д������1��
               s8temp = CallFpA.NonBlock_StartWrite(fileheader.datatype,(fileheader.Start_address + CU_OprCount),CU_OprMax);
               if(s8temp)
               {
                    CU_Errflag = WriteError;
                    CU_StateMachine = CU_StateMachine_Error;
               }
               else
               {//����д�ɹ�
                    CU_StateMachine = CU_StateMachine_WriteS2;                    
               }
               break;
          case CU_StateMachine_WriteS2:
               s32temp = CallFpA.NonBlock_CheckWrite();
               if(s32temp >= 0)
               {//д���
                    CU_OprCount += s32temp;                    
                    if(CU_OprCount < fileheader.File_Length)
                    {//����δ���
                         //������һ�ε�д���ֽ���
                         CU_OprMax = CU_ReadWriteByteNum;
                         if((fileheader.File_Length - CU_OprCount) < CU_ReadWriteByteNum)
                         {
                              CU_OprMax = (fileheader.File_Length - CU_OprCount);
                         }
                         CU_StateMachine = CU_StateMachine_WriteS1;
                         u8temp = ((u8)((CU_OprCount * 100.0) / fileheader.File_Length));
                    }
                    else
                    {//�������
                         CU_CheckSum = 0;
                         CU_OprCount = 0;
                         CU_OprMax = CU_ReadWriteByteNum;//CU_DataReceBufferMaxSize;
                         CU_StateMachine = CU_StateMachine_CheckCodeDataS1;
                         u8temp = 100;
                    }
                    CallFpA.StatuFp(WriteProgress,u8temp);
               }
               else if(s32temp < -1)
               {//д���쳣
                    CU_Errflag = WriteError;
                    CU_StateMachine = CU_StateMachine_Error;
               }
               break;
               
//---------------У�����ݲ���               
          case CU_StateMachine_CheckCodeDataS1:
               s8temp = CallFpA.NonBlock_StartRead(fileheader.datatype,(fileheader.Start_address + CU_OprCount),CU_OprMax,&CU_OprAddress);
               if(s8temp == 0)
               {//�������ɹ�
                    CU_StateMachine = CU_StateMachine_CheckCodeDataS2;
               }
               else
               {//������ʧ��     
                    CU_Errflag = ReadError;
                    CU_StateMachine = CU_StateMachine_Error;
               }
               break;
          case CU_StateMachine_CheckCodeDataS2:
               s32temp = CallFpA.NonBlock_CheckRead();
               if(s32temp >= 0)
               {//����� 
                    if(CU_OprCount < (fileheader.File_Length - CHECKSUM_SIZE))
                    {//��ȡδ���
                         CU_OprCount += s32temp; 
                         for(u16temp = 0;u16temp < s32temp;u16temp++)
                         {
                              CU_CheckSum += ((u8 *)CU_OprAddress)[u16temp];
                         }
                    
                         //������һ�ε������ֽ���
                         CU_OprMax = CU_ReadWriteByteNum;//CU_DataReceBufferMaxSize;
                         if((fileheader.File_Length - CHECKSUM_SIZE - CU_OprCount) < CU_ReadWriteByteNum)//CU_DataReceBufferMaxSize)
                         {
                              CU_OprMax = (fileheader.File_Length - CHECKSUM_SIZE - CU_OprCount);
                         }
                         
                         if(CU_OprMax == 0)
                         {//��ȡУ����
                              CU_OprMax = CHECKSUM_SIZE;
                         }
                         
                         CU_StateMachine = CU_StateMachine_CheckCodeDataS1;
                    }
                    else
                    {//��ȡ���
                         u32temp = *(u32 *)CU_OprAddress;//CU_DataReceBuffer;//�����е�У����
                         if(CU_CheckSum == u32temp)
                         {//��ȣ�����OK
                              if(fileheader.downloadcontrol == 0)
                              {
                                   CU_StateMachine = CU_StateMachine_SUCCESS;
                              }
                              else
                              {//���д����ļ���Ҫ����                                   
                                   CU_StateMachine = CU_StateMachine_WaitScanf;
                                   CallFpA.StatuFp(ReStart,0);
                              }
                         }
                         else
                         {//���������쳣                              
                              CU_Errflag = CodeCheckError;
                              CU_StateMachine = CU_StateMachine_Error;
                         }
                    }
               }
               else if(s32temp < -1)
               {//���쳣                    
                    CU_StateMachine = CU_StateMachine_Error;
                    CU_Errflag = ReadError;
               }
               break;









          case CU_StateMachine_SUCCESS:
               CU_StateMachine = CU_StateMachine_IDE;//ע��˳��Ҫ�Ե��������¼������ܻ�ı�״ֵ̬
               CallFpA.StatuFp(UpdateSuccess,0);
               break;
          case CU_StateMachine_Error:
               CU_StateMachine = CU_StateMachine_IDE;//ע��˳��Ҫ�Ե��������¼������ܻ�ı�״ֵ̬
               CallFpA.StatuFp(CU_Errflag,0);
               break;
          default:
               FUN_CodeUpdate_Start();
               break;
     }
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����û�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
������: FUN_CodeUpdate_Start
��   ��: ������� ����
��   ��:
       ��
����ֵ:
       ��
ע   ��:
       ��ʼʱ����
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2018-08-08   
-------------------------------------------------*/
void FUN_CodeUpdate_Start(void)
{
     CU_StateMachine = CU_StateMachine_WaitScanf;
}

/*-------------------------------------------------
������: FUN_CodeUpdate_Stop
��   ��: ������� ֹͣ
��   ��:
       ��
����ֵ:
       ��
ע   ��:
       ��Ҫ��ǰ������������ʱ������
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2018-08-08   
-------------------------------------------------*/
void FUN_CodeUpdate_Stop(void)
{
     CU_StateMachine = CU_StateMachine_IDE;
}


/*-------------------------------------------------
������: FUN_CodeUpdate_CheckCodeIsValid
��   ��: �ж� ָ���Ĵ�����Ƿ���Ч
��   ��:
     u8 *dp:���������׵�ַ
     u16 dl:���������ֽ���
     u8 flag:У��״̬(1:��ʼУ�� 2:����У�� 3:���һ��У�� 0:����У��)
����ֵ:
     �������һ�� ����ֵ������
     ���ط�0��ʾУ��ʧ�� 0��ʾУ��ɹ�
ע   ��:
     �������ֽ����ϴ���Ҫ�ֶ�ΰ�У��ʱ��flagȡ1,2,3
       �װ���ʼУ��ʱ��flag =1
       �м伸���������У��flag=2
       �����һ����У��ʱ��flag =3
     �������ֽ���ֻ��һ�ξͿ���У�����ʱ��flagȡ0
ʾ   ��:
��   ��:YJX
��   ��:V1.0
��   ��:V1.1
       ��1������Ϊ�����Ͷ�ȡ���ݣ���������ʹ�ϲ�û������RealTask��Ҳ���������ж� YJX_20220714
ʱ   ��:2018-08-08   
-------------------------------------------------*/
u32 FUN_CodeUpdate_CheckCodeIsValid(u8 *dp,u16 dl,u8 flag)
{
     static u32 u32result = 0;
     static u32 u32count = 0;//����������Ҫ����4�ֽ�
     static u32 lastdata;//���4���ֽ�����
     u16 u16temp;
     
     if((flag == 1) || (flag ==0))
     {
          u32result = 0;
          u32count = 0;//������0
          lastdata = 0;//���4���ֽ�������0
     }

     u32count += dl;
     for(u16temp = 0;u16temp < dl;u16temp++)
     {
          u32result += dp[u16temp];//�ۼ�
          lastdata <<= 8;
          lastdata += dp[u16temp];
     }

     if((flag == 3) || (flag == 0))
     {
          if(u32count > 4)
          {
               u32result -= (u8)(lastdata);
               u32result -= (u8)(lastdata >> 8);
               u32result -= (u8)(lastdata >> 16);
               u32result -= (u8)(lastdata >> 24);
               //���4�ֽڸ�ΪС��
               u32count = 0;
               u32count += (u8)(lastdata);
               u32count <<= 8;
               u32count += (u8)(lastdata >> 8);
               u32count <<= 8;
               u32count += (u8)(lastdata >> 16);
               u32count <<= 8;
               u32count += (u8)(lastdata >> 24);
               return (u32result - u32count);
          }
     }     
     return u32result;
}
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����ڲ�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
//�����ַ����ֽ���������\0
static u8 CodeUpdate_GetSLen(u8 *buf)
{
    u8 len = 0;

    while (*buf != 0)
    {
        len++;
        buf++;
    }

    return len;
}

//�ļ�ͷ��Ϣ����
//����ɹ�������0
//����ʧ�ܣ�����С��0
static s8 CodeUpdate_FileHeaderDecode(FILE_HEADER *p,vu8 *dp)
{
     vu16 i = 0;
     u8 u8temp;
     // ��ȡ16�ֽڵ�����
     for(u8temp = 0;u8temp < CommandSize;u8temp++) 
     {
          p->Command[u8temp] = dp[i++];
     }                       
     // �������س�����豸����: "BLEGateWay"     
     //VIR_DMACOM1_Send((u8 *)&dp[i],4);
     for(u8temp = 0;u8temp < DeviceModeSize;u8temp++) 
     {
          p->DeviceMode[u8temp] = dp[i++];
     }
     // �������س������Ŀ����: "B25 "     
     //VIR_DMACOM1_Send((u8 *)&dp[i],4);
     for(u8temp = 0;u8temp < ProjectModeSize;u8temp++) 
     {
          p->machinetype[u8temp] = dp[i++];
     }     
     // �����س�����ļ�����:����/Boot/�ֿ�1/�ֿ�2... 
     p->datatype= *(u8 *)(&dp[i++]);
     // ����������Ƿ�������ؿ�����
     p->downloadcontrol = *(u8 *)(&dp[i++]);
     // ����д��������ʼ��ַ
     p->Start_address = *(u32 *)(&dp[i]);
     i += 4;
     // ����д�����ĳ���
     p->File_Length = *(u32 *)(&dp[i]);
     i += 4;
     // ���ļ�ͷУ����
     p->Checksum = *(u16 *)(&dp[i]);
     return 0;
}

//�����ļ�ͷ��ϢУ��
//����ͨ��������0
//����ʧ�ܣ�����С��0
static s8 CodeUpdate_CheckHeaderValid(FILE_HEADER *p,vu8 *dp)
{	
     u8 i = 0,u8temp,u8ok; 
     u16 checksum = 0;

     for(u8temp = 0;u8temp < CodeUpdate_CodeTypeNum;u8temp++)
     {
          if(FileTypeLab[u8temp].Size == 0)
          {
               //VIR_DMACOM1_SendSTR("777");
               return -1;
          }
          u8ok = 1;
          //------��Ŀ�ͺż��
          if(u8ok)
          {
               for(i = 0;i < ProjectModeSize;i++)
               {
                    if(p->machinetype[i] != FileTypeLab[u8temp].ProjectMode[i])
                    {//��Ŀ�ͺż�����
                         //DRI_USART0_SendSTR("���ʹ���!");
                         u8ok = 0;
                         break;
                    }
               }
          }
          //------�豸�ͺż��
          if(u8ok)
          {
               for(i = 0;i < DeviceModeSize;i++)
               {
                    if(p->DeviceMode[i] != FileTypeLab[u8temp].DeviceMode[i])
                    {//�豸�ͺż�����
                         //DRI_USART0_SendSTR("���ʹ���!");
                         u8ok = 0;
                         break;
                    }
                    if(FileTypeLab[u8temp].DeviceMode[i] == '\0')
                    {//�ȽϽ���
                         break;
                    }
               }
          }
          //------�ж��ļ�����
          if(u8ok)
          {
               if(p->datatype != FileTypeLab[u8temp].FileTypeCode)
               {
                    //VIR_DMACOM1_SendSTR("222");
                    u8ok = 0;
               }
          }
          //------�жϼ��ص�ַ
          if(u8ok)
          {
               if(p->Start_address != FileTypeLab[u8temp].Address)
               {
                    //VIR_DMACOM1_SendSTR("333");
                    u8ok = 0;
               }
          }
          //------�ж��ֽ���
          if(u8ok)
          {
               if(p->File_Length != FileTypeLab[u8temp].Size)
               {
                    //VIR_DMACOM1_SendSTR("444");
                    u8ok = 0;
               }
          }
          //------У��͵ļ��
          if(u8ok)
          {
               for(i = 0;i < (FileHeaderSize - FileHeaderCheckSum_Size);i++)
               {
                    checksum += dp[i];
               }
               if(checksum == p->Checksum)
               {
                    //VIR_DMACOM1_SendSTR("888");
                    return 0;
               }
               //VIR_DMACOM1_SendSTR("555");
          }
     }
     //VIR_DMACOM1_SendSTR("666");
     return -1;
}


