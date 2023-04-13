#ifndef FUN_CodeUpdate_h
#define FUN_CodeUpdate_h

#define CodeUpdate_CodeTypeNum          10   //代码类型表数

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
     u8 *pmp;       //项目类型字符串指针(== 4字节，不含\0)
     u8 *dmp;       //设备类型字符串指针(<=15字节，不含\0)
     u8 FileTypeCode;//文件数据类型码
     u32 Address;   //起始地址
     u32 Size;      //字节数(为0时，表示该表无意义且是最后一张表)
}CU_FileTabType;

typedef struct
{     
     s8   (*NonBlock_StartGetHead)(u32 *,u8);     //非阻塞启动获取头回调函数指针(存放空间首地址,希望获取的字节数)，(返回:小于0启动失败 等于0启动成功)
     s32  (*NonBlock_CheckGetHead)(void);         //非阻塞判断获取头状态(返回：大等于0:实际获取字节数 -1:读未完成 小于-1:读失败)注意，需要大等于48个字节才会停止检查，否则会不断查询
     s8   (*NonBlock_StartErase)(u8,u32,u16);     //非阻塞启动擦除空间回调函数指针(参数:数据类型、擦除空间首地址、擦除字节数)，(返回:小于0启动失败 等于0启动成功)
     s32  (*NonBlock_CheckErase)(void);           //非阻塞判断擦除状态(返回:小于0擦除失败 0擦除完成 大于0擦除未完成)
     s8   (*NonBlock_StartWrite)(u8,u32,u16);     //非阻塞启动写空间回调函数指针(参数:数据类型,写空间起始地址,待写字节数)，(返回:小于0启动失败 等于0启动成功)
     s32  (*NonBlock_CheckWrite)(void);           //非阻塞判断写状态(返回：大等于0:实际写字节数 -1:写未完成 小于-1:写失败)
     s8   (*NonBlock_StartRead)(u8,u32,u16,u32 *);//非阻塞启动读空间回调函数指针(参数:数据类型,读空间起始地址,待读数据字节数,存放空间首地址)，(返回:小于0启动失败 等于0启动成功)
     s32  (*NonBlock_CheckRead)(void);            //非阻塞判断读状态(返回：大等于0:实际读到字节数 -1:读未完成 小于-1:读失败)
     void (*StatuFp)(CU_StatuEnum,u8);            //状态回调函数指针(参数:各状态,升级百分比进度(仅状态为WriteProgress时有效))
}CU_CallBackFunPA;//回调函数指针组

typedef struct
{
     CU_FileTabType *FileTabP;     //文件表指针(几种代码类型就有几张表，张数 < CodeUpdate_CodeTypeNum)
     CU_CallBackFunPA *CallBackFp; //回调函数指针组
}CUCnfType;

extern s8 FUN_CodeUpdate_Config(CUCnfType *cucp);
extern void FUN_CodeUpdate_RealTask(void);

extern void FUN_CodeUpdate_Start(void);
extern void FUN_CodeUpdate_Stop(void);
extern u32 FUN_CodeUpdate_CheckCodeIsValid(u8 *dp,u16 dl,u8 flag);

#endif

