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
【1】、GB18030点阵数据说明
       （1）、由于GB18030中文点阵标准数据量很大，而存放空间有限，故只提取其中各别编码区的字符点阵数据，并将各区的点数数据进行连接组合起来
       连接组合顺序为：
       双字节区域1点阵数据
       双字节区域2点阵数据
       双字节区域3点阵数据
       双字节区域4点阵数据
       双字节区域5点阵数据
       四字节8139区域0点阵数据
       四字节8139区域1点阵数据
       四字节8230区域点阵数据
       四字节8231区域点阵数据
       四字节8232区域点阵数据
       四字节8233区域点阵数据
       四字节8234区域点阵数据
       四字节8235区域点阵数据
       （2）、采用各区域的字符数为准，来计算偏移量，所以不管字号大小如何变，只要各区域的字符数不变，则点阵定位也是正确的

【2】、国际字符集       
       由于国际字符集大部分都是相同的，所以将各自不同的部分提取出来组合成一个统一的库
       各个国字的国际字符集的字符偏移个数参考FontLib_ICSTBL数组
       
*/
#include "FUN_FontLib.h"
#include "string.h"
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
#define MAXFONTTABNUM                        40        // 点阵表大小(与字库文件相关)

// 字库结构信息地址 
#define CHECKFONTINFO                        0x23FFF0  //校验信息的首地址
#define FONTNUMINFO                          0x0A      // 点阵表数量信息
#define FONTTABLEINFO                        0x20      // 点阵信息表
#define FONTVERSINFO                         0x10      //字库版本
#define FONTVERSSIZE                         6         //字库版本号字节数
#define MODEADDRESS                          0x09      //点阵取模式方式地址

//GB18030参数宏定义
//双字节各区域字符数
#define FontLib_Chinese2ByteArea1_CharaNum        846       //中文2字节区域1(图形区)字符数
#define FontLib_Chinese2ByteArea2_CharaNum        6768      //中文2字节区域2(汉字区)字符数
#define FontLib_Chinese2ByteArea3_CharaNum        6080      //中文2字节区域3(汉字区)字符数(编码最低字节的0x7F值无效)
#define FontLib_Chinese2ByteArea4_CharaNum        8160      //中文2字节区域4(汉字区)字符数(编码最低字节的0x7F值无效)
#define FontLib_Chinese2ByteArea5_CharaNum        192       //中文2字节区域5(图形区)字符数(编码最低字节的0x7F值无效)
//四字节各区域字符数共7721个字符
#define FontLib_Chinese4Byte_8139Area0_CharaNum   1         //中文4字节区域0字符数
#define FontLib_Chinese4Byte_8139Area1_CharaNum   160       //中文4字节区域1字符数
#define FontLib_Chinese4Byte_8230Area_CharaNum    1260      //中文4字节区域2字符数
#define FontLib_Chinese4Byte_8231Area_CharaNum    1260      //中文4字节区域3字符数
#define FontLib_Chinese4Byte_8232Area_CharaNum    1260      //中文4字节区域4字符数
#define FontLib_Chinese4Byte_8233Area_CharaNum    1260      //中文4字节区域5字符数
#define FontLib_Chinese4Byte_8234Area_CharaNum    1260      //中文4字节区域6字符数
#define FontLib_Chinese4Byte_8235Area_CharaNum    1260      //中文4字节区域7字符数


/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的变量类型定义区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
typedef struct 
{
     u8 size;       // 字号: 提示点阵大小信息，单位:点 
     u8 type;  	// 字体: Roman/OCR/.../宋体/黑体/... 
     u8 name[4];    // 名称(辅助信息0812,1224,,,) 
     u8 col;        // 点阵列宽，单位:点 
     u8 raw;        // 点阵行高，单位:点  
     u16 codepage;  // 国别 code page
     u16 offset;    // 每个字所占字节数 
     u32 baseaddr;  // 字库起始地址 
}FONT_STRU;

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数申明区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static s8 FontLib_1Byte(u8 *intex,u32 *baseadd,u32 *offset,u8 *dp);
static s8 FontLib_2Byte(u8 *intex,u32 *baseadd,u32 *offset,u8 *dp);
static s8 FontLib_4Byte(u8 *intex,u32 *baseadd,u32 *offset,u8 *dp);

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的变量申明区--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static FONT_STRU fonttabinfo[MAXFONTTABNUM];
static u8 ASCIIIntex;//西文ASCII索引号
static u8 ChinaIntex;//中文-索引号
static u8 CodePageIntex;//西文-CodePage索引号
static u8 ICSIntex;//西文-国际字符集索引号
static u8 ICSN;//国际字符集参数宏定义
static u8 FontLibMode;//取模式方式
static u8 FontVers[FONTVERSSIZE + 1];//字库版本号

//国际字符集的内容
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
//国际字符集偏移表
static const u16 FontLib_ICSTBL[]=
{
	0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,		// (0)美国 USA              
	0,	1,	12,	13,	14,	15,	6,	7,	16,	17,	18,	19,		// (1)法国 France           
	0,	1,	15,	20,	21,	22,	6,	7,	23,	24,	25,	26,		// (2)德国 Germany          
	27,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,		// (3)英国 UK               
	0,	1,	2,	28,	29,	56,	6,	7,	57,	34,	30,	11,		// (4)丹麦1 Denmark I        
	0,	31,	32,	20,	21,	56,	22,	16,	23,	24,	30,	25,		// (5)瑞典 Sweden           
	0,	1,	2,	13,	4,	16,	6,	17,	12,	55,	18,	33,		// (6)意大利 Italy            
	35,	1,	2,	36,	37,	38,	6,	7,	19,	39,	10,	11,		// (7)西班牙1 Spain I          
	0,	1,	2,	3,	40,	5,	6,	7,	8,	9,	10,	11,		// (8)日本 Japan            
	0,	31,	32,	28,	29,	56,	22,	16,	57,	34,	30,	25,		// (9)挪威 Norway           
	0,	1,	32,	28,	29,	56,	22,	16,	57,	34,	30,	25,		// (10)丹麦2 Denmark II     
	0,	1,	58,	36,	37,	38,	16,	7,	41,	39,	42,	43,		// (11)西班牙2 Spain II       
	0,	1,	58,	36,	37,	38,	16,	25,	41,	39,	42,	43,		// (12)拉丁美洲 Latin America  
	0,	1,	2,	3,	44,	5,	6,	7,	8,	9,	10,	11,		// (13)韩国 Korea          
	0,	1,	45,	46,	47,	48,	49,	50,	51,	52,	53,	54,		// (14)斯洛文尼亚 Slovenia  
	0,	40,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,		// (15)中文 Chinese
};
#define FontLib_ICSNum   sizeof(FontLib_InterCharSet)   //国际字符集个数

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的系统函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
函数名:FUN_FontLib_Config
功   能:字库配置函数
参   数:
       ReadFunP rp:阻塞型读函数指针
       u32 fadd:字库起始地址(偏移地址)
返回值:
       0:配置成功
   小于0:配置失败
注   意:
       
示   例:
作   者:YF
版   本:V1.0
时   间:2015-03-16
-------------------------------------------------*/
s8 FUN_FontLib_Config(ReadFunP rp,u32 fadd)
{
     u32 addr;
     u16 size;
     u8 OPBuffer[8];//申明一个操作缓存
     u8 num[2];
     u8 u8intex;
     u16 font_num;
     
     //校验字库是否正确
     addr = (fadd + CHECKFONTINFO);
     size = 7;
     rp(addr,OPBuffer,size);
     OPBuffer[7] = '\0';     
     if(strcmp((char *)OPBuffer,"GB18030"))
     {//校验错误
          return -1;
     }
     /* 读取点阵表数目 */
     addr = (fadd + FONTNUMINFO);
     size = 2;
     rp(addr,(u8 *)num,size);//阻塞型 读取字库数据
     font_num = (u16)num[1]*0x100 + num[0];
     
     if(font_num > MAXFONTTABNUM)
     {
          return -1;
          //font_num = MAXFONTTABNUM;
     }
     	
     // 读取字库信息表 
     addr = (fadd + FONTTABLEINFO);
     size = sizeof(FONT_STRU);
     rp(addr,(u8 *)fonttabinfo,(size * font_num));//阻塞型 读取字库数据
     
     //读取字库版本
     for(size = 0;size < (FONTVERSSIZE + 1);size++)
     {
          FontVers[size] = '\0';
     }
     addr = (fadd + FONTVERSINFO);
     size = FONTVERSSIZE;
     rp(addr,FontVers,size);
     
     //读取点阵取模式方式
     addr = (fadd + MODEADDRESS);
     size = 1;
     rp(addr,(u8 *)&FontLibMode,size);//阻塞型 读取字库数据

     //各索引号初始值(碰到第1个匹配的为默认值)
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
*               ----------------------------------以下是模块的用户函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
函数名:FUN_FontLib_Set
功   能:设置指定点阵的字体、字号、语言信息
参   数:
       u8 fontflag:设置标志(0:ASCII 1:国际字符集 2:CodePage 3:中文)
       u8 fontsize:字号，取值参考《.h的字号参数宏定义》
       u8 fonttype:字体，取值参考《.h的字体参数宏定义》
       u32 fontobj:字语言，取值参考《.h的CodePage参数宏定义 或 国际字符集参数宏定义》
返回值:       
       小于0:设置失败
       其他值:设置成功的索引号
注   意:
       ASCII和中文不存在fontobj参数，可以任意值
       设置【国际字符集】时，fontobj参考.h的《国际字符集参数宏定义》
       设置【CodePage】时，fontobj参考.h的《CodePage参数宏定义》
示   例:
作   者:YF
版   本:V1.0
时   间:2015-03-16
-------------------------------------------------*/
s8 FUN_FontLib_Set(u8 fontflag,u8 fontsize, u8 fonttype, u32 fontobj)
{
	u8 i;
	u8 index;

     if(fontflag > 3)
     {//越界
          return -1;
     }
     
     if(fontflag == 1)
     {//国际字符集
     //此处之所以国际字符集这么处理，主要原因是字库里将不同国家的国际字符集整合在一起，
     //因为大部分国际字符集的点阵是一样的，所以混合在一起，统一由一个FontLib_CodePage_ICS来识别
     //不同国家信息暂存在ICSN里，用于计算偏移量
          ICSN = fontobj;//保存国家码
          fontobj = FontLib_CodePage_ICS;//国际字符集识别固定为1
     }
     
     index = 0;
	for(i = 0; i < MAXFONTTABNUM;i++)
	{
		// 查找匹配的字号 
		if(fonttabinfo[i].size == fontsize)	// 匹配 
		{
			// 查找匹配的字体 
			if(fonttabinfo[i].type == fonttype)// 匹配 
			{
				// 查找匹配的code page 
				if(fonttabinfo[i].codepage == fontobj)	// 匹配 
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
     {//国际字符集
          ICSIntex = index;
     }
     else if(fontflag == 2)
     {//CodePage
          CodePageIntex = index;
     }
     else if(fontflag == 3)
     {//中文
          ChinaIntex = index;
     }
     return 0;
}

/*-------------------------------------------------
函数名:FUN_FontLib_OutCharaINF
功   能:基于FUN_FontLib_Set所设置的参数，输出字符信息
参   数:
       u8 *dp:字符代码首地址
       u8 dl:字符代码字节数
       FUN_FontLib_DEF_CharaINF *infp:字符信息存放的结构指针
返回值:       
       小于0:输出失败
       等于0:输出成功
注   意:
       字号、字体、国别均由FUN_FontLib_Set预先设置
示   例:
作   者:YF
版   本:V1.0
时   间:2015-03-16
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
     {//西文字符
          u8Code[0] = dp[0];
          if(FontLib_1Byte(&u8intex,&u32baseadd,&u32offset,u8Code) < 0)
          {
               s8result = -2;
          }   
     }
     else if(dl == 2)
     {//双字节中文字符
          u8Code[0] = dp[0];
          u8Code[1] = dp[1];
          if(FontLib_2Byte(&u8intex,&u32baseadd,&u32offset,u8Code) < 0)
          {
               s8result = -2;
          }
     }
     else if(dl == 4)
     {//四字节中文字符
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
          infp->size = fonttabinfo[u8intex].offset;//字符点阵字节数
          infp->address = (u32baseadd + fonttabinfo[u8intex].offset * u32offset);//字符点阵起始地址
          infp->col = fonttabinfo[u8intex].col;//字符点阵宽度，单位点
          infp->raw = fonttabinfo[u8intex].raw;//字符点阵高度，单位点
          infp->mode = FontLibMode;//字符取模式方式
     }
     return s8result;
}

/*-------------------------------------------------
函数名:FUN_FontLib_BaseSize_OutCharaINF
功   能:根据字号参数，输出相应字符信息
参   数:
       u8 fintsize:字号，取值参考《.h的字号参数宏定义》
       u8 *dp:字符代码首地址
       u8 dl:字符代码字节数
       FUN_FontLib_DEF_CharaINF *infp:字符信息存放的结构指针
返回值:       
       小于0:输出失败
       等于0:输出成功
注   意:
       【0】、只对西文字号有效，中文字号不关联
       【1】、调用前，最好先调用FUN_FontLib_Set对字体、语言进行预设置
       如果不调用FUN_FontLib_Set，则模块使用FUN_FontLib_Config函数中的默认值
       【2】、不会改变FUN_FontLib_Set函数所设置的值
示   例:
作   者:YF
版   本:V1.0
时   间:2015-03-16
-------------------------------------------------*/
s8 FUN_FontLib_BaseSize_OutCharaINF(u8 fintsize,u8 *dp,u8 dl,FUN_FontLib_DEF_CharaINF *infp)
{
     u8 u8chinaintex,u8asciiintex,u8codepageintex,u8icsintex;
     s8 s8result = 0;
     u8 u8size,u8type;
     u32 u32obj;

     //保存原参数
     u8chinaintex = ChinaIntex;
     u8asciiintex = ASCIIIntex;
     u8codepageintex = CodePageIntex;
     u8icsintex = ICSIntex;
     
     if(s8result >= 0)
     {//设置ASCII点阵参数
          u8size = fintsize;//字号
          u8type = fonttabinfo[u8asciiintex].type;//字体
          u32obj = fonttabinfo[u8asciiintex].codepage;     
          s8result = FUN_FontLib_Set(0,u8size,u8type,u32obj);
     }
     
     if(s8result >= 0)
     {//设置国际字符集点阵参数
          u8size = fintsize;//字号
          u8type = fonttabinfo[u8icsintex].type;//字体
          u32obj = ICSN;     
          s8result = FUN_FontLib_Set(1,u8size,u8type,u32obj);
     }
     
     if(s8result >= 0)
     {//设置CodePage点阵参数
          u8size = fintsize;//字号
          u8type = fonttabinfo[u8codepageintex].type;//字体
          u32obj = fonttabinfo[u8codepageintex].codepage;     
          s8result = FUN_FontLib_Set(2,u8size,u8type,u32obj);
     }
     
     //if(s8result >= 0)
     //{//设置中文点阵参数
     //     u8size = fintsize;//字号
     //     u8type = fonttabinfo[u8chinaintex].type;//字体
     //     u32obj = fonttabinfo[u8chinaintex].codepage;     
     //     s8result = FUN_FontLib_Set(3,u8size,u8type,u32obj);
     //}
     
     if(s8result >= 0)
     {
          s8result = FUN_FontLib_OutCharaINF(dp,dl,infp);
     }
     
     //恢复原参数
     ChinaIntex = u8chinaintex;
     ASCIIIntex = u8asciiintex;
     CodePageIntex = u8codepageintex;
     ICSIntex = u8icsintex;
     
     return s8result;
}



/*-------------------------------------------------
函数名:FUN_FontLib_OutLibVersion
功   能:输出字库版本信息
参   数:
       u8 *vp:返回版本信息数据存放的首地址
       u8 vl:用于存放版本信息数据空间的字节数
返回值:       
       小于0:输出失败
       等于0:输出成功
注   意:
       
示   例:
作   者:YF
版   本:V1.0
时   间:2015-03-16
-------------------------------------------------*/
s8 FUN_FontLib_OutLibVersion(u8 *vp,u8 vl)
{
     u8 u8intex;
     if(vl <= FONTVERSSIZE)
     {//空间不够
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
*               ----------------------------------以下是模块的内部函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
//intex:返回索引号 baseadd:返回基址 offset:返回偏移字符数 dp:字符数据首地址
static s8 FontLib_1Byte(u8 *intex,u32 *baseadd,u32 *offset,u8 *dp)
{
     u8 u8intex;
     u32 u32offset;//偏移字符数
     
     if(dp[0] < 0x80)
     {//ASCII码
          if((dp[0] < 0x20) || (dp[0] == 0x7f))
          {//无效编码，无数据点阵
               return -1;
          }
          // 根据国际字符集的设置,选择点阵 
          // 判断是否属于国际字符集的字符 
          for(u8intex = 0;u8intex < FontLib_ICSNum;u8intex++)
          {
               if(dp[0] == FontLib_InterCharSet[u8intex])// 是国际字符集
               {
                    break;
               }
          }

          if(u8intex >= FontLib_ICSNum)
          {//ASCII                  
               u32offset = (u32)(dp[0] - 0x20);//因为ASCII从空格开始放点阵
               u8intex = ASCIIIntex;
          }
          else
          {//国际字符集
               //以下语句有用到u8intex值，所以u8intex = ICSIntex;语句不可放在前
               u32offset = (u32)FontLib_ICSTBL[u8intex + FontLib_ICSNum * ICSN];
               u8intex = ICSIntex;
          }
     }
     else
     {//西文CodePage码
          u32offset = (u32)(dp[0] - 0x80);//因为CodePage从0x80开始放点阵
          u8intex = CodePageIntex; 
     }

     *intex = u8intex;
     *baseadd = fonttabinfo[u8intex].baseaddr;
     *offset = u32offset;
     return 0;
}

//intex:返回索引号 baseadd:返回基址 offset:返回偏移字符数 dp:字符数据首地址
static s8 FontLib_2Byte(u8 *intex,u32 *baseadd,u32 *offset,u8 *dp)
{
     u8 u8intex = ChinaIntex;
     s8 s8temp = -1;
     u32 u32areaoffset;//区域偏移字节数
     u32 u32offset;//偏移字符数

     //判断1区
     if(s8temp == -1)
     {
          if((dp[0] >= 0xa1) && (dp[0] <= 0xa9))
          {
               if((dp[1] >= 0xa1) && (dp[1] <= 0xfe))
               {
                    u32areaoffset = 0;

                    u32areaoffset *= fonttabinfo[u8intex].offset;//区域1偏移量
                    u32offset = ((dp[0] - 0xa1) * (0xfe - 0xa1 + 1) + (dp[1] - 0xa1));//编码偏移字符数
                    s8temp = 0;
               }
          }
     }
     //判断2区
     if(s8temp == -1)
     {
          if((dp[0] >= 0xb0) && (dp[0] <= 0xf7))
          {
               if((dp[1] >= 0xa1) && (dp[1] <= 0xfe))
               {
                    u32areaoffset = FontLib_Chinese2ByteArea1_CharaNum;

                    u32areaoffset *= fonttabinfo[u8intex].offset;//区域2偏移量
                    u32offset = ((dp[0] - 0xb0) * (0xfe - 0xa1 + 1) + (dp[1] - 0xa1));//编码偏移字符数
                    s8temp = 0;
               }
          }
     }
     //判断3区
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
                         
                         u32areaoffset *= fonttabinfo[u8intex].offset;//区域3偏移量
                         u32offset = ((dp[0] - 0x81) * (0xfe - 0x40) + (dp[1] - 0x40));//编码偏移字符数
                         if(dp[1] > 0x7f)
                         {
                              u32offset--;//扣掉0x7f的1次偏移
                         }
                         s8temp = 0;
                    }
               }
          }
     }
     //判断4区
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
                         
                         u32areaoffset *= fonttabinfo[u8intex].offset;//区域4偏移量
                         u32offset = ((dp[0] - 0xaa) * (0xa0 - 0x40) + (dp[1] - 0x40));//编码偏移字符数
                         if(dp[1] > 0x7f)
                         {
                              u32offset--;//扣掉0x7f的1次偏移
                         }                         
                         s8temp = 0;
                    }
               }
          }
     }
     //判断5区
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
                         
                         u32areaoffset *= fonttabinfo[u8intex].offset;//区域5偏移量
                         u32offset = ((dp[0] - 0xa8) * (0xa0 - 0x40) + (dp[1] - 0x40));//编码偏移字符数
                         if(dp[1] > 0x7f)
                         {
                              u32offset--;//扣掉0x7f的1次偏移
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

//intex:返回索引号 baseadd:返回基址 offset:返回偏移字符数 dp:字符数据首地址
static s8 FontLib_4Byte(u8 *intex,u32 *baseadd,u32 *offset,u8 *dp)
{
     u8 u8intex = ChinaIntex;
     s8 s8temp = -1;
     u32 u32areaoffset;
     u32 u32offset;//偏移字符数

     u32areaoffset = FontLib_Chinese2ByteArea1_CharaNum;
     u32areaoffset += FontLib_Chinese2ByteArea2_CharaNum;
     u32areaoffset += FontLib_Chinese2ByteArea3_CharaNum;
     u32areaoffset += FontLib_Chinese2ByteArea4_CharaNum;
     u32areaoffset += FontLib_Chinese2ByteArea5_CharaNum;
     u32areaoffset *= fonttabinfo[u8intex].offset;//区域偏移字节数
     
     //8139区
     if(s8temp == -1)
     {
          if((dp[0] == 0x81) && (dp[1] == 0x39))
          {
               if((dp[2] == 0xee) && (dp[3] == 0x39))
               {//8139区域0
                    u32offset = 0;//偏移0个字符
                    s8temp = 0;
               }
               else if((dp[2] >= 0xef) && (dp[2] <= 0xfe))
               {
                    if((dp[3] >= 0x30) && (dp[3] <= 0x39))
                    {//8139区域1
                         u32offset = ((dp[2] - 0xef) * (0x39 - 0x30 + 1) + (dp[3] - 0x30));//编码偏移字符数
                         u32offset += FontLib_Chinese4Byte_8139Area0_CharaNum;//加上区域0的字符数
                         s8temp = 0;
                    }
               }
          }
     }
     //8230区
     if(s8temp == -1)
     {
          if((dp[0] == 0x82) && (dp[1] == 0x30))
          {
               if((dp[2] >= 0x81) && (dp[2] <= 0xfe))
               {
                    if((dp[3] >= 0x30) && (dp[3] <= 0x39))
                    {//8230区域
                         u32offset = ((dp[2] - 0x81) * (0x39 - 0x30 + 1) + (dp[3] - 0x30));//编码偏移字符数
                         u32offset += FontLib_Chinese4Byte_8139Area0_CharaNum;//加上8139区域0的字符数
                         u32offset += FontLib_Chinese4Byte_8139Area1_CharaNum;//加上8139区域1的字符数
                         s8temp = 0;
                    }
               }
          }
     }
     //8231区
     if(s8temp == -1)
     {
          if((dp[0] == 0x82) && (dp[1] == 0x31))
          {
               if((dp[2] >= 0x81) && (dp[2] <= 0xfe))
               {
                    if((dp[3] >= 0x30) && (dp[3] <= 0x39))
                    {//8231区域
                         u32offset = ((dp[2] - 0x81) * (0x39 - 0x30 + 1) + (dp[3] - 0x30));//编码偏移字符数
                         u32offset += FontLib_Chinese4Byte_8139Area0_CharaNum;//加上8139区域0的字符数
                         u32offset += FontLib_Chinese4Byte_8139Area1_CharaNum;//加上8139区域1的字符数
                         u32offset += FontLib_Chinese4Byte_8230Area_CharaNum;//加上8230区域的字符数
                         s8temp = 0;
                    }
               }
          }
     }
     //8232区
     if(s8temp == -1)
     {
          if((dp[0] == 0x82) && (dp[1] == 0x32))
          {
               if((dp[2] >= 0x81) && (dp[2] <= 0xfe))
               {
                    if((dp[3] >= 0x30) && (dp[3] <= 0x39))
                    {//8232区域
                         u32offset = ((dp[2] - 0x81) * (0x39 - 0x30 + 1) + (dp[3] - 0x30));//编码偏移字符数
                         u32offset += FontLib_Chinese4Byte_8139Area0_CharaNum;//加上8139区域0的字符数
                         u32offset += FontLib_Chinese4Byte_8139Area1_CharaNum;//加上8139区域1的字符数
                         u32offset += FontLib_Chinese4Byte_8230Area_CharaNum;//加上8230区域的字符数
                         u32offset += FontLib_Chinese4Byte_8231Area_CharaNum;//加上8231区域的字符数
                         s8temp = 0;
                    }
               }
          }
     }
     //8233区
     if(s8temp == -1)
     {
          if((dp[0] == 0x82) && (dp[1] == 0x33))
          {
               if((dp[2] >= 0x81) && (dp[2] <= 0xfe))
               {
                    if((dp[3] >= 0x30) && (dp[3] <= 0x39))
                    {//8233区域
                         u32offset = ((dp[2] - 0x81) * (0x39 - 0x30 + 1) + (dp[3] - 0x30));//编码偏移字符数
                         u32offset += FontLib_Chinese4Byte_8139Area0_CharaNum;//加上8139区域0的字符数
                         u32offset += FontLib_Chinese4Byte_8139Area1_CharaNum;//加上8139区域1的字符数
                         u32offset += FontLib_Chinese4Byte_8230Area_CharaNum;//加上8230区域的字符数
                         u32offset += FontLib_Chinese4Byte_8231Area_CharaNum;//加上8231区域的字符数
                         u32offset += FontLib_Chinese4Byte_8232Area_CharaNum;//加上8232区域的字符数
                         s8temp = 0;
                    }
               }
          }
     }
     //8234区
     if(s8temp == -1)
     {
          if((dp[0] == 0x82) && (dp[1] == 0x34))
          {
               if((dp[2] >= 0x81) && (dp[2] <= 0xfe))
               {
                    if((dp[3] >= 0x30) && (dp[3] <= 0x39))
                    {//8234区域
                         u32offset = ((dp[2] - 0x81) * (0x39 - 0x30 + 1) + (dp[3] - 0x30));//编码偏移字符数
                         u32offset += FontLib_Chinese4Byte_8139Area0_CharaNum;//加上8139区域0的字符数
                         u32offset += FontLib_Chinese4Byte_8139Area1_CharaNum;//加上8139区域1的字符数
                         u32offset += FontLib_Chinese4Byte_8230Area_CharaNum;//加上8230区域的字符数
                         u32offset += FontLib_Chinese4Byte_8231Area_CharaNum;//加上8231区域的字符数
                         u32offset += FontLib_Chinese4Byte_8232Area_CharaNum;//加上8232区域的字符数
                         u32offset += FontLib_Chinese4Byte_8233Area_CharaNum;//加上8233区域的字符数
                         s8temp = 0;
                    }
               }
          }
     }
     //8235区
     if(s8temp == -1)
     {
          if((dp[0] == 0x82) && (dp[1] == 0x35))
          {
               if((dp[2] >= 0x81) && (dp[2] <= 0xfe))
               {
                    if((dp[3] >= 0x30) && (dp[3] <= 0x39))
                    {//8235区域
                         u32offset = ((dp[2] - 0x81) * (0x39 - 0x30 + 1) + (dp[3] - 0x30));//编码偏移字符数
                         u32offset += FontLib_Chinese4Byte_8139Area0_CharaNum;//加上8139区域0的字符数
                         u32offset += FontLib_Chinese4Byte_8139Area1_CharaNum;//加上8139区域1的字符数
                         u32offset += FontLib_Chinese4Byte_8230Area_CharaNum;//加上8230区域的字符数
                         u32offset += FontLib_Chinese4Byte_8231Area_CharaNum;//加上8231区域的字符数
                         u32offset += FontLib_Chinese4Byte_8232Area_CharaNum;//加上8232区域的字符数
                         u32offset += FontLib_Chinese4Byte_8233Area_CharaNum;//加上8233区域的字符数
                         u32offset += FontLib_Chinese4Byte_8234Area_CharaNum;//加上8234区域的字符数
                         s8temp = 0;
                    }
               }
          }
     }
     
     if(s8temp == 0)
     {
          *intex = u8intex;
          *baseadd = (fonttabinfo[u8intex].baseaddr + u32areaoffset);
          *offset = u32offset; //偏移字符数
     }
     return s8temp;
}


