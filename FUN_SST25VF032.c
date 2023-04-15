/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------以下是模块的修改记录区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * 内容：
 * 日期：2015-03-16
 * 作者：YF
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

       
*/
#include "FUN_SST25VF032.h"
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下模块的对接函数区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的宏定义区------------------------------------                                  *
*                                                                                                                                           *
********************************************************************************************************************************************/
#define SSTMinSectionSize     4096//可擦除写的最小扇区字节数
#define SSTMaxSize            0x400000//芯片最大空间字节数
//芯片命令码
#define READ				0x03    // SPI Flash opcode: Read up up to 25MHz
#define READ_FAST			0x0B    // SPI Flash opcode: Read up to 80MHz with 1 dummy byte
#define ERASE_4K			0x20    // SPI Flash opcode: 4KByte sector erase
#define ERASE_32K			0x52    // SPI Flash opcode: 32KByte block erase
#define ERASE_64K    		0xD8    // SPI Flash opcode: 64KByte block erase
#define ERASE_ALL			0x60    // SPI Flash opcode: Entire chip erase
#define WRITE				0x02    // SPI Flash opcode: Write one byte (or a page of up to 256 bytes, depending on device)
#define WRITE_WORD_STREAM	0xAD    // SPI Flash opcode: Write continuous stream of 16-bit words (AAI mode); available on SST25VF016B (but not on SST25VF010A)
#define RDSR				0x05    // SPI Flash opcode: Read Status Register
#define EWSR				0x50    // SPI Flash opcode: Enable Write Status Register
#define WRSR				0x01    // SPI Flash opcode: Write Status Register
#define WREN				0x06    // SPI Flash opcode: Write Enable
#define WRDI				0x04    // SPI Flash opcode: Write Disable / End AAI mode
#define RDID				0x90    // SPI Flash opcode: Read ID
#define JEDEC_ID			0x9f    // SPI Flash opcode: Read JEDEC ID
#define EBSY				0x70    // SPI Flash opcode: Enable write BUSY status on SO pin
#define DBSY				0x80    // SPI Flash opcode: Disable write BUSY status on SO pin
//芯片状态掩码
#define BUSY                  0x01    // Mask for Status Register BUSY (Internal Write Operaiton in Progress status) bit
#define WEL                   0x02    // Mask for Status Register WEL (Write Enable status) bit
#define BP0                   0x04    // Mask for Status Register BP0 (Block Protect 0) bit
#define BP1                   0x08    // Mask for Status Register BP1 (Block Protect 1) bit
#define BP2                   0x10    // Mask for Status Register BP2 (Block Protect 2) bit
#define BP3                   0x20    // Mask for Status Register BP3 (Block Protect 3) bit
#define AAI                   0x40    // Mask for Status Register AAI (Auto Address Increment Programming status) bit
#define BPL                   0x80    // Mask for Status Register BPL (BPx block protect bit read-only protect) bit

//
#define SPI_CS_Disable        dp->CS1CallFunP
#define SPI_CS_Enable         dp->CS0CallFunP
#define SPI_ReadWrite         dp->ReadWriteCallFunP
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的变量类型定义区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数申明区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static s32 SST25VF032_ParaCheck(u32 add,u16 sl);
static void SST25VF032_WaitIDE(SST25VF032DevicePort *dp);
static s8 SST25VF032_EraseSector(SST25VF032DevicePort *dp,Erase_Type type ,u32 dwAddr);
static s32 SST25VF032_MaxErase4K(SST25VF032DevicePort *dp,u32 eadd,u16 el);

static s8 SST25VF032_SectionEraseWrite(SST25VF032DevicePort *dp,u32 wadd,u8 *wd,u16 wl);

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的变量申明和宏定义区--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/


/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的系统函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/


/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的用户函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
函数名:FUN_SST25VF032_ReadData
功   能:阻塞型 SST25VF032 读数据函数
参   数:
    SST25VF032DevicePort *dp:设备端口
       u32 dwAddr:读取首地址
       u8 *vData: 存放空间首地址
       u16 wLength:读取字节数
返回值:
   小于0:表示读取错误
 大等于0:实际读到的字节数
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2022-07-19
-------------------------------------------------*/
s32 FUN_SST25VF032_ReadData(SST25VF032DevicePort *dp,u32 dwAddr, u8 *vData, u16 wLength)
{
     u8 data;
     s32 s32temp;

     s32temp = SST25VF032_ParaCheck(dwAddr,wLength);
     if(s32temp < 0)
     {
          return -1;
     }
     wLength = s32temp;//更新操作长度    

     if(wLength)
     {
          SPI_CS_Enable();// 片选有效 
          SPI_ReadWrite(READ);		// 读指令 
          SPI_ReadWrite(((u8*)&dwAddr)[2]);	// 地址 
          SPI_ReadWrite(((u8*)&dwAddr)[1]);	// 地址 
          SPI_ReadWrite(((u8*)&dwAddr)[0]);	// 地址 
          // 读数据 
          while(wLength)
          {  
               wLength--;
               data = SPI_ReadWrite(0x00);
               *vData = data;
               vData++;
          }
          SPI_CS_Disable();	  // 片选无效 
          SST25VF032_WaitIDE(dp);
     }
     return s32temp;
}

/*-------------------------------------------------
函数名:FUN_SST25VF032_GetCapacity
功   能:获取芯片容量的函数
参   数:
       SST25VF032DevicePort *dp:设备端口
返回值:
     芯片空间字节数
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2022-07-19
-------------------------------------------------*/
u32 FUN_SST25VF032_GetCapacity(SST25VF032DevicePort *dp)
{
     return(SSTMaxSize);
}

/*-------------------------------------------------
函数名:FUN_SST25VF032_WriteByte
功   能:阻塞型 SST25VF032 写字节函数
参   数:
       SST25VF032DevicePort *dp:设备端口
       u32 dwAddr:写芯片地址
       u8 vData: 待写数据
返回值:
   小于0:写入错误
   等于0:写入成功
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2022-07-19
-------------------------------------------------*/
s8 FUN_SST25VF032_WriteByte(SST25VF032DevicePort *dp,u32 dwAddr,u8 vData)
{
     s32 s32temp;

     s32temp = SST25VF032_ParaCheck(dwAddr,1);
     if(s32temp < 0)
     {
          return -1;
     }
     
     SPI_CS_Enable();    // 片选有效
     SPI_ReadWrite(WREN);// 写使能
     SPI_CS_Disable();   // 片选无效 
     SPI_CS_Enable();    // 片选有效 
     SPI_ReadWrite(WRITE);    // 写指令 
     SPI_ReadWrite(((unsigned char*)&dwAddr)[2]);	// 写入地址 
     SPI_ReadWrite(((unsigned char*)&dwAddr)[1]);	// 写入地址 
     SPI_ReadWrite(((unsigned char*)&dwAddr)[0]);	// 写入地址 
     SPI_ReadWrite(vData);    // 写入数据 
     SPI_CS_Disable();   // 片选无效 
     SST25VF032_WaitIDE(dp);
     
     return 0;
}

/*-------------------------------------------------
函数名:FUN_SST25VF032_WriteData
功   能:阻塞型 SST25VF032 写数据函数
参   数:
       SST25VF032DevicePort *dp:设备端口
       u32 dwAddr:写芯片首地址
       u8 *vData: 待写数据首地址
       u16 wLen:待写字节数
返回值:
   小于0:表示写错误
 大等于0:实际写的字节数
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2022-07-19
-------------------------------------------------*/
s32 FUN_SST25VF032_WriteData(SST25VF032DevicePort *dp,u32 dwAddr, u8 *vData, u16 wLen)
{
     u16 startnum;
     u16 pagenum,i;
     s32 s32temp;

     s32temp = SST25VF032_ParaCheck(dwAddr,wLen);
     if(s32temp < 0)
     {
          return -1;
     }
     wLen = s32temp;//更新操作字节数


     startnum = dwAddr % 256;			/* 写入地址不整页 */
     if(startnum)
     {
          startnum = 256 - startnum;
     }

/* 写入开始的不整页数据 */
     if(startnum)
     {
          SPI_CS_Enable();	  /* 片选有效 */
          SPI_ReadWrite(WREN);			/* 写使能 */
          SPI_CS_Disable();	  /* 片选无效 */
          SPI_CS_Enable();	  /* 片选有效 */
          SPI_ReadWrite(WRITE);		/* 写指令 */
          SPI_ReadWrite(((u8 *)&dwAddr)[2]);	/* 写入地址 */
          SPI_ReadWrite(((u8 *)&dwAddr)[1]);	/* 写入地址 */
          SPI_ReadWrite(((u8 *)&dwAddr)[0]);	/* 写入地址 */
          // Write data
          while((startnum) && wLen)
          {
               SPI_ReadWrite(*vData);		/* 写入数据 */
               dwAddr++;
               vData++;	
               startnum--;
               wLen--;
          }
          SPI_CS_Disable();	  /* 片选无效 */
          SST25VF032_WaitIDE(dp);
     }

/* 整页写入 */
     pagenum = wLen/256;
     while(pagenum)
     {
          SPI_CS_Enable();	  /* 片选有效 */
          SPI_ReadWrite(WREN);			/* 写使能 */
          SPI_CS_Disable();	  /* 片选无效 */
          SPI_CS_Enable();	  /* 片选有效 */
          SPI_ReadWrite(WRITE);		/* 写指令 */
          SPI_ReadWrite(((u8 *)&dwAddr)[2]);	/* 写入地址 */
          SPI_ReadWrite(((u8 *)&dwAddr)[1]);	/* 写入地址 */
          SPI_ReadWrite(((u8 *)&dwAddr)[0]);	/* 写入地址 */
          // Write data
          i = 256;
          while(i)
          {
               SPI_ReadWrite(*vData);		/* 写入数据 */
               dwAddr++;
               vData++;	
               i--;
               wLen--;
          }
          SPI_CS_Disable();	  /* 片选无效 */
          SST25VF032_WaitIDE(dp);
          pagenum--;
     }

/* 尾部不整数页 */
     if(wLen)
     {
          SPI_CS_Enable();	  /* 片选有效 */
          SPI_ReadWrite(WREN);			/* 写使能 */
          SPI_CS_Disable();	  /* 片选无效 */
          SPI_CS_Enable();	  /* 片选有效 */
          SPI_ReadWrite(WRITE);		/* 写指令 */
          SPI_ReadWrite(((u8 *)&dwAddr)[2]);	/* 写入地址 */
          SPI_ReadWrite(((u8 *)&dwAddr)[1]);	/* 写入地址 */
          SPI_ReadWrite(((u8 *)&dwAddr)[0]);	/* 写入地址 */
          // Write data
          while(wLen)
          {
               SPI_ReadWrite(*vData);		/* 写入数据 */
               dwAddr++;
               vData++;	
               wLen--;
          }
          SPI_CS_Disable();	  /* 片选无效 */
          SST25VF032_WaitIDE(dp);
     }
     return s32temp;
}

/*-------------------------------------------------
函数名:FUN_SST25VF032_WriteData2
功   能:阻塞型 SST25VF032 擦除和写数据函数
参   数:
       SST25VF032DevicePort *dp:设备端口
       u32 dwAddr:写芯片首地址
       u8 *vData: 待写数据首地址
       u16 wLen:待写字节数
返回值:
   小于0:表示写错误
 大等于0:实际写的字节数
注   意:
       该函数执行较慢，原因是因为过程带有擦除
示   例:
作   者:YJX
版   本:V1.0
时   间:2022-07-20
-------------------------------------------------*/
s32 FUN_SST25VF032_WriteData2(SST25VF032DevicePort *dp,u32 dwAddr, u8 *vData, u16 wLen)
{
     u8 SSTOprBuffer[SSTMinSectionSize];
     u16 u16size,u16i;
     s16 s16temp;
     u32 SecAdd;//扇区地址
     s32 s32temp;

     s32temp = SST25VF032_ParaCheck(dwAddr,wLen);
     if(s32temp < 0)
     {
          return -1;
     }
     wLen = s32temp;//更新操作字节数

     if(wLen == 0)
     {
          return 0;
     }



//非扇区对齐
     s16temp = (dwAddr % SSTMinSectionSize);
     if(s16temp)
     {    
          //确认扇区地址
          SecAdd = (dwAddr - s16temp);
          //读取原数据
          FUN_SST25VF032_ReadData(dp,SecAdd,SSTOprBuffer,SSTMinSectionSize);//读取数据
          //统计替换数据字节数
          u16size = (SSTMinSectionSize - s16temp);
          if(u16size > wLen)
          {
               u16size = wLen;
          }
          //开始替换数据
          for(u16i = 0;u16i < u16size;u16i++)
          {
               SSTOprBuffer[s16temp + u16i] = vData[u16i];
          }
          //擦除并写入
          SST25VF032_SectionEraseWrite(dp,SecAdd,SSTOprBuffer,SSTMinSectionSize);
          //更新信息
          wLen -= u16size;
          dwAddr += u16size;
          vData += u16size;
     }

//扇区对齐
     if(wLen)
     {
          while(wLen >= SSTMinSectionSize)
          {
               SST25VF032_SectionEraseWrite(dp,dwAddr,vData,SSTMinSectionSize);
               //更新信息
               wLen -= SSTMinSectionSize;
               dwAddr += SSTMinSectionSize;
               vData += SSTMinSectionSize;
          }
     }

//扇区对齐_结束
     if(wLen)
     {
          //读取原数据
          FUN_SST25VF032_ReadData(dp,dwAddr,SSTOprBuffer,SSTMinSectionSize);//读取数据
          //统计替换数据字节数
          u16size = wLen;
          //开始替换数据
          for(u16i = 0;u16i < u16size;u16i++)
          {
               SSTOprBuffer[u16i] = vData[u16i];
          }
          //擦除并写入
          SST25VF032_SectionEraseWrite(dp,dwAddr,SSTOprBuffer,SSTMinSectionSize);
     }
     
     return s32temp;
}

/*-------------------------------------------------
函数名:FUN_SST25VF032_Erase
功   能:阻塞型 SST25VF032 最大4K擦除操作
参   数:
       SST25VF032DevicePort *dp:设备端口
       u32 eadd:擦除起始地址
       u16 el:擦除字节数
返回值:
   小于0:表示擦除错误
  其他值:表示实际擦除字节数
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2022-07-19
-------------------------------------------------*/
s32 FUN_SST25VF032_Erase(SST25VF032DevicePort *dp,u32 eadd,u16 el)
{
     s32 s32temp,s32result;
     
     s32temp = SST25VF032_ParaCheck(eadd,el);
     if(s32temp < 0)
     {
          return -1;
     }
     el = s32temp;//更新操作字节数

     s32result = 0;
     while(el)
     {
          s32temp = SST25VF032_MaxErase4K(dp,eadd,el);
          if(s32temp < 0)
          {//错误
               return s32result;
          }
          el -= s32temp;          
          eadd += s32temp;
          s32result += s32temp;
     }

     return s32result;
}






/*-------------------------------------------------
函数名:FUN_SST25VF032_NonBlock_EraseSector
功   能:非阻塞型 SST25VF032 启动擦除扇区函数
参   数:
       SST25VF032DevicePort *dp:设备端口
       Erase_Type type:擦除扇区类型
       u32 dwAddr:擦除地址所在的扇区
返回值:
   小于0:表示启动擦除错误
   等于0:表示启动擦除成功
注   意:
       该函数是启动擦除dwAddr地址所在的扇区，是擦除整个扇区
       扇区的大小由type决定
示   例:
作   者:YJX
版   本:V1.0
时   间:2022-07-19
-------------------------------------------------*/
s8 FUN_SST25VF032_NonBlock_EraseSector(SST25VF032DevicePort *dp,Erase_Type type ,u32 dwAddr)
{
     u8 command;
     
     if(SST25VF032_ParaCheck(dwAddr,1) < 0)
     {
          return -1;
     }
     
     switch(type)
     {
          case ERASE_4K_TYPE :
               command = ERASE_4K;
               break;
          case ERASE_32K_TYPE :
               command = ERASE_32K;
               break;
          case ERASE_64K_TYPE :
               command = ERASE_64K;
               break;
          default:
               command = ERASE_4K;
               break;
     }
     SPI_CS_Enable();	  /* 片选有效 */
     SPI_ReadWrite(WREN);/* 写使能 */
     SPI_CS_Disable();	  /* 片选无效 */
     SPI_CS_Enable();	  /* 片选有效 */
     SPI_ReadWrite(command);			/* 擦除指令 */
     SPI_ReadWrite(((u8 *)&dwAddr)[2]);	/* 写入地址 */
     SPI_ReadWrite(((u8 *)&dwAddr)[1]);	/* 写入地址 */
     SPI_ReadWrite(((u8 *)&dwAddr)[0]);	/* 写入地址 */
     SPI_CS_Disable();	  /* 片选无效 */
     //SST25VF032_WaitIDE();
     return 0;
}

/*-------------------------------------------------
函数名:FUN_SST25VF032_NonBlock_CheckEraseSector
功   能:非阻塞型 SST25VF032 判断擦除扇区状态
参   数:
       SST25VF032DevicePort *dp:设备端口
返回值:
   小于0:表示擦除错误
   等于0:表示擦除成功
   大于0:表示擦除中
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2022-07-19
-------------------------------------------------*/
s8 FUN_SST25VF032_NonBlock_CheckEraseSector(SST25VF032DevicePort *dp)
{
     u8 Dummy;
     s8 s8temp;
     
     SPI_CS_Enable();	  /* 片选有效 */
     SPI_ReadWrite(RDSR);			/* 读状态 */
     Dummy = SPI_ReadWrite(0x00);
     if(Dummy & BUSY)
     {//忙中
          s8temp = 1;
     }
     else
     {
          s8temp = 0;
     }
     SPI_CS_Disable();	  /* 片选无效 */
     
     return s8temp;
}


/*-------------------------------------------------
函数名:FUN_SST25VF032_EraseChip
功   能:阻塞型 SST25VF032 擦除整片函数
参   数:
       SST25VF032DevicePort *dp:设备端口
返回值:
   小于0:表示擦除错误
   等于0:表示擦除成功
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2022-07-19
-------------------------------------------------*/
s8 FUN_SST25VF032_EraseChip(SST25VF032DevicePort *dp)
{
     SPI_CS_Enable();	  /* 片选有效 */
     SPI_ReadWrite(WREN);			/* 写使能 */
     SPI_CS_Disable();	  /* 片选有效 */
     SPI_CS_Enable();	  /* 片选有效 */
     SPI_ReadWrite(ERASE_ALL);			/* 擦除指令 */
     SPI_CS_Disable();	  /* 片选无效 */
     SST25VF032_WaitIDE(dp);
     return 0;
}

/*-------------------------------------------------
函数名:FUN_SST25VF032_ReadID
功   能:阻塞型 SST25VF032 读芯片ID函数
参   数:
       SST25VF032DevicePort *dp:设备端口
       u8 *id1：厂商ID
       u8 *id2：设备ID
返回值:
   小于0:表示读取错误
   等于0:表示读取成功
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2022-07-19
-------------------------------------------------*/
s8 FUN_SST25VF032_ReadID(SST25VF032DevicePort *dp,u8 *id1, u8 *id2)
{
     SPI_CS_Enable();	  /* 片选有效 */
     SPI_ReadWrite(RDID);	/* 写使能 */
     SPI_ReadWrite(0x00);
     SPI_ReadWrite(0x00);
     SPI_ReadWrite(0x00);	/* dumy */
     *id1 = SPI_ReadWrite(0x00);/* 厂商ID */
     *id2 = SPI_ReadWrite(0x00);/* 设备ID */
     SPI_CS_Disable();	  /* 片选无效 */
	return 0;
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
//判断参数有效性
//返回值 小于0:非法参数 其他值:允许操作字节数
static s32 SST25VF032_ParaCheck(u32 add,u16 sl)
{
     u32 u32ol;//最大允许操作字节数
     if((add >= SSTMaxSize) || (sl > SSTMaxSize))
     {//越界了
          return -1;
     }
     u32ol = (SSTMaxSize - add);//最大允许操作字节数
     if(sl < u32ol)
     {
          u32ol = sl;
     }
     return u32ol;
}

//等待flash操作结束
static void SST25VF032_WaitIDE(SST25VF032DevicePort *dp)
{
     u8 Dummy;
     SPI_CS_Enable();	  /* 片选有效 */
     SPI_ReadWrite(RDSR);			/* 读状态 */

     // Poll the BUSY bit
     do
     {
          Dummy = SPI_ReadWrite(0x00);
     } while(Dummy & BUSY);
     SPI_CS_Disable();	  /* 片选无效 */
}

static s8 SST25VF032_EraseSector(SST25VF032DevicePort *dp,Erase_Type type ,u32 dwAddr)
{
     u8 command;
     switch(type)
     {
          case ERASE_4K_TYPE :
               command = ERASE_4K;
               break;
          case ERASE_32K_TYPE :
               command = ERASE_32K;
               break;
          case ERASE_64K_TYPE :
               command = ERASE_64K;
               break;
          default:
               command = ERASE_4K;
               break;
     }
     SPI_CS_Enable();	  /* 片选有效 */
     SPI_ReadWrite(WREN);/* 写使能 */
     SPI_CS_Disable();	  /* 片选有效 */
     SPI_CS_Enable();	  /* 片选有效 */
     SPI_ReadWrite(command);			/* 擦除指令 */
     SPI_ReadWrite(((u8 *)&dwAddr)[2]);	/* 写入地址 */
     SPI_ReadWrite(((u8 *)&dwAddr)[1]);	/* 写入地址 */
     SPI_ReadWrite(((u8 *)&dwAddr)[0]);	/* 写入地址 */
     SPI_CS_Disable();	  /* 片选无效 */
     SST25VF032_WaitIDE(dp);
     return 0;
}


static s32 SST25VF032_MaxErase4K(SST25VF032DevicePort *dp,u32 eadd,u16 el)
{
     u8 SSTOprBuffer[SSTMinSectionSize];
     u16 u16el,u16offset,u16temp;
     u32 u32add;
     s32 s32temp;
     
     s32temp = SST25VF032_ParaCheck(eadd,el);
     if(s32temp < 0)
     {
          return -1;
     }
     el = s32temp;//更新操作字节数
     
     //准备工作     
     u32add = ((eadd / SSTMinSectionSize) * SSTMinSectionSize);//操作地址4K扇区对齐
     u16offset = (eadd % SSTMinSectionSize);//偏移量
     u16el = (SSTMinSectionSize - u16offset);//最大擦除字节数
     if(el < u16el)
     {
          u16el = el;
     }

     //读取并处理数据
     if(u16el != SSTMinSectionSize)
     {
          //读取数据
          if(FUN_SST25VF032_ReadData(dp,u32add,SSTOprBuffer,SSTMinSectionSize) != SSTMinSectionSize)
          {
               return -2;
          }
          //擦除的地方写0xff
          for(u16temp = 0;u16temp < u16el;u16temp++)
          {
               SSTOprBuffer[u16offset + u16temp] = 0xff;
          }
     }

     //擦除 
     SST25VF032_EraseSector(dp,ERASE_4K_TYPE,u32add);

     //恢复数据
     if(u16el != SSTMinSectionSize)
     {
          if(FUN_SST25VF032_WriteData(dp,u32add,SSTOprBuffer,SSTMinSectionSize) != SSTMinSectionSize)
          {
               return -3;
          }
     }

     return (s16)u16el;
}



//4K扇区的擦除和写入
//返回值：0表示操作成功 小于0表示操作失败
static s8 SST25VF032_SectionEraseWrite(SST25VF032DevicePort *dp,u32 wadd,u8 *wd,u16 wl)
{
     if((wadd % SSTMinSectionSize) || (wl != SSTMinSectionSize))
     {//写入地址不是扇区对齐 或 长度不是扇区尺寸
          return -1;
     }
     SST25VF032_EraseSector(dp,ERASE_4K_TYPE,wadd);//擦除扇区
     FUN_SST25VF032_WriteData(dp,wadd,wd,wl);
     return 0;
}

