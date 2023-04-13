/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------������ģ����޸ļ�¼��-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * ���ݣ�
 * ���ڣ�2020-03-18
 * ���ߣ�YJX
 * �汾�ţ�V1.0�����棩
 ***********************************************
 * �޸����ݣ�������FUN_String_HexStrToChar
 * �޸����ڣ�20201207
 * �޸����ߣ�
 * �� �� �ţ�V2.0
 ***********************************************
 * �޸����ݣ�������FUN_String_Ltrim
 * �޸����ڣ�20230307
 * �޸����ߣ�
 * �� �� �ţ�V2.1
 ***********************************************
*/
/********************************************************************************************************************************************
*                                                                                                                                           *
*                ---------------------------------������ģ���ʹ��˵����-----------------------------------------                           *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*                                       �ص�˵����

*/
#include "TypeMro.h"
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
������:FUN_String_STRIsSame
��   ��:�Ƚ��ַ����Ƿ�һ�� ����
��   ��:
       u8 *dp1:���Ƚ��ַ����׵�ַ
       u8 *dp2:�Ƚ��ַ����׵�ַ
����ֵ:
       0:һ�� 
      -1:��һ��
ע   ��:
       �Ƚ��254���ַ�
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-02-10
-------------------------------------------------*/
s8 FUN_String_STRIsSame(u8 *dp1,u8 *dp2)
{
     u16 u16intex;
     for(u16intex = 0;u16intex < U16MAZVALUE;u16intex++)
     {
          if(dp1[u16intex] != dp2[u16intex])
          {
               return -1;
          }
          else if(dp1[u16intex] == '\0')
          {//�ȽϽ���
               return 0;
          }
     }
     return -2;//���ȹ���
}

/*-------------------------------------------------
������:FUN_String_STRIsSame2
��   ��:�Ƚ��ַ����Ƿ�һ�� ����
��   ��:
       u8 *dp1:���Ƚ��ַ����׵�ַ
       u8 *dp2:�Ƚ��ַ����׵�ַ
       u16 dl:�Ƚϵĳ����ֽ���
����ֵ:
       0:һ�� 
      -1:��һ��
ע   ��:
       dp1�������ַ�����������\0��β��
ʾ   ��:
��   ��:YJX
��   ��:V1.1
ʱ   ��:2021-02-19
-------------------------------------------------*/
s8 FUN_String_STRIsSame2(u8 *dp1,u8 *dp2,u16 dl)
{
     u16 u16intex;
     for(u16intex = 0;u16intex < dl;u16intex++)
     {
          if(dp1[u16intex] != dp2[u16intex])
          {
               return -1;
          }
     }
     return 0;
}

/*-------------------------------------------------
������:FUN_String_Split
��   ��: �ַ����ָ� ����
��   ��:
       u8 *dp:���ָ��ַ����׵�ַ(��\0��β)
       u8 *fp:�ָ��ַ����׵�ַ(��\0��β)
       u16 *ip:�ָ�������Ŵ���׵�ַ
       u8 il:�ָ�������Ŵ�ſռ䳤��
����ֵ:
       ʵ����Ч�ķָ����
       ���С��0�����ʾ��ŷָ��Ŀռ䲻����תΪ������Ϊ�ѷָ�ĵ���
ע   ��:
       ip�����д�ŷָ��������ţ�(��0��)
ʾ   ��:
          �� dp = "12345674598458"
             fp = "45"
          �� ��������3��ip = {3,7,11}
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-03-07
-------------------------------------------------*/
s16 FUN_String_Split(u8 *dp,u8 *fp,u16 *ip,u8 il)
{
     u8 u8intex;
     u16 u16intex;
     s16 s16ipintex;//����������

     if(il == 0)
     {
          return 0;
     }

     u8intex = 0;//
     s16ipintex = 0;//����ϵ�������
     for(u16intex = 0;u16intex < U16MAZVALUE;u16intex++)
     {
          if(dp[u16intex] == '\0')
          {
               break;//�˳�ѭ��
          }
          else if(dp[u16intex] == fp[u8intex])
          {
               u8intex++;
               if(fp[u8intex] == '\0')
               {                    
                    if(s16ipintex < il)
                    {//�ռ乻��
                         ip[s16ipintex++] = (u16intex + 1 - u8intex); 
                    }
                    else
                    {//�ռ䲻����
                         s16ipintex = (0 - s16ipintex);//��Ϊ����
                         break;//�˳�ѭ��
                    }
                    u8intex = 0;
               }
          }
          else
          {
               u8intex = 0;
          }
     }

     return s16ipintex;
}

/*-------------------------------------------------
������:FUN_String_Split2
��   ��: ���ݷָ� ����
��   ��:
       u8 *dp:���ָ��ֽ������׵�ַ
       u16 dl:���ָ��ֽ���
       u8 *fp:�ָ������׵�ַ
       u8 fl:�ָ������ֽ���
       u16 *ip:�ָ�������Ŵ���׵�ַ
       u8 il:�ָ�������Ŵ�ſռ䳤��
����ֵ:
       ʵ����Ч�ķָ����
       ���С��0�����ʾ��ŷָ��Ŀռ䲻����תΪ������Ϊ�ѷָ�ĵ���
ע   ��:
       ip�����д�ŷָ��������ţ�(��0��)
ʾ   ��:
          �� dp = {1,2,3,4,5,6,7,4,5,9,8,4,5,8}
             fp = {4,5}
          �� ��������3��ip = {3,7,11}
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-03-07
-------------------------------------------------*/
s16 FUN_String_Split2(u8 *dp,u16 dl,u8 *fp,u8 fl,u16 *ip,u8 il)
{
     u8 u8intex;
     u16 u16intex;
     s16 s16ipintex;//����������

     if((dl == 0) || (fl == 0) || (il == 0) || (fl > dl))
     {
          return 0;
     }

     u8intex = 0;//
     s16ipintex = 0;//����ϵ�������
     for(u16intex = 0;u16intex < dl;u16intex++)
     {
          if(dp[u16intex] == fp[u8intex])
          {
               u8intex++;
               if(u8intex >= fl)
               {                    
                    if(s16ipintex < il)
                    {//�ռ乻��
                         ip[s16ipintex++] = (u16intex + 1 - fl); 
                    }
                    else
                    {//�ռ䲻����
                         s16ipintex = (0 - s16ipintex);//��Ϊ����
                         break;//�˳�ѭ��
                    }
                    u8intex = 0;
               }
          }
          else
          {
               u8intex = 0;
          }
     }

     return s16ipintex;
}

/*-------------------------------------------------
������:FUN_String_Ltrim
��   ��:ȥ���ַ������ո� ����
��   ��:
       u8 *sp:�ַ����׵�ַ
����ֵ:
       ����ȥ�����ո���ַ������׵�ַ
ע   ��:
       sp�������ַ�����������\0��β�ģ���С��65535���ַ�
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-03-07
-------------------------------------------------*/
u8 *FUN_String_Ltrim(u8 *sp)
{
     u16 u16intex;
     for(u16intex = 0;u16intex < U16MAZVALUE;u16intex++)
     {
          if(sp[u16intex] != ' ')
          {
               return &sp[u16intex];
          }
     }
}

/*-------------------------------------------------
������:FUN_String_StrLeng
��   ��:�����ַ����ֽ��� ����
��   ��:
       u8 *sp:�ַ����׵�ַ
����ֵ:
       �ֽ���(������\0)
ע   ��:
       sp�������ַ�����������\0��β�ģ���С��65535���ַ�
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2020-08-27
-------------------------------------------------*/
u16 FUN_String_StrLen(u8 *sp)
{
     u16 u16intex;
     for(u16intex = 0;u16intex < U16MAZVALUE;u16intex++)
     {
          if(sp[u16intex] == '\0')
          {
               return u16intex;
          }
     }
     return 0;
}

/*-------------------------------------------------
������:FUN_String_StrLink
��   ��:�ַ������� ����
��   ��:
       u8 *sp��Ŀ���ַ����׵�ַ(��\0��β)
       u16 sl��Ŀ���ַ����ռ��ֽ���
       u8 *dp���������ַ����׵�ַ(��\0��β)
����ֵ:
       ���Ӻ��ַ�������Ч�ֽ���(������\0)
ע   ��:
       sp��dp�������ַ�����������\0��β�ģ���С��65535���ַ�
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-03-08
-------------------------------------------------*/
u16 FUN_String_StrLink(u8 *sp,u16 sl,u8 *dp)
{
     u16 u16intex;
     u16 u16yei;//ԭ������������
     u16 u16wi;//��ʼ����������
     
     if(sl == 0)
     {
          return 0;
     }
     //Ѱ���ַ���������
     u16yei = sl;
     for(u16intex = 0;u16intex < sl;u16intex++)
     {
          if(sp[u16intex] == '\0')
          {
               u16yei = u16intex;
               break;
          }
     }
     //�ռ䲻����Ÿ��������
     if((u16yei == sl) || (1 == (sl - u16yei)))
     {
          return u16yei;
     }
     //��ʼ����
     u16wi = u16yei;//��������������
     for(u16intex = 0;u16intex < (sl - u16yei);u16intex++)
     {          
          sp[u16wi++] = dp[u16intex];   
          if(dp[u16intex] == '\0')
          {
               return (u16wi - 1);
          }       
     }
     //���ӵ��ռ�����û���ҵ�dp�Ľ���������ָ�ԭ��(������)
     sp[u16yei] = '\0';
     return u16yei;
}

/*-------------------------------------------------
������:FUN_String_StrLink2
��   ��:�ַ������� ����
��   ��:
       u8 *sp��Ŀ���ַ����׵�ַ(��\0��β)
       u16 sl��Ŀ���ַ����ռ��ֽ���
       u8 *dp�����������ݵ��׵�ַ
       u16 dl�����������ݵ��ֽ���
����ֵ:
       ���Ӻ��ַ�������Ч�ֽ���(������\0)
ע   ��:
       sp�������ַ�����������\0��β�ģ���С��65535���ַ�
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-03-08
-------------------------------------------------*/
u16 FUN_String_StrLink2(u8 *sp,u16 sl,u8 *dp,u16 dl)
{
     u16 u16intex;
     u16 u16yei;//ԭ������������
     
     if(sl == 0)
     {
          return 0;
     }
     //Ѱ���ַ���������
     u16yei = sl;
     for(u16intex = 0;u16intex < sl;u16intex++)
     {
          if(sp[u16intex] == '\0')
          {
               u16yei = u16intex;
               break;
          }
     }
     //�ռ䲻����Ÿ��������
     if((u16yei == sl) || (1 == (sl - u16yei)) 
     || (dl == 0) || (dl > (sl - u16yei - 1)))
     {//�ռ䲻����Ÿ��������
          return u16yei;
     }
     //��ʼ����
     for(u16intex = 0;u16intex < dl;u16intex++)
     {
          sp[u16yei++] = dp[u16intex];
     }
     //���ӽ�����
     sp[u16yei] = '\0';
     return u16yei;
}

/*-------------------------------------------------
������:FUN_String_StrLink3
��   ��:�ַ������� ����
��   ��:
       u8 *sp��Ŀ���ַ����׵�ַ(��\0��β)
       u16 sl��Ŀ���ַ����ռ��ֽ���
       u8 *dp���������ַ����׵�ַ(��\0��β)
       u16 *dl���������Ӻ��ַ�������Ч�ֽ���(������\0)
����ֵ:
       0�����ӳɹ�
   С��0������ʧ��(�ռ䲻��)----*dl�ŵ���ԭ��δ���ӵ�*sp�ַ����ĳ���
ע   ��:
       sp��dp�������ַ�����������\0��β�ģ���С��65535���ַ�
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2023-03-17
-------------------------------------------------*/
s8 FUN_String_StrLink3(u8 *sp,u16 sl,u8 *dp,u16 *dl)
{
     u16 u16intex;
     u16 u16yei;//ԭ������������
     u16 u16wi;//��ʼ����������
     
     if(sl == 0)
     {
          *dl = 0;
          return -1;
     }
     //Ѱ���ַ���������
     u16yei = sl;
     for(u16intex = 0;u16intex < sl;u16intex++)
     {
          if(sp[u16intex] == '\0')
          {
               u16yei = u16intex;
               break;
          }
     }
     //�ռ䲻����Ÿ��������
     if((u16yei == sl) || (1 == (sl - u16yei)))
     {
          *dl = u16yei;
          return -2;
     }
     //��ʼ����
     u16wi = u16yei;//��������������
     for(u16intex = 0;u16intex < (sl - u16yei);u16intex++)
     {          
          sp[u16wi++] = dp[u16intex];   
          if(dp[u16intex] == '\0')
          {
               *dl = (u16wi - 1);
               return 0;
          }       
     }
     //���ӵ��ռ�����û���ҵ�dp�Ľ���������ָ�ԭ��(������)
     sp[u16yei] = '\0';
     *dl = u16yei;
     return -3;
}

/*-------------------------------------------------
������:FUN_String_StrCopy
��   ��:�ַ���srp ���� ��sdp�� ����
��   ��:
       u8 *sdp:�ַ����׵�ַ
       u8 sdl:sdp�ռ��ֽ���
       u8 *srp:�ַ����׵�ַ
����ֵ:
       ��
ע   ��:
       srp�������ַ�����������\0��β��
       sdp����������϶�����\0��β��
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2020-08-27
-------------------------------------------------*/
void FUN_String_StrCopy(u8 *sdp,u8 sdl,u8 *srp)
{
     u8 u8intex;
     u8 u8ms = (sdl - 1);
     for(u8intex = 0;u8intex < u8ms;u8intex++)
     {
          sdp[u8intex] = srp[u8intex];
          if(sdp[u8intex] == '\0')
          {
               break;
          }
     }
     if(u8intex == u8ms)
     {
          sdp[u8intex] = '\0';
     }
}

/*-------------------------------------------------
������:FUN_String_HexStrToChar
��   ��:ʮ�������ַ� ת ��ֵ ����
��   ��:
       u8 *hstp:��ת���ַ����׵�ַ
       u8 *rv:ת������ֵ����ĵ�ַ
����ֵ:
       0:ת���ɹ�
   С��0:ת��ʧ��
ע   ��:
       u8 *hstp��ָ��������ֽ����̶�Ϊ2
ʾ   ��:
     ������"3e"���򷵻�62
��   ��:YJX
��   ��:V1.0
ʱ   ��:2020-12-07
-------------------------------------------------*/
s8 FUN_String_HexStrToChar(u8 *hstp,u8 *rv)
{
     u8 u8intex,u8result = 0;
     for(u8intex = 0;u8intex < 2;u8intex++)
     {
          u8result <<= 4;
          if((hstp[u8intex] >= '0') && (hstp[u8intex] <= '9'))
          {               
               u8result += (hstp[u8intex] - '0');
          }
          else if((hstp[u8intex] >= 'A') && (hstp[u8intex] <= 'F'))
          {
               u8result += (hstp[u8intex] - 'A' + 10);
          }
          else if((hstp[u8intex] >= 'a') && (hstp[u8intex] <= 'f'))
          {
               u8result += (hstp[u8intex] - 'a' + 10);
          }
          else
          {
               return -1;
          }
     }
     *rv = u8result;
     return 0;
}

/*-------------------------------------------------
������:FUN_String_CharToHexStr
��   ��:��ֵ תʮ�������ַ� ����
��   ��:
       u8 hex:��ת������ֵ
       u8 *sp:ת���������׵�ַ
����ֵ:
       ��
ע   ��:
       
ʾ   ��:
     ��hex=0x3e  ��sp[0] = '3' sp[1] = 'E'
��   ��:YJX
��   ��:V1.0
ʱ   ��:2020-08-27
-------------------------------------------------*/
void FUN_String_CharToHexStr(u8 hex,u8 *sp)
{
     u8 u8temp;
     //������4λ
     u8temp = (hex >> 4);
     sp[0] = (u8temp + '0');
     if(u8temp >= 10)
     {
          sp[0] = (u8temp - 10 + 'A');
     }
     //������4λ
     u8temp = (hex & 0x0f);
     sp[1] = (u8temp + '0');
     if(u8temp >= 10)
     {
          sp[1] = (u8temp - 10 + 'A');
     }
}

/*-------------------------------------------------
������:FUN_String_StrToS32
��   ��:�ַ��� ת ��ֵ ����
��   ��:
       u8 *sp:��ת���ַ����׵�ַ
       s32 *n:ת���󷵻ص�ֵ
����ֵ:
       С��0:ת��ʧ��
       �����0:ת���ɹ�
ע   ��:
       
ʾ   ��:
     ��"1234" ת�� ����1234
��   ��:YJX
��   ��:V1.0
ʱ   ��:2020-08-27
-------------------------------------------------*/
s8 FUN_String_StrToS32(u8 *sp,s32 *n)
{
     u8 u8intex;
     s8 fh = 0;
     s32 s32result = 0;
     for(u8intex = 0;u8intex < 31;u8intex++)
     {
          if(sp[u8intex] == '\0')
          {//����
               *n = (s32result * fh);
               return 0;
          }
          else if((sp[u8intex] == '-') && (u8intex == 0))
          {
               fh = -1;//�Ǹ���
          }
          else if((sp[u8intex] == '+') && (u8intex == 0))
          {
               fh = 1;//������
          }
          else if((sp[u8intex] < '0') || (sp[u8intex] > '9'))
          {//�Ƿ��ַ�
               return -1;
          }
          else
          {
               if(fh == 0)
               {
                    fh = 1;//������
               }
               s32result = (s32result * 10 + sp[u8intex] - '0');
          }          
     }
     //λ�������31λ
     return -1;
}

/*-------------------------------------------------
������:FUN_String_SmallToS32
��   ��:С��4�ֽ� ת ��ֵ ����
��   ��:
       u8 *sp:С��4�ֽ������׵�ַ
����ֵ:
       ת�������ֵ
ע   ��:
       
ʾ   ��:
     
��   ��:YJX
��   ��:V1.0
ʱ   ��:2020-11-05
-------------------------------------------------*/
s32 FUN_String_SmallToS32(u8 *sp)
{
     s32 s32temp = 0;
     s32temp = sp[3];
     s32temp <<= 8;
     s32temp += sp[2];
     s32temp <<= 8;
     s32temp += sp[1];
     s32temp <<= 8;
     s32temp += sp[0];
     return s32temp;
}

/*-------------------------------------------------
������:FUN_String_S32ToStr
��   ��:��ֵ ת �ַ��� ����
��   ��:
       s32 n:��ת������
       u8 *sp:ת�����ſռ���׵�ַ
       u8 sl:��ſռ��ֽ���
����ֵ:
       С��0:ת��ʧ��
       �����0:ת���ɹ�����Ч�ֽ���������\0
ע   ��:
       ����ַ�������\0��β
ʾ   ��:
     ��-1234 ת�� ����"-1234"
��   ��:YJX
��   ��:V1.0
ʱ   ��:2020-08-27
-------------------------------------------------*/
s8 FUN_String_S32ToStr(s32 n,u8 *sp,u8 sl)
{
     u8 sarr[12],u8intex,u8temp,weishu = 0;
     s32 ntemp = n;
     if(n == 0)
     {
          sp[0] = '0';
          sp[1] = '\0';
          return 1;
     }
     if(n < 0)
     {
          ntemp = (0 - ntemp);//תΪ����
     }
     for(u8intex = 0;u8intex < 12;u8intex++)
     {
          if(ntemp)
          {//������ֵ
               sarr[u8intex] = ((ntemp % 10) + '0');
               ntemp /= 10;
          }
          else
          {
               weishu = u8intex;
               break;//������
          }
     }
     //�жϴ�ſռ��Ƿ��㹻
     if(((n < 0) && (sl < (weishu + 2))) || (sl < (weishu + 1)))
     {//����Ǹ��� ��//���ϸ��ź�\0�����ֽ� �� ���������/����\0����ֽ�
          return -1;//�ռ䲻��
     }     
     u8temp = 0;
     if(n < 0)
     {
          sp[u8temp++] = '-';
     }
     for(u8intex = 0;u8intex < weishu;u8intex++)
     {
          sp[u8temp++] = sarr[weishu - u8intex - 1];
     }
     sp[u8temp] = '\0';
     return u8temp;
}

/*-------------------------------------------------
������:FUN_String_S32ToStr
��   ��:��ֵ ת �ַ��� ����
��   ��:
       float fn:��ת������
       u8 *sp:ת�����ſռ���׵�ַ
       u8 sl:��ſռ��ֽ���
����ֵ:
       С��0:ת��ʧ��
       �����0:ת���ɹ�
ע   ��:
       ����ַ�����\0��β
ʾ   ��:
     ֻʶ��С�����3λ
     ��-12.3456 ת�� ����"-12.345"
��   ��:YJX
��   ��:V1.0
ʱ   ��:2020-08-27
-------------------------------------------------*/
s8 FUN_String_FloatToStr(float fn,u8 *sp,u8 sl)
{
     float ftemp = (fn * 1000);//�ȳ�1000
     s32 s32temp = (s32)ftemp;//��ǿ��תΪ����
     u8 strdata[16],u8num,u8intex,u8j;
     if(FUN_String_S32ToStr(s32temp,strdata,16) < 0)//����תΪ�ַ���
     {//ת��ʧ��
          return -1;
     }
     u8num = FUN_String_StrLen(strdata);
     if(sl < (u8num + 3))//��3��ʾ����һ��С�����һ��\0 ���� ����С��������λʱ����Ҫ��������һ��0
     {//�ռ䲻��
          return -2;
     }

     if(u8num < 3)
     {
          if(u8num == 1)
          {
               if(sl < (u8num + 5))//��3��ʾ����һ��С�����һ��\0 ���� ����С��������λʱ����Ҫ��������һ��0
               {//�ռ䲻��
                    return -2;
               }
               strdata[2] = strdata[0];
               strdata[0] = '0';
               strdata[1] = '0';//��2��0
          }
          else
          {
               if(sl < (u8num + 4))//��3��ʾ����һ��С�����һ��\0 ���� ����С��������λʱ����Ҫ��������һ��0
               {//�ռ䲻��
                    return -2;
               }
               strdata[2] = strdata[1];
               strdata[1] = strdata[0];
               strdata[0] = '0';//��1��0
          }
          strdata[3] = '\0';
          u8num = 3;
     }
     
     
     u8j = 0;
     for(u8intex = 0;u8intex < u8num;u8intex++)
     {
          if(u8intex == (u8num - 3))
          {
               if(u8intex == 0)
               {//С��������λ��������'0'
                    sp[u8j++] = '0';
               }
               sp[u8j++] = '.';//���ַ�����Ӧλ������С����
          }
          sp[u8j++] = strdata[u8intex];
     }
     sp[u8j++] = '\0';
     return 0;
}

/*-------------------------------------------------
������:FUN_String_StrToFloat
��   ��:�ַ��� ת ������ ����
��   ��:
       u8 *sp:��ת���ַ����׵�ַ
       float *fn:ת���󷵻ص�ֵ
����ֵ:
       С��0:ת��ʧ��
       �����0:ת���ɹ�
ע   ��:
       
ʾ   ��:
     ֻʶ��С�����3λ
     ��"-12.3456" ת�� ����-12.345
��   ��:YJX
��   ��:V1.0
ʱ   ��:2020-08-27
-------------------------------------------------*/
s8 FUN_String_StrToFloat(u8 *sp,float *fn)
{
     u8 u8intex;
     s8 fh = 0;
     s8 pointi = -1;//С����������λ��
     s32 s32result = 0;
     for(u8intex = 0;u8intex < 31;u8intex++)
     {
          if(pointi >= 0)
          {//����г��ֹ�С����
               pointi++;//���ڽ������10��pointi�η�
          }
          
          if(sp[u8intex] == '\0')
          {//����
               s32result = (s32result * fh);
               if(pointi)
               {//���ܲ���С���㣬Ҳ���ܴ�С����
                    *fn = (float)s32result;
                    if(pointi > 0)
                    {//������С����
                         pointi--;//�۳���������1�μ���
                         for(u8intex = 0;u8intex < pointi;u8intex++)
                         {
                              //����3.3��2.3������������10.0��Ƚ����⣬��������
                              //3.3 / 10.0�����������0.329
                              //2.3 / 10.0�����������0.229                              
                              *fn = (*fn / 10.0);
                         }
                    }
                    return 0;
               }
               return -1;
          }
          else if((sp[u8intex] == '-') && (u8intex == 0))
          {
               fh = -1;//�Ǹ���
          }
          else if((sp[u8intex] == '+') && (u8intex == 0))
          {
               fh = 1;//������
          }
          else if((sp[u8intex] == '.') && (u8intex != 0) && (pointi < 0))
          {//�ҵ�С����
               pointi = 0;
          }
          else if((sp[u8intex] < '0') || (sp[u8intex] > '9'))
          {//�Ƿ��ַ�
               return -1;
          }
          else
          {
               if(fh == 0)
               {
                    fh = 1;//������
               }
               s32result = (s32result * 10 + sp[u8intex] - '0');
          }           
     }
     //λ�������31λ
     return -1;
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------������ģ����ڲ�����������------------------------------------                            *
*                                                                                                                                           *
********************************************************************************************************************************************/

