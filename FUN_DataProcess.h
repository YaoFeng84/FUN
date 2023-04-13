#ifndef FUN_DataProcess_h
#define FUN_DataProcess_h

extern s8 FUN_DataProcess_STRIsSame(u8 *dp1,u8 *dp2,u8 dl);

extern u8 FUN_DataProcess_StrLen(u8 *sp);

extern void FUN_DataProcess_StrCopy(u8 *sdp,u8 sdl,u8 *srp);

extern s8 FUN_DataProcess_HexStrToChar(u8 *hstp,u8 *rv);

extern void FUN_DataProcess_CharToHexStr(u8 hex,u8 *sp);

extern s8 FUN_DataProcess_StrToS32(u8 *sp,s32 *n);

extern s32 FUN_DataProcess_SmallToS32(u8 *sp);

extern s8 FUN_DataProcess_StrToFloat(u8 *sp,float *fn);

extern s8 FUN_DataProcess_S32ToStr(s32 n,u8 *sp,u8 sl);

extern s8 FUN_DataProcess_FloatToStr(float fn,u8 *sp,u8 sl);

#endif


