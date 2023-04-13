#ifndef FUN_PinLevelOPR_h
#define FUN_PinLevelOPR_h

#define PLO_OFF     0    //关闭
#define PLO_ON      1    //开启

typedef struct
{
     u8 StartState;        //起始状态
     u8 MiddState;         //中间状态(静默)
     u8 EndState;          //结束状态
     s16 OnTimeValue;      //开启长度，单位毫秒 或 次数(负数为无限)
     s16 OffTimeValue;     //关闭长度，单位毫秒 或 次数(负数为无限)
     u16 MiddTimeValue;    //静默长度，单位毫秒 或 次数(大于0)
     s16 SmallNumValue;    //小次数参考值(负数为无限次)
     s16 BigNumValue;      //大次数参考值(负数为无限次)
}PLOParaSet;

typedef struct
{
     u8 StateMachine;      //状态机
     u8 StartState;        //起始状态
     u8 MiddState;         //中间状态(静默)
     u8 EndState;          //结束状态
     s16 OnTimeValue;      //开启持续次数参考值(负数为无限次)
     s16 OnTimeCounter;    //开启时间计数值
     s16 OffTimeValue;     //关闭持续次数参考值(负数为无限次)
     s16 OffTimeCounter;   //关闭时间计数值
     u16 MiddTimeValue;    //静默时间参考值(大于0)
     u16 MiddTimeCounter;  //静默时间计数值
     s16 SmallNumValue;    //小次数参考值(负数为无限次)
     s16 SmallNumCounter;  //小次数计数值
     s16 BigNumValue;      //大次数参考值(负数为无限次)
     s16 BigNumCounter;    //大次数计数值
     void(*On)(void);      //开启函数
     void(*Off)(void);     //关闭函数
     void(*Turn)(void);    //翻转函数
}PLOTYPE;


extern void FUN_PinLevelOPR_Config(PLOTYPE *plo,void(*On)(void),void(*Off)(void),void(*Turn)(void),u16 ptms);
extern void FUN_PinLevelOPR_Start_N(PLOTYPE *plo,PLOParaSet ploPara);
extern void FUN_PinLevelOPR_Start_T(PLOTYPE *plo,PLOParaSet ploPara);
extern void FUN_PinLevelOPR_Process(PLOTYPE *Plo);

#endif

