#pragma once

#include "hash.h"

typedef UINT64 BitBoard;

/* AI—p‘åˆæ•Ï” */
extern int CNT_B;
extern int CNT_W;
extern int PassCnt;
extern int LIMIT_DEPTH;
extern int INF_DEPTH;
extern int HINT_DEPTH;
extern int INF_WINDOW;
extern int TurnNum;
extern int MPC_CUT_VAL;
extern int MPC_FLAG;

extern char msg[64];
extern int line[STACK_SIZE];
/* AIŠÖ˜A‚Ì•Ï” */
extern int LIMIT_DEPTH;

extern int bestLineSerachFlag;
extern int bestLineHeadKey;
extern char goodMoveList[32];

/* AIŠÖ˜A‚ÌŠÖ” */
int pv_search_middle(BitBoard, BitBoard, char, int, int, int, Hash *, int);
int CPU_AI_SHARROW_sharrow(BitBoard, BitBoard, char, int, int, int, int);
int ordering_alpha_beta(BitBoard b_board, BitBoard w_board, char depth, 
	int alpha, int beta, int color, int turn, int pass_cnt);
//int _fastcall CPU_AI(BitBoard, BitBoard, char, int, int, int, int);
//int _fastcall CPU_AI(BitBoard, BitBoard, char, int, int, int, int, int);
int EndSolverWinLoss(BitBoard, BitBoard, char, int, int, int, Hash *hash, int);
int EndSolverExact(BitBoard, BitBoard, char, int, int, int, Hash *hash, int);
int EndSolverExact_test(BitBoard, BitBoard, char, int, int, int, int, int[]);
int alpha_beta_final_move(BitBoard , BitBoard , BitBoard , char , int , int , Hash *hash , int);

void init_bestline(void);
void print_bestline(Hash *hash, int depth);