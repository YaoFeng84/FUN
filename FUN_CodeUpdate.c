/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------以下是模块的修改记录区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * 内容：
 * 日期：2018-07-26
 * 作者：YJX
 * 版本号：V1.0（初版）
 ***********************************************
 * 修改内容：增加设备类型判断
 * 修改日期：2021-11-26
 * 修改作者：YJX
 * 版 本 号：V1.1
 ***********************************************
 * 修改内容：增加非阻塞删除、读、写的事件
 * 修改日期：2022-01-04
 * 修改作者：YJX
 * 版 本 号：V1.2
 ***********************************************
*/
/********************************************************************************************************************************************
*                                                                                                                                           *
*                ---------------------------------以下是模块的使用说明区-----------------------------------------                           *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*                                       特点说明：
          支持多种文件类型的下载更新
【1】、函数
本模块共提供6个函数，其中2个平台函数，4个功能函数
平台函数如下：
FUN_CodeUpdate_Config配置函数
FUN_CodeUpdate_RealTask任务函数
功能函数如下：
FUN_CodeUpdate_ReceData接收函数，用于接收送来的代码数据
FUN_CodeUpdate_Start启动函数，配置函数执行后会自动执行启动函数
FUN_CodeUpdate_Stop停止函数。



*/
#include "TypeMro_N.h"
#include "FUN_CodeUpdate.h"
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
#define ProjectModeSize            4    //项目类型长度
#define DeviceModeSize             16   //设备类型长度

//------bin文件信息宏定义------
#define CHECKSUM_SIZE	          4    //CheckSum检验码所占的字节数
#define CommandSize                16   //bin文件头中命令的大小，单位字节
#define FileHeaderSize             48   //bin文件头大小，单位字节
#define FileHeaderCheckSum_Size    2    //头数据文件字节数

//
#define CU_ReadWriteByteNum        500  //单次读写字节数
#define CU_EraseByteNum            32768//单次擦除字节数(不可大等于65536，受函数形参限制)

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块内的变量类型定义区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
typedef struct 
{
     u8 Command[CommandSize];           //预留
	u8 machinetype[ProjectModeSize];   //4个字节项目号
	u8 DeviceMode[DeviceModeSize];     //设备类型
	u8 datatype;                       //代码类型
	u8  downloadcontrol;               //下载控制(0:表示完成后就结束升级 非0:表示完成后还有新的代码要升级)
     u32 Start_address;                 //起始存放地址
	u32 File_Length;                   //字节数
	u16 Checksum;                      //校验码
} FILE_HEADER;

typedef struct
{
     u8 ProjectMode[ProjectModeSize];   //项目类型
     u8 DeviceMode[DeviceModeSize];     //设备类型,'\0'结尾
     u8 FileTypeCode;                   //文件类型码
     u32 Address;                       //起始地址
     u32 Size;                          //字节数(为0时，表示该表无意义且是最后一张表)
}FILETYPETAB;

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数申明区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static u8 CodeUpdate_GetSLen(u8 *buf);
static s8 CodeUpdate_FileHeaderDecode(FILE_HEADER *p,vu8 *dp);
static s8 CodeUpdate_CheckHeaderValid(FILE_HEADER *p,vu8 *dp);

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块内的 变量 与 宏定义 申明区--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static FILETYPETAB FileTypeLab[CodeUpdate_CodeTypeNum + 1];//加1主要是为了存放最后一张无效的结束表
static FILE_HEADER fileheader;//申明一个文件头结构体变量
static CU_CallBackFunPA CallFpA;//回调函数指针组

#define CU_StateMachine_IDE             0//空闲
#define CU_StateMachine_WaitScanf       1//等待并扫描启动
#define CU_StateMachine_WaitHead        2//等待头数据
#define CU_StateMachine_CheckHeadData   3//校验头数据
#define CU_StateMachine_EraseS1         4//擦除第1步
#define CU_StateMachine_EraseS2         5//擦除第2步
#define CU_StateMachine_WriteS1         6//写入第1步
#define CU_StateMachine_WriteS2         7//写入第2步
#define CU_StateMachine_CheckCodeDataS1 8//校验代码数据第1步
#define CU_StateMachine_CheckCodeDataS2 9//校验代码数据第2步
#define CU_StateMachine_SUCCESS         10//成功
#define CU_StateMachine_Error           20//错误状态
static u8  CU_StateMachine;

static CU_StatuEnum CU_Errflag;//错误号
static u32 CU_OprCount;//操作计数(用于读写字节量计数)
static u32 CU_OprMax;//用于单次读写最大值
static u32 CU_CheckSum;//代码校验
static u32 CU_OprAddress;//操作地址
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的系统函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
函数名: FUN_CodeUpdate_Config
功   能: 配置
参   数:
       CUCnfType *fttp:初始化配置结构体指针
返回值:
     0：配置成功
 小于0：配置失败

注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2018-08-08
-------------------------------------------------*/
s8 FUN_CodeUpdate_Config(CUCnfType *cucp)
{
     u8 u8temp,u8temp2;
     
     //清空表
     for(u8temp = 0;u8temp < CodeUpdate_CodeTypeNum;u8temp++)
     {
          FileTypeLab[u8temp].Size = 0;//清空表
     }
     //加载表
     for(u8temp = 0;u8temp < CodeUpdate_CodeTypeNum;u8temp++)
     {          
          u8 *ps = cucp->FileTabP[u8temp].pmp;//项目类型首地址
          u8 *ds = cucp->FileTabP[u8temp].dmp;//设备类型首地址

          if(cucp->FileTabP[u8temp].Size)
          {               
               u8temp2 = CodeUpdate_GetSLen(ps);//判断项目字符串字节数
               if(u8temp2 != ProjectModeSize)
               {
                    return -1;//非法项目字符串
               }

               u8temp2 = CodeUpdate_GetSLen(ds);//判断设备型号字符串字节数
               if((u8temp2 >= DeviceModeSize) || (u8temp2 == 0))
               {
                    return -2;//非法设备类型字符串
               }

               //保存项目类型表
               for(u8temp2 = 0;u8temp2 < ProjectModeSize;u8temp2++)
               {
                    FileTypeLab[u8temp].ProjectMode[u8temp2] = ps[u8temp2];
               }

               //保存设备类型表
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
     
     //设置回调函数指针组
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
函数名: FUN_CodeUpdate_RealTask
功   能: 升级实时线程
参   数:
       无
返回值:
       无
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
修   改:
       【1】、增加了擦除时返回擦除空间的类型 YJX_20220713
时   间:2018-08-08   
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
//---------------等待头数据操作
          case CU_StateMachine_WaitScanf://等待头数据 
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
          case CU_StateMachine_WaitHead://等待头数据获取完成
               s32temp = CallFpA.NonBlock_CheckGetHead();
               if(s32temp >= FileHeaderSize)
               {
                    fileheader.File_Length = 0;//长度字节数清0
                    //头数据解码
                    CodeUpdate_FileHeaderDecode(&fileheader,(vu8 *)CU_OprAddress);
                    if(CodeUpdate_CheckHeaderValid(&fileheader,(vu8 *)CU_OprAddress) < 0)
                    {//头数据无效
                         CU_Errflag = HeadDataError;
                         CU_StateMachine = CU_StateMachine_Error;
                    }
                    else
                    {//头数据有效
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
//---------------擦除操作               
          case CU_StateMachine_EraseS1://擦除操作第1步
               s8temp = CallFpA.NonBlock_StartErase(fileheader.datatype,fileheader.Start_address + CU_OprCount,CU_OprMax);
               if(s8temp == 0)
               {//启动擦除OK    
                    CU_StateMachine = CU_StateMachine_EraseS2;
               }
               else
               {//启动擦除异常
                    CU_Errflag = EraseError;
                    CU_StateMachine = CU_StateMachine_Error;
               }
               break;
          case CU_StateMachine_EraseS2://擦除操作第2步
               s32temp = CallFpA.NonBlock_CheckErase();
               if(s32temp >= 0)
               {//擦除完成                    
                    CU_OprCount += s32temp;  
                    if(CU_OprCount < fileheader.File_Length)
                    {//擦除未完成
                         //计算下一次擦除的字节数
                         CU_OprMax = CU_EraseByteNum;
                         if((fileheader.File_Length - CU_OprCount) < CU_EraseByteNum)
                         {
                              CU_OprMax = (fileheader.File_Length - CU_OprCount);
                         }
                         CU_StateMachine = CU_StateMachine_EraseS1;
                    }
                    else
                    {//擦除完成
                         CU_OprCount = 0;
                         CU_OprMax = CU_ReadWriteByteNum;
                         CU_StateMachine = CU_StateMachine_WriteS1;
                    }
               }
               else if(s32temp < -1)
               {//擦除出错 或 擦除超时   
                    CU_Errflag = EraseError;
                    CU_StateMachine = CU_StateMachine_Error;
               }
               break;

//---------------写数据操作
          case CU_StateMachine_WriteS1://写操作第1步
               s8temp = CallFpA.NonBlock_StartWrite(fileheader.datatype,(fileheader.Start_address + CU_OprCount),CU_OprMax);
               if(s8temp)
               {
                    CU_Errflag = WriteError;
                    CU_StateMachine = CU_StateMachine_Error;
               }
               else
               {//启动写成功
                    CU_StateMachine = CU_StateMachine_WriteS2;                    
               }
               break;
          case CU_StateMachine_WriteS2:
               s32temp = CallFpA.NonBlock_CheckWrite();
               if(s32temp >= 0)
               {//写完成
                    CU_OprCount += s32temp;                    
                    if(CU_OprCount < fileheader.File_Length)
                    {//升级未完成
                         //计算下一次单写的字节数
                         CU_OprMax = CU_ReadWriteByteNum;
                         if((fileheader.File_Length - CU_OprCount) < CU_ReadWriteByteNum)
                         {
                              CU_OprMax = (fileheader.File_Length - CU_OprCount);
                         }
                         CU_StateMachine = CU_StateMachine_WriteS1;
                         u8temp = ((u8)((CU_OprCount * 100.0) / fileheader.File_Length));
                    }
                    else
                    {//升级完成
                         CU_CheckSum = 0;
                         CU_OprCount = 0;
                         CU_OprMax = CU_ReadWriteByteNum;//CU_DataReceBufferMaxSize;
                         CU_StateMachine = CU_StateMachine_CheckCodeDataS1;
                         u8temp = 100;
                    }
                    CallFpA.StatuFp(WriteProgress,u8temp);
               }
               else if(s32temp < -1)
               {//写入异常
                    CU_Errflag = WriteError;
                    CU_StateMachine = CU_StateMachine_Error;
               }
               break;
               
//---------------校验数据操作               
          case CU_StateMachine_CheckCodeDataS1:
               s8temp = CallFpA.NonBlock_StartRead(fileheader.datatype,(fileheader.Start_address + CU_OprCount),CU_OprMax,&CU_OprAddress);
               if(s8temp == 0)
               {//启动读成功
                    CU_StateMachine = CU_StateMachine_CheckCodeDataS2;
               }
               else
               {//启动读失败     
                    CU_Errflag = ReadError;
                    CU_StateMachine = CU_StateMachine_Error;
               }
               break;
          case CU_StateMachine_CheckCodeDataS2:
               s32temp = CallFpA.NonBlock_CheckRead();
               if(s32temp >= 0)
               {//读完成 
                    if(CU_OprCount < (fileheader.File_Length - CHECKSUM_SIZE))
                    {//读取未完成
                         CU_OprCount += s32temp; 
                         for(u16temp = 0;u16temp < s32temp;u16temp++)
                         {
                              CU_CheckSum += ((u8 *)CU_OprAddress)[u16temp];
                         }
                    
                         //计算下一次单读的字节数
                         CU_OprMax = CU_ReadWriteByteNum;//CU_DataReceBufferMaxSize;
                         if((fileheader.File_Length - CHECKSUM_SIZE - CU_OprCount) < CU_ReadWriteByteNum)//CU_DataReceBufferMaxSize)
                         {
                              CU_OprMax = (fileheader.File_Length - CHECKSUM_SIZE - CU_OprCount);
                         }
                         
                         if(CU_OprMax == 0)
                         {//读取校验码
                              CU_OprMax = CHECKSUM_SIZE;
                         }
                         
                         CU_StateMachine = CU_StateMachine_CheckCodeDataS1;
                    }
                    else
                    {//读取完成
                         u32temp = *(u32 *)CU_OprAddress;//CU_DataReceBuffer;//代码中的校验码
                         if(CU_CheckSum == u32temp)
                         {//相等，代码OK
                              if(fileheader.downloadcontrol == 0)
                              {
                                   CU_StateMachine = CU_StateMachine_SUCCESS;
                              }
                              else
                              {//还有代码文件需要升级                                   
                                   CU_StateMachine = CU_StateMachine_WaitScanf;
                                   CallFpA.StatuFp(ReStart,0);
                              }
                         }
                         else
                         {//代码升级异常                              
                              CU_Errflag = CodeCheckError;
                              CU_StateMachine = CU_StateMachine_Error;
                         }
                    }
               }
               else if(s32temp < -1)
               {//读异常                    
                    CU_StateMachine = CU_StateMachine_Error;
                    CU_Errflag = ReadError;
               }
               break;









          case CU_StateMachine_SUCCESS:
               CU_StateMachine = CU_StateMachine_IDE;//注意顺序不要对调，下面事件语句可能会改变状态值
               CallFpA.StatuFp(UpdateSuccess,0);
               break;
          case CU_StateMachine_Error:
               CU_StateMachine = CU_StateMachine_IDE;//注意顺序不要对调，下面事件语句可能会改变状态值
               CallFpA.StatuFp(CU_Errflag,0);
               break;
          default:
               FUN_CodeUpdate_Start();
               break;
     }
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的用户函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
函数名: FUN_CodeUpdate_Start
功   能: 代码更新 启动
参   数:
       无
返回值:
       无
注   意:
       开始时调用
示   例:
作   者:YJX
版   本:V1.0
时   间:2018-08-08   
-------------------------------------------------*/
void FUN_CodeUpdate_Start(void)
{
     CU_StateMachine = CU_StateMachine_WaitScanf;
}

/*-------------------------------------------------
函数名: FUN_CodeUpdate_Stop
功   能: 代码更新 停止
参   数:
       无
返回值:
       无
注   意:
       想要提前结束代码升级时，调用
示   例:
作   者:YJX
版   本:V1.0
时   间:2018-08-08   
-------------------------------------------------*/
void FUN_CodeUpdate_Stop(void)
{
     CU_StateMachine = CU_StateMachine_IDE;
}


/*-------------------------------------------------
函数名: FUN_CodeUpdate_CheckCodeIsValid
功   能: 判断 指定的代码段是否有效
参   数:
     u8 *dp:代码数据首地址
     u16 dl:代码数据字节数
     u8 flag:校验状态(1:开始校验 2:继续校验 3:最后一包校验 0:单包校验)
返回值:
     不是最后一包 返回值无意义
     返回非0表示校验失败 0表示校验成功
注   意:
     当代码字节数较大需要分多次包校验时，flag取1,2,3
       首包开始校验时，flag =1
       中间几包，则继续校验flag=2
       当最后一个包校验时，flag =3
     当代码字节数只用一次就可以校验完成时，flag取0
示   例:
作   者:YJX
版   本:V1.0
版   本:V1.1
       【1】、改为阻塞型读取数据，这样，即使上层没有运行RealTask，也可以正常判断 YJX_20220714
时   间:2018-08-08   
-------------------------------------------------*/
u32 FUN_CodeUpdate_CheckCodeIsValid(u8 *dp,u16 dl,u8 flag)
{
     static u32 u32result = 0;
     static u32 u32count = 0;//计数，至少要大于4字节
     static u32 lastdata;//最后4个字节数据
     u16 u16temp;
     
     if((flag == 1) || (flag ==0))
     {
          u32result = 0;
          u32count = 0;//计数清0
          lastdata = 0;//最后4个字节数据清0
     }

     u32count += dl;
     for(u16temp = 0;u16temp < dl;u16temp++)
     {
          u32result += dp[u16temp];//累加
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
               //最后4字节改为小端
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
*               ----------------------------------以下是模块的内部函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
//返回字符串字节数，不含\0
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

//文件头信息解码
//解码成功，返回0
//解码失败，返回小于0
static s8 CodeUpdate_FileHeaderDecode(FILE_HEADER *p,vu8 *dp)
{
     vu16 i = 0;
     u8 u8temp;
     // 读取16字节的命令
     for(u8temp = 0;u8temp < CommandSize;u8temp++) 
     {
          p->Command[u8temp] = dp[i++];
     }                       
     // 读待下载程序的设备类型: "BLEGateWay"     
     //VIR_DMACOM1_Send((u8 *)&dp[i],4);
     for(u8temp = 0;u8temp < DeviceModeSize;u8temp++) 
     {
          p->DeviceMode[u8temp] = dp[i++];
     }
     // 读待下载程序的项目类型: "B25 "     
     //VIR_DMACOM1_Send((u8 *)&dp[i],4);
     for(u8temp = 0;u8temp < ProjectModeSize;u8temp++) 
     {
          p->machinetype[u8temp] = dp[i++];
     }     
     // 读下载程序的文件类型:程序/Boot/字库1/字库2... 
     p->datatype= *(u8 *)(&dp[i++]);
     // 读下载完后是否继续下载控制字
     p->downloadcontrol = *(u8 *)(&dp[i++]);
     // 读待写入程序的起始地址
     p->Start_address = *(u32 *)(&dp[i]);
     i += 4;
     // 读待写入程序的长度
     p->File_Length = *(u32 *)(&dp[i]);
     i += 4;
     // 读文件头校验码
     p->Checksum = *(u16 *)(&dp[i]);
     return 0;
}

//下载文件头信息校验
//检验通过，返回0
//检验失败，返回小于0
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
          //------项目型号检测
          if(u8ok)
          {
               for(i = 0;i < ProjectModeSize;i++)
               {
                    if(p->machinetype[i] != FileTypeLab[u8temp].ProjectMode[i])
                    {//项目型号检测错误
                         //DRI_USART0_SendSTR("机型错误!");
                         u8ok = 0;
                         break;
                    }
               }
          }
          //------设备型号检测
          if(u8ok)
          {
               for(i = 0;i < DeviceModeSize;i++)
               {
                    if(p->DeviceMode[i] != FileTypeLab[u8temp].DeviceMode[i])
                    {//设备型号检测错误
                         //DRI_USART0_SendSTR("机型错误!");
                         u8ok = 0;
                         break;
                    }
                    if(FileTypeLab[u8temp].DeviceMode[i] == '\0')
                    {//比较结束
                         break;
                    }
               }
          }
          //------判断文件类型
          if(u8ok)
          {
               if(p->datatype != FileTypeLab[u8temp].FileTypeCode)
               {
                    //VIR_DMACOM1_SendSTR("222");
                    u8ok = 0;
               }
          }
          //------判断加载地址
          if(u8ok)
          {
               if(p->Start_address != FileTypeLab[u8temp].Address)
               {
                    //VIR_DMACOM1_SendSTR("333");
                    u8ok = 0;
               }
          }
          //------判断字节数
          if(u8ok)
          {
               if(p->File_Length != FileTypeLab[u8temp].Size)
               {
                    //VIR_DMACOM1_SendSTR("444");
                    u8ok = 0;
               }
          }
          //------校验和的检查
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


