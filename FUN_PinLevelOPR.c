/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------������ģ����޸ļ�¼��-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * ���ݣ�
 * ���ڣ�2021-10-22
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
��1������FUN�����ŵ�ƽ����ģ��ֻ�ṩһ������ǡ�ģ�飬���Բ���LED��Buzz�ȣ�ʵ�ֲ�ͬ�Ŀ���Ч������Ҫ�������ſ�������ͷ�ļ��е�PLOTYPE�������Ͷ��弸����������������ͬһ������ǡ�ģ����ж�ʱ����

��2��������˵����
FUN_PinLevelOPR_ConfigΪ���ú�������Ҫ����ÿ�����ŵ�ƽ�Ŀ������رպͷ�ת�ĺ���ָ�룬�������Ĭ��Ϊ�ر�״̬
FUN_PinLevelOPR_ProcessΪ���ſ���Ч�����������̶����������ã��������ڵĳ���ֱ��Ӱ�쿪�ظ�״̬��Ч��ʱ��
               ����Ч�����еĶ�ʱ�����Ը��������ο���ɵġ�
FUN_PinLevelOPR_Start_NΪ��������ʱ���ſ���Ч�������ú�����
FUN_PinLevelOPR_Start_TΪ��ʱ�䶨ʱ���ſ���Ч�������ú�����


��3����ʹ��ʾ��
Ҫ��Buzz����500ms��ת��Led˫��6�κ�Ϩ��(����100ms ����100ms ����500ms)
(1)��ʹ��PLOTYPE����2�����Ƶı������ֱ�ΪBuzz��Led
(2)������FUN_PinLevelOPR_Config������Buzz��Led�������������г�ʼ��
     ���裺
     Led������ΪLed_On()
     Led����ΪLed_Off()
     Led��ת����ΪLed_Turn()
     Buzz�캯��ΪBuzz_On()
     Buzz���캯��ΪBuzz_Off()
     Buzz��ת����ΪBuzz_Turn()
     ��ʼ��Buzz��FUN_PinLevelOPR_Config(&Buzz,Buzz_On,Buzz_Off,Buzz_Turn)
     ��ʼ��Led��FUN_PinLevelOPR_Config(&Led,Led_On,Led_Off,Led_Turn)
(3)���̶�50ms����һ��   FUN_PinLevelOPR_Process(&Buzz)��FUN_PinLevelOPR_Process(&Led)��������
     ʹ��PLOParaSet������Plo����Ч���Ĳ���
     PLOParaSet Plo;

     [Buzz]��������Ч���Ĳ���Ϊ��
     [Ҫ��Buzz����500ms��ת]
     Plo.StartState = PLO_ON;        //��ʼ״̬
     Plo.MiddState = PLO_OFF;         //�м�״̬(��Ĭ)
     Plo.EendState = PLO_OFF;         //����״̬
     Plo.OnTimeValue = 10;      //����������ο�ֵ(����Ϊ���޴�)
     Plo.OffTimeValue = 10;     //������������ο�ֵ(����Ϊ���޴�)
     Plo.SmallNumValue = -1;    //С�����ο�ֵ(����Ϊ���޴�)
     Plo.MiddTimeValue = -1;    //��Ĭ���������ο�ֵ(������Բ�����)     
     Plo.BigNumValue = -1;      //������ο�ֵ(����Ϊ���޴�)(������Բ�����)
     FUN_PinLevelOPR_Start_N(&Buzz,Plo);//����Buzz�µ�����Ч��

     [Led]������ʾЧ���Ĳ���Ϊ��
     [Led˫��6�κ�Ϩ��(����50ms ����200ms ����500ms)]
     Plo.StartState = LED_ON;        //��ʼ״̬
     Plo.MiddState = LED_OFF;         //�м�״̬(��Ĭ)
     Plo.EendState = LED_OFF;         //����״̬
     Plo.OnTimeValue = 1;      //�������������ο�ֵ(����Ϊ���޴�)
     Plo.OffTimeValue = 4;     //Ϩ����������ο�ֵ(����Ϊ���޴�)
     Plo.SmallNumValue = 2;    //С�����ο�ֵ(����Ϊ���޴�)(����˫��)
     Plo.MiddTimeValue = 10;    //��Ĭ���������ο�ֵ(˫��֮��500msΪ��)     
     Plo.BigNumValue = 6;      //������ο�ֵ(����Ϊ���޴�)(����6��˫��)
     FUN_PinLevelOPR_Start_N(&Led,Plo);//����Led�µ���ʾЧ��

��4��������ԭ��(��Led��Ϊ��)
����ߵ�ƽ�ǵ��� �͵�ƽ��Ϩ��

----| ��t  |----| ��t      ��Ĭ��t       |---|   |---|             |----------------------
 ��t|_____| ��t|____|_______________|   |___|     |___|_________|
��ʼ̬Ϊ��

|<----2��С����---->|
|<----------------------2�δ����---------------------------->|<--����̬Ϊ��(����)

��������˵����

��ʼ̬:ѭ����ʼ״̬(StartState)����ʾѭ���տ�ʼ��״̬
��t:������ʱ��(OnTimeValue)����ʾ����ʱ��
��t:Ϩ���ʱ��(OffTimeValue)����ʾϨ��ʱ��
��Ĭ��t:��Ĭ״̬(MiddState)Ϊ���ʱ��(MiddTimeValue)����ʾ��Ĭ״̬��ʱ��
2��С����:С�����ο�ֵ(SmallNumValue)����ʾ��2��
2�δ����:������ο�ֵ(BigNumValue)����ʾѭ��2��
����̬:������״̬(EendState)����ʾѭ��������״̬



*/
#include "TypeMro_N.h"
#include "FUN_PinLevelOPR.h"
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------����ģ��ĶԽӺ�����-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/

#define PloStateMachine_IDE   0
#define PloStateMachine_S0    1
#define PloStateMachine_ON    2
#define PloStateMachine_Midd  3
#define PloStateMachine_OFF   4

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


/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ��ı��������ͺ궨����--------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
static u16 PloProcessPeriod;//Plo���������ʱ��(����û����һ��FUN_PinLevelOPR_Process����)����λms
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ���ϵͳ����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*-------------------------------------------------
������:FUN_PinLevelOPR_Config
��   ��:FUN�� ���ŵ�ƽ���� ����
��   ��:
       PLOTYPE *plo������ָ��
       void(*On)(void)����������ָ��
       void(*Off)(void)���رպ���ָ��
       void(*Turn)(void)����ת����ָ��
       u16 ptms����û����һ��FUN_PinLevelOPR_Process��������λms
����ֵ:
       ��
ע   ��:
       
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2021-10-18
-------------------------------------------------*/
void FUN_PinLevelOPR_Config(PLOTYPE *plo,void(*On)(void),void(*Off)(void),void(*Turn)(void),u16 ptms)
{          
     plo->On = On;
     plo->Off = Off;
     plo->Turn = Turn;
     plo->Off();//ִ�йر�
     plo->StateMachine = PloStateMachine_IDE;
     PloProcessPeriod = ptms;
}

/*-------------------------------------------------
������:FUN_PinLevelOPR_Start_N
��   ��:FUN�� �������ſ���Ч�� ����
��   ��:
       PLOTYPE *plo������ָ��
       PLOParaSet ploPara����ʾЧ������
����ֵ:
       ��
ע   ��:
       ����������ʱ
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2021-10-18
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
     plo->StateMachine = PloStateMachine_S0;//���´����״̬��ʼ���У��������Ǵ�ͷ��ʼ�µ�Ч����ʾ
}

/*-------------------------------------------------
������:FUN_PinLevelOPR_Start_T
��   ��:FUN�� �������ſ���Ч�� ����
��   ��:
       PLOTYPE *plo������ָ��
       PLOParaSet ploPara����ʾЧ������
����ֵ:
       ��
ע   ��:
       ��ʱ������ʱ
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2021-10-18
-------------------------------------------------*/
void FUN_PinLevelOPR_Start_T(PLOTYPE *plo,PLOParaSet ploPara)
{
     plo->StartState = ploPara.StartState;
     plo->MiddState = ploPara.MiddState;
     plo->EndState = ploPara.EndState;
     //ʱ�� ת ����
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
������:FUN_PinLevelOPR_Process
��   ��:FUN�� ���� ����
��   ��:
       PLOTYPE *Plo������ָ��
����ֵ:
       ��
ע   ��:
       ��Ҫ�����������ô���
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2021-10-18
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
               {//���� �� ����ʱ��
                    if(Plo->OnTimeCounter > 0)
                    {//����ʱ��
                         Plo->OnTimeCounter -= 1;
                    }
               }
               if(Plo->OnTimeCounter == 0)
               {
                    if(Plo->StartState == PLO_OFF)
                    {
                         if(Plo->SmallNumCounter)
                         {//��� �� ���޴�
                              if(Plo->SmallNumCounter > 0)
                              {//���
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
                         {//���뾲Ĭ
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
               {//���� �� ����ʱ��
                    if(Plo->OffTimeCounter > 0)
                    {//����ʱ��
                         Plo->OffTimeCounter -= 1;
                    }
               }
               if(Plo->OffTimeCounter == 0)
               {
                    if(Plo->StartState == PLO_ON)
                    {
                         if(Plo->SmallNumCounter)
                         {//��� �� ���޴�
                              if(Plo->SmallNumCounter > 0)
                              {//���
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
                         {//���뾲Ĭ
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
                    {//��� �� ���޴�
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
                    {//������ѭ��
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
*               ----------------------------------������ģ����û�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/


/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����ڲ�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/

 


