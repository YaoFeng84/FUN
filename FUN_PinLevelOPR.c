/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------以下是模块的修改记录区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * 内容：
 * 日期：2021-10-22
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
【1】、该FUN级引脚电平操作模块只提供一个“外壳”模块，可以操作LED、Buzz等，实现不同的控制效果，需要几个引脚可以利用头文件中的PLOTYPE变量类型定义几个变量，变量引用同一个“外壳”模块进行定时操作

【2】、函数说明：
FUN_PinLevelOPR_Config为配置函数，主要配置每个引脚电平的开启、关闭和翻转的函数指针，配置完后，默认为关闭状态
FUN_PinLevelOPR_Process为引脚控制效果处理函数，固定周期来调用，调用周期的长短直接影响开关各状态的效果时间
               开关效果所有的定时都是以该周期来参考完成的。
FUN_PinLevelOPR_Start_N为按次数定时引脚开关效果的设置函数。
FUN_PinLevelOPR_Start_T为按时间定时引脚开关效果的设置函数。


【3】、使用示例
要求Buzz持续500ms翻转；Led双闪6次后熄灭(短亮100ms 短灭100ms 长灭500ms)
(1)、使用PLOTYPE申明2个控制的变量，分别为Buzz和Led
(2)、调用FUN_PinLevelOPR_Config函数对Buzz和Led这两个变量进行初始化
     假设：
     Led亮函数为Led_On()
     Led灭函数为Led_Off()
     Led翻转函数为Led_Turn()
     Buzz响函数为Buzz_On()
     Buzz不响函数为Buzz_Off()
     Buzz翻转函数为Buzz_Turn()
     初始化Buzz：FUN_PinLevelOPR_Config(&Buzz,Buzz_On,Buzz_Off,Buzz_Turn)
     初始化Led：FUN_PinLevelOPR_Config(&Led,Led_On,Led_Off,Led_Turn)
(3)、固定50ms调用一下   FUN_PinLevelOPR_Process(&Buzz)和FUN_PinLevelOPR_Process(&Led)函数，则
     使用PLOParaSet来申明Plo开关效果的参数
     PLOParaSet Plo;

     [Buzz]设置响声效果的参数为：
     [要求Buzz持续500ms翻转]
     Plo.StartState = PLO_ON;        //起始状态
     Plo.MiddState = PLO_OFF;         //中间状态(静默)
     Plo.EendState = PLO_OFF;         //结束状态
     Plo.OnTimeValue = 10;      //响持续次数参考值(负数为无限次)
     Plo.OffTimeValue = 10;     //不响持续次数参考值(负数为无限次)
     Plo.SmallNumValue = -1;    //小次数参考值(负数为无限次)
     Plo.MiddTimeValue = -1;    //静默持续次数参考值(这个可以不设置)     
     Plo.BigNumValue = -1;      //大次数参考值(负数为无限次)(这个可以不设置)
     FUN_PinLevelOPR_Start_N(&Buzz,Plo);//启动Buzz新的响声效果

     [Led]设置显示效果的参数为：
     [Led双闪6次后熄灭(短亮50ms 短灭200ms 长灭500ms)]
     Plo.StartState = LED_ON;        //起始状态
     Plo.MiddState = LED_OFF;         //中间状态(静默)
     Plo.EendState = LED_OFF;         //结束状态
     Plo.OnTimeValue = 1;      //点亮持续次数参考值(负数为无限次)
     Plo.OffTimeValue = 4;     //熄灭持续次数参考值(负数为无限次)
     Plo.SmallNumValue = 2;    //小次数参考值(负数为无限次)(设置双闪)
     Plo.MiddTimeValue = 10;    //静默持续次数参考值(双闪之间500ms为灭)     
     Plo.BigNumValue = 6;      //大次数参考值(负数为无限次)(设置6次双闪)
     FUN_PinLevelOPR_Start_N(&Led,Plo);//启动Led新的显示效果

【4】、工作原理(以Led灯为例)
假设高电平是点亮 低电平是熄灭

----| 灭t  |----| 灭t      静默灭t       |---|   |---|             |----------------------
 亮t|_____| 亮t|____|_______________|   |___|     |___|_________|
起始态为高

|<----2次小次数---->|
|<----------------------2次大次数---------------------------->|<--结束态为高(点亮)

上述参数说明：

起始态:循环起始状态(StartState)，表示循环刚开始的状态
亮t:点亮的时间(OnTimeValue)，表示点亮时间
灭t:熄灭的时间(OffTimeValue)，表示熄灭时间
静默灭t:静默状态(MiddState)为灭的时间(MiddTimeValue)，表示静默状态及时间
2次小次数:小次数参考值(SmallNumValue)，表示闪2次
2次大次数:大次数参考值(BigNumValue)，表示循环2次
结束态:结束后状态(EendState)，表示循环结束后状态



*/
#include "TypeMro_N.h"
#include "FUN_PinLevelOPR.h"
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下模块的对接函数区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/

#define PloStateMachine_IDE   0
#define PloStateMachine_S0    1
#define PloStateMachine_ON    2
#define PloStateMachine_Midd  3
#define PloStateMachine_OFF   4

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


/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的变量申明和宏定义区--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static u16 PloProcessPeriod;//Plo处理的周期时间(即多久会调用一次FUN_PinLevelOPR_Process函数)，单位ms
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的系统函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
函数名:FUN_PinLevelOPR_Config
功   能:FUN级 引脚电平操作 配置
参   数:
       PLOTYPE *plo：对象指针
       void(*On)(void)：开启函数指针
       void(*Off)(void)：关闭函数指针
       void(*Turn)(void)：翻转函数指针
       u16 ptms：多久会调用一次FUN_PinLevelOPR_Process函数，单位ms
返回值:
       无
注   意:
       
示   例:
作   者:YJX
版   本:V1.0
时   间:2021-10-18
-------------------------------------------------*/
void FUN_PinLevelOPR_Config(PLOTYPE *plo,void(*On)(void),void(*Off)(void),void(*Turn)(void),u16 ptms)
{          
     plo->On = On;
     plo->Off = Off;
     plo->Turn = Turn;
     plo->Off();//执行关闭
     plo->StateMachine = PloStateMachine_IDE;
     PloProcessPeriod = ptms;
}

/*-------------------------------------------------
函数名:FUN_PinLevelOPR_Start_N
功   能:FUN级 启动引脚开关效果 函数
参   数:
       PLOTYPE *plo：对象指针
       PLOParaSet ploPara：显示效果参数
返回值:
       无
注   意:
       按次数来计时
示   例:
作   者:YJX
版   本:V1.0
时   间:2021-10-18
-------------------------------------------------*/
void FUN_PinLevelOPR_Start_N(PLOTYPE *plo,PLOParaSet ploPara)
{
     plo->StartState = ploPara.StartState;
     plo->MiddState = ploPara.MiddState;
     plo->EndState = ploPara.EndState;
     plo->OnTimeValue = ploPara.OnTimeValue;
     plo->OffTimeValue = ploPara.OffTimeValue;
     plo->MiddTimeValue = ploPara.MiddTimeValue;
     plo->SmallNumValue = ploPara.SmallNumValue;
     plo->BigNumValue = ploPara.BigNumValue;
     plo->StateMachine = PloStateMachine_S0;//重新从这个状态开始运行，这样就是从头开始新的效果显示
}

/*-------------------------------------------------
函数名:FUN_PinLevelOPR_Start_T
功   能:FUN级 启动引脚开关效果 函数
参   数:
       PLOTYPE *plo：对象指针
       PLOParaSet ploPara：显示效果参数
返回值:
       无
注   意:
       按时间来计时
示   例:
作   者:YJX
版   本:V1.0
时   间:2021-10-18
-------------------------------------------------*/
void FUN_PinLevelOPR_Start_T(PLOTYPE *plo,PLOParaSet ploPara)
{
     plo->StartState = ploPara.StartState;
     plo->MiddState = ploPara.MiddState;
     plo->EndState = ploPara.EndState;
     //时间 转 次数
     plo->OnTimeValue = -1;
     if(ploPara.OnTimeValue >= 0)
     {
          plo->OnTimeValue = (ploPara.OnTimeValue / PloProcessPeriod);
          if(plo->OnTimeValue == 0)
          {
               plo->OnTimeValue = 1;
          }
     }
     plo->OffTimeValue = -1;
     if(ploPara.OffTimeValue >= 0)
     {
          plo->OffTimeValue = (ploPara.OffTimeValue / PloProcessPeriod);
          if(plo->OffTimeValue == 0)
          {
               plo->OffTimeValue = 1;
          }
     }


     {
          plo->MiddTimeValue = (ploPara.MiddTimeValue / PloProcessPeriod);
          if(plo->MiddTimeValue == 0)
          {
               plo->MiddTimeValue = 1;
          }
     }
     
     plo->SmallNumValue = ploPara.SmallNumValue;
     plo->BigNumValue = ploPara.BigNumValue;
     plo->StateMachine = PloStateMachine_S0;
}


/*-------------------------------------------------
函数名:FUN_PinLevelOPR_Process
功   能:FUN级 处理 函数
参   数:
       PLOTYPE *Plo：对象指针
返回值:
       无
注   意:
       需要周期性来调用处理
示   例:
作   者:YJX
版   本:V1.0
时   间:2021-10-18
-------------------------------------------------*/
void FUN_PinLevelOPR_Process(PLOTYPE *Plo)
{
     switch(Plo->StateMachine)
     {
          case PloStateMachine_IDE:               
               break;
          case PloStateMachine_S0:
               Plo->BigNumCounter = Plo->BigNumValue;
               
               Plo->SmallNumCounter = Plo->SmallNumValue;               
               Plo->MiddTimeCounter = Plo->MiddTimeValue;
               if(Plo->StartState == PLO_ON)
               {
                    Plo->On();
                    Plo->OnTimeCounter = Plo->OnTimeValue;
                    Plo->StateMachine = PloStateMachine_ON;
               }
               else
               {
                    Plo->Off();
                    Plo->OffTimeCounter = Plo->OffTimeValue;
                    Plo->StateMachine = PloStateMachine_OFF;
               }         
               break;
          case PloStateMachine_ON:
               if(Plo->OnTimeCounter)
               {//有限 或 无限时间
                    if(Plo->OnTimeCounter > 0)
                    {//有限时间
                         Plo->OnTimeCounter -= 1;
                    }
               }
               if(Plo->OnTimeCounter == 0)
               {
                    if(Plo->StartState == PLO_OFF)
                    {
                         if(Plo->SmallNumCounter)
                         {//多次 或 无限次
                              if(Plo->SmallNumCounter > 0)
                              {//多次
                                   Plo->SmallNumCounter -= 1;
                              }   
                              if(Plo->SmallNumCounter)
                              {
                                   Plo->Off();
                                   Plo->OffTimeCounter = Plo->OffTimeValue;
                                   Plo->StateMachine = PloStateMachine_OFF;
                              }
                         }
                         if(Plo->SmallNumCounter == 0)
                         {//进入静默
                              if(Plo->MiddState == PLO_ON)
                              {
                                   Plo->On();
                              }
                              else
                              {
                                   Plo->Off();
                              }
                              Plo->MiddTimeCounter = Plo->MiddTimeValue;
                              Plo->StateMachine = PloStateMachine_Midd;
                         }
                    }
                    else
                    {
                         Plo->Off();
                         Plo->OffTimeCounter = Plo->OffTimeValue;
                         Plo->StateMachine = PloStateMachine_OFF;
                    }
               }
               break;
          case PloStateMachine_OFF:
               if(Plo->OffTimeCounter)
               {//有限 或 无限时间
                    if(Plo->OffTimeCounter > 0)
                    {//有限时间
                         Plo->OffTimeCounter -= 1;
                    }
               }
               if(Plo->OffTimeCounter == 0)
               {
                    if(Plo->StartState == PLO_ON)
                    {
                         if(Plo->SmallNumCounter)
                         {//多次 或 无限次
                              if(Plo->SmallNumCounter > 0)
                              {//多次
                                   Plo->SmallNumCounter -= 1;
                              }
                              if(Plo->SmallNumCounter)
                              {
                                   Plo->On();
                                   Plo->OnTimeCounter = Plo->OnTimeValue;
                                   Plo->StateMachine = PloStateMachine_ON;
                              }
                         }
                         if(Plo->SmallNumCounter == 0)
                         {//进入静默
                              if(Plo->MiddState == PLO_ON)
                              {
                                   Plo->On();
                              }
                              else
                              {
                                   Plo->Off();
                              }
                              Plo->MiddTimeCounter = Plo->MiddTimeValue;
                              Plo->StateMachine = PloStateMachine_Midd;
                         }
                    }
                    else
                    {
                         Plo->On();
                         Plo->OnTimeCounter = Plo->OnTimeValue;
                         Plo->StateMachine = PloStateMachine_ON;
                    }                    
               }
               break;
          case PloStateMachine_Midd:
               if(Plo->MiddTimeCounter)
               {
                    Plo->MiddTimeCounter -= 1;
               }
               if(Plo->MiddTimeCounter == 0)
               {
                    if(Plo->BigNumCounter)
                    {//多次 或 无限次
                         if(Plo->BigNumCounter > 0)
                         {
                              Plo->BigNumCounter -= 1;
                         }
                         if(Plo->BigNumCounter)
                         {
                              Plo->SmallNumCounter = Plo->SmallNumValue;               
                              Plo->MiddTimeCounter = Plo->MiddTimeValue;
                              if(Plo->StartState == PLO_ON)
                              {
                                   Plo->On();
                                   Plo->OnTimeCounter = Plo->OnTimeValue;
                                   Plo->StateMachine = PloStateMachine_ON;
                              }
                              else
                              {
                                   Plo->Off();
                                   Plo->OffTimeCounter = Plo->OffTimeValue;
                                   Plo->StateMachine = PloStateMachine_OFF;
                              } 
                         }
                    }

                    if(Plo->BigNumCounter == 0)
                    {//结束大循环
                         if(Plo->EndState == PLO_ON)
                         {
                              Plo->On();
                         }
                         else
                         {
                              Plo->Off();
                         }
                         Plo->StateMachine = PloStateMachine_IDE;
                    }
               }
               break;
          default:
               Plo->Off();
               Plo->StateMachine = PloStateMachine_IDE;
               break;
     }
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的用户函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/


/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数代码区------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/

 


