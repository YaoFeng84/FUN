#ifndef FUN_CodeUpdate_h
#define FUN_CodeUpdate_h

#define CodeUpdate_CodeTypeNum          10   //�������ͱ���

typedef enum
{
     HeadDataError = 0u,
     EraseError,
     WriteError,
     ReadError,
     CodeCheckError,
     WaitUpdateHead,
     StartErase,
     WriteProgress,
     ReStart,
     UpdateSuccess
}CU_StatuEnum;

typedef struct
{
     u8 *pmp;       //��Ŀ�����ַ���ָ��(== 4�ֽڣ�����\0)
     u8 *dmp;       //�豸�����ַ���ָ��(<=15�ֽڣ�����\0)
     u8 FileTypeCode;//�ļ�����������
     u32 Address;   //��ʼ��ַ
     u32 Size;      //�ֽ���(Ϊ0ʱ����ʾ�ñ��������������һ�ű�)
}CU_FileTabType;

typedef struct
{     
     s8   (*NonBlock_StartGetHead)(u32 *,u8);     //������������ȡͷ�ص�����ָ��(��ſռ��׵�ַ,ϣ����ȡ���ֽ���)��(����:С��0����ʧ�� ����0�����ɹ�)
     s32  (*NonBlock_CheckGetHead)(void);         //�������жϻ�ȡͷ״̬(���أ������0:ʵ�ʻ�ȡ�ֽ��� -1:��δ��� С��-1:��ʧ��)ע�⣬��Ҫ�����48���ֽڲŻ�ֹͣ��飬����᲻�ϲ�ѯ
     s8   (*NonBlock_StartErase)(u8,u32,u16);     //���������������ռ�ص�����ָ��(����:�������͡������ռ��׵�ַ�������ֽ���)��(����:С��0����ʧ�� ����0�����ɹ�)
     s32  (*NonBlock_CheckErase)(void);           //�������жϲ���״̬(����:С��0����ʧ�� 0������� ����0����δ���)
     s8   (*NonBlock_StartWrite)(u8,u32,u16);     //����������д�ռ�ص�����ָ��(����:��������,д�ռ���ʼ��ַ,��д�ֽ���)��(����:С��0����ʧ�� ����0�����ɹ�)
     s32  (*NonBlock_CheckWrite)(void);           //�������ж�д״̬(���أ������0:ʵ��д�ֽ��� -1:дδ��� С��-1:дʧ��)
     s8   (*NonBlock_StartRead)(u8,u32,u16,u32 *);//�������������ռ�ص�����ָ��(����:��������,���ռ���ʼ��ַ,���������ֽ���,��ſռ��׵�ַ)��(����:С��0����ʧ�� ����0�����ɹ�)
     s32  (*NonBlock_CheckRead)(void);            //�������ж϶�״̬(���أ������0:ʵ�ʶ����ֽ��� -1:��δ��� С��-1:��ʧ��)
     void (*StatuFp)(CU_StatuEnum,u8);            //״̬�ص�����ָ��(����:��״̬,�����ٷֱȽ���(��״̬ΪWriteProgressʱ��Ч))
}CU_CallBackFunPA;//�ص�����ָ����

typedef struct
{
     CU_FileTabType *FileTabP;     //�ļ���ָ��(���ִ������;��м��ű����� < CodeUpdate_CodeTypeNum)
     CU_CallBackFunPA *CallBackFp; //�ص�����ָ����
}CUCnfType;

extern s8 FUN_CodeUpdate_Config(CUCnfType *cucp);
extern void FUN_CodeUpdate_RealTask(void);

extern void FUN_CodeUpdate_Start(void);
extern void FUN_CodeUpdate_Stop(void);
extern u32 FUN_CodeUpdate_CheckCodeIsValid(u8 *dp,u16 dl,u8 flag);

#endif

