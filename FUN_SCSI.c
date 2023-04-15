/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------������ģ����޸ļ�¼��-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * ���ݣ�
 * ���ڣ�2019-04-16
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
          Ŀǰģ��U��д�ٶ�:Լ16K�ֽ�/S
          Ŀǰģ��U�̶��ٶ�:Լ190K�ֽ�/S
*/

#include "FUN_SCSI.h"

#define DeviceTypeCode        0x05 ///* �豸����: 0x00��ʾ�ƶ�����(U��)  0x05��ʾ���� 0x1f��ʾ���ƶ��豸(Ӳ��)Ŀǰ��֧�� */
#define AddressOffset         2097152//��ַƫ�ƣ���λ�ֽ�
#define MAX_LUN_NUM           0    //�̷������(0:��ʾֻ��1������)�����15

#define SCSI_Buffer_SIZE      66//4096//66//��ֵ�󣬻���󣬽������ݾͿ�

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------����ģ���ĶԽӺ�����-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
#define USB_SendData(DP,DL)        API_USB_StoresClass_SendData(DP,DL)
#define USB_Stop()                 API_USB_StoresClass_Close()//�ر�USB����
#define IN_ENDP_STALL()            //IN�˵����//SetEPTxStatus(ENDP1, EP_TX_STALL);
#define OUT_ENDP_STALL()           //OUT�˵����//SetEPRxStatus(ENDP2, EP_RX_STALL);
#define IN_ENDP_ENABLE()           //IN�˵�ʹ��//SetEPTxStatus(ENDP1, EP_TX_VALID);
#define OUT_ENDP_ENABLE()          //OUT�˵�ʹ��//SetEPRxStatus(ENDP2, EP_RX_VALID);/* enable the Endpoint to receive the next cmd*/

#define GetStorageSpace(SP)        API_W25Q32_GetCapacity(SP)
#define ReadStorageData(AD,BP,BL)  API_W25Q32_ReadData(AD+AddressOffset,BP,BL)
#define WriteStorageData(AD,BP,BL) API_W25Q32_WriteData2(AD+AddressOffset,BP,BL)//ע�⣬��Ҫ���в������ܵ�д����

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ı���������--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/


/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ڵĺ궨����------------------------------------                                *
*                                                                                                                                           *
********************************************************************************************************************************************/

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ڵı������Ͷ�����  �� �궨�� ������------------------------------------                            *
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


/****** BOT(����������)״̬�� ******/
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
/****** CBW��CSW���ݰ������궨�� ******/
#define BOT_CBW_SIGNATURE             0x43425355  //CBW�����ʶ��
#define BOT_CSW_SIGNATURE             0x53425355  //CSW״̬��ʶ��
#define BOT_CBW_PACKET_LENGTH         31          //CBW��������ȣ���λ�ֽ�
#define CSW_DATA_LENGTH               0x000D      //CSW״̬�����ȣ���λ�ֽ�
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
/****** SCSI����궨�� ******/
#define SCSI_TEST_UNIT_READY                        0x00    //�����Ƿ�׼����---------------------
#define SCSI_REQUEST_SENSE                          0x03    //���������豸����ִ�н������״̬����
#define SCSI_FORMAT_UNIT                            0x04
#define SCSI_READ6                                  0x08
#define SCSI_WRITE6                                 0x0A
#define SCSI_INQUIRY                                0x12    //��ȡ������Ϣ(���������ң���Ʒ���ƣ���Ʒ�汾��)---------------
#define SCSI_MODE_SELECT6                           0x15    //����host���ⲿ�豸���ò���
#define SCSI_MODE_SENSE6                            0x1A    //��host�������-----------------
#define SCSI_START_STOP_UNIT                        0x1B    //����/ֹͣ�洢��Ԫ��Դ(д����)
#define SCSI_SEND_DIAGNOSTIC                        0x1D
#define SCSI_ALLOW_MEDIUM_REMOVAL                   0x1E    //��ֹ/����洢�����ƶ�
#define SCSI_READ_FORMAT_CAPACITIES                 0x23    //��ȡ�豸�洢��С��Block(һ��Ϊһ��������С��Ĭ��Ϊ512)���ȵ���Ϣ
#define SCSI_READ_CAPACITY10                        0x25    //Ҫ���豸�������һ����������źͿ�ĳ���---------------
#define SCSI_READ10                                 0x28    //host�Ӵ洢���ʶ�ȡ����������---------
#define SCSI_WRITE10                                0x2A    //host��洢����д�����������
#define SCSI_VERIFY10                               0x2F    //��洢����֤����
#define SCSI_READTOC                                0x43    //��ȡ����Ŀ¼�ṹ
#define SCSI_MODE_SELECT10                          0x55    //����host���ⲿ�豸���ò���
#define SCSI_MODE_SENSE10                           0x5A    //��host�������
#define SCSI_READ16                                 0x88
#define SCSI_WRITE16                                0x8A
#define SCSI_VERIFY16                               0x8F
#define SCSI_READ_CAPACITY16                        0x9E
#define SCSI_REPORT_LUNS                            0xA0    //��ȡ�豸��LUN����LUN�嵥
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
*               ----------------------------------������ģ����ڲ�����������------------------------------------                            *
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
*               ----------------------------------������ģ���ڵ� ���� �� �궨�� ������--------------------------------                            *
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
     /* �������� */
     'S', 't', 'a', 'r', 'm', 'a', 'c', 'h',' ',
     /* ��Ʒʶ���� */
     'F', 'l', 'a', 's', 'h', 'D', 'i', 's', 'k',' ',
     ' ', ' ', ' ', ' ', ' ', ' ', 
     /* ��Ʒ�汾�� */
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
MODE_SENSE6_CMD��Ҫ���ص����ݽṹ���£�
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
MODE_SENSE10_CMD��Ҫ���ص����ݽṹ���£�
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
//     0x00,0x10,                 //���������С����λ�ֽ�
//     0x05,                       //��������
//     0x00,                       //д����
//     0x00,0x00,                 //Ԥ��
//     0x00,0x08,                 //�������������
//     0x00,0x00,0x00,0x00,     //���Ŀ��
//     0x00,0x00,0x00,0x00      //��ߴ磬��λ�ֽڣ��˴���ҪΪ512
//};

static const u8 usbcdrom_read_toc_data[] = //�������ר�ã����ڷ��ع��̵�Ŀ¼�ṹ����
{
     0x00,0x12,                  /* ������ֽ��� */
     0x01,                        /* first track */
     0x01,                        /* last track */
     
     0x00,                        //(Ԥ��)
     0x14,                        //(adr, control)
     0x01,                        //(track being described) 
     0x00,                        //(Ԥ��)
     0x00,0x00,0x00,0x00,       // (�����߼����ַ:0)---�ǲ���������???
     
     0x00,                         //(Ԥ��)
     0x14,                         //(adr, control)
     0xAA,                         //(track being described (leadout))
     0x00,                         //(Ԥ��)
     0x00,0x00,0xA2,0x8A         //(start logical block address 41610)
};

static u8 Scsi_Sense_Data[] =//�˴����ɸ�Ϊ��������
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

static u8 ReadFormatCapacity_Data[] =//�˴����ɸ�Ϊ��������
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
��ReadCapacity10_Data��
���һ���߼���ż��㷽��:
���裬�洢�豸�Ŀռ��СΪsps�ֽ�,��ߴ�ΪCDROM_BLOCK = 512
��ô��������block_num = (sps + 512 - 1) / 512
ע�⣬�����512��Ϊ��ߴ磬��������С����λ���ֽ�
��Ϊ����SCSIϵͳ�У������Ǵ�0��ʼ��ŵģ���ô���һ�����block_last = block_num - 1
��ע�⣺SCSIϵͳ���ô��(big endian)��ͨ��ARM������������������С��(little endian)����Ҫ��block_last��CDROM_BLOCK��ɴ�ˡ�
USBSDMS_READ_CAPACITY_CMD��Ҫ���ص����ݽṹ���£�
                   struct read_cap_data
                    {
                       unsigned long block_last;  //the last block number
                       unsigned long block_len;   //in bytes
                    } ;
*/
static u8 ReadCapacity10_Data[] =//�˴����ɸ�Ϊ��������
{
     /* ���һ���߼����(�߼����0��ʼ) */
     0,
     0,
     0,
     0,

     /* �߼���ߴ磬�����ȣ���λ�ֽڣ���������С����Ϊ512 */
     0,
     0,
     0,
     0
};


#define StorageSectorSize     512//�洢�豸��������С����λ�ֽ�
static u32 Mass_Block_Size;     //���С����λ�ֽ�
static u32 Mass_Block_Count;    //������

static Bulk_Only_CBW CBW;
static Bulk_Only_CSW CSW;
static u32 SCSI_LBA,SCSI_BlkLen;//�߼����ַ���鴫������


static u8 SCSI_Buffer[SCSI_Buffer_SIZE];
static u32 SCSI_Buffer_WIntex;

static u32 OUTWriteAddress,OUTWriteSize;

u8 sflag = 0;
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ϵͳ����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
������: FUN_SCSI_Config
��  ��: SCSIģ������ú���
��  ��:
       ��
����ֵ:
       0:���óɹ�
   С��0:����ʧ��    
ע  ��:
       
ʾ  ��:
��  ��:YJX
��  ��:V1.0
ʱ  ��:2019-05-29  
-------------------------------------------------*/
s8 FUN_SCSI_Config(void)
{     
     Bot_State = Bot_State;//��ֹ���뾯��
     Bot_State = BOT_IDLE;
     Receive_State = RECEIVE_DECODE;//����״̬
     SCSI_Buffer_WIntex = 0;//����д�����Ŵ�0��ʼ
     
     return 0;
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����û�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
u8 FUN_SCSI_GetMaxLUNNum(void)
{//��������������ȷ���豸֧�ֵ��߼���Ԫ����MaxLUN��ֵ������0~15.ע��:LUN�Ǵ�0��ʼ��
 //���������򲻴��ڵ�LUN����CBW
     return MAX_LUN_NUM;
}

s32 FUN_SCSI_ReceiveDataProcess(u8 *dap,u32 dal)
{//
     u32 u32opern;
     u32 u32Counter;
     u32opern = (SCSI_Buffer_SIZE - SCSI_Buffer_WIntex);
     if(dal <= (SCSI_Buffer_SIZE - SCSI_Buffer_WIntex))
     {//���ݷŵ���
          u32opern = dal;
     }
     
     //DRI_UART0_Send_String("1");
     //DRI_UART0_Send_Byte(0x0d);
     //DRI_UART0_Send_Byte(0x0a);
     
     for(u32Counter = 0;u32Counter < u32opern;u32Counter++)
     {
          SCSI_Buffer[SCSI_Buffer_WIntex++] = dap[u32Counter];
     }
 //------ ��ʼ����CBW����------
     if(Receive_State == RECEIVE_DECODE)
     {//��Ҫ���յ�CBW���ݰ�
          //DRI_UART0_Send_String("A");
          //DRI_UART0_Send_Byte(0x0d);
          //DRI_UART0_Send_Byte(0x0a);
          if(SCSI_Buffer_WIntex >= BOT_CBW_PACKET_LENGTH)
          {//���ݸ�����      
               //DRI_UART0_Send_String("B");
               //DRI_UART0_Send_Byte(0x0d);
               //DRI_UART0_Send_Byte(0x0a);
               SCSI_Buffer_WIntex -= BOT_CBW_PACKET_LENGTH;//���»���д������ 
               for (u32Counter = 0; u32Counter < BOT_CBW_PACKET_LENGTH; u32Counter++)
               {
                    *((u8 *)&CBW + u32Counter) = SCSI_Buffer[u32Counter];
               }
               CSW.dTag = CBW.dTag;
               CSW.dDataResidue = CBW.dDataLength;
               
               if(CBW.dSignature == BOT_CBW_SIGNATURE)
               {//��CBW���ݰ�
                                  
                    //DRI_UART0_Send_String("C");
                    //DRI_UART0_Send_Byte(0x0d);
                    //DRI_UART0_Send_Byte(0x0a);
               
                    if ((CBW.bLUN > MAX_LUN_NUM) || (CBW.bCBLength < 1) || (CBW.bCBLength > 16))
                    {//��Ч��CBW���ݰ�     
                         //DRI_UART0_Send_String("G");
                         //DRI_UART0_Send_Byte(0x0d);
                         //DRI_UART0_Send_Byte(0x0a);
                         
                         Bot_Abort(BOTH_DIR);//�˴���Ҫִ����IN��OUT�˵��������
                         Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
                         //Set_CSW (CSW_CMD_FAILED,SEND_CSW_DISABLE);
                         Send_CSW_Data(CSW_CMD_FAILED);
                    }
                    else
                    {//��ЧCBW���ݰ�
                         //DRI_UART0_Send_String("����ЧCBW���ݰ�!!!");
                         //DRI_UART0_Send_String("D");
                         //DRI_UART0_Send_Byte(0x0d);
                         //DRI_UART0_Send_Byte(0x0a);
                         
                         switch (CBW.CB[0])
                         {
                              case SCSI_TEST_UNIT_READY://��ѯ�豸�Ƿ�׼����
                                   SCSI_TestUnitReady_Cmd(CBW.bLUN);
                                   break;
                              case SCSI_REQUEST_SENSE:
                                   SCSI_RequestSense_Cmd(CBW.bLUN);
                                   break;
                              case SCSI_INQUIRY://��ȡ������Ϣ
                                   SCSI_Inquiry_Cmd(CBW.bLUN);
                                   break;
                              case SCSI_START_STOP_UNIT://֧�ֵ����Ĳ���
                                   SCSI_Start_Stop_Unit_Cmd(CBW.bLUN);
                                   //delaynms(100);
                                   //PRO_USBCNF_Config(0);//��Ϊ��ӡ
                                   break;
                              case SCSI_ALLOW_MEDIUM_REMOVAL:
                                   SCSI_Start_Stop_Unit_Cmd(CBW.bLUN);
                                   break;
                              case SCSI_MODE_SENSE6://��Host�������
                                   SCSI_ModeSense6_Cmd(CBW.bLUN);
                                   break;
                              case SCSI_MODE_SENSE10:
                                   SCSI_ModeSense10_Cmd(CBW.bLUN);
                                   break;
                              case SCSI_READ_FORMAT_CAPACITIES://��ѯ��ǰ������ʽ
                                   SCSI_ReadFormatCapacity_Cmd(CBW.bLUN);
                                   break;
                              case SCSI_READ_CAPACITY10://Ҫ���豸���ص�ǰ����
                                   SCSI_ReadCapacity10_Cmd(CBW.bLUN);
                                   break;
                              case SCSI_READ10://�Ӵ洢���ʶ�ȡ����������
                                   /* �����߼����ַ */
                                   SCSI_LBA = ((CBW.CB[2] << 24) | (CBW.CB[3] << 16) | (CBW.CB[4] <<  8) | CBW.CB[5]);
                                   /* ���㴫����� */
                                   SCSI_BlkLen = ((CBW.CB[7] <<  8) | CBW.CB[8]);
                                   SCSI_Read10_Cmd(CBW.bLUN,SCSI_LBA,SCSI_BlkLen);
                                   break;
                              case SCSI_WRITE10://д���������ݵ��洢����
                                   /* �����߼����ַ */
                                   SCSI_LBA = ((CBW.CB[2] << 24) | (CBW.CB[3] << 16) | (CBW.CB[4] <<  8) | CBW.CB[5]);
                                   /* ���㴫����� */
                                   SCSI_BlkLen = ((CBW.CB[7] <<  8) | CBW.CB[8]);
                                   SCSI_Write10_Cmd(CBW.bLUN,SCSI_LBA,SCSI_BlkLen);
                                   break;
                              case SCSI_VERIFY10:
                                   SCSI_Verify10_Cmd(CBW.bLUN);
                                   break;
                              case SCSI_FORMAT_UNIT:
                                   SCSI_Format_Cmd(CBW.bLUN);
                                   break;
                              case SCSI_READTOC://�������ר��
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
               {//��Ч��CBW���ݰ�
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
     {//��Ҫ���յ�����
          //DRI_UART0_Send_String("2");
          //DRI_UART0_Send_Byte(0x0d);
          //DRI_UART0_Send_Byte(0x0a);
          if(SCSI_Buffer_WIntex)
          {//������
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
                         OUTWriteAddress += wn;//����д��ַ
                         OUTWriteSize -= wn;//����д���ȣ���λ�ֽ�
                         SCSI_Buffer_WIntex -= wn;//���»���д������ 
                    }
                    //{
                    //     u8 abcde[20];
                    //     FUN_TypeChange_IntToStr(OUTWriteSize,abcde);
                    //     DRI_UART0_Send_String("6ʣ���ֽ�:");
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
*               ----------------------------------������ģ����ڲ�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static void Bot_Abort(u8 Direction)
{
     switch (Direction)
     {
          case DIR_IN://IN�˵����
               IN_ENDP_STALL();
               break;
          case DIR_OUT://OUT�˵����
               OUT_ENDP_STALL();
               break;
          case BOTH_DIR://IN �� OUT�˵㶼����
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
     CSW.dDataResidue = 0;//ʵ�ʴ������ݸ�������������������ֽ���֮��˴�Ϊ0��ʾ���������
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

//��ȡʧ�ܷ���1 ��ȡ�ɹ�����0
static u16 MAL_GetStatus (u8 lun)
{//��ȡ�洢�豸״̬
     u32 storagesize;//�洢�ռ�     
     if(GetStorageSpace(&storagesize))
     {//��ȡʧ��
          return 1;
     }
     Mass_Block_Size = StorageSectorSize;//���С����������С�̶�Ϊ512�ֽ�
     Mass_Block_Count = (storagesize / Mass_Block_Size);
     return 0;
}

static void Read_Memory(u8 lun,u32 Memory_Offset,u32 Transfer_Length)
{//Memory_OffsetΪ���ַ��Transfer_LengthΪ������
     u8 rbuff[128];
     u32 raddress = Memory_Offset * Mass_Block_Size;//�������ȡ���׵�ַ
     u32 rlenth = Transfer_Length * Mass_Block_Size;//�������ȡ���ֽ���
     u32 rn;

     while(rlenth)
     {
          rn = rlenth;
          if(rn > 128)
          {
               rn = 128;
          }
          ReadStorageData(raddress,rbuff,rn);//��ȡ����
          USB_SendData(rbuff,rn);//���Ͷ���������          
          raddress += rn;//���¶���ַ
          rlenth -= rn;//���¶�����
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


//��ȡ�豸��Ϣ����
static void SCSI_Inquiry_Cmd(u8 lun)
{
     u8 *Inquiry_Data;
     u16 Inquiry_Data_Length;

     if (CBW.CB[1] & 0x01)
     {/*Evpd������*/
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

//��ѯ��ǰ������ʽ(0x23)
static void SCSI_ReadFormatCapacity_Cmd(u8 lun)
{
     if(MAL_GetStatus(lun))
     {//��ȡʧ��
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

//���ص�ǰ����(0x25)
static void SCSI_ReadCapacity10_Cmd(u8 lun)
{
     if(MAL_GetStatus(lun))
     {//��ȡʧ��
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
     if(SCSI_Address_Management(CBW.bLUN, SCSI_READ10, LBA, BlockNbr))/* �жϵ�ַ�ط�Խ�� */
     {//��ַԽ��          
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
     if(SCSI_Address_Management(CBW.bLUN, SCSI_WRITE10, LBA, BlockNbr))/* �жϵ�ַ�ط�Խ�� */
     {//��ַԽ��          
          return;
     }  
     if((CBW.bmFlags & 0x80) == 0)
     {          
          OUTWriteAddress = LBA * Mass_Block_Size;//�����д����׵�ַ
          OUTWriteSize = BlockNbr * Mass_Block_Size;//�����д����ֽ���
          Receive_State = RECEIVE_DATA;//���Ľ���״̬
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

