/*#
  # ヒント機能関連の関数です(未実装)
  #  
  ##############################################
*/

#include "stdafx.h"
#include "KZReversi.h"
#include "GetRev.h"
#include "AI.h"
#include "GetPattern.h"
#include "ordering.h"
#include "game.h"

int HINT_LEVEL;
int HINT_DEPTH;
BOOL m_FlagHint;
double EvalData[64];
int PosData[64];

int Hint_WinLoss(BitBoard b_board, BitBoard w_board, char depth, int alpha, int beta, int turn, int pass_cnt)
{
	return 0;
}

int Hint_Exact(BitBoard b_board, BitBoard w_board, char depth, int alpha, int beta, int turn, int pass_cnt)
{
	return 0;
}

int Hint_AI_sharrow(BitBoard b_board, BitBoard w_board, char depth, int alpha, int beta, int turn, int pass_cnt)
{
	return 0;

}

/* ヒント用AI */
int Hint_AI(BitBoard b_board, BitBoard w_board, char depth, int alpha, int beta, int turn, int pass_cnt){

	return 0;

}