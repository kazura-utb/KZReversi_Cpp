#pragma once


#define ABORT 65536
#define CPU 0
#define HUMAN 1

extern int turn;
extern int MAXEVAL;

/* ゲーム進行フラグ */
extern BOOL m_FlagForWhite;
extern BOOL m_FlagSearchWin;
extern BOOL m_FlagInGame;
extern BOOL m_FlagInterrupt;
extern BOOL m_FlagPerfectSearch;
extern BOOL m_FlagBook;

extern BOOL CpuColor;
extern BOOL Flag_Edit;
extern BOOL Flag_Hint;
extern BOOL Flag_Abort;

extern BOOL Interrupt_flag;
extern BOOL AI_THINKING;
extern BOOL Force_SearchWin;
extern BOOL Force_SearchSD;

extern int difficult_table[12];

extern int player[2];

extern Hash *ghash;

/* ゲーム進行用関数 */
int IsPass(BitBoard, BitBoard);
int IsWin(BitBoard, BitBoard);

int CpuProcess(HWND, HWND);
int middle_process(BitBoard, BitBoard, int, Hash *hash);
int sharrow_process(BitBoard, BitBoard, int);

int RemainStone(BitBoard, BitBoard);