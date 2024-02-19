#ifndef FUN_String_h
#define FUN_String_h

#include "TypeMro.h"

extern s8 FUN_String_STRIsSame(u8 *dp1,u8 *dp2);
extern s8 FUN_String_STRIsSame2(u8 *dp1,u8 *dp2,u16 dl);

extern s16 FUN_String_Split(u8 *dp,u8 *fp,u16 *ip,u8 il);
extern s16 FUN_String_Split2(u8 *dp,u16 dl,u8 *fp,u8 fl,u16 *ip,u8 il);
extern s16 FUN_String_Split3(u8 *dp,u8 *fp,u16 *ip,u8 il);

extern u8 *FUN_String_Ltrim(u8 *sp);

extern u16 FUN_String_StrLen(u8 *sp);

extern u16 FUN_String_StrLink(u8 *sp,u16 sl,u8 *dp);
extern u16 FUN_String_StrLink2(u8 *sp,u16 sl,u8 *dp,u16 dl);
extern s8 FUN_String_StrLink3(u8 *sp,u16 sl,u8 *dp,u16 *dl);

extern void FUN_String_StrCopy(u8 *sdp,u8 sdl,u8 *srp);

extern s8 FUN_String_HexStrToChar(u8 *hstp,u8 *rv);

extern void FUN_String_CharToHexStr(u8 hex,u8 *sp);

extern s8 FUN_String_StrToS32(u8 *sp,s32 *n);

extern s32 FUN_String_SmallToS32(u8 *sp);

extern s8 FUN_String_StrToFloat(u8 *sp,float *fn);

extern s8 FUN_String_S32ToStr(s32 n,u8 *sp,u8 sl);

extern s8 FUN_String_FloatToStr(float fn,u8 *sp,u8 sl);

#endif


