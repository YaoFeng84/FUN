#ifndef FUN_PinLevelOPR_h
#define FUN_PinLevelOPR_h

#define PLO_OFF     0    //�ر�
#define PLO_ON      1    //����

typedef struct
{
     u8 StartState;        //��ʼ״̬
     u8 MiddState;         //�м�״̬(��Ĭ)
     u8 EndState;          //����״̬
     s16 OnTimeValue;      //�������ȣ���λ���� �� ����(����Ϊ����)
     s16 OffTimeValue;     //�رճ��ȣ���λ���� �� ����(����Ϊ����)
     u16 MiddTimeValue;    //��Ĭ���ȣ���λ���� �� ����(����0)
     s16 SmallNumValue;    //С�����ο�ֵ(����Ϊ���޴�)
     s16 BigNumValue;      //������ο�ֵ(����Ϊ���޴�)
}PLOParaSet;

typedef struct
{
     u8 StateMachine;      //״̬��
     u8 StartState;        //��ʼ״̬
     u8 MiddState;         //�м�״̬(��Ĭ)
     u8 EndState;          //����״̬
     s16 OnTimeValue;      //�������������ο�ֵ(����Ϊ���޴�)
     s16 OnTimeCounter;    //����ʱ�����ֵ
     s16 OffTimeValue;     //�رճ��������ο�ֵ(����Ϊ���޴�)
     s16 OffTimeCounter;   //�ر�ʱ�����ֵ
     u16 MiddTimeValue;    //��Ĭʱ��ο�ֵ(����0)
     u16 MiddTimeCounter;  //��Ĭʱ�����ֵ
     s16 SmallNumValue;    //С�����ο�ֵ(����Ϊ���޴�)
     s16 SmallNumCounter;  //С��������ֵ
     s16 BigNumValue;      //������ο�ֵ(����Ϊ���޴�)
     s16 BigNumCounter;    //���������ֵ
     void(*On)(void);      //��������
     void(*Off)(void);     //�رպ���
     void(*Turn)(void);    //��ת����
}PLOTYPE;


extern void FUN_PinLevelOPR_Config(PLOTYPE *plo,void(*On)(void),void(*Off)(void),void(*Turn)(void),u16 ptms);
extern void FUN_PinLevelOPR_Start_N(PLOTYPE *plo,PLOParaSet ploPara);
extern void FUN_PinLevelOPR_Start_T(PLOTYPE *plo,PLOParaSet ploPara);
extern void FUN_PinLevelOPR_Process(PLOTYPE *Plo);

#endif

