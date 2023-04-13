#ifndef FUN_Memory_h
#define FUN_Memory_h

typedef struct 
{//MEMORY CONTROL BLOCK--(MCB �� ������)
    void *OSMemAddr;     //ָ�򱾷�����洢�ռ����ʼָ��
    void *OSMemFreeList; //ָ���������������һ�����õķ������ָ��
                         //����(��ָ�򱾷���������һ�����õ��ڴ���ָ��)
    u32  OSMemBlkSize;   //����������ÿ���ڴ��Ĵ�С����λ�ֽ�
    u32  OSMemNBlks;     //�����������ڴ�������
    u32  OSMemNFree;     //����������ʣ����ڴ����
    u8  *OSMemName;      //���������б���ָ��
}OS_MEM;


typedef struct 
{
    void   *OSAddr;      //ָ�������洢������ʼ��ַ       
    void   *OSFreeList;  //ָ��������ڿ����ڴ�����ʼ��ַ                
    u32  OSBlkSize;      //ÿ���ڴ����ֽ���        
    u32  OSNBlks;        //���������ڴ�������        
    u32  OSNFree;        //��������δ���ڴ�������    
    u32  OSNUsed;        //�������������ڴ�������          
}OS_MEM_DATA;


#endif
