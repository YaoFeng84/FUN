#ifndef FUN_Memory_h
#define FUN_Memory_h

typedef struct 
{//MEMORY CONTROL BLOCK--(MCB 或 分区表)
    void *OSMemAddr;     //指向本分区表存储空间的起始指针
    void *OSMemFreeList; //指向分区总链表内下一个可用的分区表的指针
                         //复用(或指向本分区表内下一个可用的内存块的指针)
    u32  OSMemBlkSize;   //本分区表中每个内存块的大小，单位字节
    u32  OSMemNBlks;     //本分区表中内存块的总数
    u32  OSMemNFree;     //本分区表中剩余的内存块数
    u8  *OSMemName;      //本分区表中表名指针
}OS_MEM;


typedef struct 
{
    void   *OSAddr;      //指向分区表存储区的起始地址       
    void   *OSFreeList;  //指向分区表内空闲内存块的起始地址                
    u32  OSBlkSize;      //每个内存块的字节数        
    u32  OSNBlks;        //分区表中内存块的数量        
    u32  OSNFree;        //分区表中未用内存块的数量    
    u32  OSNUsed;        //分区表中已用内存块的数量          
}OS_MEM_DATA;


#endif
