#ifndef FUN_SCSI_h
#define FUN_SCSI_h

#include "TypeMro.h"

extern s8 FUN_SCSI_Config(void);
extern u8 FUN_SCSI_GetMaxLUNNum(void);
extern s32 FUN_SCSI_ReceiveDataProcess(u8 *dap,u32 dal);

#endif
