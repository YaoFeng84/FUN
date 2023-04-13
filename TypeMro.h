#ifndef TypeMro_h
#define TypeMro_h

//此文件为芯片级的变量类型重定义的头文件
//根据不同的编译器，所定义的变量宽度有所不同。

//---8位宽变量类型重定义---
typedef signed char                     s8;
typedef unsigned char                   u8;
typedef volatile signed char            vs8;
typedef volatile unsigned char          vu8;
//---16位宽变量类型重定义---
typedef signed short int                s16;
typedef volatile signed short int       vs16;
typedef unsigned short int              u16;
typedef volatile unsigned short int    vu16;
//---32位宽变量类型重定义---
typedef signed int                      s32;
typedef volatile signed int             vs32;
typedef unsigned int                    u32;
typedef volatile unsigned int           vu32;
//---64位宽变量类型重定义---
typedef signed long long                s64;
typedef volatile signed long long       vs64;
typedef unsigned long long              u64;
typedef volatile unsigned long long     vu64;

//---定义变量的绝对地址---
//如 u8 Flag ABSADDRESS(0x50);表示将变量绝对地址定义在0x50处
#define ABSADDRESS(add)                 __attribute__((at(add)))
//
#define U16MAZVALUE                     65535     //u16最值数
//
#define NULL_VP                         (void *)0
#define NULL_U8P                        (u8 *)0
#endif

