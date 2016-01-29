/*#
  # 手の並べ替え、様々なデータのソートを行います
  #  
  ##############################################
*/

#include "stdafx.h"
#include "KZReversi.h"
#include "GetRev.h"
#include "GetPattern.h"
#include "hint.h"
#include "hash.h"
#include "AI.h"

//void sort_eval_data(int cnt)
//{
//	int i = 0;
//	int h = cnt * 10 / 13;
//	int swaps;
//	double d_temp;
//	int i_temp;
//	if(cnt == 1){ return; }
//	while(1)
//	{
//		swaps = 0;
//		for(i = 0; i + h < cnt; i++)
//		{
//			if(EvalData[i] < EvalData[i + h])
//			{
//				d_temp = EvalData[i];
//				EvalData[i] = EvalData[i + h];
//				EvalData[i + h] = d_temp;
//				i_temp = PosData[i];
//				PosData[i] = PosData[i + h];
//				PosData[i + h] = i_temp;
//				swaps++;
//			}
//		}
//		if(h == 1)
//		{
//			if(swaps == 0)
//			{
//				break;
//			}
//		}
//		else
//		{
//			h = h * 10 / 13;
//		}
//	}
//}

void sort_move_list(char *move_list, int eval_list[], int cnt)
{
	int i = 0;
	int h = cnt * 10 / 13;
	int temp, swaps;
	char ctemp;

	if(cnt == 1){ return; }
	while(1)
	{
		swaps = 0;
		for(i = 0; i + h < cnt; i++)
		{
			if(eval_list[i] < eval_list[i + h])
			{
				ctemp = move_list[i];
				move_list[i] = move_list[i + h];
				move_list[i + h] = ctemp;
				temp = eval_list[i];
				eval_list[i] = eval_list[i + h];
				eval_list[i + h] = temp;
				swaps++;
			}
		}
		if(h == 1)
		{
			if(swaps == 0)
			{
				break;
			}
		}
		else
		{
			h = h * 10 / 13;
		}
	}
}

void sort_move_list(MOVELIST *move_list, int eval_list[], int cnt)
{
	int i = 0;
	int h = cnt * 10 / 13;
	int temp, swaps;
	MOVELIST ctemp;

	if(cnt == 1){ return; }
	while(1)
	{
		swaps = 0;
		for(i = 0; i + h < cnt; i++)
		{
			if(eval_list[i] < eval_list[i + h])
			{
				ctemp = move_list[i];
				move_list[i] = move_list[i + h];
				move_list[i + h] = ctemp;
				temp = eval_list[i];
				eval_list[i] = eval_list[i + h];
				eval_list[i + h] = temp;
				swaps++;
			}
		}
		if(h == 1)
		{
			if(swaps == 0)
			{
				break;
			}
		}
		else
		{
			h = h * 10 / 13;
		}
	}
}

/* コムソート */
void sort_list_use_table(MOVELIST *pos_list, int move_list[], int cnt)
{
	int i = 0;
	int h = cnt * 10 / 13;
	int temp, swaps;
	MOVELIST move_temp;

	if(cnt <= 1){ return; }
	while(1)
	{
		swaps = 0;
		for(i = 0; i + h < cnt; i++)
		{
			if(move_list[i] > move_list[i + h])
			{
				temp = move_list[i];
				move_list[i] = move_list[i + h];
				move_list[i + h] = temp;
				move_temp = pos_list[i];
				pos_list[i] = pos_list[i + h];
				pos_list[i + h] = move_temp;
				swaps++;
			}
		}
		if(h == 1)
		{
			if(swaps == 0)
			{
				break;
			}
		}
		else
		{
			h = h * 10 / 13;
		}
	}
}

/* コムソート */
void sort_list_use_table(char *pos_list, int move_list[], BitBoard rev_list[], int cnt)
{
	int i = 0;
	int h = cnt * 10 / 13;
	int temp, swaps;
	char move_temp;
	BitBoard rev_temp;

	if(cnt <= 1){ return; }
	while(1)
	{
		swaps = 0;
		for(i = 0; i + h < cnt; i++)
		{
			if(move_list[i] > move_list[i + h])
			{
				temp = move_list[i];
				move_list[i] = move_list[i + h];
				move_list[i + h] = temp;

				move_temp = pos_list[i];
				pos_list[i] = pos_list[i + h];
				pos_list[i + h] = move_temp;

				rev_temp = rev_list[i];
				rev_list[i] = rev_list[i + h];
				rev_list[i + h] = rev_temp;

				swaps++;
			}
		}
		if(h == 1)
		{
			if(swaps == 0)
			{
				break;
			}
		}
		else
		{
			h = h * 10 / 13;
		}
	}
}

/* コムソート */
/*void sort_list(int pos_list[], int move_list[], int cnt)
{
	int i = 0;
	int h = cnt * 10 / 13;
	int temp, swaps;
	if(cnt == 1){ return; }
	while(1)
	{
		swaps = 0;
		for(i = 0; i + h < cnt; i++)
		{
			if(move_list[i] > move_list[i + h])
			{
				temp = move_list[i];
				move_list[i] = move_list[i + h];
				move_list[i + h] = temp;
				temp = pos_list[i];
				pos_list[i] = pos_list[i + h];
				pos_list[i + h] = temp;



				swaps++;
			}
		}
		if(h == 1)
		{
			if(swaps == 0)
			{
				break;
			}
		}
		else
		{
			h = h * 10 / 13;
		}
	}
}*/

/* 隅の安定性を算出 */
int get_corner_stability(BitBoard color){
	
	int n_stable = 0;

	if(color & a1){ n_stable += CountBit(color & (0x0000000000000103)); }	// a1, a2, b1
	if(color & a8){ n_stable += CountBit(color & (0x00000000000080c0)); }	// a7, a8, b8
	if(color & h1){ n_stable += CountBit(color & (0x0301000000000000)); }	// g1, h1, h2
	if(color & h8){ n_stable += CountBit(color & (0xc080000000000000)); }	// g8, h7, h8

	return n_stable;
}

/* 静的順序づけ(ビット列からの取得) */
int get_order_position(BitBoard blank)
{
	/* 8 point*/
	BitBoard moves;

	if((moves = blank & 0x8100000000000081) != 0)
	{
		return CountBit((~moves) & (moves - 1));
	}
	/* 7 point */
	if((moves = blank & 0x240000240000) != 0)
	{
		return CountBit((~moves) & (moves - 1));
	}
	/* 6 point*/
	if((moves = blank & 0x1800008181000018) != 0)
	{
		return CountBit((~moves) & (moves - 1));
	}
	/* 5 point*/
	if((moves = blank & 0x2400810000810024) != 0)
	{
		return CountBit((~moves) & (moves - 1));
	}
	/* 4 point*/
	if((moves = blank & 0x182424180000) != 0)
	{
		return CountBit((~moves) & (moves - 1));
	}
	/* 3 point */
	if((moves = blank & 0x18004242001800) != 0)
	{
		return CountBit((~moves) & (moves - 1));
	}
	/* 2 point*/
	if((moves = blank & 0x24420000422400) != 0)
	{
		return CountBit((~moves) & (moves - 1));
	}
	/* 1 point*/
	if((moves = blank & 0x4281000000008142) != 0)
	{
		return CountBit((~moves) & (moves - 1));
	}

	/* 0 point*/
	return CountBit((~blank) & (blank - 1));

}

/* 序盤中盤用 move ordering */
char move_ordering_middle(char *pos_list, BitBoard b_board, BitBoard w_board, 
	Hash *hash, BitBoard moves, BitBoard rev_list[], char depth,
	int alpha ,int beta, int color)
{
	char cnt = 0;
	int pos = 0;
	BitBoard rev;
	BitBoard move_b, move_w, ligal_move_w;
	int score, move_cnt;
	int key, lower, upper;
	int score_list[35];

	if(depth < 4 && LIMIT_DEPTH >= 6){
		do{
			pos = CountBit((moves & (-(INT64)moves)) - 1);
			/* 反転データ取得 */
			rev = GetRev[pos](b_board, w_board);
			move_b = b_board^((one << pos) | rev);
			move_w = w_board^rev;
			init_index_board(move_w, move_b);

			score = ordering_alpha_beta(move_w, move_b, 5 - depth, -NEGAMAX, -NEGAMIN, 
				color ^ 1, TurnNum + depth + 1, 0);
			score_list[cnt] = score;
			pos_list[cnt] = (char )pos;
			rev_list[cnt] = rev;
			cnt++;
			moves = moves & (moves - 1);
		}while(moves);
	}
	else if(depth == 4){
		do{
			pos = CountBit((moves & (-(INT64)moves)) - 1);
			/* 反転データ取得 */
			rev = GetRev[pos](b_board, w_board);

			move_b = b_board^((one << pos) | rev);
			move_w = w_board^rev;
			init_index_board(move_w, move_b);
			score = GetEvalFromPattern(move_w, move_b, color ^ 1, TurnNum + depth + 1);
			score_list[cnt] = score;
			pos_list[cnt] = (char )pos;
			rev_list[cnt] = rev;
			cnt++;
			moves = moves & (moves - 1);
		}while(moves);
	}
	else
	{
		do{
			score = 0;
			pos = CountBit((moves & (-(INT64)moves)) - 1);
			/* 反転データ取得 */
			rev = GetRev[pos](b_board, w_board);
		
			move_b = b_board^((one << pos) | rev);
			move_w = w_board^rev;

			/* 発展型開放度 */
			//degree = (b_board ^ move_b) | (w_board ^ move_w);
			//degree |= ((degree << 1) & 0xfefefefefefefefe ) | ((degree >> 1) & 0x7f7f7f7f7f7f7f7f );
			//degree = ((degree << 8) | degree | (degree >> 8)) & (~(move_b | move_w));
			//score = CountBit(degree);

			ligal_move_w = CreateMoves(move_w, move_b, &move_cnt);
			//if((one << pos) & 0x8100000000000081){
			//	score -= 4;
			//}

			key = KEY_HASH_MACRO(move_w, move_b);
			if(hash->data[key].b_board == move_w && hash->data[key].w_board == move_b){
				if(hash->data[key].depth + 1 >= LIMIT_DEPTH - depth){
					lower = hash->data[key].lower;
					upper = hash->data[key].upper;
					if(upper == lower){
						score -= 65536;
					}
					else if(lower >= -alpha)
					{
						score -= 65536;
					}
					else if(upper <= -beta)
					{
						score -= 65536;
					}
					else{
						score -= 8192;
					}
				}
				else{
					score -= 4096;
				}
			}	
				/* 位置得点 */
				score -= pos_eval[pos] << 2;
				score += CountBit(rev) << 4;
				/* 敵の着手可能数を取得 */
				score += ((CountBit(ligal_move_w) + CountBit(ligal_move_w & 0x8100000000000081)) << 4) - get_corner_stability(move_b);
				score += CountBit(get_potential_moves(move_w, move_b, ~(move_b | move_w))) << 3;
			score_list[cnt] = score;
			pos_list[cnt] = (char )pos;
			rev_list[cnt] = rev;
			cnt++;
			moves = moves & (moves - 1);

		}while(moves);
	}
	/* 敵の得点の少ない順にソート */
	sort_list_use_table(pos_list, score_list, rev_list, cnt);

	return cnt;
}

/*  終盤用 move ordering */
char move_ordering_end(char *pos_list, BitBoard b_board, BitBoard w_board, Hash *hash, BitBoard moves, BitBoard rev_list[], char depth)
{
	char cnt = 0;
	int pos = 0;
	BitBoard rev, ligal_move_w;
	BitBoard move_b, move_w;
	int score, move_cnt;
	int score_list[35];
	//int key;

	do{
		score = 0;
		pos = CountBit((moves & (-(INT64)moves)) - 1);
		/* 反転データ取得 */
		rev = GetRev[pos](b_board, w_board);
		
		move_b = b_board^((one << pos) | rev);
		move_w = w_board^rev;

		ligal_move_w = CreateMoves(move_w, move_b, &move_cnt);
		//if((one << pos) & 0x8100000000000081){
		//	score -= 4;
		//}

		/* 敵の着手可能数を取得 */
		score += ((CountBit(ligal_move_w) + CountBit(ligal_move_w & 0x8100000000000081)) << 4);
		score -= get_corner_stability(move_b);
		//score += CountBit(get_potential_moves(move_w, move_b, ~(move_b | move_w)));

		score_list[cnt] = score;
		pos_list[cnt] = (char )pos;
		rev_list[cnt] = rev;
		cnt++;
		moves = moves & (moves - 1);
	}while(moves);

	/* 敵の得点の少ない順にソート */
	sort_list_use_table(pos_list, score_list, rev_list, cnt);

	return cnt;
}


/*  終盤用 move ordering */
char move_ordering_end(MOVELIST *pos_list, BitBoard b_board, BitBoard w_board, BitBoard moves)
{
	char cnt = 0;
	int pos = 0;
	BitBoard rev, ligal_move_w;
	BitBoard move_b, move_w;
	int score = 0, move_cnt;
	int score_list[36];

	do{
		score = 0;
		pos = CountBit((moves & (-(INT64)moves)) - 1);
		/* 反転データ取得 */
		rev = GetRev[pos](b_board, w_board);
		
		move_b = b_board^((one << pos) | rev);
		move_w = w_board^rev;

		ligal_move_w = CreateMoves(move_w, move_b, &move_cnt);

		/* 敵に隅を明け渡さない */
		score = CountBit(ligal_move_w & 0x8100000000000081) << 1;
		/* 適切な星Ｃ打ちの可能性大の場合 */
		if((one << pos) & 0x42C300000000C342){
			if(score == 0)
			{
				score--;
			}
		}
		/*if(0x8100000000000081 & (one << pos)){
			score -= 4;
		}*/

		/* 敵の着手可能数を取得 */
		score += (move_cnt << 5);
		score += get_corner_stability(move_w) - get_corner_stability(move_b);

		score_list[cnt] = score;
		pos_list[cnt].move = (char )pos;
		pos_list[cnt].rev = rev;
		cnt++;
		moves = moves & (moves - 1);
	}while(moves);

	/* 敵の得点の少ない順にソート */
	sort_list_use_table(pos_list, score_list, cnt);

	return cnt;
}
