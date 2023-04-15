/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------������ģ����޸ļ�¼��-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * ���ݣ�
 * ���ڣ�2015-03-16
 * ���ߣ�YF
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

       
*/
#include "FUN_SST25VF032.h"
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------����ģ��ĶԽӺ�����-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ��ĺ궨����------------------------------------                                  *
*                                                                                                                                           *
********************************************************************************************************************************************/
#define SSTMinSectionSize     4096//�ɲ���д����С�����ֽ���
#define SSTMaxSize            0x400000//оƬ���ռ��ֽ���
//оƬ������
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
//оƬ״̬����
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
*               ----------------------------------������ģ��ı������Ͷ�����------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����ڲ�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static s32 SST25VF032_ParaCheck(u32 add,u16 sl);
static void SST25VF032_WaitIDE(SST25VF032DevicePort *dp);
static s8 SST25VF032_EraseSector(SST25VF032DevicePort *dp,Erase_Type type ,u32 dwAddr);
static s32 SST25VF032_MaxErase4K(SST25VF032DevicePort *dp,u32 eadd,u16 el);

static s8 SST25VF032_SectionEraseWrite(SST25VF032DevicePort *dp,u32 wadd,u8 *wd,u16 wl);

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ��ı��������ͺ궨����--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/


/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ϵͳ����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/


/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����û�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
������:FUN_SST25VF032_ReadData
��   ��:������ SST25VF032 �����ݺ���
��   ��:
    SST25VF032DevicePort *dp:�豸�˿�
       u32 dwAddr:��ȡ�׵�ַ
       u8 *vData: ��ſռ��׵�ַ
       u16 wLength:��ȡ�ֽ���
����ֵ:
   С��0:��ʾ��ȡ����
 �����0:ʵ�ʶ������ֽ���
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2022-07-19
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
     wLength = s32temp;//���²�������    

     if(wLength)
     {
          SPI_CS_Enable();// Ƭѡ��Ч 
          SPI_ReadWrite(READ);		// ��ָ�� 
          SPI_ReadWrite(((u8*)&dwAddr)[2]);	// ��ַ 
          SPI_ReadWrite(((u8*)&dwAddr)[1]);	// ��ַ 
          SPI_ReadWrite(((u8*)&dwAddr)[0]);	// ��ַ 
          // ������ 
          while(wLength)
          {  
               wLength--;
               data = SPI_ReadWrite(0x00);
               *vData = data;
               vData++;
          }
          SPI_CS_Disable();	  // Ƭѡ��Ч 
          SST25VF032_WaitIDE(dp);
     }
     return s32temp;
}

/*-------------------------------------------------
������:FUN_SST25VF032_GetCapacity
��   ��:��ȡоƬ�����ĺ���
��   ��:
       SST25VF032DevicePort *dp:�豸�˿�
����ֵ:
     оƬ�ռ��ֽ���
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2022-07-19
-------------------------------------------------*/
u32 FUN_SST25VF032_GetCapacity(SST25VF032DevicePort *dp)
{
     return(SSTMaxSize);
}

/*-------------------------------------------------
������:FUN_SST25VF032_WriteByte
��   ��:������ SST25VF032 д�ֽں���
��   ��:
       SST25VF032DevicePort *dp:�豸�˿�
       u32 dwAddr:доƬ��ַ
       u8 vData: ��д����
����ֵ:
   С��0:д�����
   ����0:д��ɹ�
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2022-07-19
-------------------------------------------------*/
s8 FUN_SST25VF032_WriteByte(SST25VF032DevicePort *dp,u32 dwAddr,u8 vData)
{
     s32 s32temp;

     s32temp = SST25VF032_ParaCheck(dwAddr,1);
     if(s32temp < 0)
     {
          return -1;
     }
     
     SPI_CS_Enable();    // Ƭѡ��Ч
     SPI_ReadWrite(WREN);// дʹ��
     SPI_CS_Disable();   // Ƭѡ��Ч 
     SPI_CS_Enable();    // Ƭѡ��Ч 
     SPI_ReadWrite(WRITE);    // дָ�� 
     SPI_ReadWrite(((unsigned char*)&dwAddr)[2]);	// д���ַ 
     SPI_ReadWrite(((unsigned char*)&dwAddr)[1]);	// д���ַ 
     SPI_ReadWrite(((unsigned char*)&dwAddr)[0]);	// д���ַ 
     SPI_ReadWrite(vData);    // д������ 
     SPI_CS_Disable();   // Ƭѡ��Ч 
     SST25VF032_WaitIDE(dp);
     
     return 0;
}

/*-------------------------------------------------
������:FUN_SST25VF032_WriteData
��   ��:������ SST25VF032 д���ݺ���
��   ��:
       SST25VF032DevicePort *dp:�豸�˿�
       u32 dwAddr:доƬ�׵�ַ
       u8 *vData: ��д�����׵�ַ
       u16 wLen:��д�ֽ���
����ֵ:
   С��0:��ʾд����
 �����0:ʵ��д���ֽ���
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2022-07-19
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
     wLen = s32temp;//���²����ֽ���


     startnum = dwAddr % 256;			/* д���ַ����ҳ */
     if(startnum)
     {
          startnum = 256 - startnum;
     }

/* д�뿪ʼ�Ĳ���ҳ���� */
     if(startnum)
     {
          SPI_CS_Enable();	  /* Ƭѡ��Ч */
          SPI_ReadWrite(WREN);			/* дʹ�� */
          SPI_CS_Disable();	  /* Ƭѡ��Ч */
          SPI_CS_Enable();	  /* Ƭѡ��Ч */
          SPI_ReadWrite(WRITE);		/* дָ�� */
          SPI_ReadWrite(((u8 *)&dwAddr)[2]);	/* д���ַ */
          SPI_ReadWrite(((u8 *)&dwAddr)[1]);	/* д���ַ */
          SPI_ReadWrite(((u8 *)&dwAddr)[0]);	/* д���ַ */
          // Write data
          while((startnum) && wLen)
          {
               SPI_ReadWrite(*vData);		/* д������ */
               dwAddr++;
               vData++;	
               startnum--;
               wLen--;
          }
          SPI_CS_Disable();	  /* Ƭѡ��Ч */
          SST25VF032_WaitIDE(dp);
     }

/* ��ҳд�� */
     pagenum = wLen/256;
     while(pagenum)
     {
          SPI_CS_Enable();	  /* Ƭѡ��Ч */
          SPI_ReadWrite(WREN);			/* дʹ�� */
          SPI_CS_Disable();	  /* Ƭѡ��Ч */
          SPI_CS_Enable();	  /* Ƭѡ��Ч */
          SPI_ReadWrite(WRITE);		/* дָ�� */
          SPI_ReadWrite(((u8 *)&dwAddr)[2]);	/* д���ַ */
          SPI_ReadWrite(((u8 *)&dwAddr)[1]);	/* д���ַ */
          SPI_ReadWrite(((u8 *)&dwAddr)[0]);	/* д���ַ */
          // Write data
          i = 256;
          while(i)
          {
               SPI_ReadWrite(*vData);		/* д������ */
               dwAddr++;
               vData++;	
               i--;
               wLen--;
          }
          SPI_CS_Disable();	  /* Ƭѡ��Ч */
          SST25VF032_WaitIDE(dp);
          pagenum--;
     }

/* β��������ҳ */
     if(wLen)
     {
          SPI_CS_Enable();	  /* Ƭѡ��Ч */
          SPI_ReadWrite(WREN);			/* дʹ�� */
          SPI_CS_Disable();	  /* Ƭѡ��Ч */
          SPI_CS_Enable();	  /* Ƭѡ��Ч */
          SPI_ReadWrite(WRITE);		/* дָ�� */
          SPI_ReadWrite(((u8 *)&dwAddr)[2]);	/* д���ַ */
          SPI_ReadWrite(((u8 *)&dwAddr)[1]);	/* д���ַ */
          SPI_ReadWrite(((u8 *)&dwAddr)[0]);	/* д���ַ */
          // Write data
          while(wLen)
          {
               SPI_ReadWrite(*vData);		/* д������ */
               dwAddr++;
               vData++;	
               wLen--;
          }
          SPI_CS_Disable();	  /* Ƭѡ��Ч */
          SST25VF032_WaitIDE(dp);
     }
     return s32temp;
}

/*-------------------------------------------------
������:FUN_SST25VF032_WriteData2
��   ��:������ SST25VF032 ������д���ݺ���
��   ��:
       SST25VF032DevicePort *dp:�豸�˿�
       u32 dwAddr:доƬ�׵�ַ
       u8 *vData: ��д�����׵�ַ
       u16 wLen:��д�ֽ���
����ֵ:
   С��0:��ʾд����
 �����0:ʵ��д���ֽ���
ע   ��:
       �ú���ִ�н�����ԭ������Ϊ���̴��в���
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2022-07-20
-------------------------------------------------*/
s32 FUN_SST25VF032_WriteData2(SST25VF032DevicePort *dp,u32 dwAddr, u8 *vData, u16 wLen)
{
     u8 SSTOprBuffer[SSTMinSectionSize];
     u16 u16size,u16i;
     s16 s16temp;
     u32 SecAdd;//������ַ
     s32 s32temp;

     s32temp = SST25VF032_ParaCheck(dwAddr,wLen);
     if(s32temp < 0)
     {
          return -1;
     }
     wLen = s32temp;//���²����ֽ���

     if(wLen == 0)
     {
          return 0;
     }



//����������
     s16temp = (dwAddr % SSTMinSectionSize);
     if(s16temp)
     {    
          //ȷ��������ַ
          SecAdd = (dwAddr - s16temp);
          //��ȡԭ����
          FUN_SST25VF032_ReadData(dp,SecAdd,SSTOprBuffer,SSTMinSectionSize);//��ȡ����
          //ͳ���滻�����ֽ���
          u16size = (SSTMinSectionSize - s16temp);
          if(u16size > wLen)
          {
               u16size = wLen;
          }
          //��ʼ�滻����
          for(u16i = 0;u16i < u16size;u16i++)
          {
               SSTOprBuffer[s16temp + u16i] = vData[u16i];
          }
          //������д��
          SST25VF032_SectionEraseWrite(dp,SecAdd,SSTOprBuffer,SSTMinSectionSize);
          //������Ϣ
          wLen -= u16size;
          dwAddr += u16size;
          vData += u16size;
     }

//��������
     if(wLen)
     {
          while(wLen >= SSTMinSectionSize)
          {
               SST25VF032_SectionEraseWrite(dp,dwAddr,vData,SSTMinSectionSize);
               //������Ϣ
               wLen -= SSTMinSectionSize;
               dwAddr += SSTMinSectionSize;
               vData += SSTMinSectionSize;
          }
     }

//��������_����
     if(wLen)
     {
          //��ȡԭ����
          FUN_SST25VF032_ReadData(dp,dwAddr,SSTOprBuffer,SSTMinSectionSize);//��ȡ����
          //ͳ���滻�����ֽ���
          u16size = wLen;
          //��ʼ�滻����
          for(u16i = 0;u16i < u16size;u16i++)
          {
               SSTOprBuffer[u16i] = vData[u16i];
          }
          //������д��
          SST25VF032_SectionEraseWrite(dp,dwAddr,SSTOprBuffer,SSTMinSectionSize);
     }
     
     return s32temp;
}

/*-------------------------------------------------
������:FUN_SST25VF032_Erase
��   ��:������ SST25VF032 ���4K��������
��   ��:
       SST25VF032DevicePort *dp:�豸�˿�
       u32 eadd:������ʼ��ַ
       u16 el:�����ֽ���
����ֵ:
   С��0:��ʾ��������
  ����ֵ:��ʾʵ�ʲ����ֽ���
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2022-07-19
-------------------------------------------------*/
s32 FUN_SST25VF032_Erase(SST25VF032DevicePort *dp,u32 eadd,u16 el)
{
     s32 s32temp,s32result;
     
     s32temp = SST25VF032_ParaCheck(eadd,el);
     if(s32temp < 0)
     {
          return -1;
     }
     el = s32temp;//���²����ֽ���

     s32result = 0;
     while(el)
     {
          s32temp = SST25VF032_MaxErase4K(dp,eadd,el);
          if(s32temp < 0)
          {//����
               return s32result;
          }
          el -= s32temp;          
          eadd += s32temp;
          s32result += s32temp;
     }

     return s32result;
}






/*-------------------------------------------------
������:FUN_SST25VF032_NonBlock_EraseSector
��   ��:�������� SST25VF032 ����������������
��   ��:
       SST25VF032DevicePort *dp:�豸�˿�
       Erase_Type type:������������
       u32 dwAddr:������ַ���ڵ�����
����ֵ:
   С��0:��ʾ������������
   ����0:��ʾ���������ɹ�
ע   ��:
       �ú�������������dwAddr��ַ���ڵ��������ǲ�����������
       �����Ĵ�С��type����
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2022-07-19
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
     SPI_CS_Enable();	  /* Ƭѡ��Ч */
     SPI_ReadWrite(WREN);/* дʹ�� */
     SPI_CS_Disable();	  /* Ƭѡ��Ч */
     SPI_CS_Enable();	  /* Ƭѡ��Ч */
     SPI_ReadWrite(command);			/* ����ָ�� */
     SPI_ReadWrite(((u8 *)&dwAddr)[2]);	/* д���ַ */
     SPI_ReadWrite(((u8 *)&dwAddr)[1]);	/* д���ַ */
     SPI_ReadWrite(((u8 *)&dwAddr)[0]);	/* д���ַ */
     SPI_CS_Disable();	  /* Ƭѡ��Ч */
     //SST25VF032_WaitIDE();
     return 0;
}

/*-------------------------------------------------
������:FUN_SST25VF032_NonBlock_CheckEraseSector
��   ��:�������� SST25VF032 �жϲ�������״̬
��   ��:
       SST25VF032DevicePort *dp:�豸�˿�
����ֵ:
   С��0:��ʾ��������
   ����0:��ʾ�����ɹ�
   ����0:��ʾ������
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2022-07-19
-------------------------------------------------*/
s8 FUN_SST25VF032_NonBlock_CheckEraseSector(SST25VF032DevicePort *dp)
{
     u8 Dummy;
     s8 s8temp;
     
     SPI_CS_Enable();	  /* Ƭѡ��Ч */
     SPI_ReadWrite(RDSR);			/* ��״̬ */
     Dummy = SPI_ReadWrite(0x00);
     if(Dummy & BUSY)
     {//æ��
          s8temp = 1;
     }
     else
     {
          s8temp = 0;
     }
     SPI_CS_Disable();	  /* Ƭѡ��Ч */
     
     return s8temp;
}


/*-------------------------------------------------
������:FUN_SST25VF032_EraseChip
��   ��:������ SST25VF032 ������Ƭ����
��   ��:
       SST25VF032DevicePort *dp:�豸�˿�
����ֵ:
   С��0:��ʾ��������
   ����0:��ʾ�����ɹ�
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2022-07-19
-------------------------------------------------*/
s8 FUN_SST25VF032_EraseChip(SST25VF032DevicePort *dp)
{
     SPI_CS_Enable();	  /* Ƭѡ��Ч */
     SPI_ReadWrite(WREN);			/* дʹ�� */
     SPI_CS_Disable();	  /* Ƭѡ��Ч */
     SPI_CS_Enable();	  /* Ƭѡ��Ч */
     SPI_ReadWrite(ERASE_ALL);			/* ����ָ�� */
     SPI_CS_Disable();	  /* Ƭѡ��Ч */
     SST25VF032_WaitIDE(dp);
     return 0;
}

/*-------------------------------------------------
������:FUN_SST25VF032_ReadID
��   ��:������ SST25VF032 ��оƬID����
��   ��:
       SST25VF032DevicePort *dp:�豸�˿�
       u8 *id1������ID
       u8 *id2���豸ID
����ֵ:
   С��0:��ʾ��ȡ����
   ����0:��ʾ��ȡ�ɹ�
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2022-07-19
-------------------------------------------------*/
s8 FUN_SST25VF032_ReadID(SST25VF032DevicePort *dp,u8 *id1, u8 *id2)
{
     SPI_CS_Enable();	  /* Ƭѡ��Ч */
     SPI_ReadWrite(RDID);	/* дʹ�� */
     SPI_ReadWrite(0x00);
     SPI_ReadWrite(0x00);
     SPI_ReadWrite(0x00);	/* dumy */
     *id1 = SPI_ReadWrite(0x00);/* ����ID */
     *id2 = SPI_ReadWrite(0x00);/* �豸ID */
     SPI_CS_Disable();	  /* Ƭѡ��Ч */
	return 0;
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����ڲ�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
//�жϲ�����Ч��
//����ֵ С��0:�Ƿ����� ����ֵ:��������ֽ���
static s32 SST25VF032_ParaCheck(u32 add,u16 sl)
{
     u32 u32ol;//�����������ֽ���
     if((add >= SSTMaxSize) || (sl > SSTMaxSize))
     {//Խ����
          return -1;
     }
     u32ol = (SSTMaxSize - add);//�����������ֽ���
     if(sl < u32ol)
     {
          u32ol = sl;
     }
     return u32ol;
}

//�ȴ�flash��������
static void SST25VF032_WaitIDE(SST25VF032DevicePort *dp)
{
     u8 Dummy;
     SPI_CS_Enable();	  /* Ƭѡ��Ч */
     SPI_ReadWrite(RDSR);			/* ��״̬ */

     // Poll the BUSY bit
     do
     {
          Dummy = SPI_ReadWrite(0x00);
     } while(Dummy & BUSY);
     SPI_CS_Disable();	  /* Ƭѡ��Ч */
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
     SPI_CS_Enable();	  /* Ƭѡ��Ч */
     SPI_ReadWrite(WREN);/* дʹ�� */
     SPI_CS_Disable();	  /* Ƭѡ��Ч */
     SPI_CS_Enable();	  /* Ƭѡ��Ч */
     SPI_ReadWrite(command);			/* ����ָ�� */
     SPI_ReadWrite(((u8 *)&dwAddr)[2]);	/* д���ַ */
     SPI_ReadWrite(((u8 *)&dwAddr)[1]);	/* д���ַ */
     SPI_ReadWrite(((u8 *)&dwAddr)[0]);	/* д���ַ */
     SPI_CS_Disable();	  /* Ƭѡ��Ч */
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
     el = s32temp;//���²����ֽ���
     
     //׼������     
     u32add = ((eadd / SSTMinSectionSize) * SSTMinSectionSize);//������ַ4K��������
     u16offset = (eadd % SSTMinSectionSize);//ƫ����
     u16el = (SSTMinSectionSize - u16offset);//�������ֽ���
     if(el < u16el)
     {
          u16el = el;
     }

     //��ȡ����������
     if(u16el != SSTMinSectionSize)
     {
          //��ȡ����
          if(FUN_SST25VF032_ReadData(dp,u32add,SSTOprBuffer,SSTMinSectionSize) != SSTMinSectionSize)
          {
               return -2;
          }
          //�����ĵط�д0xff
          for(u16temp = 0;u16temp < u16el;u16temp++)
          {
               SSTOprBuffer[u16offset + u16temp] = 0xff;
          }
     }

     //���� 
     SST25VF032_EraseSector(dp,ERASE_4K_TYPE,u32add);

     //�ָ�����
     if(u16el != SSTMinSectionSize)
     {
          if(FUN_SST25VF032_WriteData(dp,u32add,SSTOprBuffer,SSTMinSectionSize) != SSTMinSectionSize)
          {
               return -3;
          }
     }

     return (s16)u16el;
}



//4K�����Ĳ�����д��
//����ֵ��0��ʾ�����ɹ� С��0��ʾ����ʧ��
static s8 SST25VF032_SectionEraseWrite(SST25VF032DevicePort *dp,u32 wadd,u8 *wd,u16 wl)
{
     if((wadd % SSTMinSectionSize) || (wl != SSTMinSectionSize))
     {//д���ַ������������ �� ���Ȳ��������ߴ�
          return -1;
     }
     SST25VF032_EraseSector(dp,ERASE_4K_TYPE,wadd);//��������
     FUN_SST25VF032_WriteData(dp,wadd,wd,wl);
     return 0;
}

