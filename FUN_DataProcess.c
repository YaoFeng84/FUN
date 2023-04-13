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
 * �޸����ݣ�������FUN_DataProcess_HexStrToChar
 * �޸����ڣ�20201207
 * �޸����ߣ�
 * �� �� �ţ�V2.0
 ***********************************************
*/
/********************************************************************************************************************************************
*                                                                                                                                           *
*                ---------------------------------������ģ���ʹ��˵����-----------------------------------------                           *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*                                       �ص�˵����

*/
#include "TypeMro_N.h"
#include "FUN_DataProcess.h"
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
������:FUN_DataProcess_STRIsSame
��   ��:�Ƚ��ַ����Ƿ�һ�� ����
��   ��:
       u8 *dp1:���Ƚ��ַ����׵�ַ
       u8 *dp2:�Ƚ��ַ����׵�ַ
       u8 dl:�Ƚϵĳ���
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
s8 FUN_DataProcess_STRIsSame(u8 *dp1,u8 *dp2,u8 dl)
{
     u8 u8intex;
     for(u8intex = 0;u8intex < dl;u8intex++)
     {
          if(dp1[u8intex] != dp2[u8intex])
          {
               return -1;
          }
          else if(dp1[u8intex] == '\0')
          {//���Ƚ�Դ������ǰ����
               return -1;
          }
     }
     if((dp1[u8intex] != '\0') || (dp2[u8intex] != '\0'))
     {//���Ƚ�Դ���ݻ�û�н���
          return -1;
     }
     return 0;
}

/*-------------------------------------------------
������:FUN_DataProcess_StrLeng
��   ��:�����ַ����ֽ��� ����
��   ��:
       u8 *sp:�ַ����׵�ַ
����ֵ:
       �ֽ���(������\0)
ע   ��:
       sp�������ַ�����������\0��β�ģ���С��250���ַ�
ʾ   ��:
��   ��:YJX
��   ��:V1.0
ʱ   ��:2020-08-27
-------------------------------------------------*/
u8 FUN_DataProcess_StrLen(u8 *sp)
{
     u8 u8intex;
     for(u8intex = 0;u8intex < 250;u8intex++)
     {
          if(sp[u8intex] == '\0')
          {
               return u8intex;
          }
     }
     return 0;
}

/*-------------------------------------------------
������:FUN_DataProcess_StrCopy
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
void FUN_DataProcess_StrCopy(u8 *sdp,u8 sdl,u8 *srp)
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
������:FUN_DataProcess_HexStrToChar
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
s8 FUN_DataProcess_HexStrToChar(u8 *hstp,u8 *rv)
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
������:FUN_DataProcess_CharToHexStr
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
void FUN_DataProcess_CharToHexStr(u8 hex,u8 *sp)
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
������:FUN_DataProcess_StrToS32
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
s8 FUN_DataProcess_StrToS32(u8 *sp,s32 *n)
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
������:FUN_DataProcess_SmallToS32
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
s32 FUN_DataProcess_SmallToS32(u8 *sp)
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
������:FUN_DataProcess_S32ToStr
��   ��:��ֵ ת �ַ��� ����
��   ��:
       s32 n:��ת������
       u8 *sp:ת�����ſռ���׵�ַ
       u8 sl:��ſռ��ֽ���
����ֵ:
       С��0:ת��ʧ��
       �����0:ת���ɹ�
ע   ��:
       ����ַ�������\0��β
ʾ   ��:
     ��-1234 ת�� ����"-1234"
��   ��:YJX
��   ��:V1.0
ʱ   ��:2020-08-27
-------------------------------------------------*/
s8 FUN_DataProcess_S32ToStr(s32 n,u8 *sp,u8 sl)
{
     u8 sarr[12],u8intex,u8temp,weishu = 0;
     s32 ntemp = n;
     if(n == 0)
     {
          sp[0] = '0';
          sp[1] = '\0';
          return 0;
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
     return 0;
}

/*-------------------------------------------------
������:FUN_DataProcess_S32ToStr
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
s8 FUN_DataProcess_FloatToStr(float fn,u8 *sp,u8 sl)
{
     float ftemp = (fn * 1000);//�ȳ�1000
     s32 s32temp = (s32)ftemp;//��ǿ��תΪ����
     u8 strdata[16],u8num,u8intex,u8j;
     if(FUN_DataProcess_S32ToStr(s32temp,strdata,16) < 0)//����תΪ�ַ���
     {//ת��ʧ��
          return -1;
     }
     u8num = FUN_DataProcess_StrLen(strdata);
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
������:FUN_DataProcess_StrToFloat
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
s8 FUN_DataProcess_StrToFloat(u8 *sp,float *fn)
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

