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
 * 修改内容：增加流控
 * 修改日期：2020-12-30
 * 修改作者：YJX
 * 版本号：V2.0
 ***********************************************
 * 修改内容：写缓存指针延后更新
 * 修改日期：2023-02-10
 * 修改作者：YJX
 * 版本号：V2.1
 ***********************************************
*/
/********************************************************************************************************************************************
*                                                                                                                                           *
*                ---------------------------------以下是模块的使用说明区-----------------------------------------                           *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*                                       特点说明：
          输入接口的缓存操作模块
          
          关于《特性规则》
【1】、写指针指向的空间永远是空的
【2】、读指针指向的空间永远是有数据的
【3】、当读和写指针一样时，规定为缓存全空
【4】、由【3】可知，当读指针-写指针=1时，规定为缓存全满

          关于《流控说明》
    通过.h文件中是否定义BufferFlowControl，来预编译是否使用流控。
    如果.h文件中有定义BufferFlowControl，则使用模块流控功能
    如果.h文件中没定义BufferFlowControl，则未用模块流控功能

          关于《特殊类型变量说明》
【1】、BufferCNFType类型为配置专用，配置时需要用此类型申请变量，并对变量值进行初始化，将该变量做为形参给FUN_BufferOPR_Config函数。
【2】、BufferCNFType类型各参数说明如下：
       BufferCNFType.BufferStartP:      为缓存首地址
       BufferCNFType.BufferLen:         为缓存字节数
       BufferCNFType.BufferCP:          为缓存控制变量地址
       BufferCNFType.MaxStartReceSize:  为空闲字节数大于该值时会调用StartReceF回调函数，用于开始接收
       BufferCNFType.MinStopReceSize:   为空闲字节数小于该值时会调用StopReceF回调函数，用于停止接收
       BufferCNFType.StopReceF:         为停止接收的回调函数，该函数用于执行停止接收的语句或函数
       BufferCNFType.StartReceF:        为开始接收的回调函数，该函数用于执行开始接收的语句或函数
【3】、BufferOPRType类型为管理每段缓存专用的。每段缓存都要有对应的BufferOPRType变量来控制。一般为全局变量
【4】、读取分为【预读】和【直读】
      【预读】数据读出后，缓存中的数据不释放，数据仍存在。可以调用FUN_BufferOPR_MoveRP函数来释放相应量的缓存空间
      【直读】数据读出后，缓存中的数据被释放，数据不存在。


          关于《使用说明》    
例如：要对以下缓存进行环形操作控制。
u8 ReceBuffer[100];
BufferOPRType ReceBufferControl;
要求:小于20字节时要停止接收 大于60字节时要继续接收
停止接收函数:StopRece()
开始接收函数:StartRece()


配置过程如下：
BufferCNFType bcf;                 //申明一个BufferCNFType类型的变量bcf
bcf.BufferStartP = ReceBuffer;     //设置缓存首地址
bcf.BufferLen = 100;               //设置缓存字节数
bcf.BufferCP = &ReceBufferControl; //设置缓存控制变量
bcf.MaxStartReceSize = 60;         //设置空闲字节数大于60字节就开始接收
bcf.MinStopReceSize = 20;          //设置空闲字节数小于20字节就停止接收
bcf.StopReceF = StopRece;          //设置禁止接收回调函数
bcf.StartReceF = StartRece;        //设置使能接收回调函数
FUN_BufferOPR_Config(&bcf);        //调用配置函数，即可完成配置
判断函数的返回值即可。

使用时：
【1】、往缓存写数据，待写数据首地址wdp，待写字节数为wdl
FUN_BufferOPR_WriteData(&ReceBufferControl,wdp,wdl);
根据返回值判断实际写入字节数

【2】、预读缓存数据，存放首地址rdp,读字节数为rdl
FUN_BufferOPR_ReadData(&ReceBufferControl,0,rdp,rdl);
根据返回值判断实际读到的字节数

其他函数相似使用

*/

#include "FUN_BufferOPR.h"
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

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块内的变量类型定义区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数申明区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块内的 变量 与 宏定义 申明区--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的系统函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
函数名: FUN_BufferOPR_Config
功   能: 缓存配置
参   数:
       BufferCNFType *BOT:缓存配置类型变量首地址
返回值:
       0:配置成功
       小于0:配置失败
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2018-08-08   
-------------------------------------------------*/
s8 FUN_BufferOPR_Config(BufferCNFType *Bot)
{
     if(
     (Bot->BufferLen == 0) 
#ifdef BufferFlowControl       
     || (Bot->MaxStartReceSize == 0) || 
     (Bot->MinStopReceSize == 0) || 
     (Bot->BufferLen < Bot->MaxStartReceSize) || 
     (Bot->BufferLen < Bot->MinStopReceSize) || 
     (Bot->MaxStartReceSize == Bot->MinStopReceSize) || 
     (Bot->MinStopReceSize > Bot->MaxStartReceSize) 
#endif     
     )
     {
          return -1;
     }

     Bot->BufferCP->BufferStartP = Bot->BufferStartP;
     Bot->BufferCP->BufferLen = Bot->BufferLen;
     Bot->BufferCP->BufferEndP = (Bot->BufferStartP + (Bot->BufferLen - 1));
     Bot->BufferCP->BufferWP = Bot->BufferStartP;
     Bot->BufferCP->BufferRP = Bot->BufferStartP;
#ifdef BufferFlowControl  
     Bot->BufferCP->MaxStartReceSize = Bot->MaxStartReceSize;
     Bot->BufferCP->MinStopReceSize = Bot->MinStopReceSize;
     Bot->BufferCP->StartReceF = Bot->StartReceF;
     Bot->BufferCP->StopReceF = Bot->StopReceF;

     Bot->StartReceF();//执行开始接收回调函数
#endif     
     return 0;
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的用户函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
函数名: FUN_BufferOPR_WriteData
功   能: 缓存---阻塞型写数据函数
参   数:
       BufferOPRType *BOT:缓存操作类型变量首地址
       u8 *wdp:待写数据首地址指针
       u32 wdl:待写数据的个数，单位:字节
返回值:
       实际写入的个数，单位:字节
注   意:
       
示   例:
作   者:YJX
版   本:V1.0 : 原版
时   间:2018-10-17   
-------------------------------------------------
作   者:YJX
版   本:V1.1 : 更新写入数据完成后，再更新写指针
时   间:2023-02-10   
-------------------------------------------------*/
u32 FUN_BufferOPR_WriteData(BufferOPRType *Bot,u8 *wdp,u32 wdl)
{
     u32 wn = 0;//实际写入的字节数
     u32 wntemp,freesize; 
     u8 *writep = Bot->BufferWP;
     
     //freesize = GetFreeSize(Bot);//获取空闲空间 
     freesize = (Bot->BufferLen - FUN_BufferOPR_GetUseSize(Bot) - 1);//获取空闲空间(减1是为了避免规则【3】的出现)
     {           
          if(freesize > 0)
          {
               //MutexOpen;   //互斥开启 关中断
               //计算能写入的数据字节数
               wn = wdl;
               if(freesize < wdl)
               {
                    wn = freesize;
               }               
               wntemp = wn;//可以写入的字节数
               //循环写入数据(小数量数据写入，以字节为单位进行写操作，不做优化处理)
               while(wntemp--)
               {
                    *(writep) = *wdp;
                    wdp++;//更新数据源指针 
                    writep = ((writep) + 1);//更新写指针
                    //判断写指针是否越界
                    if((writep) > (Bot->BufferEndP))
                    {//如果越界
                         writep = (Bot->BufferStartP);
                    }
               }
               Bot->BufferWP = writep;//更新写指针 
               //MutexClose;//互斥关闭 开中断
          }
#ifdef BufferFlowControl            
          //以下是流控处理
          freesize = (Bot->BufferLen - FUN_BufferOPR_GetUseSize(Bot) - 1);//获取空闲空间(减1是为了避免规则【3】的出现)
          if(freesize < Bot->MinStopReceSize)
          {//空闲字节 小于 最小停止接收字节，则停止接收
               Bot->StopReceF();//执行停止接收回调函数
          }
#endif
     }
     
     return wn;
}
// {
//      u32 wn = 0;//实际写入的字节数
//      u32 wntemp,freesize; 
     
//      //freesize = GetFreeSize(Bot);//获取空闲空间 
//      freesize = (Bot->BufferLen - FUN_BufferOPR_GetUseSize(Bot) - 1);//获取空闲空间(减1是为了避免规则【3】的出现)
//      {           
//           if(freesize > 0)
//           {
//                //MutexOpen;   //互斥开启 关中断
//                //计算能写入的数据字节数
//                wn = wdl;
//                if(freesize < wdl)
//                {
//                     wn = freesize;
//                }               
//                wntemp = wn;//可以写入的字节数
//                //循环写入数据(小数量数据写入，以字节为单位进行写操作，不做优化处理)
//                while(wntemp--)
//                {
//                     *(Bot->BufferWP) = *wdp;
//                     wdp++;//更新数据源指针 
//                     Bot->BufferWP = ((Bot->BufferWP) + 1);//更新写指针
//                     //判断写指针是否越界
//                     if((Bot->BufferWP) > (Bot->BufferEndP))
//                     {//如果越界
//                          Bot->BufferWP = (Bot->BufferStartP);
//                     }
//                } 
//                //MutexClose;//互斥关闭 开中断
//           }
// #ifdef BufferFlowControl            
//           //以下是流控处理
//           freesize = (Bot->BufferLen - FUN_BufferOPR_GetUseSize(Bot) - 1);//获取空闲空间(减1是为了避免规则【3】的出现)
//           if(freesize < Bot->MinStopReceSize)
//           {//空闲字节 小于 最小停止接收字节，则停止接收
//                Bot->StopReceF();//执行停止接收回调函数
//           }
// #endif
//      }
//      return wn;
// }




/*-------------------------------------------------
函数名: FUN_BufferOPR_ReadData
功   能: 缓存---阻塞型读数据函数
参   数:
       BufferOPRType *BOT:缓存操作类型变量首地址
       u8 rm:读模式(0:预读 非0:直读)
       u8 *rdp:读到数据存放空间的首地址指针
       u32 rdl:待读的数据个数，单位:字节
返回值:
       实际读出的个数，单位:字节
注   意:
       预读取后，空闲空间尺寸不变化
       直读取后，空闲空间尺寸发生变化
示   例:
作   者:YJX
版   本:V1.0
时   间:2018-10-17   
-------------------------------------------------*/
u32 FUN_BufferOPR_ReadData(BufferOPRType *Bot,u8 rm,u8 *rdp,u32 rdl)
{
     u32 rn = 0;//实际读取的字节数   
     u32 rntemp,usesize;
     u8 *rp = (Bot->BufferRP);
     {          
          usesize = FUN_BufferOPR_GetUseSize(Bot);//获取有数据空间          
          if(usesize > 0)
          {
               //MutexOpen;//互斥开启 关中断
               //计算能读取的数据字节数
               rn = rdl;
               if(usesize < rdl)
               {
                    rn = usesize;
               }
               rntemp = rn;
               //循环读取数据(小数量数据读取，以字节为单位进行读操作，不做优化处理)
               while(rntemp--)
               {
                    *rdp = *rp;
                    rdp++;//更新数据存储指针
                    rp++;//更新读指针
                    //判断读指针是否越界
                    if(rp > (Bot->BufferEndP))
                    {//如果越界
                         rp = (Bot->BufferStartP);
                    }                
               }  
               if(rm)
               {//如果是直读，则直接修改读指针
                    (Bot->BufferRP) = rp;
#ifdef BufferFlowControl 
                    {
                         //以下是流控处理
                         u32 freesize;
                         freesize = (Bot->BufferLen - FUN_BufferOPR_GetUseSize(Bot) - 1);//获取空闲空间(减1是为了避免规则【3】的出现)
                         if(freesize > Bot->MaxStartReceSize)
                         {//空闲字节 大于 最大开始接收字节，则开始接收
                              Bot->StartReceF();//执行开始接收回调函数
                         }
                    }
#endif                    
               }
               //MutexClose;//互斥关闭 开中断
          }
     }
     return rn;
}

/*-------------------------------------------------
函数名: FUN_BufferOPR_MoveRP
功   能: 缓存---移动读指针函数
参   数:
       BufferOPRType *BOT:缓存操作类型变量首地址
       u32 mbn:移动字节数
返回值:
       实际移动字节数
注   意:
       主要用于配合预读方式来使用，如预读了11字节，当中只使用了4个字节
       则可以使用该函数移4字节来释放4字节的空间（因为预读没有释放空间）
示   例:
作   者:YJX
版   本:V1.0
时   间:2018-10-17   
-------------------------------------------------*/
u32 FUN_BufferOPR_MoveRP(BufferOPRType *Bot,u32 mbn)
{
     u32 usesize;//总的可以移动字节数
     u32 forwordbyte;//正向移动字节数
     
     
     //MutexOpen;//互斥开启 关中断
     if(Bot->BufferWP > Bot->BufferRP)
     {
          usesize = (Bot->BufferWP - Bot->BufferRP);
          if(mbn > usesize)
          {
               mbn = usesize;
          }
          Bot->BufferRP += mbn;//移动读指针
     }
     else if(Bot->BufferWP < Bot->BufferRP)
     {
          usesize = (Bot->BufferLen - (Bot->BufferRP - Bot->BufferWP));
          if(mbn > usesize)
          {
               mbn = usesize;
          }
          forwordbyte = (Bot->BufferEndP - Bot->BufferRP);//当前读指针到缓存结尾可移的字节数
          if(mbn <= forwordbyte)
          {//当前读指针 到 缓存结尾 的字节数 大等于 要移动的字节数
               Bot->BufferRP += mbn;//移动读指针
          }
          else
          {
               Bot->BufferRP = (Bot->BufferStartP + mbn - forwordbyte - 1);//移动读指针
          }
     }
     else
     {
          usesize = 0;
          mbn = 0;
     }
#ifdef BufferFlowControl 
     {
          //以下是流控处理
          u32 freesize;
          freesize = (Bot->BufferLen - FUN_BufferOPR_GetUseSize(Bot) - 1);//获取空闲空间(减1是为了避免规则【3】的出现)
          if(freesize > Bot->MaxStartReceSize)
          {//空闲字节 大于 最大开始接收字节，则开始接收
               Bot->StartReceF();//执行开始接收回调函数
          }
     }
#endif     
     //MutexClose;//互斥关闭 开中断
     return mbn;
}

/*-------------------------------------------------
函数名: FUN_BufferOPR_SetWP
功   能: 缓存---基于起始地址的偏移量 的函数
参   数:
       BufferOPRType *BOT:缓存操作类型变量首地址
       u32 nwp:偏移字节数
返回值:
       无
注   意:
       未做越界判断！！！
示   例:
作   者:YJX
版   本:V1.0
时   间:2018-10-17   
-------------------------------------------------*/
void FUN_BufferOPR_SetWP(BufferOPRType *Bot,u32 nwp)
{
     Bot->BufferWP = (Bot->BufferStartP + nwp);
}


/*-------------------------------------------------
函数名: FUN_BufferOPR_Reset
功   能: 缓存---复位函数
参   数:
       BufferOPRType *BOT:缓存操作类型变量首地址
返回值:
       无
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2018-10-17   
-------------------------------------------------*/
void FUN_BufferOPR_Reset(BufferOPRType *Bot)
{
     //Bot->BufferWP = Bot->BufferStartP;
     //Bot->BufferRP = Bot->BufferStartP; 
     Bot->BufferRP = Bot->BufferWP;
#ifdef BufferFlowControl       
     Bot->StartReceF();//执行开始接收回调函数
#endif     
}

/*-------------------------------------------------
函数名: FUN_BufferOPR_GetFreeSize
功   能: 缓存---获取空闲空间个数函数
参   数:
       BufferOPRType *BOT:缓存操作类型变量首地址
返回值:
       返回空闲空间的个数，单位:字节
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2018-10-17   
-------------------------------------------------*/
u32 FUN_BufferOPR_GetFreeSize(BufferOPRType *Bot)
{
     return (Bot->BufferLen - FUN_BufferOPR_GetUseSize(Bot) - 1);//(减1是为了避免规则【3】的出现)
}

/*-------------------------------------------------
函数名: FUN_BufferOPR_GetUseSize
功   能: 缓存---获取已使用空间个数函数
参   数:
       BufferOPRType *BOT:缓存操作类型变量首地址
返回值:
       返回已使用空间的个数，单位:字节
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2018-10-17   
-------------------------------------------------*/
u32 FUN_BufferOPR_GetUseSize(BufferOPRType *Bot)
{
     u32 usesize;
     //MutexOpen;//互斥开启 关中断
     if(Bot->BufferWP > Bot->BufferRP)
     {
          usesize = (Bot->BufferWP - Bot->BufferRP);

     }
     else if(Bot->BufferWP < Bot->BufferRP)
     {
          usesize = (Bot->BufferLen - (Bot->BufferRP - Bot->BufferWP));

     }
     else
     {
          usesize = 0;
     }
     //MutexClose;//互斥关闭 开中断
     return usesize;
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/



