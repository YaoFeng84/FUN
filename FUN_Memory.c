/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------������ģ����޸ļ�¼��-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * ���ݣ�
 * ���ڣ�2023-02-21
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
          
     ��1������ģ���ڲ� �� һ�ŷ����������������Ʊ�ģ���������͵ķ���
     ��ν��ͬ���͵ķ��� ����
     ������A�����СΪ10K����3���飩
     ������B�����СΪ5K����6���飩
     ��2�����������¼�ű������ڵ��ڴ����Ϣ���ڴ���С���ڴ������
     �����ڵĸ��ڴ�� �ڸ��俪ͷ4�ֽڿռ��� ָ�򣨱����ţ� ��һ���ڴ��ĵ�ַ
     �������ͽ����ڴ���γ�һ���������ӵ�����
     ���������Ƿֱ����ڸ��ԵĿ�ͷ4�ֽڲ��֣���ռ�ö���Ŀռ䡣     
----------     -----------     -----------     -----------
| FreeList-----|-->   *--|-----|-->   *--|-----|-->   *--|--->0
| ������ |     | �ڴ��1 |      | �ڴ��2 |     | �ڴ��3  |
|       |     |         |     |         |     |          |
----------    -----------    -----------    -----------
     ��3����


*/

#include "FUN_Memory.h"
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
#define OS_MEM_NAME_EN        1u    /*     Enable memory partition names                            */
#define OS_MAX_MEM_PART       10u    /* Max. number of memory partitions                             */


#define OS_ERR_MEM_INVALID_PART        90u
#define OS_ERR_MEM_INVALID_BLKS        91u
#define OS_ERR_MEM_INVALID_SIZE        92u
#define OS_ERR_MEM_NO_FREE_BLKS        93u
#define OS_ERR_MEM_FULL                94u
#define OS_ERR_MEM_INVALID_PBLK        95u
#define OS_ERR_MEM_INVALID_PMEM        96u
#define OS_ERR_MEM_INVALID_PDATA       97u
#define OS_ERR_MEM_INVALID_ADDR        98u
//#define OS_ERR_MEM_NAME_TOO_LONG       99u

#define OS_ERR_PNAME_NULL              12u


#define OS_ERR_NONE                     0u

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
static void OS_MemClr (u8  *pdest,u16  size);
static u8  OS_StrLen (u8 *psrc);

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ڵ� ���� �� �궨�� ������--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
OS_MEM *OSMemFreeList;           //ָ����õķ�����ָ��
OS_MEM OSMemTbl[OS_MAX_MEM_PART];//����������

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ϵͳ����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
������: FUN_Memory_Init
��   ��: �ڴ����ģ���ʼ��
��   ��:
       ��
                
����ֵ:
      ��
      
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-02-21   
-------------------------------------------------*/
void FUN_Memory_Init(void)
{
#if OS_MAX_MEM_PART == 1u
     OS_MemClr((u8 *)&OSMemTbl[0], sizeof(OSMemTbl));//��0���з�����
     OSMemFreeList = (OS_MEM *)&OSMemTbl[0];         //���÷�����ָ��ָ�������0
     OSMemFreeList->OSMemName = (u8 *)"?";           //������0�ķ�����ָ��ָ��?
#endif

#if OS_MAX_MEM_PART >= 2u
     OS_MEM  *pmem;
     u16   i;

     OS_MemClr((u8 *)&OSMemTbl[0], sizeof(OSMemTbl));//��0���з�����
     for (i = 0u; i < (OS_MAX_MEM_PART - 1u); i++) 
     {//�����һ�ŷ������⣬��ʼ��ÿ��������
          pmem                = &OSMemTbl[i];               //��ֵ����ʼ���������ָ��
          pmem->OSMemFreeList = (void *)&OSMemTbl[i + 1u];  //��ǰ������� ���÷�����ָ�� ָ�� ��һ�ŷ�����
          pmem->OSMemName  = (u8 *)(void *)"?";             //��ǰ������� ����ָ��ָ��?
     }
     //��ʼ�����һ�ŷ�����
     pmem                = &OSMemTbl[i]; //��ֵ���һ�Ŵ���ʼ���������ָ��
     pmem->OSMemFreeList = (void *)0;    //���һ�ŷ������ ���÷�����ָ�� ָ���
     pmem->OSMemName = (u8 *)(void *)"?";//���һ�ŷ������ ����ָ��ָ��?

     OSMemFreeList   = &OSMemTbl[0];     //���÷�����ָ��ָ���һ�ŷ�����
#endif
}


/*-------------------------------------------------
������: FUN_Memory_Config
��   ��: ����һ���̶���С���ڴ����
��   ��:
       void *addr������������ ����ʼ��ַ
       u32 nblks��Ҫ�ӷ����������ڴ����
       u32 blksize�����ڴ������ÿ����Ĵ�С�����ֽ�Ϊ��λ����
       u8 *perr���������
                0�������ɹ�
                
����ֵ:
       
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-02-21   
-------------------------------------------------*/

/*
*********************************************************************************************************
*                                      CREATE A MEMORY PARTITION
*
* Description : Create a fixed-sized memory partition that will be managed by uC/OS-II.
*
* Arguments   : addr     is the starting address of the memory partition
*
*               nblks    is the number of memory blocks to create from the partition.
*
*               blksize  is the size (in bytes) of each block in the memory partition.
*
*               perr     is a pointer to a variable containing an error message which will be set by
*                        this function to either:
*
*                        OS_ERR_NONE              if the memory partition has been created correctly.
*                        OS_ERR_MEM_INVALID_ADDR  if you are specifying an invalid address for the memory
*                                                 storage of the partition or, the block does not align
*                                                 on a pointer boundary
*                        OS_ERR_MEM_INVALID_PART  no free partitions available
*                        OS_ERR_MEM_INVALID_BLKS  user specified an invalid number of blocks (must be >= 2)
*                        OS_ERR_MEM_INVALID_SIZE  user specified an invalid block size
*                                                   - must be greater than the size of a pointer
*                                                   - must be able to hold an integral number of pointers
* Returns    : != (OS_MEM *)0  is the partition was created
*              == (OS_MEM *)0  if the partition was not created because of invalid arguments or, no
*                              free partition is available.
*********************************************************************************************************
*/

//OS_MEM *OSMemCreate (void *addr,u32 nblks,u32 blksize,u8 *perr)
//{
//     OS_MEM *pmem;
//     u8     *pblk;
//     void   **plink;
//     u32    loops;
//     u32    i;

//�����Ϸ��ж�
//     if(addr == (void *)0) 
//     {/* Must pass a valid address for the memory part.*/
//          *perr = OS_ERR_MEM_INVALID_ADDR;
//          return ((OS_MEM *)0);
//     }
//     if(((u32)addr & (sizeof(void *) - 1u)) != 0u)
//     {/* Must be pointer size aligned                */
//          *perr = OS_ERR_MEM_INVALID_ADDR;
//          return ((OS_MEM *)0);
//     }
//     if(nblks < 2u) 
//     {/* Must have at least 2 blocks per partition     */
//          *perr = OS_ERR_MEM_INVALID_BLKS;
//          return ((OS_MEM *)0);
//     }
//     if(blksize < sizeof(void *)) 
//     {/* Must contain space for at least a pointer     */
//          *perr = OS_ERR_MEM_INVALID_SIZE;
//          return ((OS_MEM *)0);
//     }

//-----------
//     pmem = OSMemFreeList;     //�����õķ�����ָ�� ��ֵ��pmem
//     if(OSMemFreeList != (OS_MEM *)0) 
//     {//�����ǰ��������� 
//          OSMemFreeList = (OS_MEM *)OSMemFreeList->OSMemFreeList;//���¿��÷�����ָ��
//     }


//     if(pmem == (OS_MEM *)0) 
//     {//�����ǰ����������
//          *perr = OS_ERR_MEM_INVALID_PART;
//          return ((OS_MEM *)0);
//     }

//     plink = (void **)addr;             /* Create linked list of free memory blocks      */
//     pblk  = (u8 *)addr;
//     loops  = nblks - 1u;
//     for(i = 0u; i < loops; i++) 
//     {
//          pblk +=  blksize;             /* Point to the FOLLOWING block                  */
//          *plink = (void  *)pblk;       /* Save pointer to NEXT block in CURRENT block   */
//          plink = (void **)pblk;        /* Position to  NEXT      block                  */
//     }
//     *plink              = (void *)0;   /* Last memory block points to NULL              */
//     pmem->OSMemAddr     = addr;        /* Store start address of memory partition       */
//     pmem->OSMemFreeList = addr;        /* Initialize pointer to pool of free blocks     */
//     pmem->OSMemNFree    = nblks;       /* Store number of free blocks in MCB            */
//     pmem->OSMemNBlks    = nblks;
//     pmem->OSMemBlkSize  = blksize;     /* Store block size of each memory blocks        */
//     *perr               = OS_ERR_NONE;
//     return (pmem);
//}




















/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����û�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
������: FUN_Memory_Create
��   ��: ����һ���̶���С���ڴ����
��   ��:
       void *addr������������ ����ʼ��ַ����Ҫ��ַ���룩
       u32 nblks��Ҫ�ӷ����������ڴ������>=2��
       u32 blksize�����ڴ������ÿ����Ĵ�С(>=4)�����ֽ�Ϊ��λ����
       OS_MEM *pp�����ط�����ָ��
                
����ֵ:
       0�������ɹ�
      -1����Ч��ַ
      -2���ڴ��������С
      -3��ÿ���ڴ��ռ��С
      -4����������ռ䲻��
ע   ��:
       ���pp != (OS_MEM *)0 ���д���
       ���pp == (OS_MEM *)0 ���޴���
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-02-21   
-------------------------------------------------*/
s32 FUN_Memory_Create(void *addr,u32 nblks,u32 blksize,OS_MEM *pp)
{
     OS_MEM *pmem;
     u8     *pblk;
     void   **plink;
     u32    loops;
     u32    i;

     pp = ((OS_MEM *)0);
     pp = pp;
//�����Ϸ��ж�
     if(addr == (void *)0) 
     {//��ַָ�벻��Ϊ��
          return -1;
     }
     if(((u32)addr & (sizeof(void *) - 1u)) != 0u)
     {//��ַָ��������
          return -1;
     }
     if(nblks < 2u) 
     {//�ڴ������С��2��
          return -2;
     }
     if(blksize < sizeof(void *)) 
     {//ÿ���ڴ���ֽ���С�� 4
          return -3;
     }

//-----------
     pmem = OSMemFreeList;     //�����õķ�����ָ�� ��ֵ��pmem
     if(OSMemFreeList != (OS_MEM *)0) 
     {//�����ǰ��������� 
          OSMemFreeList = (OS_MEM *)OSMemFreeList->OSMemFreeList;//���¿��÷�����ָ��
     }


     if(pmem == (OS_MEM *)0) 
     {//�����ǰ����������
          return -4;
     }

//-----��ʼ�� �������� ���ø��洢����ʼ���ֿռ� �����һ���洢��ĵ�ַ �������ڴ��ĵ�������
     plink = (void **)addr;//plink��λ���׸��ڴ��Ŀ�ͷ
     pblk  = (u8 *)addr; //���Ϊ�ֽ�ָ�뵽pblk�������ֽڼ�ƫ�Ʋ���
     loops = nblks - 1u;
     for(i = 0u; i < loops; i++) 
     {
          pblk  +=  blksize;            //��ǰpblkƫ��blksize�ֽں�(ָ����һ���ڴ���ַ),ƫ�ƺ��Է���pblk��
          *plink = (void  *)pblk;       //��pblkǿ��ת��Ϊvoid*�󣬱��浽*plink����ǰ�ڴ�鿪ͷ��4�ֽڿռ��У���
          plink  = (void **)pblk;       //��pblkǿ��ת��Ϊvoid**�󣬸���plink��ʹplink��λ���¸��洢��Ŀ�ͷ
     }
     *plink              = (void *)0;   //���һ���ڴ�����һ���ڴ���ַָ��ָ���
     
     pmem->OSMemAddr     = addr;        //��MCB�б��汾������洢�ռ���׵�ַ
     pmem->OSMemFreeList = addr;        //���ã�ָ�򱾷�������һ�����õ��ڴ���ַ
     pmem->OSMemNFree    = nblks;       //��MCB�б���ʣ����ڴ����
     pmem->OSMemNBlks    = nblks;       //��MCB�б����ڴ������
     pmem->OSMemBlkSize  = blksize;     //��MCB�б���ÿ���ڴ��Ĵ�С
     pp = pmem;
     return 0;
}


/*-------------------------------------------------
������: FUN_Memory_Get
��   ��: ��ָ���ķ���������һ���ڴ��
��   ��:
       OS_MEM *pmem��������ָ��
       void *mp���������뵽���ڴ���׵�ַ
                
����ֵ:
       0������ɹ�
      -1����������Ч
      -2���޿����ڴ��
ע   ��:
       ���mp != (OS_MEM *)0 ������ɹ�
       ���mp == (OS_MEM *)0 ������ʧ��
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-02-21   
-------------------------------------------------*/
s32 FUN_Memory_Get(OS_MEM *pmem,void *mp)
{
     void *pblk;

     mp = ((void *)0);
     mp = mp;
     //��������Ϸ��ж�
     if(pmem == (OS_MEM *)0) 
     {//��������Ч
          return -1;
     }

     //-----
     if(pmem->OSMemNFree > 0u)
     {//�����ﻹ��ʣ���ڴ��
          pblk = pmem->OSMemFreeList;    //�����ڴ��ָ��
          pmem->OSMemFreeList = *(void **)pblk;//����ָ�򱾷�������һ�����õ��ڴ��
          pmem->OSMemNFree--;//��������ʣ���ڴ������1
          mp = pblk;
          return 0;
     }
     //������û��ʣ���ڴ��
     return -2;
}

/*-------------------------------------------------
������: FUN_Memory_Put
��   ��: ��ָ���ķ����黹һ���ڴ��
��   ��:
       OS_MEM *pmem��������ָ��
       void *pblk�����黹�ڴ���׵�ַ
       u32 blksize�����黹�ڴ���ֽ���
                
����ֵ:
       0������ɹ�
      -1����������Ч
      -2���黹�ڴ���ַ��Ч
      -3���黹�ڴ��ռ���Ч
      -4��������ռ�����
      
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-02-21   
-------------------------------------------------*/
s32 FUN_Memory_Put(OS_MEM *pmem,void *pblk,u32 blksize)
{
     //��������Ϸ��ж�
     if(pmem == (OS_MEM *)0)
     {//��������Ч
          return -1;
     }
     if(pblk == (void *)0)
     {//�黹���ڴ���׵�ַ��Ч
          return -2;
     }
     if(blksize != pmem->OSMemBlkSize)
     {//�黹�Ŀռ䲻��
          return -3;
     }

     //----------
     if (pmem->OSMemNFree >= pmem->OSMemNBlks)
     {//����������
          return -3;
     }
     
     *(void **)pblk      = pmem->OSMemFreeList;   //�黹�ڴ���ͷ4�ֽڿռ� ���� ��ǰ��������õ��ڴ���׵�ַ(�������)
     pmem->OSMemFreeList = pblk;   //��ǰ��������õ��ڴ���ַ ����Ϊ �黹�ڴ��ռ��ַ
     pmem->OSMemNFree++; //ʣ���ڴ������1
     return 0; 
}



/*-------------------------------------------------
������: FUN_Memory_Query
��   ��: ������״̬��ѯ
��   ��:
       OS_MEM *pmem��������ָ��
       OS_MEM_DATA *p_mem_data����ѯ���ݷ��ؽṹ��ָ��
                
����ֵ:
       0����ѯ�ɹ�
      -1����������Ч
      -2���Ƿ����ص�ַ
      
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-02-21   
-------------------------------------------------*/
s32 FUN_Memory_Query (OS_MEM *pmem,OS_MEM_DATA *p_mem_data)
{
     //��������Ϸ��ж�
     if(pmem == (OS_MEM *)0) 
     {//�Ƿ�������
          return (-1);
     }
     if(p_mem_data == (OS_MEM_DATA *)0) 
     {//�Ƿ���ַ
          return (-2);
     }

     //-------------------
     p_mem_data->OSAddr     = pmem->OSMemAddr;
     p_mem_data->OSFreeList = pmem->OSMemFreeList;
     p_mem_data->OSBlkSize  = pmem->OSMemBlkSize;
     p_mem_data->OSNBlks    = pmem->OSMemNBlks;
     p_mem_data->OSNFree    = pmem->OSMemNFree;
     p_mem_data->OSNUsed    = p_mem_data->OSNBlks - p_mem_data->OSNFree;
     return (0);
}


u8 OSMemNameGet(OS_MEM *pmem,u8 **pname,u8 *perr)
{
     u8 len;
     //��������Ϸ��ж�
     if(pmem == (OS_MEM *)0) 
     {  
          *perr = OS_ERR_MEM_INVALID_PMEM;
          return (0u);
     }
     if(pname == (u8 **)0) 
     {  
          *perr = OS_ERR_PNAME_NULL;
          return (0u);
     }

     //---------------
     *pname = pmem->OSMemName;
     len    = OS_StrLen(*pname);
     *perr  = OS_ERR_NONE;
     return (len);
}

void OSMemNameSet(OS_MEM *pmem,u8 *pname,u8 *perr)
{
     //��������Ϸ��ж�
     if(pmem == (OS_MEM *)0) 
     {                  
          *perr = OS_ERR_MEM_INVALID_PMEM;
          return;
     }
     if(pname == (u8 *)0) 
     {                   
          *perr = OS_ERR_PNAME_NULL;
          return;
     }
     //------------
     pmem->OSMemName = pname;
     *perr           = OS_ERR_NONE;
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����ڲ�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
//�ڴ���0
static void OS_MemClr(u8 *pdest,u16 size)
{
     while (size > 0u) 
     {
          *pdest++ = (u8)0;
          size--;
     }
}

//�����ַ�������
static u8 OS_StrLen(u8 *psrc)
{
     u8 len;

     //��������Ϸ��ж�
     if(psrc == (u8 *)0) 
     {
          return (0u);
     }


     len = 0u;
     while(*psrc != 0) 
     {
          psrc++;
          len++;
     }
     return (len);
}



