/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------以下是模块的修改记录区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * 内容：
 * 日期：2019-04-16
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
          目前模拟U盘写速度:约16K字节/S
          目前模拟U盘读速度:约190K字节/S
*/

#include "FUN_SCSI.h"

#define DeviceTypeCode        0x05 ///* 设备类型: 0x00表示移动设置(U盘)  0x05表示光驱 0x1f表示非移动设备(硬盘)目前不支持 */
#define AddressOffset         2097152//地址偏移，单位字节
#define MAX_LUN_NUM           0    //盘符最大数(0:表示只有1个分区)，最大到15

#define SCSI_Buffer_SIZE      66//4096//66//数值大，缓存大，交换数据就快

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下模块间的对接函数区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
#define USB_SendData(DP,DL)        API_USB_StoresClass_SendData(DP,DL)
#define USB_Stop()                 API_USB_StoresClass_Close()//关闭USB连接
#define IN_ENDP_STALL()            //IN端点挂起//SetEPTxStatus(ENDP1, EP_TX_STALL);
#define OUT_ENDP_STALL()           //OUT端点挂起//SetEPRxStatus(ENDP2, EP_RX_STALL);
#define IN_ENDP_ENABLE()           //IN端点使能//SetEPTxStatus(ENDP1, EP_TX_VALID);
#define OUT_ENDP_ENABLE()          //OUT端点使能//SetEPRxStatus(ENDP2, EP_RX_VALID);/* enable the Endpoint to receive the next cmd*/

#define GetStorageSpace(SP)        API_W25Q32_GetCapacity(SP)
#define ReadStorageData(AD,BP,BL)  API_W25Q32_ReadData(AD+AddressOffset,BP,BL)
#define WriteStorageData(AD,BP,BL) API_W25Q32_WriteData2(AD+AddressOffset,BP,BL)//注意，需要具有擦除功能的写操作

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块间的变量申明区--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/


/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块内的宏定义区------------------------------------                                *
*                                                                                                                                           *
********************************************************************************************************************************************/

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块内的变量类型定义区  与 宏定义 申明区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
typedef struct
{
  u32 dSignature;
  u32 dTag;
  u32 dDataLength;
  u8  bmFlags;
  u8  bLUN;
  u8  bCBLength;
  u8  CB[16];
}Bulk_Only_CBW;
typedef struct
{
  u32 dSignature;
  u32 dTag;
  u32 dDataResidue;
  u8  bStatus;
}Bulk_Only_CSW;


/****** BOT(仅批量传输)状态机 ******/
#define BOT_IDLE                      0       /* Idle state */
#define BOT_DATA_OUT                  1       /* Data Out state */
#define BOT_DATA_IN                   2       /* Data In state */
#define BOT_DATA_IN_LAST              3       /* Last Data In Last */
#define BOT_CSW_Send                  4       /* Command Status Wrapper */
#define BOT_ERROR                     5       /* error state */
static u8 Bot_State;

#define RECEIVE_DECODE        0
#define RECEIVE_DATA          1
static u8 Receive_State;
/****** CBW和CSW数据包参数宏定义 ******/
#define BOT_CBW_SIGNATURE             0x43425355  //CBW命令块识别
#define BOT_CSW_SIGNATURE             0x53425355  //CSW状态块识别
#define BOT_CBW_PACKET_LENGTH         31          //CBW命令包长度，单位字节
#define CSW_DATA_LENGTH               0x000D      //CSW状态包长度，单位字节
#define CSW_CMD_PASSED                0x00
#define CSW_CMD_FAILED                0x01
#define CSW_PHASE_ERROR               0x02

#define SEND_CSW_DISABLE              0
#define SEND_CSW_ENABLE               1
//
#define READ_FORMAT_CAPACITY_DATA_LEN               0x0C
#define READ_CAPACITY10_DATA_LEN                    0x08
#define MODE_SENSE10_DATA_LEN                       0x08
#define MODE_SENSE6_DATA_LEN                        0x04
#define REQUEST_SENSE_DATA_LEN                      0x12
#define STANDARD_INQUIRY_DATA_LEN                   0x24
#define BLKVFY                                      0x04
/****** SCSI命令宏定义 ******/
#define SCSI_TEST_UNIT_READY                        0x00    //测试是否准备好---------------------
#define SCSI_REQUEST_SENSE                          0x03    //主机请求设备返回执行结果，及状态数据
#define SCSI_FORMAT_UNIT                            0x04
#define SCSI_READ6                                  0x08
#define SCSI_WRITE6                                 0x0A
#define SCSI_INQUIRY                                0x12    //索取器件信息(如生产厂家，产品名称，产品版本等)---------------
#define SCSI_MODE_SELECT6                           0x15    //允许host对外部设备设置参数
#define SCSI_MODE_SENSE6                            0x1A    //向host传输参数-----------------
#define SCSI_START_STOP_UNIT                        0x1B    //启动/停止存储单元电源(写保护)
#define SCSI_SEND_DIAGNOSTIC                        0x1D
#define SCSI_ALLOW_MEDIUM_REMOVAL                   0x1E    //禁止/允许存储介质移动
#define SCSI_READ_FORMAT_CAPACITIES                 0x23    //获取设备存储大小，Block(一般为一个扇区大小，默认为512)长度等信息
#define SCSI_READ_CAPACITY10                        0x25    //要求设备返回最后一个块的索引号和块的长度---------------
#define SCSI_READ10                                 0x28    //host从存储介质读取二进制数据---------
#define SCSI_WRITE10                                0x2A    //host向存储介质写入二进制数据
#define SCSI_VERIFY10                               0x2F    //存存储中验证数据
#define SCSI_READTOC                                0x43    //读取光盘目录结构
#define SCSI_MODE_SELECT10                          0x55    //允许host对外部设备设置参数
#define SCSI_MODE_SENSE10                           0x5A    //向host传输参数
#define SCSI_READ16                                 0x88
#define SCSI_WRITE16                                0x8A
#define SCSI_VERIFY16                               0x8F
#define SCSI_READ_CAPACITY16                        0x9E
#define SCSI_REPORT_LUNS                            0xA0    //索取设备的LUN数和LUN清单
#define SCSI_READ12                                 0xA8
#define SCSI_WRITE12                                0xAA
#define SCSI_VERIFY12                               0xAF


#define NO_SENSE		                         0
#define RECOVERED_ERROR		                    1
#define NOT_READY		                         2
#define MEDIUM_ERROR		                    3
#define HARDWARE_ERROR		                    4
#define ILLEGAL_REQUEST		                    5
#define UNIT_ATTENTION		                    6
#define DATA_PROTECT		                    7
#define BLANK_CHECK		                    8
#define VENDOR_SPECIFIC		                    9
#define COPY_ABORTED		                    10
#define ABORTED_COMMAND		                    11
#define VOLUME_OVERFLOW		                    13
#define MISCOMPARE		                         14


#define INVALID_COMMAND                          0x20
#define INVALID_FIELED_IN_COMMAND               0x24
#define PARAMETER_LIST_LENGTH_ERROR             0x1A
#define INVALID_FIELD_IN_PARAMETER_LIST         0x26
#define ADDRESS_OUT_OF_RANGE                    0x21
#define MEDIUM_NOT_PRESENT 			           0x3A
#define MEDIUM_HAVE_CHANGED			           0x28


#define DIR_IN                        0
#define DIR_OUT                       1
#define BOTH_DIR                      2
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数申明区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static void Bot_Abort(u8 Direction);
static void Set_Scsi_Sense_Data(u8 lun,u8 Sens_Key,u8 Asc);
static void Send_CSW_Data(u8 CSW_Status);
static void Send_Zero_Data(u32 zeronum);
static void SCSI_TestUnitReady_Cmd(u8 lun);
static void SCSI_Read_Toc_Cmd(u8 lun);
static void SCSI_Inquiry_Cmd(u8 lun);
static void SCSI_ReadFormatCapacity_Cmd(u8 lun);
static void SCSI_ReadCapacity10_Cmd(u8 lun);
static void SCSI_ModeSense10_Cmd(u8 lun);
static void SCSI_ModeSense6_Cmd(u8 lun);
static void SCSI_Read10_Cmd(u8 lun,u32 LBA,u32 BlockNbr);
static void SCSI_Start_Stop_Unit_Cmd(u8 lun);
static void SCSI_Write10_Cmd(u8 lun,u32 LBA,u32 BlockNbr);//
static void SCSI_RequestSense_Cmd(u8 lun);
static void SCSI_Verify10_Cmd(u8 lun);
static void SCSI_Format_Cmd(u8 lun);
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块内的 变量 与 宏定义 申明区--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
#define STANDARD_INQUIRY_DATA_LEN                   0x24

//static const u8 Page00_Inquiry_Data[] =
//{
//     0x00, /* PERIPHERAL QUALIFIER & PERIPHERAL DEVICE TYPE*/
//     0x00,
//     0x00,
//     0x00,
//     0x00 /* Supported Pages 00*/
//};

// char usbcdrom_inquiry_data[] = {
//                      0x05,                        /*= 5 for CDROM; 0 for usb mass storage */
//                      0x80,                        /*= RMB BYTE is set by inquiry data */
//                      0x02,                        /* ANSI SCSI 2 */
//                      0x00,                        /*= Data format = 0,  */
//                      0x33,                        /*= Additional length   */
//                      0x00, 0x00, 0x00,
//                      /* Manufacturer = SOMwireless.com */
//                      'S', 'O', 'M', 'w', 'i', 'r', 'e', 'l', 'e', 's', 's', '.', 'c', 'o', 'm', 0x20,
//                      /* Product = ISO9660 Image */
//                      0x49, 0x6d, 0x61, 0x67, 0x65, 0x20, 0x20, 0x20,
//                      /* Revision = 2.31 for new transparent command */
//                      0x32, 0x2E, 0x33, 0x31, 
//                      0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,  
//                      0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
//                      0x20,0x20,0x20,0x20
//                   };

static const u8 Standard_Inquiry_Data[] =
{
     DeviceTypeCode,     
     0x80,          /* RMB = 1: Removable Medium */
     0x02,          /* Version: No conformance claim to standard */
     0x02,
     36 - 4,          /* Additional Length */
     0x00,          /* SCCS = 1: Storage Controller Component */
     0x00,
     0x00,
     /* 产商名称 */
     'S', 't', 'a', 'r', 'm', 'a', 'c', 'h',' ',
     /* 产品识别名 */
     'F', 'l', 'a', 's', 'h', 'D', 'i', 's', 'k',' ',
     ' ', ' ', ' ', ' ', ' ', ' ', 
     /* 产品版本号 */
     '1', '.', '0'
};

static const u8 Standard_Inquiry_Data2[] =
{
     0x00,          /* Direct Access Device */
     0x80,          /* RMB = 1: Removable Medium */
     0x02,          /* Version: No conformance claim to standard */
     0x02,
     36 - 4,          /* Additional Length */
     0x00,          /* SCCS = 1: Storage Controller Component */
     0x00,
     0x00,
     /* Vendor Identification */
     'S', 't', 'a', 'r', 'm', 'a', 'c', 'h',' ',
     /* Product Identification */
     'N', 'A', 'N', 'D', ' ', 'F', 'l', 'a', 's', 'h', ' ',
     'D', 'i', 's', 'k', ' ',
     /* Product Revision Level */
     '1', '.', '0'
};

static const u8 Mode_Sense6_data[] =
{
     0x03,
     0x00,
     0x00,
     0x00,
};

static const u8 Mode_Sense10_data[] =
{
     0x00,
     0x06,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00,
     0x00
};

/*
MODE_SENSE6_CMD需要返回的数据结构如下：
                   struct mode_sense6_data
                    {
                        char  length;               //sizeof( struct mode_sense6_data )
                        char  media_type;           //0x05 for CDROM, 0x00 for U disk
                        char  device_specific;      //0x00
                        char  block_descriptor_len; //sizeof( block_last ) + sizeof( block_len )
                        unsigned long block_last;   //the last block number
                        unsigned long block_len;    //in bytes
                    } ;
*/
//static const u8 usbcdrom_Mode_Sense6_data[] = 
//{
//     
//};

/*
MODE_SENSE10_CMD需要返回的数据结构如下：
                   struct mode_sense10_data
                    {
                       unsigned short  length;                    //sizeof( struct mode_sense10_data )
                       char  media_type;                          //0x05 for CDROM, 0x00 for U disk
                       char  write_protect;                       //0x00
                       unsigned short  reserved;                  //0x0000
                       unsigned short  block_descriptor_len;      //sizeof( block_last ) + sizeof( block_len )
                       unsigned long block_last;                  //the last block number
                       unsigned long block_len;                   //in bytes

                   } ;

*/
//static u8 usbcdrom_Mode_Sense10_data[] = 
//{
//     0x00,0x10,                 //整个数组大小，单位字节
//     0x05,                       //光驱类型
//     0x00,                       //写保护
//     0x00,0x00,                 //预留
//     0x00,0x08,                 //下面块描述长度
//     0x00,0x00,0x00,0x00,     //最后的块号
//     0x00,0x00,0x00,0x00      //块尺寸，单位字节，此处需要为512
//};

static const u8 usbcdrom_read_toc_data[] = //虚拟光驱专用，用于返回光盘的目录结构数据
{
     0x00,0x12,                  /* 下面的字节数 */
     0x01,                        /* first track */
     0x01,                        /* last track */
     
     0x00,                        //(预留)
     0x14,                        //(adr, control)
     0x01,                        //(track being described) 
     0x00,                        //(预留)
     0x00,0x00,0x00,0x00,       // (启动逻辑块地址:0)---是不是自启动???
     
     0x00,                         //(预留)
     0x14,                         //(adr, control)
     0xAA,                         //(track being described (leadout))
     0x00,                         //(预留)
     0x00,0x00,0xA2,0x8A         //(start logical block address 41610)
};

static u8 Scsi_Sense_Data[] =//此处不可改为常量类型
{
     0x70, /*RespCode*/
     0x00, /*SegmentNumber*/
     NO_SENSE, /* Sens_Key*/
     0x00,
     0x00,
     0x00,
     0x00, /*Information*/
     0x0A, /*AdditionalSenseLength*/
     0x00,
     0x00,
     0x00,
     0x00, /*CmdInformation*/
     NO_SENSE, /*Asc*/
     0x00, /*ASCQ*/
     0x00, /*FRUC*/
     0x00, /*TBD*/
     0x00,
     0x00 /*SenseKeySpecific*/
};

static u8 ReadFormatCapacity_Data[] =//此处不可改为常量类型
{
     0x00,
     0x00,
     0x00,
     0x08, /* Capacity List Length */

     /* Block Count */
     0,
     0,
     0,
     0,

     /* Block Length */
     0x02,/* Descriptor Code: Formatted Media */
     0,
     0,
     0
};

/*
在ReadCapacity10_Data中
最后一个逻辑块号计算方法:
假设，存储设备的空间大小为sps字节,块尺寸为CDROM_BLOCK = 512
那么，块数量block_num = (sps + 512 - 1) / 512
注意，上面的512即为块尺寸，即扇区大小，单位是字节
因为，在SCSI系统中，块总是从0开始编号的，那么最后一个块号block_last = block_num - 1
请注意：SCSI系统采用大端(big endian)，通常ARM编译器都将代码编译成小端(little endian)，需要把block_last和CDROM_BLOCK变成大端。
USBSDMS_READ_CAPACITY_CMD需要返回的数据结构如下：
                   struct read_cap_data
                    {
                       unsigned long block_last;  //the last block number
                       unsigned long block_len;   //in bytes
                    } ;
*/
static u8 ReadCapacity10_Data[] =//此处不可改为常量类型
{
     /* 最后一个逻辑块号(逻辑块从0开始) */
     0,
     0,
     0,
     0,

     /* 逻辑块尺寸，即长度，单位字节，即扇区大小，即为512 */
     0,
     0,
     0,
     0
};


#define StorageSectorSize     512//存储设备的扇区大小，单位字节
static u32 Mass_Block_Size;     //块大小，单位字节
static u32 Mass_Block_Count;    //块数量

static Bulk_Only_CBW CBW;
static Bulk_Only_CSW CSW;
static u32 SCSI_LBA,SCSI_BlkLen;//逻辑块地址，块传输数量


static u8 SCSI_Buffer[SCSI_Buffer_SIZE];
static u32 SCSI_Buffer_WIntex;

static u32 OUTWriteAddress,OUTWriteSize;

u8 sflag = 0;
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的系统函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
函数名: FUN_SCSI_Config
功  能: SCSI模块的配置函数
参  数:
       无
返回值:
       0:配置成功
   小于0:配置失败    
注  意:
       
示  例:
作  者:YJX
版  本:V1.0
时  间:2019-05-29  
-------------------------------------------------*/
s8 FUN_SCSI_Config(void)
{     
     Bot_State = Bot_State;//防止编译警告
     Bot_State = BOT_IDLE;
     Receive_State = RECEIVE_DECODE;//解码状态
     SCSI_Buffer_WIntex = 0;//缓存写索引号从0开始
     
     return 0;
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的用户函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
u8 FUN_SCSI_GetMaxLUNNum(void)
{//最大分区数，用来确认设备支持的逻辑单元数。MaxLUN的值必须是0~15.注意:LUN是从0开始的
 //主机不能向不存在的LUN发送CBW
     return MAX_LUN_NUM;
}

s32 FUN_SCSI_ReceiveDataProcess(u8 *dap,u32 dal)
{//
     u32 u32opern;
     u32 u32Counter;
     u32opern = (SCSI_Buffer_SIZE - SCSI_Buffer_WIntex);
     if(dal <= (SCSI_Buffer_SIZE - SCSI_Buffer_WIntex))
     {//数据放得下
          u32opern = dal;
     }
     
     //DRI_UART0_Send_String("1");
     //DRI_UART0_Send_Byte(0x0d);
     //DRI_UART0_Send_Byte(0x0a);
     
     for(u32Counter = 0;u32Counter < u32opern;u32Counter++)
     {
          SCSI_Buffer[SCSI_Buffer_WIntex++] = dap[u32Counter];
     }
 //------ 开始解码CBW数据------
     if(Receive_State == RECEIVE_DECODE)
     {//需要接收到CBW数据包
          //DRI_UART0_Send_String("A");
          //DRI_UART0_Send_Byte(0x0d);
          //DRI_UART0_Send_Byte(0x0a);
          if(SCSI_Buffer_WIntex >= BOT_CBW_PACKET_LENGTH)
          {//数据个数够      
               //DRI_UART0_Send_String("B");
               //DRI_UART0_Send_Byte(0x0d);
               //DRI_UART0_Send_Byte(0x0a);
               SCSI_Buffer_WIntex -= BOT_CBW_PACKET_LENGTH;//更新缓存写索引号 
               for (u32Counter = 0; u32Counter < BOT_CBW_PACKET_LENGTH; u32Counter++)
               {
                    *((u8 *)&CBW + u32Counter) = SCSI_Buffer[u32Counter];
               }
               CSW.dTag = CBW.dTag;
               CSW.dDataResidue = CBW.dDataLength;
               
               if(CBW.dSignature == BOT_CBW_SIGNATURE)
               {//是CBW数据包
                                  
                    //DRI_UART0_Send_String("C");
                    //DRI_UART0_Send_Byte(0x0d);
                    //DRI_UART0_Send_Byte(0x0a);
               
                    if ((CBW.bLUN > MAX_LUN_NUM) || (CBW.bCBLength < 1) || (CBW.bCBLength > 16))
                    {//无效的CBW数据包     
                         //DRI_UART0_Send_String("G");
                         //DRI_UART0_Send_Byte(0x0d);
                         //DRI_UART0_Send_Byte(0x0a);
                         
                         Bot_Abort(BOTH_DIR);//此处需要执行让IN和OUT端点挂起的语句
                         Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
                         //Set_CSW (CSW_CMD_FAILED,SEND_CSW_DISABLE);
                         Send_CSW_Data(CSW_CMD_FAILED);
                    }
                    else
                    {//有效CBW数据包
                         //DRI_UART0_Send_String("是有效CBW数据包!!!");
                         //DRI_UART0_Send_String("D");
                         //DRI_UART0_Send_Byte(0x0d);
                         //DRI_UART0_Send_Byte(0x0a);
                         
                         switch (CBW.CB[0])
                         {
                              case SCSI_TEST_UNIT_READY://查询设备是否准备好
                                   SCSI_TestUnitReady_Cmd(CBW.bLUN);
                                   break;
                              case SCSI_REQUEST_SENSE:
                                   SCSI_RequestSense_Cmd(CBW.bLUN);
                                   break;
                              case SCSI_INQUIRY://索取器件信息
                                   SCSI_Inquiry_Cmd(CBW.bLUN);
                                   break;
                              case SCSI_START_STOP_UNIT://支持弹出的操作
                                   SCSI_Start_Stop_Unit_Cmd(CBW.bLUN);
                                   //delaynms(100);
                                   //PRO_USBCNF_Config(0);//改为打印
                                   break;
                              case SCSI_ALLOW_MEDIUM_REMOVAL:
                                   SCSI_Start_Stop_Unit_Cmd(CBW.bLUN);
                                   break;
                              case SCSI_MODE_SENSE6://向Host传输参数
                                   SCSI_ModeSense6_Cmd(CBW.bLUN);
                                   break;
                              case SCSI_MODE_SENSE10:
                                   SCSI_ModeSense10_Cmd(CBW.bLUN);
                                   break;
                              case SCSI_READ_FORMAT_CAPACITIES://查询当前容量格式
                                   SCSI_ReadFormatCapacity_Cmd(CBW.bLUN);
                                   break;
                              case SCSI_READ_CAPACITY10://要求设备返回当前容量
                                   SCSI_ReadCapacity10_Cmd(CBW.bLUN);
                                   break;
                              case SCSI_READ10://从存储介质读取二进制数据
                                   /* 计算逻辑块地址 */
                                   SCSI_LBA = ((CBW.CB[2] << 24) | (CBW.CB[3] << 16) | (CBW.CB[4] <<  8) | CBW.CB[5]);
                                   /* 计算传输块数 */
                                   SCSI_BlkLen = ((CBW.CB[7] <<  8) | CBW.CB[8]);
                                   SCSI_Read10_Cmd(CBW.bLUN,SCSI_LBA,SCSI_BlkLen);
                                   break;
                              case SCSI_WRITE10://写二进制数据到存储介质
                                   /* 计算逻辑块地址 */
                                   SCSI_LBA = ((CBW.CB[2] << 24) | (CBW.CB[3] << 16) | (CBW.CB[4] <<  8) | CBW.CB[5]);
                                   /* 计算传输块数 */
                                   SCSI_BlkLen = ((CBW.CB[7] <<  8) | CBW.CB[8]);
                                   SCSI_Write10_Cmd(CBW.bLUN,SCSI_LBA,SCSI_BlkLen);
                                   break;
                              case SCSI_VERIFY10:
                                   SCSI_Verify10_Cmd(CBW.bLUN);
                                   break;
                              case SCSI_FORMAT_UNIT:
                                   SCSI_Format_Cmd(CBW.bLUN);
                                   break;
                              case SCSI_READTOC://虚拟光驱专用
                                   SCSI_Read_Toc_Cmd(CBW.bLUN);
                                   break;
                              default:
                                   {
                                        Bot_Abort(BOTH_DIR);
                                        Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_COMMAND);
                                        Send_Zero_Data(CBW.dDataLength);  
                                        Send_CSW_Data(CSW_CMD_FAILED);
                                   }
                                   break;
                         }
                         //DRI_UART0_Send_String("E");
                         //DRI_UART0_Send_Byte(0x0d);
                         //DRI_UART0_Send_Byte(0x0a);
                    }             
               }
               else
               {//无效的CBW数据包
                    //DRI_UART0_Send_String("F");
                    //DRI_UART0_Send_Byte(0x0d);
                    //DRI_UART0_Send_Byte(0x0a);
                    
                    Bot_Abort(BOTH_DIR);
                    Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_COMMAND);
                    //Set_CSW (CSW_CMD_FAILED,SEND_CSW_DISABLE);
                    Send_CSW_Data(CSW_CMD_FAILED);
               }
          }    
     }
     else if(Receive_State == RECEIVE_DATA)
     {//需要接收到数据
          //DRI_UART0_Send_String("2");
          //DRI_UART0_Send_Byte(0x0d);
          //DRI_UART0_Send_Byte(0x0a);
          if(SCSI_Buffer_WIntex)
          {//有数据
               //DRI_UART0_Send_String("3");
               //DRI_UART0_Send_Byte(0x0d);
               //DRI_UART0_Send_Byte(0x0a);
               if(OUTWriteSize)
               {//            
                    //DRI_UART0_Send_String("4");
                    //DRI_UART0_Send_Byte(0x0d);
                    //DRI_UART0_Send_Byte(0x0a);
                    if((SCSI_Buffer_WIntex == OUTWriteSize) || (SCSI_Buffer_WIntex >= SCSI_Buffer_SIZE))
                    {
                         u32 wn = OUTWriteSize;
                         //DRI_UART0_Send_String("5");
                         if(wn > SCSI_Buffer_WIntex)
                         {
                              wn = SCSI_Buffer_WIntex;
                         }
                         WriteStorageData(OUTWriteAddress,SCSI_Buffer,wn);
                         OUTWriteAddress += wn;//更新写地址
                         OUTWriteSize -= wn;//更新写长度，单位字节
                         SCSI_Buffer_WIntex -= wn;//更新缓存写索引号 
                    }
                    //{
                    //     u8 abcde[20];
                    //     FUN_TypeChange_IntToStr(OUTWriteSize,abcde);
                    //     DRI_UART0_Send_String("6剩余字节:");
                    //     DRI_UART0_Send_String(abcde);
                    //     DRI_UART0_Send_String("||||||");
                    //}
                    //DRI_UART0_Send_String("5");
                    //DRI_UART0_Send_Byte(0x0d);
                    //DRI_UART0_Send_Byte(0x0a);
                    if(OUTWriteSize == 0)
                    {
                         //DRI_UART0_Send_String("--------7----------------");
                         //DRI_UART0_Send_Byte(0x0d);
                         //DRI_UART0_Send_Byte(0x0a);
                         Send_CSW_Data(CSW_CMD_PASSED);
                         Receive_State = RECEIVE_DECODE;
                    }
               }
               else
               {
                    //DRI_UART0_Send_String("8");
                    //DRI_UART0_Send_Byte(0x0d);
                    //DRI_UART0_Send_Byte(0x0a);
                    Send_CSW_Data(CSW_CMD_PASSED);
                    Receive_State = RECEIVE_DECODE;
               }
          }
          else if(OUTWriteSize == 0)
          {
               //DRI_UART0_Send_String("9");
               //DRI_UART0_Send_Byte(0x0d);
               //DRI_UART0_Send_Byte(0x0a);
               Send_CSW_Data(CSW_CMD_PASSED);
               Receive_State = RECEIVE_DECODE;
          }
     }
     return (s32)u32opern;
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static void Bot_Abort(u8 Direction)
{
     switch (Direction)
     {
          case DIR_IN://IN端点挂起
               IN_ENDP_STALL();
               break;
          case DIR_OUT://OUT端点挂起
               OUT_ENDP_STALL();
               break;
          case BOTH_DIR://IN 和 OUT端点都挂起
               IN_ENDP_STALL();
               OUT_ENDP_STALL();
               break;
          default:
               break;
     }
}

static void Set_Scsi_Sense_Data(u8 lun,u8 Sens_Key,u8 Asc)
{
     Scsi_Sense_Data[2] = Sens_Key;
     Scsi_Sense_Data[12] = Asc;
}

static void Send_CSW_Data(u8 CSW_Status)
{
     CSW.dSignature = BOT_CSW_SIGNATURE;
     CSW.dTag = CBW.dTag;
     CSW.dDataResidue = 0;//实际传输数据个数和期望传输的数据字节数之差，此处为0表示都传输完成
     CSW.bStatus = CSW_Status;
     USB_SendData(((u8 *)&CSW), CSW_DATA_LENGTH);
}

static void Send_Zero_Data(u32 zeronum)
{
     u8 zeroarr[64];
     u32 sendedbytelenth = 0,sendnum,zerointex;
     for(zerointex = 0;zerointex < 64;zerointex++)
     {
          zeroarr[zerointex] = 0;
     }
     while(zeronum - sendedbytelenth)
     {
          sendnum = (zeronum - sendedbytelenth);
          if(sendnum > 64)
          {
               sendnum = 64;
          }          
          USB_SendData(zeroarr,sendnum);
          sendedbytelenth += sendnum;
     }   
}

//获取失败返回1 获取成功返回0
static u16 MAL_GetStatus (u8 lun)
{//获取存储设备状态
     u32 storagesize;//存储空间     
     if(GetStorageSpace(&storagesize))
     {//获取失败
          return 1;
     }
     Mass_Block_Size = StorageSectorSize;//块大小，即扇区大小固定为512字节
     Mass_Block_Count = (storagesize / Mass_Block_Size);
     return 0;
}

static void Read_Memory(u8 lun,u32 Memory_Offset,u32 Transfer_Length)
{//Memory_Offset为块地址，Transfer_Length为块数量
     u8 rbuff[128];
     u32 raddress = Memory_Offset * Mass_Block_Size;//计算出读取的首地址
     u32 rlenth = Transfer_Length * Mass_Block_Size;//计算出读取的字节数
     u32 rn;

     while(rlenth)
     {
          rn = rlenth;
          if(rn > 128)
          {
               rn = 128;
          }
          ReadStorageData(raddress,rbuff,rn);//读取数据
          USB_SendData(rbuff,rn);//发送读到的数据          
          raddress += rn;//更新读地址
          rlenth -= rn;//更新读长度
     }   
}

static void SCSI_TestUnitReady_Cmd(u8 lun)
{
     if(MAL_GetStatus(lun))
     {
          Set_Scsi_Sense_Data(CBW.bLUN, NOT_READY, MEDIUM_NOT_PRESENT);
          //Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
          Send_CSW_Data(CSW_CMD_FAILED);
          Bot_Abort(DIR_IN);
          return;
     }
     else
     {
          //Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
          Send_CSW_Data(CSW_CMD_PASSED);
     }
}

static s8 SCSI_Address_Management(u8 lun,u8 Cmd,u32 LBA,u32 BlockNbr)
{
     if ((LBA + BlockNbr) > Mass_Block_Count)
     {
          if (Cmd == SCSI_WRITE10)
          {
               Bot_Abort(BOTH_DIR);
          }
          Bot_Abort(DIR_IN);
          Set_Scsi_Sense_Data(lun, ILLEGAL_REQUEST, ADDRESS_OUT_OF_RANGE);
          //Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
          Send_CSW_Data(CSW_CMD_FAILED);
          return -1;
     }

     if(CBW.dDataLength != BlockNbr * Mass_Block_Size)
     {
          if (Cmd == SCSI_WRITE10)
          {
               Bot_Abort(BOTH_DIR);
          }
          else
          {
               Bot_Abort(DIR_IN);
          }
          Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
          //Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
          Send_CSW_Data(CSW_CMD_FAILED);
          return -1;
     }
     return 0;
}




static void SCSI_Read_Toc_Cmd(u8 lun)
{
     USB_SendData((u8 *)usbcdrom_read_toc_data,sizeof(usbcdrom_read_toc_data));
     Send_CSW_Data(CSW_CMD_PASSED);
}


//获取设备信息命令
static void SCSI_Inquiry_Cmd(u8 lun)
{
     u8 *Inquiry_Data;
     u16 Inquiry_Data_Length;

     if (CBW.CB[1] & 0x01)
     {/*Evpd被设置*/
          //Inquiry_Data = (u8 *)Page00_Inquiry_Data;
          //Inquiry_Data_Length = 5;
          //sflag = 1;
          Send_Zero_Data(CBW.dDataLength);
          Send_CSW_Data(CSW_CMD_FAILED);
          //sflag = 0;
     }
     else
     {
          if(lun == 0)
          {
               Inquiry_Data = (u8 *)Standard_Inquiry_Data;
          }
          else
          {
               Inquiry_Data = (u8 *)Standard_Inquiry_Data2;
          }

          if (CBW.CB[4] <= STANDARD_INQUIRY_DATA_LEN)
          {
               Inquiry_Data_Length = CBW.CB[4];
          }
          else
          {
               Inquiry_Data_Length = STANDARD_INQUIRY_DATA_LEN;
          }
          USB_SendData(Inquiry_Data,Inquiry_Data_Length);
          Send_CSW_Data(CSW_CMD_PASSED);
     }
}

//查询当前容量格式(0x23)
static void SCSI_ReadFormatCapacity_Cmd(u8 lun)
{
     if(MAL_GetStatus(lun))
     {//获取失败
          Set_Scsi_Sense_Data(CBW.bLUN, NOT_READY, MEDIUM_NOT_PRESENT);
          //Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
          Send_CSW_Data(CSW_CMD_FAILED);
          Bot_Abort(DIR_IN);
          return;
     }

     ReadFormatCapacity_Data[4] = (u8)(Mass_Block_Count >> 24);
     ReadFormatCapacity_Data[5] = (u8)(Mass_Block_Count >> 16);
     ReadFormatCapacity_Data[6] = (u8)(Mass_Block_Count >>  8);
     ReadFormatCapacity_Data[7] = (u8)(Mass_Block_Count);

     ReadFormatCapacity_Data[9] = (u8)(Mass_Block_Size >>  16);
     ReadFormatCapacity_Data[10] = (u8)(Mass_Block_Size >>  8);
     ReadFormatCapacity_Data[11] = (u8)(Mass_Block_Size);
     //Transfer_Data_Request(ReadFormatCapacity_Data, READ_FORMAT_CAPACITY_DATA_LEN);
     USB_SendData(ReadFormatCapacity_Data,READ_FORMAT_CAPACITY_DATA_LEN);
     Send_CSW_Data(CSW_CMD_PASSED);
}

//返回当前容量(0x25)
static void SCSI_ReadCapacity10_Cmd(u8 lun)
{
     if(MAL_GetStatus(lun))
     {//获取失败
          Set_Scsi_Sense_Data(CBW.bLUN, NOT_READY, MEDIUM_NOT_PRESENT);
          //Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
          Send_CSW_Data(CSW_CMD_FAILED);
          Bot_Abort(DIR_IN);
          return;
     }

     ReadCapacity10_Data[0] = (u8)((Mass_Block_Count - 1) >> 24);
     ReadCapacity10_Data[1] = (u8)((Mass_Block_Count - 1) >> 16);
     ReadCapacity10_Data[2] = (u8)((Mass_Block_Count - 1) >>  8);
     ReadCapacity10_Data[3] = (u8)(Mass_Block_Count - 1);

     ReadCapacity10_Data[4] = (u8)(Mass_Block_Size >>  24);
     ReadCapacity10_Data[5] = (u8)(Mass_Block_Size >>  16);
     ReadCapacity10_Data[6] = (u8)(Mass_Block_Size >>  8);
     ReadCapacity10_Data[7] = (u8)(Mass_Block_Size);
     //Transfer_Data_Request(ReadCapacity10_Data, READ_CAPACITY10_DATA_LEN);
     USB_SendData(ReadCapacity10_Data,READ_CAPACITY10_DATA_LEN);
     Send_CSW_Data(CSW_CMD_PASSED);
}

static void SCSI_ModeSense10_Cmd(u8 lun)
{
     //Transfer_Data_Request(Mode_Sense10_data, MODE_SENSE10_DATA_LEN);
     USB_SendData((u8 *)Mode_Sense10_data, MODE_SENSE10_DATA_LEN);
     Send_CSW_Data(CSW_CMD_PASSED);
}

static void SCSI_ModeSense6_Cmd(u8 lun)
{
     //Transfer_Data_Request((u8 *)Mode_Sense6_data, MODE_SENSE6_DATA_LEN);
     USB_SendData((u8 *)Mode_Sense6_data, MODE_SENSE6_DATA_LEN);
     Send_CSW_Data(CSW_CMD_PASSED);
}

static void SCSI_Read10_Cmd(u8 lun,u32 LBA,u32 BlockNbr)
{
     if(SCSI_Address_Management(CBW.bLUN, SCSI_READ10, LBA, BlockNbr))/* 判断地址地否越界 */
     {//地址越界          
          return;
     }  
     if (CBW.bmFlags & 0x80)
     {          
          Read_Memory(lun,LBA,BlockNbr);
          Send_CSW_Data(CSW_CMD_PASSED);
     }
     else
     {
          Bot_Abort(BOTH_DIR);
          Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
          Send_CSW_Data(CSW_CMD_FAILED);
     }
}

static void SCSI_Start_Stop_Unit_Cmd(u8 lun)
{
     //USB_Stop();
     Send_CSW_Data (CSW_CMD_PASSED);
}

static void SCSI_Write10_Cmd(u8 lun,u32 LBA,u32 BlockNbr)
{
     if(SCSI_Address_Management(CBW.bLUN, SCSI_WRITE10, LBA, BlockNbr))/* 判断地址地否越界 */
     {//地址越界          
          return;
     }  
     if((CBW.bmFlags & 0x80) == 0)
     {          
          OUTWriteAddress = LBA * Mass_Block_Size;//计算出写入的首地址
          OUTWriteSize = BlockNbr * Mass_Block_Size;//计算出写入的字节数
          Receive_State = RECEIVE_DATA;//更改接收状态
     }
     else
     {
          Bot_Abort(BOTH_DIR);
          Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
          Send_CSW_Data(CSW_CMD_FAILED);
     }
}

static void SCSI_RequestSense_Cmd (u8 lun)
{
     u8 Request_Sense_data_Length;
     if (CBW.CB[4] <= REQUEST_SENSE_DATA_LEN)
     {
          Request_Sense_data_Length = CBW.CB[4];
     }
     else
     {
          Request_Sense_data_Length = REQUEST_SENSE_DATA_LEN;
     }
     //Transfer_Data_Request(Scsi_Sense_Data, Request_Sense_data_Length);
     USB_SendData(Scsi_Sense_Data,Request_Sense_data_Length);
     Send_CSW_Data(CSW_CMD_PASSED);
}

static void SCSI_Verify10_Cmd(u8 lun)
{
     if ((CBW.dDataLength == 0) && !(CBW.CB[1] & BLKVFY))/* BLKVFY not set*/
     {
          Send_CSW_Data(CSW_CMD_PASSED);
     }
     else
     {
          Bot_Abort(BOTH_DIR);
          Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
          Send_CSW_Data(CSW_CMD_FAILED);
     }
}

static void SCSI_Format_Cmd(u8 lun)
{
     if(MAL_GetStatus(lun))
     {
          Set_Scsi_Sense_Data(CBW.bLUN, NOT_READY, MEDIUM_NOT_PRESENT);
          Send_CSW_Data(CSW_CMD_FAILED);
          Bot_Abort(DIR_IN);
          return;
     }
     Send_CSW_Data(CSW_CMD_PASSED);
//  else
//  {
//    NAND_Format();
//    Send_CSW_Data(CSW_CMD_PASSED);
//  }

}

