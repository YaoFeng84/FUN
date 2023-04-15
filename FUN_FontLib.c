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
��1����GB18030��������˵��
       ��1��������GB18030���ĵ����׼�������ܴ󣬶���ſռ����ޣ���ֻ��ȡ���и�����������ַ��������ݣ����������ĵ������ݽ��������������
       �������˳��Ϊ��
       ˫�ֽ�����1��������
       ˫�ֽ�����2��������
       ˫�ֽ�����3��������
       ˫�ֽ�����4��������
       ˫�ֽ�����5��������
       ���ֽ�8139����0��������
       ���ֽ�8139����1��������
       ���ֽ�8230�����������
       ���ֽ�8231�����������
       ���ֽ�8232�����������
       ���ֽ�8233�����������
       ���ֽ�8234�����������
       ���ֽ�8235�����������
       ��2�������ø�������ַ���Ϊ׼��������ƫ���������Բ����ֺŴ�С��α䣬ֻҪ��������ַ������䣬�����λҲ����ȷ��

��2���������ַ���       
       ���ڹ����ַ����󲿷ֶ�����ͬ�ģ����Խ����Բ�ͬ�Ĳ�����ȡ������ϳ�һ��ͳһ�Ŀ�
       �������ֵĹ����ַ������ַ�ƫ�Ƹ����ο�FontLib_ICSTBL����
       
*/
#include "FUN_FontLib.h"
#include "string.h"
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
#define MAXFONTTABNUM                        40        // ������С(���ֿ��ļ����)

// �ֿ�ṹ��Ϣ��ַ 
#define CHECKFONTINFO                        0x23FFF0  //У����Ϣ���׵�ַ
#define FONTNUMINFO                          0x0A      // �����������Ϣ
#define FONTTABLEINFO                        0x20      // ������Ϣ��
#define FONTVERSINFO                         0x10      //�ֿ�汾
#define FONTVERSSIZE                         6         //�ֿ�汾���ֽ���
#define MODEADDRESS                          0x09      //����ȡģʽ��ʽ��ַ

//GB18030�����궨��
//˫�ֽڸ������ַ���
#define FontLib_Chinese2ByteArea1_CharaNum        846       //����2�ֽ�����1(ͼ����)�ַ���
#define FontLib_Chinese2ByteArea2_CharaNum        6768      //����2�ֽ�����2(������)�ַ���
#define FontLib_Chinese2ByteArea3_CharaNum        6080      //����2�ֽ�����3(������)�ַ���(��������ֽڵ�0x7Fֵ��Ч)
#define FontLib_Chinese2ByteArea4_CharaNum        8160      //����2�ֽ�����4(������)�ַ���(��������ֽڵ�0x7Fֵ��Ч)
#define FontLib_Chinese2ByteArea5_CharaNum        192       //����2�ֽ�����5(ͼ����)�ַ���(��������ֽڵ�0x7Fֵ��Ч)
//���ֽڸ������ַ�����7721���ַ�
#define FontLib_Chinese4Byte_8139Area0_CharaNum   1         //����4�ֽ�����0�ַ���
#define FontLib_Chinese4Byte_8139Area1_CharaNum   160       //����4�ֽ�����1�ַ���
#define FontLib_Chinese4Byte_8230Area_CharaNum    1260      //����4�ֽ�����2�ַ���
#define FontLib_Chinese4Byte_8231Area_CharaNum    1260      //����4�ֽ�����3�ַ���
#define FontLib_Chinese4Byte_8232Area_CharaNum    1260      //����4�ֽ�����4�ַ���
#define FontLib_Chinese4Byte_8233Area_CharaNum    1260      //����4�ֽ�����5�ַ���
#define FontLib_Chinese4Byte_8234Area_CharaNum    1260      //����4�ֽ�����6�ַ���
#define FontLib_Chinese4Byte_8235Area_CharaNum    1260      //����4�ֽ�����7�ַ���


/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ��ı������Ͷ�����------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
typedef struct 
{
     u8 size;       // �ֺ�: ��ʾ�����С��Ϣ����λ:�� 
     u8 type;  	// ����: Roman/OCR/.../����/����/... 
     u8 name[4];    // ����(������Ϣ0812,1224,,,) 
     u8 col;        // �����п���λ:�� 
     u8 raw;        // �����иߣ���λ:��  
     u16 codepage;  // ���� code page
     u16 offset;    // ÿ������ռ�ֽ��� 
     u32 baseaddr;  // �ֿ���ʼ��ַ 
}FONT_STRU;

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����ڲ�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static s8 FontLib_1Byte(u8 *intex,u32 *baseadd,u32 *offset,u8 *dp);
static s8 FontLib_2Byte(u8 *intex,u32 *baseadd,u32 *offset,u8 *dp);
static s8 FontLib_4Byte(u8 *intex,u32 *baseadd,u32 *offset,u8 *dp);

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ��ı���������--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static FONT_STRU fonttabinfo[MAXFONTTABNUM];
static u8 ASCIIIntex;//����ASCII������
static u8 ChinaIntex;//����-������
static u8 CodePageIntex;//����-CodePage������
static u8 ICSIntex;//����-�����ַ���������
static u8 ICSN;//�����ַ��������궨��
static u8 FontLibMode;//ȡģʽ��ʽ
static u8 FontVers[FONTVERSSIZE + 1];//�ֿ�汾��

//�����ַ���������
static const u8 FontLib_InterCharSet[]=
{
	0x23,//#
	0x24,//$
	0x40,//@
	0x5B,//[
	0x5C,//\ ,
	0x5D,//]
	0x5E,//^
	0x60,//`
	0x7B,//{
	0x7C,//|
	0x7D,//}
	0x7E,//~
};
//�����ַ���ƫ�Ʊ�
static const u16 FontLib_ICSTBL[]=
{
	0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,		// (0)���� USA              
	0,	1,	12,	13,	14,	15,	6,	7,	16,	17,	18,	19,		// (1)���� France           
	0,	1,	15,	20,	21,	22,	6,	7,	23,	24,	25,	26,		// (2)�¹� Germany          
	27,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,		// (3)Ӣ�� UK               
	0,	1,	2,	28,	29,	56,	6,	7,	57,	34,	30,	11,		// (4)����1 Denmark I        
	0,	31,	32,	20,	21,	56,	22,	16,	23,	24,	30,	25,		// (5)��� Sweden           
	0,	1,	2,	13,	4,	16,	6,	17,	12,	55,	18,	33,		// (6)����� Italy            
	35,	1,	2,	36,	37,	38,	6,	7,	19,	39,	10,	11,		// (7)������1 Spain I          
	0,	1,	2,	3,	40,	5,	6,	7,	8,	9,	10,	11,		// (8)�ձ� Japan            
	0,	31,	32,	28,	29,	56,	22,	16,	57,	34,	30,	25,		// (9)Ų�� Norway           
	0,	1,	32,	28,	29,	56,	22,	16,	57,	34,	30,	25,		// (10)����2 Denmark II     
	0,	1,	58,	36,	37,	38,	16,	7,	41,	39,	42,	43,		// (11)������2 Spain II       
	0,	1,	58,	36,	37,	38,	16,	25,	41,	39,	42,	43,		// (12)�������� Latin America  
	0,	1,	2,	3,	44,	5,	6,	7,	8,	9,	10,	11,		// (13)���� Korea          
	0,	1,	45,	46,	47,	48,	49,	50,	51,	52,	53,	54,		// (14)˹�������� Slovenia  
	0,	40,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,		// (15)���� Chinese
};
#define FontLib_ICSNum   sizeof(FontLib_InterCharSet)   //�����ַ�������

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ϵͳ����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
������:FUN_FontLib_Config
��   ��:�ֿ����ú���
��   ��:
       ReadFunP rp:�����Ͷ�����ָ��
       u32 fadd:�ֿ���ʼ��ַ(ƫ�Ƶ�ַ)
����ֵ:
       0:���óɹ�
   С��0:����ʧ��
ע   ��:
       
ʾ   ��:
��   ��:YF
��   ��:V1.0
ʱ   ��:2015-03-16
-------------------------------------------------*/
s8 FUN_FontLib_Config(ReadFunP rp,u32 fadd)
{
     u32 addr;
     u16 size;
     u8 OPBuffer[8];//����һ����������
     u8 num[2];
     u8 u8intex;
     u16 font_num;
     
     //У���ֿ��Ƿ���ȷ
     addr = (fadd + CHECKFONTINFO);
     size = 7;
     rp(addr,OPBuffer,size);
     OPBuffer[7] = '\0';     
     if(strcmp((char *)OPBuffer,"GB18030"))
     {//У�����
          return -1;
     }
     /* ��ȡ�������Ŀ */
     addr = (fadd + FONTNUMINFO);
     size = 2;
     rp(addr,(u8 *)num,size);//������ ��ȡ�ֿ�����
     font_num = (u16)num[1]*0x100 + num[0];
     
     if(font_num > MAXFONTTABNUM)
     {
          return -1;
          //font_num = MAXFONTTABNUM;
     }
     	
     // ��ȡ�ֿ���Ϣ�� 
     addr = (fadd + FONTTABLEINFO);
     size = sizeof(FONT_STRU);
     rp(addr,(u8 *)fonttabinfo,(size * font_num));//������ ��ȡ�ֿ�����
     
     //��ȡ�ֿ�汾
     for(size = 0;size < (FONTVERSSIZE + 1);size++)
     {
          FontVers[size] = '\0';
     }
     addr = (fadd + FONTVERSINFO);
     size = FONTVERSSIZE;
     rp(addr,FontVers,size);
     
     //��ȡ����ȡģʽ��ʽ
     addr = (fadd + MODEADDRESS);
     size = 1;
     rp(addr,(u8 *)&FontLibMode,size);//������ ��ȡ�ֿ�����

     //�������ų�ʼֵ(������1��ƥ���ΪĬ��ֵ)
     for(u8intex = 0; u8intex < MAXFONTTABNUM;u8intex++)
     {
          if(fonttabinfo[u8intex].codepage == FontLib_CodePage_ASCII)
          {
               ASCIIIntex = u8intex;
               break;
          }
     }
     for(u8intex = 0; u8intex < MAXFONTTABNUM;u8intex++)
     {
          if(fonttabinfo[u8intex].codepage == FontLib_CodePage_GB18030)
          {
               ChinaIntex = u8intex;
               break;
          }
     }
     for(u8intex = 0; u8intex < MAXFONTTABNUM;u8intex++)
     {
          if(fonttabinfo[u8intex].codepage == FontLib_CodePage_CP437)
          {
               CodePageIntex = u8intex;
               break;
          }
     }
     for(u8intex = 0; u8intex < MAXFONTTABNUM;u8intex++)
     {
          if(fonttabinfo[u8intex].codepage == FontLib_CodePage_ICS)
          {
               ICSIntex = u8intex;
               ICSN = FontLib_ICSN_USA;
               break;
          }
     }      
     return 0;
}



/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����û�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
������:FUN_FontLib_Set
��   ��:����ָ����������塢�ֺš�������Ϣ
��   ��:
       u8 fontflag:���ñ�־(0:ASCII 1:�����ַ��� 2:CodePage 3:����)
       u8 fontsize:�ֺţ�ȡֵ�ο���.h���ֺŲ����궨�塷
       u8 fonttype:���壬ȡֵ�ο���.h����������궨�塷
       u32 fontobj:�����ԣ�ȡֵ�ο���.h��CodePage�����궨�� �� �����ַ��������궨�塷
����ֵ:       
       С��0:����ʧ��
       ����ֵ:���óɹ���������
ע   ��:
       ASCII�����Ĳ�����fontobj��������������ֵ
       ���á������ַ�����ʱ��fontobj�ο�.h�ġ������ַ��������궨�塷
       ���á�CodePage��ʱ��fontobj�ο�.h�ġ�CodePage�����궨�塷
ʾ   ��:
��   ��:YF
��   ��:V1.0
ʱ   ��:2015-03-16
-------------------------------------------------*/
s8 FUN_FontLib_Set(u8 fontflag,u8 fontsize, u8 fonttype, u32 fontobj)
{
	u8 i;
	u8 index;

     if(fontflag > 3)
     {//Խ��
          return -1;
     }
     
     if(fontflag == 1)
     {//�����ַ���
     //�˴�֮���Թ����ַ�����ô������Ҫԭ�����ֿ��ｫ��ͬ���ҵĹ����ַ���������һ��
     //��Ϊ�󲿷ֹ����ַ����ĵ�����һ���ģ����Ի����һ��ͳһ��һ��FontLib_CodePage_ICS��ʶ��
     //��ͬ������Ϣ�ݴ���ICSN����ڼ���ƫ����
          ICSN = fontobj;//���������
          fontobj = FontLib_CodePage_ICS;//�����ַ���ʶ��̶�Ϊ1
     }
     
     index = 0;
	for(i = 0; i < MAXFONTTABNUM;i++)
	{
		// ����ƥ����ֺ� 
		if(fonttabinfo[i].size == fontsize)	// ƥ�� 
		{
			// ����ƥ������� 
			if(fonttabinfo[i].type == fonttype)// ƥ�� 
			{
				// ����ƥ���code page 
				if(fonttabinfo[i].codepage == fontobj)	// ƥ�� 
				{
					index = i;
					break;
				}
			}	
		}
	}
	
	if(i >= MAXFONTTABNUM)
	{
		return -1;
     }
     if(fontflag == 0)
     {//ASCII
          ASCIIIntex = index;
     }
     else if(fontflag == 1)
     {//�����ַ���
          ICSIntex = index;
     }
     else if(fontflag == 2)
     {//CodePage
          CodePageIntex = index;
     }
     else if(fontflag == 3)
     {//����
          ChinaIntex = index;
     }
     return 0;
}

/*-------------------------------------------------
������:FUN_FontLib_OutCharaINF
��   ��:����FUN_FontLib_Set�����õĲ���������ַ���Ϣ
��   ��:
       u8 *dp:�ַ������׵�ַ
       u8 dl:�ַ������ֽ���
       FUN_FontLib_DEF_CharaINF *infp:�ַ���Ϣ��ŵĽṹָ��
����ֵ:       
       С��0:���ʧ��
       ����0:����ɹ�
ע   ��:
       �ֺš����塢�������FUN_FontLib_SetԤ������
ʾ   ��:
��   ��:YF
��   ��:V1.0
ʱ   ��:2015-03-16
-------------------------------------------------*/
s8 FUN_FontLib_OutCharaINF(u8 *dp,u8 dl,FUN_FontLib_DEF_CharaINF *infp)
{
     s8 s8result = 0;
     u8 u8Code[4];
     u8 u8intex;
     u32 u32offset;
     u32 u32baseadd;
     
     if(dl == 0)
     {
          s8result = -1;
     }
     else if(dl == 1)
     {//�����ַ�
          u8Code[0] = dp[0];
          if(FontLib_1Byte(&u8intex,&u32baseadd,&u32offset,u8Code) < 0)
          {
               s8result = -2;
          }   
     }
     else if(dl == 2)
     {//˫�ֽ������ַ�
          u8Code[0] = dp[0];
          u8Code[1] = dp[1];
          if(FontLib_2Byte(&u8intex,&u32baseadd,&u32offset,u8Code) < 0)
          {
               s8result = -2;
          }
     }
     else if(dl == 4)
     {//���ֽ������ַ�
          u8Code[0] = dp[0];
          u8Code[1] = dp[1];
          u8Code[2] = dp[2];
          u8Code[3] = dp[3];
          if(FontLib_4Byte(&u8intex,&u32baseadd,&u32offset,u8Code) < 0)
          {
               s8result = -2;
          }
     }
     else
     {
          s8result = -3;
     }

     if(s8result == 0)
     {
          infp->size = fonttabinfo[u8intex].offset;//�ַ������ֽ���
          infp->address = (u32baseadd + fonttabinfo[u8intex].offset * u32offset);//�ַ�������ʼ��ַ
          infp->col = fonttabinfo[u8intex].col;//�ַ������ȣ���λ��
          infp->raw = fonttabinfo[u8intex].raw;//�ַ�����߶ȣ���λ��
          infp->mode = FontLibMode;//�ַ�ȡģʽ��ʽ
     }
     return s8result;
}

/*-------------------------------------------------
������:FUN_FontLib_BaseSize_OutCharaINF
��   ��:�����ֺŲ����������Ӧ�ַ���Ϣ
��   ��:
       u8 fintsize:�ֺţ�ȡֵ�ο���.h���ֺŲ����궨�塷
       u8 *dp:�ַ������׵�ַ
       u8 dl:�ַ������ֽ���
       FUN_FontLib_DEF_CharaINF *infp:�ַ���Ϣ��ŵĽṹָ��
����ֵ:       
       С��0:���ʧ��
       ����0:����ɹ�
ע   ��:
       ��0����ֻ�������ֺ���Ч�������ֺŲ�����
       ��1��������ǰ������ȵ���FUN_FontLib_Set�����塢���Խ���Ԥ����
       ���������FUN_FontLib_Set����ģ��ʹ��FUN_FontLib_Config�����е�Ĭ��ֵ
       ��2��������ı�FUN_FontLib_Set���������õ�ֵ
ʾ   ��:
��   ��:YF
��   ��:V1.0
ʱ   ��:2015-03-16
-------------------------------------------------*/
s8 FUN_FontLib_BaseSize_OutCharaINF(u8 fintsize,u8 *dp,u8 dl,FUN_FontLib_DEF_CharaINF *infp)
{
     u8 u8chinaintex,u8asciiintex,u8codepageintex,u8icsintex;
     s8 s8result = 0;
     u8 u8size,u8type;
     u32 u32obj;

     //����ԭ����
     u8chinaintex = ChinaIntex;
     u8asciiintex = ASCIIIntex;
     u8codepageintex = CodePageIntex;
     u8icsintex = ICSIntex;
     
     if(s8result >= 0)
     {//����ASCII�������
          u8size = fintsize;//�ֺ�
          u8type = fonttabinfo[u8asciiintex].type;//����
          u32obj = fonttabinfo[u8asciiintex].codepage;     
          s8result = FUN_FontLib_Set(0,u8size,u8type,u32obj);
     }
     
     if(s8result >= 0)
     {//���ù����ַ����������
          u8size = fintsize;//�ֺ�
          u8type = fonttabinfo[u8icsintex].type;//����
          u32obj = ICSN;     
          s8result = FUN_FontLib_Set(1,u8size,u8type,u32obj);
     }
     
     if(s8result >= 0)
     {//����CodePage�������
          u8size = fintsize;//�ֺ�
          u8type = fonttabinfo[u8codepageintex].type;//����
          u32obj = fonttabinfo[u8codepageintex].codepage;     
          s8result = FUN_FontLib_Set(2,u8size,u8type,u32obj);
     }
     
     //if(s8result >= 0)
     //{//�������ĵ������
     //     u8size = fintsize;//�ֺ�
     //     u8type = fonttabinfo[u8chinaintex].type;//����
     //     u32obj = fonttabinfo[u8chinaintex].codepage;     
     //     s8result = FUN_FontLib_Set(3,u8size,u8type,u32obj);
     //}
     
     if(s8result >= 0)
     {
          s8result = FUN_FontLib_OutCharaINF(dp,dl,infp);
     }
     
     //�ָ�ԭ����
     ChinaIntex = u8chinaintex;
     ASCIIIntex = u8asciiintex;
     CodePageIntex = u8codepageintex;
     ICSIntex = u8icsintex;
     
     return s8result;
}



/*-------------------------------------------------
������:FUN_FontLib_OutLibVersion
��   ��:����ֿ�汾��Ϣ
��   ��:
       u8 *vp:���ذ汾��Ϣ���ݴ�ŵ��׵�ַ
       u8 vl:���ڴ�Ű汾��Ϣ���ݿռ���ֽ���
����ֵ:       
       С��0:���ʧ��
       ����0:����ɹ�
ע   ��:
       
ʾ   ��:
��   ��:YF
��   ��:V1.0
ʱ   ��:2015-03-16
-------------------------------------------------*/
s8 FUN_FontLib_OutLibVersion(u8 *vp,u8 vl)
{
     u8 u8intex;
     if(vl <= FONTVERSSIZE)
     {//�ռ䲻��
          return -1;
     }

     for(u8intex = 0;u8intex < (FONTVERSSIZE + 1);u8intex++)
     {
          vp[u8intex] = FontVers[u8intex];
     }          
     return 0;
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����ڲ�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
//intex:���������� baseadd:���ػ�ַ offset:����ƫ���ַ��� dp:�ַ������׵�ַ
static s8 FontLib_1Byte(u8 *intex,u32 *baseadd,u32 *offset,u8 *dp)
{
     u8 u8intex;
     u32 u32offset;//ƫ���ַ���
     
     if(dp[0] < 0x80)
     {//ASCII��
          if((dp[0] < 0x20) || (dp[0] == 0x7f))
          {//��Ч���룬�����ݵ���
               return -1;
          }
          // ���ݹ����ַ���������,ѡ����� 
          // �ж��Ƿ����ڹ����ַ������ַ� 
          for(u8intex = 0;u8intex < FontLib_ICSNum;u8intex++)
          {
               if(dp[0] == FontLib_InterCharSet[u8intex])// �ǹ����ַ���
               {
                    break;
               }
          }

          if(u8intex >= FontLib_ICSNum)
          {//ASCII                  
               u32offset = (u32)(dp[0] - 0x20);//��ΪASCII�ӿո�ʼ�ŵ���
               u8intex = ASCIIIntex;
          }
          else
          {//�����ַ���
               //����������õ�u8intexֵ������u8intex = ICSIntex;��䲻�ɷ���ǰ
               u32offset = (u32)FontLib_ICSTBL[u8intex + FontLib_ICSNum * ICSN];
               u8intex = ICSIntex;
          }
     }
     else
     {//����CodePage��
          u32offset = (u32)(dp[0] - 0x80);//��ΪCodePage��0x80��ʼ�ŵ���
          u8intex = CodePageIntex; 
     }

     *intex = u8intex;
     *baseadd = fonttabinfo[u8intex].baseaddr;
     *offset = u32offset;
     return 0;
}

//intex:���������� baseadd:���ػ�ַ offset:����ƫ���ַ��� dp:�ַ������׵�ַ
static s8 FontLib_2Byte(u8 *intex,u32 *baseadd,u32 *offset,u8 *dp)
{
     u8 u8intex = ChinaIntex;
     s8 s8temp = -1;
     u32 u32areaoffset;//����ƫ���ֽ���
     u32 u32offset;//ƫ���ַ���

     //�ж�1��
     if(s8temp == -1)
     {
          if((dp[0] >= 0xa1) && (dp[0] <= 0xa9))
          {
               if((dp[1] >= 0xa1) && (dp[1] <= 0xfe))
               {
                    u32areaoffset = 0;

                    u32areaoffset *= fonttabinfo[u8intex].offset;//����1ƫ����
                    u32offset = ((dp[0] - 0xa1) * (0xfe - 0xa1 + 1) + (dp[1] - 0xa1));//����ƫ���ַ���
                    s8temp = 0;
               }
          }
     }
     //�ж�2��
     if(s8temp == -1)
     {
          if((dp[0] >= 0xb0) && (dp[0] <= 0xf7))
          {
               if((dp[1] >= 0xa1) && (dp[1] <= 0xfe))
               {
                    u32areaoffset = FontLib_Chinese2ByteArea1_CharaNum;

                    u32areaoffset *= fonttabinfo[u8intex].offset;//����2ƫ����
                    u32offset = ((dp[0] - 0xb0) * (0xfe - 0xa1 + 1) + (dp[1] - 0xa1));//����ƫ���ַ���
                    s8temp = 0;
               }
          }
     }
     //�ж�3��
     if(s8temp == -1)
     {
          if((dp[0] >= 0x81) && (dp[0] <= 0xa0))
          {
               if((dp[1] >= 0x40) && (dp[1] <= 0xfe))
               {
                    if(dp[1] != 0x7f)
                    {
                         u32areaoffset = FontLib_Chinese2ByteArea1_CharaNum;
                         u32areaoffset += FontLib_Chinese2ByteArea2_CharaNum;
                         
                         u32areaoffset *= fonttabinfo[u8intex].offset;//����3ƫ����
                         u32offset = ((dp[0] - 0x81) * (0xfe - 0x40) + (dp[1] - 0x40));//����ƫ���ַ���
                         if(dp[1] > 0x7f)
                         {
                              u32offset--;//�۵�0x7f��1��ƫ��
                         }
                         s8temp = 0;
                    }
               }
          }
     }
     //�ж�4��
     if(s8temp == -1)
     {
          if((dp[0] >= 0xaa) && (dp[0] <= 0xfe))
          {
               if((dp[1] >= 0x40) && (dp[1] <= 0xa0))
               {
                    if(dp[1] != 0x7f)
                    {
                         u32areaoffset = FontLib_Chinese2ByteArea1_CharaNum;
                         u32areaoffset += FontLib_Chinese2ByteArea2_CharaNum;
                         u32areaoffset += FontLib_Chinese2ByteArea3_CharaNum;
                         
                         u32areaoffset *= fonttabinfo[u8intex].offset;//����4ƫ����
                         u32offset = ((dp[0] - 0xaa) * (0xa0 - 0x40) + (dp[1] - 0x40));//����ƫ���ַ���
                         if(dp[1] > 0x7f)
                         {
                              u32offset--;//�۵�0x7f��1��ƫ��
                         }                         
                         s8temp = 0;
                    }
               }
          }
     }
     //�ж�5��
     if(s8temp == -1)
     {
          if((dp[0] >= 0xa8) && (dp[0] <= 0xa9))
          {
               if((dp[1] >= 0x40) && (dp[1] <= 0xa0))
               {
                    if(dp[1] != 0x7f)
                    {
                         u32areaoffset = FontLib_Chinese2ByteArea1_CharaNum;
                         u32areaoffset += FontLib_Chinese2ByteArea2_CharaNum;
                         u32areaoffset += FontLib_Chinese2ByteArea3_CharaNum;
                         u32areaoffset += FontLib_Chinese2ByteArea4_CharaNum;
                         
                         u32areaoffset *= fonttabinfo[u8intex].offset;//����5ƫ����
                         u32offset = ((dp[0] - 0xa8) * (0xa0 - 0x40) + (dp[1] - 0x40));//����ƫ���ַ���
                         if(dp[1] > 0x7f)
                         {
                              u32offset--;//�۵�0x7f��1��ƫ��
                         }
                         s8temp = 0;
                    }
               }
          }
     }

     if(s8temp == 0)
     {
          *intex = u8intex;
          *baseadd = (fonttabinfo[u8intex].baseaddr + u32areaoffset);
          *offset = u32offset; 
     }
     return s8temp;
}

//intex:���������� baseadd:���ػ�ַ offset:����ƫ���ַ��� dp:�ַ������׵�ַ
static s8 FontLib_4Byte(u8 *intex,u32 *baseadd,u32 *offset,u8 *dp)
{
     u8 u8intex = ChinaIntex;
     s8 s8temp = -1;
     u32 u32areaoffset;
     u32 u32offset;//ƫ���ַ���

     u32areaoffset = FontLib_Chinese2ByteArea1_CharaNum;
     u32areaoffset += FontLib_Chinese2ByteArea2_CharaNum;
     u32areaoffset += FontLib_Chinese2ByteArea3_CharaNum;
     u32areaoffset += FontLib_Chinese2ByteArea4_CharaNum;
     u32areaoffset += FontLib_Chinese2ByteArea5_CharaNum;
     u32areaoffset *= fonttabinfo[u8intex].offset;//����ƫ���ֽ���
     
     //8139��
     if(s8temp == -1)
     {
          if((dp[0] == 0x81) && (dp[1] == 0x39))
          {
               if((dp[2] == 0xee) && (dp[3] == 0x39))
               {//8139����0
                    u32offset = 0;//ƫ��0���ַ�
                    s8temp = 0;
               }
               else if((dp[2] >= 0xef) && (dp[2] <= 0xfe))
               {
                    if((dp[3] >= 0x30) && (dp[3] <= 0x39))
                    {//8139����1
                         u32offset = ((dp[2] - 0xef) * (0x39 - 0x30 + 1) + (dp[3] - 0x30));//����ƫ���ַ���
                         u32offset += FontLib_Chinese4Byte_8139Area0_CharaNum;//��������0���ַ���
                         s8temp = 0;
                    }
               }
          }
     }
     //8230��
     if(s8temp == -1)
     {
          if((dp[0] == 0x82) && (dp[1] == 0x30))
          {
               if((dp[2] >= 0x81) && (dp[2] <= 0xfe))
               {
                    if((dp[3] >= 0x30) && (dp[3] <= 0x39))
                    {//8230����
                         u32offset = ((dp[2] - 0x81) * (0x39 - 0x30 + 1) + (dp[3] - 0x30));//����ƫ���ַ���
                         u32offset += FontLib_Chinese4Byte_8139Area0_CharaNum;//����8139����0���ַ���
                         u32offset += FontLib_Chinese4Byte_8139Area1_CharaNum;//����8139����1���ַ���
                         s8temp = 0;
                    }
               }
          }
     }
     //8231��
     if(s8temp == -1)
     {
          if((dp[0] == 0x82) && (dp[1] == 0x31))
          {
               if((dp[2] >= 0x81) && (dp[2] <= 0xfe))
               {
                    if((dp[3] >= 0x30) && (dp[3] <= 0x39))
                    {//8231����
                         u32offset = ((dp[2] - 0x81) * (0x39 - 0x30 + 1) + (dp[3] - 0x30));//����ƫ���ַ���
                         u32offset += FontLib_Chinese4Byte_8139Area0_CharaNum;//����8139����0���ַ���
                         u32offset += FontLib_Chinese4Byte_8139Area1_CharaNum;//����8139����1���ַ���
                         u32offset += FontLib_Chinese4Byte_8230Area_CharaNum;//����8230������ַ���
                         s8temp = 0;
                    }
               }
          }
     }
     //8232��
     if(s8temp == -1)
     {
          if((dp[0] == 0x82) && (dp[1] == 0x32))
          {
               if((dp[2] >= 0x81) && (dp[2] <= 0xfe))
               {
                    if((dp[3] >= 0x30) && (dp[3] <= 0x39))
                    {//8232����
                         u32offset = ((dp[2] - 0x81) * (0x39 - 0x30 + 1) + (dp[3] - 0x30));//����ƫ���ַ���
                         u32offset += FontLib_Chinese4Byte_8139Area0_CharaNum;//����8139����0���ַ���
                         u32offset += FontLib_Chinese4Byte_8139Area1_CharaNum;//����8139����1���ַ���
                         u32offset += FontLib_Chinese4Byte_8230Area_CharaNum;//����8230������ַ���
                         u32offset += FontLib_Chinese4Byte_8231Area_CharaNum;//����8231������ַ���
                         s8temp = 0;
                    }
               }
          }
     }
     //8233��
     if(s8temp == -1)
     {
          if((dp[0] == 0x82) && (dp[1] == 0x33))
          {
               if((dp[2] >= 0x81) && (dp[2] <= 0xfe))
               {
                    if((dp[3] >= 0x30) && (dp[3] <= 0x39))
                    {//8233����
                         u32offset = ((dp[2] - 0x81) * (0x39 - 0x30 + 1) + (dp[3] - 0x30));//����ƫ���ַ���
                         u32offset += FontLib_Chinese4Byte_8139Area0_CharaNum;//����8139����0���ַ���
                         u32offset += FontLib_Chinese4Byte_8139Area1_CharaNum;//����8139����1���ַ���
                         u32offset += FontLib_Chinese4Byte_8230Area_CharaNum;//����8230������ַ���
                         u32offset += FontLib_Chinese4Byte_8231Area_CharaNum;//����8231������ַ���
                         u32offset += FontLib_Chinese4Byte_8232Area_CharaNum;//����8232������ַ���
                         s8temp = 0;
                    }
               }
          }
     }
     //8234��
     if(s8temp == -1)
     {
          if((dp[0] == 0x82) && (dp[1] == 0x34))
          {
               if((dp[2] >= 0x81) && (dp[2] <= 0xfe))
               {
                    if((dp[3] >= 0x30) && (dp[3] <= 0x39))
                    {//8234����
                         u32offset = ((dp[2] - 0x81) * (0x39 - 0x30 + 1) + (dp[3] - 0x30));//����ƫ���ַ���
                         u32offset += FontLib_Chinese4Byte_8139Area0_CharaNum;//����8139����0���ַ���
                         u32offset += FontLib_Chinese4Byte_8139Area1_CharaNum;//����8139����1���ַ���
                         u32offset += FontLib_Chinese4Byte_8230Area_CharaNum;//����8230������ַ���
                         u32offset += FontLib_Chinese4Byte_8231Area_CharaNum;//����8231������ַ���
                         u32offset += FontLib_Chinese4Byte_8232Area_CharaNum;//����8232������ַ���
                         u32offset += FontLib_Chinese4Byte_8233Area_CharaNum;//����8233������ַ���
                         s8temp = 0;
                    }
               }
          }
     }
     //8235��
     if(s8temp == -1)
     {
          if((dp[0] == 0x82) && (dp[1] == 0x35))
          {
               if((dp[2] >= 0x81) && (dp[2] <= 0xfe))
               {
                    if((dp[3] >= 0x30) && (dp[3] <= 0x39))
                    {//8235����
                         u32offset = ((dp[2] - 0x81) * (0x39 - 0x30 + 1) + (dp[3] - 0x30));//����ƫ���ַ���
                         u32offset += FontLib_Chinese4Byte_8139Area0_CharaNum;//����8139����0���ַ���
                         u32offset += FontLib_Chinese4Byte_8139Area1_CharaNum;//����8139����1���ַ���
                         u32offset += FontLib_Chinese4Byte_8230Area_CharaNum;//����8230������ַ���
                         u32offset += FontLib_Chinese4Byte_8231Area_CharaNum;//����8231������ַ���
                         u32offset += FontLib_Chinese4Byte_8232Area_CharaNum;//����8232������ַ���
                         u32offset += FontLib_Chinese4Byte_8233Area_CharaNum;//����8233������ַ���
                         u32offset += FontLib_Chinese4Byte_8234Area_CharaNum;//����8234������ַ���
                         s8temp = 0;
                    }
               }
          }
     }
     
     if(s8temp == 0)
     {
          *intex = u8intex;
          *baseadd = (fonttabinfo[u8intex].baseaddr + u32areaoffset);
          *offset = u32offset; //ƫ���ַ���
     }
     return s8temp;
}


