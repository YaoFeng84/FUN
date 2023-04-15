/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------以下是模块的修改记录区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * 内容：
 * 日期：2023-02-21
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
          
     【1】、本模块内部 有 一张分区总链表，用来控制本模块所有类型的分区
     所谓不同类型的分区 比如
     （分区A，块大小为10K，共3个块）
     （分区B，块大小为5K，共6个块）
     【2】、分区表记录着本分区内的内存块信息（内存块大小，内存块数）
     分区内的各内存块 在各其开头4字节空间内 指向（保存着） 下一个内存块的地址
     这样，就将各内存块形成一个单向链接的链表。
     而该链表是分别存放在各自的开头4字节部分，不占用额外的空间。     
----------     -----------     -----------     -----------
| FreeList-----|-->   *--|-----|-->   *--|-----|-->   *--|--->0
| 分区表 |     | 内存块1 |      | 内存块2 |     | 内存块3  |
|       |     |         |     |         |     |          |
----------    -----------    -----------    -----------
     【3】、


*/

#include "FUN_Memory.h"
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下模块间的对接函数区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/

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
*               ----------------------------------以下是模块内的变量类型定义区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/


/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数申明区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static void OS_MemClr (u8  *pdest,u16  size);
static u8  OS_StrLen (u8 *psrc);

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块内的 变量 与 宏定义 申明区--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
OS_MEM *OSMemFreeList;           //指向可用的分区表指针
OS_MEM OSMemTbl[OS_MAX_MEM_PART];//分区总链表

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的系统函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
函数名: FUN_Memory_Init
功   能: 内存分区模块初始化
参   数:
       无
                
返回值:
      无
      
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2023-02-21   
-------------------------------------------------*/
void FUN_Memory_Init(void)
{
#if OS_MAX_MEM_PART == 1u
     OS_MemClr((u8 *)&OSMemTbl[0], sizeof(OSMemTbl));//清0所有分区表
     OSMemFreeList = (OS_MEM *)&OSMemTbl[0];         //可用分区表指针指向分区表0
     OSMemFreeList->OSMemName = (u8 *)"?";           //分区表0的分区名指针指向?
#endif

#if OS_MAX_MEM_PART >= 2u
     OS_MEM  *pmem;
     u16   i;

     OS_MemClr((u8 *)&OSMemTbl[0], sizeof(OSMemTbl));//清0所有分区表
     for (i = 0u; i < (OS_MAX_MEM_PART - 1u); i++) 
     {//除最后一张分区表外，初始化每个分区表
          pmem                = &OSMemTbl[i];               //赋值待初始化分区表的指针
          pmem->OSMemFreeList = (void *)&OSMemTbl[i + 1u];  //当前分区表的 可用分区表指针 指向 下一张分区表
          pmem->OSMemName  = (u8 *)(void *)"?";             //当前分区表的 名字指针指向?
     }
     //初始化最后一张分区表
     pmem                = &OSMemTbl[i]; //赋值最后一张待初始化分区表的指针
     pmem->OSMemFreeList = (void *)0;    //最后一张分区表的 可用分区表指针 指向空
     pmem->OSMemName = (u8 *)(void *)"?";//最后一张分区表的 名字指针指向?

     OSMemFreeList   = &OSMemTbl[0];     //可用分区表指针指向第一张分区表
#endif
}


/*-------------------------------------------------
函数名: FUN_Memory_Config
功   能: 创建一个固定大小的内存分区
参   数:
       void *addr：待创建分区 的起始地址
       u32 nblks：要从分区创建的内存块数
       u32 blksize：是内存分区中每个块的大小（以字节为单位）。
       u8 *perr：创建结果
                0：创建成功
                
返回值:
       
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2023-02-21   
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

//参数合法判断
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
//     pmem = OSMemFreeList;     //将可用的分区表指针 赋值给pmem
//     if(OSMemFreeList != (OS_MEM *)0) 
//     {//如果当前分区表可用 
//          OSMemFreeList = (OS_MEM *)OSMemFreeList->OSMemFreeList;//更新可用分区表指针
//     }


//     if(pmem == (OS_MEM *)0) 
//     {//如果当前分区表不可用
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
*               ----------------------------------以下是模块的用户函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
函数名: FUN_Memory_Create
功   能: 创建一个固定大小的内存分区
参   数:
       void *addr：待创建分区 的起始地址（需要地址对齐）
       u32 nblks：要从分区创建的内存块数（>=2）
       u32 blksize：是内存分区中每个块的大小(>=4)（以字节为单位）。
       OS_MEM *pp：返回分区表指针
                
返回值:
       0：创建成功
      -1：无效地址
      -2：内存块数量过小
      -3：每个内存块空间过小
      -4：分区链表空间不够
注   意:
       如果pp != (OS_MEM *)0 则有创建
       如果pp == (OS_MEM *)0 则无创建
示   例:
作   者:YJX
版   本:V1.0
时   间:2023-02-21   
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
//参数合法判断
     if(addr == (void *)0) 
     {//地址指针不可为空
          return -1;
     }
     if(((u32)addr & (sizeof(void *) - 1u)) != 0u)
     {//地址指针必须对齐
          return -1;
     }
     if(nblks < 2u) 
     {//内存块数量小于2个
          return -2;
     }
     if(blksize < sizeof(void *)) 
     {//每个内存块字节数小于 4
          return -3;
     }

//-----------
     pmem = OSMemFreeList;     //将可用的分区表指针 赋值给pmem
     if(OSMemFreeList != (OS_MEM *)0) 
     {//如果当前分区表可用 
          OSMemFreeList = (OS_MEM *)OSMemFreeList->OSMemFreeList;//更新可用分区表指针
     }


     if(pmem == (OS_MEM *)0) 
     {//如果当前分区表不可用
          return -4;
     }

//-----开始在 本分区内 利用各存储块起始部分空间 存放下一个存储块的地址 创建各内存块的单向链表
     plink = (void **)addr;//plink定位到首个内存块的开头
     pblk  = (u8 *)addr; //另存为字节指针到pblk，便于字节级偏移操作
     loops = nblks - 1u;
     for(i = 0u; i < loops; i++) 
     {
          pblk  +=  blksize;            //当前pblk偏移blksize字节后(指向下一个内存块地址),偏移后仍放在pblk中
          *plink = (void  *)pblk;       //将pblk强制转换为void*后，保存到*plink（当前内存块开头的4字节空间中）。
          plink  = (void **)pblk;       //将pblk强制转换为void**后，更新plink，使plink定位到下个存储块的开头
     }
     *plink              = (void *)0;   //最后一个内存块的下一个内存块地址指针指向空
     
     pmem->OSMemAddr     = addr;        //在MCB中保存本分区表存储空间的首地址
     pmem->OSMemFreeList = addr;        //复用，指向本分区表下一个可用的内存块地址
     pmem->OSMemNFree    = nblks;       //在MCB中保存剩余的内存块数
     pmem->OSMemNBlks    = nblks;       //在MCB中保存内存块总数
     pmem->OSMemBlkSize  = blksize;     //在MCB中保存每个内存块的大小
     pp = pmem;
     return 0;
}


/*-------------------------------------------------
函数名: FUN_Memory_Get
功   能: 在指定的分区中申请一个内存块
参   数:
       OS_MEM *pmem：分区表指针
       void *mp：返回申请到的内存块首地址
                
返回值:
       0：申请成功
      -1：分区表无效
      -2：无空闲内存块
注   意:
       如果mp != (OS_MEM *)0 则申请成功
       如果mp == (OS_MEM *)0 则申请失败
示   例:
作   者:YJX
版   本:V1.0
时   间:2023-02-21   
-------------------------------------------------*/
s32 FUN_Memory_Get(OS_MEM *pmem,void *mp)
{
     void *pblk;

     mp = ((void *)0);
     mp = mp;
     //输入参数合法判断
     if(pmem == (OS_MEM *)0) 
     {//分区表无效
          return -1;
     }

     //-----
     if(pmem->OSMemNFree > 0u)
     {//分区里还用剩余内存块
          pblk = pmem->OSMemFreeList;    //调出内存块指针
          pmem->OSMemFreeList = *(void **)pblk;//调整指向本分区内下一个可用的内存块
          pmem->OSMemNFree--;//本分区的剩余内存块数减1
          mp = pblk;
          return 0;
     }
     //分区里没有剩余内存块
     return -2;
}

/*-------------------------------------------------
函数名: FUN_Memory_Put
功   能: 向指定的分区归还一个内存块
参   数:
       OS_MEM *pmem：分区表指针
       void *pblk：待归还内存块首地址
       u32 blksize：待归还内存块字节数
                
返回值:
       0：申请成功
      -1：分区表无效
      -2：归还内存块地址无效
      -3：归还内存块空间无效
      -4：分区表空间已满
      
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2023-02-21   
-------------------------------------------------*/
s32 FUN_Memory_Put(OS_MEM *pmem,void *pblk,u32 blksize)
{
     //输入参数合法判断
     if(pmem == (OS_MEM *)0)
     {//分区表无效
          return -1;
     }
     if(pblk == (void *)0)
     {//归还的内存块首地址无效
          return -2;
     }
     if(blksize != pmem->OSMemBlkSize)
     {//归还的空间不对
          return -3;
     }

     //----------
     if (pmem->OSMemNFree >= pmem->OSMemNBlks)
     {//分区表已满
          return -3;
     }
     
     *(void **)pblk      = pmem->OSMemFreeList;   //归还内存块的头4字节空间 存入 当前分区表可用的内存块首地址(插入操作)
     pmem->OSMemFreeList = pblk;   //当前分区表可用的内存块地址 更新为 归还内存块空间地址
     pmem->OSMemNFree++; //剩余内存块数加1
     return 0; 
}



/*-------------------------------------------------
函数名: FUN_Memory_Query
功   能: 分区表状态查询
参   数:
       OS_MEM *pmem：分区表指针
       OS_MEM_DATA *p_mem_data：查询数据返回结构体指针
                
返回值:
       0：查询成功
      -1：分区表无效
      -2：非法返回地址
      
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2023-02-21   
-------------------------------------------------*/
s32 FUN_Memory_Query (OS_MEM *pmem,OS_MEM_DATA *p_mem_data)
{
     //输入参数合法判断
     if(pmem == (OS_MEM *)0) 
     {//非法分区表
          return (-1);
     }
     if(p_mem_data == (OS_MEM_DATA *)0) 
     {//非法地址
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
     //输入参数合法判断
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
     //输入参数合法判断
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
*               ----------------------------------以下是模块的内部函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
//内存清0
static void OS_MemClr(u8 *pdest,u16 size)
{
     while (size > 0u) 
     {
          *pdest++ = (u8)0;
          size--;
     }
}

//计算字符串长度
static u8 OS_StrLen(u8 *psrc)
{
     u8 len;

     //输入参数合法判断
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



