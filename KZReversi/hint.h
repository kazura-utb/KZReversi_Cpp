#pragma once

/* ヒントフラグ */
extern int HINT_LEVEL;
extern BOOL m_FlagHint;
extern double EvalData[64];
extern int PosData[64];

/* ヒント関連関数 */
int Hint_AI(BitBoard, BitBoard, char, int, int, int, int);
int Hint_WinLoss(BitBoard, BitBoard, char, int, int, int, int);
int Hint_Exact(BitBoard, BitBoard, char, int, int, int, int);