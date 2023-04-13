#ifndef FUN_SST25VF032_h
#define FUN_SST25VF032_h

//
typedef enum
{
     ERASE_4K_TYPE  = 0x01,  
     ERASE_32K_TYPE = 0x02,	
     ERASE_64K_TYPE = 0x03,
     ERASE_ALL_TYPE = 0x04, 	
}Erase_Type;

typedef struct
{
     u8(*ReadWriteCallFunP)(u8);//阻塞型字节读写回调函数指针
     void(*CS1CallFunP)(void);//阻塞型片选引脚输出1回调函数指针
     void(*CS0CallFunP)(void);//阻塞型片选引脚输出0回调函数指针
}SST25VF032DevicePort;


extern s32 FUN_SST25VF032_ReadData(SST25VF032DevicePort *dp,u32 dwAddr, u8 *vData, u16 wLength);
extern u32 FUN_SST25VF032_GetCapacity(SST25VF032DevicePort *dp);
extern s8 FUN_SST25VF032_WriteByte(SST25VF032DevicePort *dp,u32 dwAddr,u8 vData);
extern s32 FUN_SST25VF032_WriteData(SST25VF032DevicePort *dp,u32 dwAddr, u8 *vData, u16 wLen);
extern s32 FUN_SST25VF032_WriteData2(SST25VF032DevicePort *dp,u32 dwAddr, u8 *vData, u16 wLen);
extern s32 FUN_SST25VF032_Erase(SST25VF032DevicePort *dp,u32 eadd,u16 el);
extern s8 FUN_SST25VF032_NonBlock_EraseSector(SST25VF032DevicePort *dp,Erase_Type type ,u32 dwAddr);
extern s8 FUN_SST25VF032_NonBlock_CheckEraseSector(SST25VF032DevicePort *dp);
extern s8 FUN_SST25VF032_EraseChip(SST25VF032DevicePort *dp);
extern s8 FUN_SST25VF032_ReadID(SST25VF032DevicePort *dp,u8 *id1, u8 *id2);

#endif

