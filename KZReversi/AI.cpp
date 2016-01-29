/*#
# CPUのAI本体です
#  
##############################################
*/

#include "stdafx.h"
#include "KZReversi.h"
#include "GetRev.h"
#include "GetPattern.h"
#include "ordering.h"
#include "game.h"
#include "hash.h"

/* AI用グローバル変数 */
char AI_msg[64];
char wld_msg[3][6] = {"Loss","Draw","Win"};
//t_table table[TABLE_SIZE];
int LIMIT_DEPTH, INF_DEPTH;
INT64 COUNT_NODE, COUNT_NODE2, BASE_NODE_NUM;
int CNT_B, CNT_W;
int MAXEVAL;
int eval_list[34];
int turn, TurnNum;
int CpuColor;

int bestLineSerachFlag;
int bestLineHeadKey;

char goodMoveList[60];

MPCINFO MPCInfo[22];
double MPC_CUT_VAL;
int MPC_FLAG;

int QuadParityTable[64] = 
{
	0,0,0,0,1,1,1,1,
	0,0,0,0,1,1,1,1,
	0,0,0,0,1,1,1,1,
	0,0,0,0,1,1,1,1,
	2,2,2,2,3,3,3,3,
	2,2,2,2,3,3,3,3,
	2,2,2,2,3,3,3,3,
	2,2,2,2,3,3,3,3
};

static const int stability_threshold[] = { 65, 65, 65, 65, 65, 46, 38, 30, 24,
	24, 24, 24, 0, 0, 0, 0, 0, 0, 0 };

void init_bestline(){

	for(int i = 0; i < 60; i++){
		goodMoveList[i] = -1;
	}
}

void print_bestline(Hash *hash, int depth){
	if(depth < 0){
		return;
	}
	char msg[256];
	OutputDebugString("bestline::");
	for(int i = 0; goodMoveList[i] != -1; i++){
		sprintf_s(msg, "%s-", cordinates_table[goodMoveList[i]]);
		OutputDebugString(msg);
	}

}

/* 合法手生成 */
BitBoard CreateMoves(BitBoard b_board, BitBoard w_board, int *count){

	BitBoard moves;

	bit_b_board.high = (b_board >> 32);
	bit_b_board.low = (b_board & 0xffffffff);

	bit_w_board.high = (w_board >> 32);
	bit_w_board.low = (w_board & 0xffffffff);

	*count = bit_mob(bit_b_board, bit_w_board, &moves);

	return moves;
}

void hash_update(HashInfo *hash_info, int bestmove, char depth, int max, int alpha, int beta, int lower, int upper){

	hash_info->locked = FALSE;
	hash_info->bestmove = (char)bestmove;
	hash_info->depth = depth;

	if(max >= beta)
	{
		hash_info->lower = max;
		hash_info->upper = upper;
	}
	else if(max <= alpha)
	{
		hash_info->lower = lower;
		hash_info->upper = max;
	}
	else
	{
		hash_info->lower = max;
		hash_info->upper = max;
	}
}

void hash_create(
	HashInfo *hash_info, 
	BitBoard b_board, BitBoard w_board,
	int bestmove, 
	int move_cnt,
	char depth, 
	int max, 
	int alpha, int beta, 
	int lower, int upper){


		/* 置換表に登録 */
		hash_info->b_board = b_board;
		hash_info->w_board = w_board;
		hash_info->depth = depth;
		/* 現在の局面の指し手優先順位と着手可能数を保存 */
		hash_info->move_cnt = move_cnt;
		hash_info->bestmove = bestmove;

		hash_info->locked = FALSE;


		if(max >= beta)
		{
			hash_info->lower = max;
			hash_info->upper = upper;
		}
		else if(max <= alpha)
		{
			hash_info->lower = lower;
			hash_info->upper = max;
		}
		else
		{
			hash_info->lower = max;
			hash_info->upper = max;
		}
}

int ordering_alpha_beta(BitBoard b_board, BitBoard w_board, char depth, 
	int alpha, int beta, int color, int turn, int pass_cnt)
{

	/* アボート処理 */
	if(Flag_Abort)
	{
		return ABORT;
	}

	COUNT_NODE++;

	if(depth == 0){
		/* 葉ノード(読みの限界値のノード)の場合は評価値を算出 */
		init_index_board(b_board, w_board);
		return GetEvalFromPattern(b_board, w_board, color, turn - 1);
	}

	int move_cnt;
	int max;                    //現在の最高評価値
	int eval;                   //評価値の保存
	BitBoard rev;
	BitBoard moves;             //合法手のリストアップ

	/* 合法手生成とパスの処理 */
	if((moves = CreateMoves(b_board, w_board, &move_cnt)) == 0){
		if(pass_cnt == 1)
		{

			/* 勝ち(1)と負け(-1)および引き分け(0)であれば、それ相応の評価値を返す */
			if(b_board == 0)
			{
				return -1400064;
			}
			else if(w_board == 0)
			{
				return 1400064;
			}
			else {
				CNT_B = CountBit(b_board);
				CNT_W = CountBit(w_board);

				if(CNT_B > CNT_W){
					return 1400000 + (CNT_B - CNT_W);
				}
				else{
					return -1400000 - (CNT_B - CNT_W);
				}
			}
		}
		max = -ordering_alpha_beta(w_board, b_board, depth, -beta, -alpha, color ^ 1, turn, 1);

		return max;
	}
	else
	{
		int pos;
		max = NEGAMIN;
		do{
			/* 静的順序づけ（少ないコストで大幅に高速化するみたい） */
			pos = get_order_position(moves);
			rev = GetRev[pos](b_board, w_board);

			eval = -ordering_alpha_beta( w_board^rev, b_board^((one << pos)|rev), 
				depth - 1, -beta, -alpha, color ^ 1, turn + 1, 0);

			if(beta <= eval){
				return eval;
			}

			/* 今までより良い局面が見つかれば最善手の更新 */
			if(eval > max)
			{
				max = eval;
				alpha = max(alpha, eval);
			}

			moves ^= one << pos;
		}while(moves);
	}

	return max;

}

int alpha_betaSearch(BitBoard b_board, BitBoard w_board, char depth, 
	int alpha, int beta, int color, int turn, int pass_cnt)
{

	/* アボート処理 */
	if(Flag_Abort)
	{
		return ABORT;
	}

	COUNT_NODE++;

	if(depth == 0){
		/* 葉ノード(読みの限界値のノード)の場合は評価値を算出 */
		init_index_board(b_board, w_board);
		return GetEvalFromPattern(b_board, w_board, color, turn - 1);
	}

	if (depth >= MPC_DEPTH_MIN) {
		if(turn >= 36){
			MPC_CUT_VAL = 2.0;
		}
		else{
			MPC_CUT_VAL = 1.5;
		}
		MPCINFO *mpc_info = &MPCInfo[depth - MPC_DEPTH_MIN];
		int value = (int)(alpha - (mpc_info->deviation * MPC_CUT_VAL) - mpc_info->offset);
		int eval = alpha_betaSearch(b_board, w_board, mpc_info->depth, value - 1, value, color, turn, pass_cnt);
		if (eval < value) {
			return alpha;
		}
		value = (int)(beta + (mpc_info->deviation * MPC_CUT_VAL) - mpc_info->offset);
		eval = alpha_betaSearch(b_board, w_board, mpc_info->depth, value, value + 1, color, turn, pass_cnt);
		if (eval > value) {
			return beta;
		}
	}

	int move_cnt;
	int max;                    //現在の最高評価値
	int eval;                   //評価値の保存
	BitBoard rev;
	BitBoard moves;             //合法手のリストアップ

	/* 合法手生成とパスの処理 */
	if((moves = CreateMoves(b_board, w_board, (int *)(&move_cnt))) == 0){
		if(pass_cnt == 1)
		{
			/* 勝ち(1)と負け(-1)および引き分け(0)であれば、それ相応の評価値を返す */
			if(b_board == 0)
			{
				return -1400064;
			}
			else if(w_board == 0)
			{
				return 1400064;
			}
			else {
				CNT_B = CountBit(b_board);
				CNT_W = CountBit(w_board);

				if(CNT_B >= CNT_W){
					return 1400000 + (CNT_B - CNT_W);
				}
				else{
					return -1400000 + (CNT_B - CNT_W);
				}
			}
		}
		max = -alpha_betaSearch( w_board, b_board, depth, -beta, -alpha, color ^ 1, turn, 1);

		return max;
	}
	else
	{
		int pos;
		max = NEGAMIN;
		do{
			/* 静的順序づけ（少ないコストで大幅に高速化するみたい） */
			pos = get_order_position(moves);
			rev = GetRev[pos](b_board, w_board);
			/* ターンを進めて再帰処理へ */
			eval = -alpha_betaSearch( w_board^rev, b_board^((one << pos)|rev), 
				depth - 1, -beta, -alpha, color ^ 1, turn + 1, 0);
			if(beta <= eval){
				return eval;
			}

			/* 今までより良い局面が見つかれば最善手の更新 */
			if(eval > max)
			{
				max = eval;
				alpha = max(alpha, eval);
			}

			moves ^= one << pos;
		}while(moves);
	}

	return max;

}

int pv_search_middle(BitBoard bk, BitBoard wh, char depth, int alpha, int beta, int color, Hash *hash, int pass_cnt)
{
	/* アボート処理 */
	if(Flag_Abort)
	{
		return ABORT;
	}

	COUNT_NODE++;

	if(depth < 4 && LIMIT_DEPTH > 2)
	{
		// 葉に近い探索
		return alpha_betaSearch(bk, wh, depth, alpha, beta, color, TurnNum + LIMIT_DEPTH - depth, pass_cnt);
	}
	else if(depth == 0){
		/* 葉ノード(読みの限界値のノード)の場合は評価値を算出 */
		init_index_board(bk, wh);
		return GetEvalFromPattern(bk, wh, color, TurnNum + LIMIT_DEPTH - 1);
	}

	if(COUNT_NODE - COUNT_NODE2 > BASE_NODE_NUM){
		end_t = timeGetTime();
		double timer = (end_t - start_t) / 1000;
		if(COUNT_NODE > 1000000000){
			sprintf_s(AI_msg, "nodes:%.2f[Gn], NPS:%.1f[Kn]", COUNT_NODE / (double)1000000000, (COUNT_NODE / timer) / (double)1000);
			BASE_NODE_NUM = 100000;
		}
		else if(COUNT_NODE > 1000000){
			sprintf_s(AI_msg, "nodes:%.1f[Mn], NPS:%.1f[Kn]", COUNT_NODE / (double)1000000, (COUNT_NODE / timer) / (double)1000);
			BASE_NODE_NUM = 100000;
		}
		else if(COUNT_NODE > 1000){
			sprintf_s(AI_msg, "nodes:%.1f[Kn], NPS:%.1f[Kn]", COUNT_NODE / (double)1000, (COUNT_NODE / timer) / (double)1000);
			BASE_NODE_NUM = 100;
		}

		else{
			sprintf_s(AI_msg, "nodes:%lld, NPS:%d[n]", COUNT_NODE, (int)((COUNT_NODE / (end_t - start_t)) / 1000));
			BASE_NODE_NUM = 1;
		}

		SendMessage(hStatus, SB_SETTEXT, (WPARAM)0 | 0, (LPARAM)AI_msg);
		COUNT_NODE2 = COUNT_NODE;
	}

	BOOL entry_flag;
	int ret;
	int lower, upper;
	HashInfo hash_info;

	/* キーを生成 */
	int key = KEY_HASH_MACRO(bk, wh);
	/* 置換表を検索 */
	if((ret = HashGet(hash, key, bk, wh, &hash_info)) == TRUE)
	{
		if(hash_info.depth - depth >= 0){
			lower = hash_info.lower;
			if(lower >= beta)
			{
				return lower;
			}
			upper = hash_info.upper;
			if(upper <= alpha || upper == lower)
			{
				return upper;
			}
			alpha = max(alpha, lower);
			beta = min(beta, upper);
		}
		else{
			hash_info.depth = depth;
			lower = NEGAMIN;
			upper = NEGAMAX;
		}
		entry_flag = TRUE;
	}
	else
	{
		hash_info.depth = depth;
		entry_flag = FALSE;
		lower = NEGAMIN;
		upper = NEGAMAX;
	}

	if (depth >= MPC_DEPTH_MIN && depth <= 24) {
		int t_num = TurnNum + LIMIT_DEPTH - depth;
		if(t_num >= 36){
			MPC_CUT_VAL = 1.96;
		}
		else{
			MPC_CUT_VAL = 1.4;
		}
		MPCINFO *mpc_info = &MPCInfo[depth - MPC_DEPTH_MIN];
		int value = (int)(alpha - (mpc_info->deviation * MPC_CUT_VAL) - mpc_info->offset);
		int eval = alpha_betaSearch(bk, wh, mpc_info->depth, value - 1, value, color, t_num, pass_cnt);
		if (eval < value) {
			return alpha;
		}
		value = (int)(beta + (mpc_info->deviation * MPC_CUT_VAL) - mpc_info->offset);
		eval = alpha_betaSearch(bk, wh, mpc_info->depth, value, value + 1, color, t_num, pass_cnt);
		if (eval > value) {
			return beta;
		}
	}

	int max, max_move;
	int eval;
	int move_cnt;
	BitBoard moves, rev;
	max = NEGAMIN;
	int p;
	int a_window = alpha;
	BitBoard rev_list[35];
	char pos_list[35];
	bool pv_flag = false;

	if(entry_flag == TRUE)
	{
		hash_info.locked = TRUE;
		/* 置換表から前の探索における最善手を取得 */
		p = hash_info.bestmove;
		rev = GetRev[p](bk, wh);
		/* PV値を取得できると信じてやってみる(これをやると遅いのかもしれない・・・評価関数の精度による) */
		max_move = p;
		eval = -pv_search_middle(wh^rev, bk^((one << p) | rev), depth - 1, 
			-beta, -a_window, color ^ 1, hash, 0);
		if(eval >= beta)
		{
			return beta;   // fail-soft beta-cutoff
		}
		if(eval > max)
		{
			a_window = max(a_window, eval);
			max = eval;
			if (eval > alpha) {
				pv_flag = true;
			}
		}

		// 以降，前の探索の最善手が最善ではない可能性がある場合に通る
		moves = CreateMoves(bk, wh, (int *)(&move_cnt));
		if(move_cnt == 0){
			if(pass_cnt == 1)
			{
				if(bk == 0)
				{
					return -1400064;
				}
				if(wh == 0)
				{
					return 1400064;
				}
				CNT_B = CountBit(bk);
				CNT_W = CountBit(wh);

				if(CNT_B > CNT_W){
					return 1400000 + (CNT_B - CNT_W);
				}
				else{
					return -1400000 + (CNT_B - CNT_W);
				}
			}
			max = -pv_search_middle(wh, bk, depth, -beta, -alpha, color ^ 1, hash, 1);
			return max;
		}

		// 置換表の最善手を除去
		moves ^= (one << p);
		move_cnt--;

		if(move_cnt != 0)
		{
			// 着手の適当な順序付け
			if(move_cnt > 1){
				move_ordering_middle(pos_list, bk, wh, hash, moves, rev_list, 
					LIMIT_DEPTH - depth, alpha, beta, color);
			}
			else{
				// 残り着手が1手しかない場合
				pos_list[0] = CountBit(moves - 1);
				rev_list[0] = GetRev[pos_list[0]](bk, wh);
			}

			for(int i = 0; i < move_cnt; i++){

				p = pos_list[i];
				rev = rev_list[i];

				if(pv_flag == true){
					// PV値を取得できているのでnull-window探索
					eval = -pv_search_middle(wh ^ rev, bk ^ ((one << p) | rev), depth - 1, 
						-(a_window + 1), -a_window, color ^ 1, hash, 0);
					if (eval > a_window && eval < beta){ // in fail-soft
						// re-search
						eval = -pv_search_middle(wh ^ rev, bk ^ ((one << p) | rev), depth - 1, 
							-beta, -eval, color ^ 1, hash, 0);
					}
				}
				else {
					// PV値を取得できていないので通常幅での探索
					eval = -pv_search_middle(wh^rev, bk^((one << p) | rev), depth - 1, 
						-beta, -a_window, color ^ 1, hash, 0);
				}

				if(eval >= beta)
				{
					return beta;   // fail-soft beta-cutoff
				}
				if(eval > max)
				{
					a_window = max(a_window, eval);
					max = eval;
					max_move = p;
					if (eval > alpha) {
						pv_flag = true;
					}
				}
			}
		}
		// 置換表更新
		hash_update(&hash_info, max_move, depth, max, alpha, beta, lower, upper);
		HashSet(hash, key, &hash_info);
	}
	else{
		/* 合法手生成とパスの処理 */
		moves = CreateMoves(bk, wh, (int *)(&move_cnt));
		if(move_cnt == 0){
			if(pass_cnt == 1)
			{
				if(bk == 0)
				{
					return -1400064;
				}
				if(wh == 0)
				{
					return 1400064;
				}
				CNT_B = CountBit(bk);
				CNT_W = CountBit(wh);

				if(CNT_B > CNT_W){
					return 1400000 + (CNT_B - CNT_W);
				}
				else{
					return -1400000 + (CNT_B - CNT_W);
				}
			}
			max = -pv_search_middle(wh, bk, depth, -beta, -alpha, color ^ 1, hash, 1);
			return max;
		}

		// 着手の適当な順序付け
		if(move_cnt > 1){
			move_ordering_middle(pos_list, bk, wh, hash, moves, rev_list, 
				LIMIT_DEPTH - depth, alpha, beta, color);
		}
		else {
			// 残り着手が1手しかない場合
			pos_list[0] = CountBit(moves - 1);
			rev_list[0] = GetRev[pos_list[0]](bk, wh);
		}

		// オーダリングの先頭の手を最善として探索
		p = pos_list[0];
		rev = rev_list[0];
		max_move = p;
		eval = -pv_search_middle(wh^rev, bk^((one << p) | rev), depth - 1, 
			-beta, -a_window, color ^ 1, hash, 0);
		if(eval >= beta)
		{
			return beta;   // fail-soft beta-cutoff
		}
		if(eval > max)
		{
			a_window = max(a_window, eval);
			max = eval;
		}

		for(int i = 1; i < move_cnt; i++){

			p = pos_list[i];
			rev = rev_list[i];

			if(pv_flag == true){
				// PV値を取得できているのでnull-window探索
				eval = -pv_search_middle(wh ^ rev, bk ^ ((one << p) | rev), depth - 1, 
					-(a_window + 1), -a_window, color ^ 1, hash, 0);
				if (eval > a_window && eval < beta){ // in fail-soft
					// re-search
					eval = -pv_search_middle(wh ^ rev, bk ^ ((one << p) | rev), depth - 1, 
						-beta, -eval, color ^ 1, hash, 0);
				}
			}
			else {
				// PV値を取得できていないので通常幅での探索
				eval = -pv_search_middle(wh^rev, bk^((one << p) | rev), depth - 1, 
					-beta, -a_window, color ^ 1, hash, 0);
			}

			if(eval >= beta)
			{
				return beta;   // fail-soft beta-cutoff
			}
			if(eval > max)
			{
				a_window = max(a_window, eval);
				max = eval;
				max_move = p;
				if (eval > alpha) {
					pv_flag = true;
				}
			}
		}

		/* 置換表に登録 */
		if(hash->data[key].locked == FALSE && ret != LOCKED)
		{
			hash_create(&hash_info, bk, wh, max_move, move_cnt, 
				depth, max, alpha, beta, lower, upper);
			HashSet(hash, key, &hash_info);
		}
	}

	return max;
}

void cpy_stack(int cpied[], int cpy[], int depth)
{
	int i;
	for(i = depth; i < STACK_SIZE; i++)
	{
		cpied[i] = cpy[i];
	}
}

void cpy_zerostack(int cpied[], int depth)
{
	int i;
	for(i = depth; i < STACK_SIZE; i++)
	{
		cpied[i] = -1;
	}
}

void print_line(int line[])
{
	int i, pass_cnt = 0;
	char line_msg[512] = {0};
	if(line[0] == -1) { return; }
	for(i = 0; i < STACK_SIZE; i++)
	{
		if(line[i] == -1)
		{
			pass_cnt++;
			if(line[i + 1] == -1)
			{
				strcat_s(line_msg, "...");
				break;
			}
			if(line[i + 1] == -2)
			{
				line_msg[strlen(line_msg)] = '\0';
				break;
			}
			strcat_s(line_msg, "pass");
		}
		else
		{
			if(line[i] >= A1 && line[i] <= H8)
			{
				strcat_s(line_msg, cordinates_table[line[i]]);
				pass_cnt = 0;
			}
			else
			{
				break;
			}
		}
		strcat_s(line_msg, "-");
	}
	line_msg[strlen(line_msg) - 1] = '\0';
	strcat_s(line_msg, "\0");
	SetWindowText(Edit_hWnd, line_msg);
	SendMessage(hStatus, SB_SETTEXT, (WPARAM)3 | 0, (LPARAM)line_msg);
	UpdateWindow(Edit_hWnd);
}

/* CPU:AIの本体(終盤ソルバー最終石差探索) #FFO40 4秒程度 nps:8000k */
//ネガスカウト + move ordering + 置換表をノードの高さによって組み合わせる
//int EndSolverExact_test(BitBoard b_board, BitBoard w_board, char depth, int alpha, int beta, int turn, int pass_cnt, int line_stack[])
//{
//	return 0;
//}

int alpha_beta_final_move(BitBoard b_board, BitBoard w_board, BitBoard move1, Hash *hash, int alpha, int beta)
{
	COUNT_NODE++;

	int pos = CountBit(move1 - 1);
	BitBoard rev = GetRev[pos](b_board, w_board);
	/* 打てる？ */
	if(rev)
	{
		CNT_B = CountBit(b_board ^ (move1 | rev));
		CNT_W = CountBit(w_board ^ rev);
		return CNT_B - CNT_W;
	}
	else{
		rev = GetRev[pos](w_board, b_board);
		/* 打てる？ */
		if(rev)
		{
			CNT_B = CountBit(b_board ^ rev);
			CNT_W = CountBit(w_board ^ (move1 | rev));
			return CNT_B - CNT_W;
		}
		else{

			if(b_board == 0)
			{
				return -64;
			}
			if(w_board == 0)
			{
				return 64;
			}
			CNT_B = CountBit(b_board);
			CNT_W = CountBit(w_board);
			return CNT_B - CNT_W;

		}
	}

}

//int _fastcall EndSolverExactFinalTwoMoves(BitBoard b_board, BitBoard w_board, BitBoard blank, char depth, int alpha, int beta, int turn, int pass_cnt){
//
//	COUNT_NODE++;
//
//	int pos1 = CountBit((~blank) & (blank - 1));
//	BitBoard move1 = one << pos1;
//
//	BitBoard rev = GetRev[pos1](b_board, w_board);
//	/* 打てる？ */
//	if(rev)
//	{
//		return -EndSolverExactFinalOneMove(w_board^rev, b_board^(move1 | rev), blank ^ move1, NULL, -beta, -alpha);
//	}
//	else{
//		/* 第２候補手生成 */
//		BitBoard move2 = blank ^ move1;
//		int pos2 = CountBit(move2 - 1);
//		rev = GetRev[pos2](b_board, w_board);
//		/* 打てる？ */
//		if(rev)
//		{
//			return -EndSolverExactFinalOneMove(w_board^rev, b_board^(move2 | rev), move1, NULL, -beta, -alpha);
//		}
//		else{
//			if(pass_cnt == 1){
//				CNT_B = CountBit(b_board);
//				CNT_W = CountBit(w_board);
//				return CNT_B - CNT_W;
//			}
//			/* 先手が２箇所空きのどこも打てない */
//			return -EndSolverExactFinalTwoMoves(w_board, b_board, blank, depth, -beta, -alpha, turn, 1);
//		}
//	}
//
//}


int alpha_beta_solve_exact(BitBoard b_board, BitBoard w_board, char depth, int alpha, int beta, BitBoard blank, int pass_cnt)
{
	/* アボート処理 */
	if(Flag_Abort)
	{
		return ABORT;
	}

	COUNT_NODE++;

	//int blankNum = CountBit(blank);
	/* 空きマスの取得 */
	if(depth == 0)
	{
		/* 空きマスがないとき */
		CNT_B = CountBit(b_board);
		return (CNT_B - 32) << 1;

	}
	/* 最終二手展開 */
	//if(depth == 2){
	//	BitBoard blank = ~(b_board | w_board);
	//	return EndSolverExactFinalTwoMoves(b_board, w_board, blank, depth, alpha, beta, turn, pass_cnt);
	//}
	/* 最終一手展開 */
	if(depth == 1){
		return alpha_beta_final_move(b_board, w_board, blank, NULL, alpha, beta);
	}


	int max;		     //現在の最高評価値
	int eval;                   //評価値の保存
	BitBoard rev;
	BitBoard moves = blank;
	int pos;
	BitBoard pos_bit;
	max = -65;
	//BitBoard blank = ~(b_board | w_board);
	do{
		/*
		　 ここに来るのは６マス以下の空きなので、CreateMovesを呼ぶより
		  反転データ取得と合法手を同時にチェックしたほうが圧倒的に速い)
		  */
		pos = CountBit((~moves) & (moves - 1));
		pos_bit = one << pos;
		rev = GetRev[pos](b_board, w_board);

		/* 打てる？ */
		if(rev)
		{
			/* ターンを進めて再帰処理へ */
			eval = -alpha_beta_solve_exact(w_board^rev, b_board^(pos_bit ^ rev), depth - 1, -beta, -alpha, blank ^ pos_bit, 0);
			if(beta <= eval){
				return eval;
			}
			/* 今までより良い局面が見つかれば最善手の更新 */
			if(eval > max)
			{
				max = eval;
				if (alpha < eval) {
					/* 下限を更新 */
					alpha = eval;
				}
			}

		}
		/* 1が立っている桁に移動 01100100 なら 000011001 になる*/
		moves ^= pos_bit;
	}while(moves);

	/* パスの処理(空きマス全部に打てなければmax = -65 のまま) */
	if(max == -65){
		if(b_board == 0)
		{
			return -64;
		}
		if(w_board == 0)
		{
			return 64;
		}
		if(pass_cnt == 1)
		{

			CNT_B = CountBit(b_board);
			CNT_W = CountBit(w_board);
			return CNT_B - CNT_W;
		}
		else{
			max = -alpha_beta_solve_exact(w_board, b_board, depth, -beta, -alpha, blank, 1);
		}
	}

	return max;

}

int pv_search_solve_exact(BitBoard bk, BitBoard wh, char depth, int alpha, int beta, int color, Hash *hash, int pass_cnt)
{
	/* アボート処理 */
	if(Flag_Abort)
	{
		return ABORT;
	}

	COUNT_NODE++;

	if(depth <= 6 && INF_DEPTH > 12)
	{
		// 葉に近い探索
		return alpha_beta_solve_exact(bk, wh, depth, alpha, beta, ~(bk | wh), pass_cnt);
	}

	if(COUNT_NODE - COUNT_NODE2 > BASE_NODE_NUM){
		end_t = timeGetTime();
		double timer = (end_t - start_t) / 1000;
		if(COUNT_NODE > 1000000000){
			sprintf_s(AI_msg, "nodes:%.2f[Gn], NPS:%.1f[Kn]", COUNT_NODE / (double)1000000000, (COUNT_NODE / timer) / (double)1000);
			BASE_NODE_NUM = 100000;
		}
		else if(COUNT_NODE > 1000000){
			sprintf_s(AI_msg, "nodes:%.1f[Mn], NPS:%.1f[Kn]", COUNT_NODE / (double)1000000, (COUNT_NODE / timer) / (double)1000);
			BASE_NODE_NUM = 100000;
		}
		else if(COUNT_NODE > 1000){
			sprintf_s(AI_msg, "nodes:%.1f[Kn], NPS:%.1f[Kn]", COUNT_NODE / (double)1000, (COUNT_NODE / timer) / (double)1000);
			BASE_NODE_NUM = 100;
		}

		else{
			sprintf_s(AI_msg, "nodes:%lld, NPS:%d[n]", COUNT_NODE, (int)((COUNT_NODE / (end_t - start_t)) / 1000));
			BASE_NODE_NUM = 1;
		}

		SendMessage(hStatus, SB_SETTEXT, (WPARAM)0 | 0, (LPARAM)AI_msg);
		COUNT_NODE2 = COUNT_NODE;
	}

	BOOL entry_flag;
	int ret;
	int lower, upper;
	HashInfo hash_info;

	/* キーを生成 */
	int key = KEY_HASH_MACRO(bk, wh);
	/* 置換表を検索 */
	if((ret = HashGet(hash, key, bk, wh, &hash_info)) == TRUE)
	{
		if(hash_info.depth - depth >= 0){
			lower = hash_info.lower;
			if(lower >= beta)
			{
				return lower;
			}
			upper = hash_info.upper;
			if(upper <= alpha || upper == lower)
			{
				return upper;
			}
			alpha = max(alpha, lower);
			beta = min(beta, upper);
		}
		else{
			//hash_info.depth = depth;
			lower = NEGAMIN;
			upper = NEGAMAX;
		}
		entry_flag = TRUE;
	}
	else
	{
		//hash_info.depth = depth;
		entry_flag = FALSE;
		lower = NEGAMIN;
		upper = NEGAMAX;
	}

	// 終盤用MPC探索(今後実装)
	/*if (depth >= MPC_DEPTH_MIN && depth <= 24) {
	int t_num = TurnNum + LIMIT_DEPTH - depth;
	if(t_num >= 36){
	MPC_CUT_VAL = 1.4;
	}
	else{
	MPC_CUT_VAL = 1.0;
	}
	MPCINFO *mpc_info = &MPCInfo[depth - MPC_DEPTH_MIN];
	int value = alpha - (mpc_info->deviation * MPC_CUT_VAL) - mpc_info->offset;
	int eval = CPU_AI_SHARROW_sharrow(bk, wh, mpc_info->depth, value - 1, value, t_num, pass_cnt);
	if (eval < value) {
	return alpha;
	}
	value = beta + (mpc_info->deviation * MPC_CUT_VAL) - mpc_info->offset;
	eval = CPU_AI_SHARROW_sharrow(bk, wh, mpc_info->depth, value, value + 1, t_num, pass_cnt);
	if (eval > value) {
	return beta;
	}
	}*/

	int max, max_move;
	int eval;
	int move_cnt;
	BitBoard moves, rev;
	max = -65;
	int p;
	int a_window = alpha;
	BitBoard rev_list[35];
	char pos_list[35];
	bool pv_flag = false;

	if(entry_flag == TRUE)
	{
		hash_info.locked = TRUE;
		/* 置換表から前の探索における最善手を取得 */
		p = hash_info.bestmove;
		rev = GetRev[p](bk, wh);
		/* PVS探索 */
		max_move = p;
		eval = -pv_search_solve_exact(wh^rev, bk^((one << p) | rev), depth - 1, -beta, -a_window, color ^ 1, hash, 0);
		if(eval >= beta)
		{
			return beta;   // fail-soft beta-cutoff
		}

		a_window = max(a_window, eval);
		max = eval;
		if (eval > alpha) {
			pv_flag = true;
		}

		// 以降，前の探索の最善手が最善ではない可能性がある場合に通る
		// 置換表からの取得局面なので moves が0になることはありえない
		moves = CreateMoves(bk, wh, (int *)(&move_cnt));

		// 置換表の最善手を除去
		moves ^= (one << p);
		move_cnt--;

		if(move_cnt != 0)
		{
			// 着手の適当な順序付け
			if(move_cnt > 1){
				if(LIMIT_DEPTH - depth >= 12){
					move_ordering_end(pos_list, bk, wh, hash, 
						moves, rev_list, INF_DEPTH - depth);
				}
				else{
					move_ordering_middle(pos_list, bk, wh, hash, 
						moves, rev_list, INF_DEPTH - depth, alpha, beta, color);
				}
			}
			else{
				pos_list[0] = CountBit(moves - 1);
				rev_list[0] = GetRev[pos_list[0]](bk, wh);
			}

			for(int i = 0; i < move_cnt; i++){

				p = pos_list[i];
				rev = rev_list[i];

				if(pv_flag == true){
					// PV値を取得できているのでnull-window探索
					eval = -pv_search_solve_exact(wh ^ rev, bk ^ ((one << p) | rev), depth - 1, 
						-(a_window + 1), -a_window, color ^ 1, hash, 0);
					if (eval > a_window && eval < beta){ // in fail-soft
						// re-search
						eval = -pv_search_solve_exact(wh ^ rev, bk ^ ((one << p) | rev), depth - 1, 
							-beta, -eval, color ^ 1, hash, 0);
					}
				}
				else {
					// PV値を取得できていないので通常幅での探索
					eval = -pv_search_solve_exact(wh^rev, bk^((one << p) | rev), depth - 1, 
						-beta, -a_window, color ^ 1, hash, 0);
				}

				if(eval >= beta)
				{
					return beta;   // fail-soft beta-cutoff
				}
				if(eval > max)
				{
					a_window = max(a_window, eval);
					max = eval;
					max_move = p;
					if (eval > alpha) {
						pv_flag = true;
					}
				}
			}
		}
		// 置換表更新
		hash_update(&hash_info, max_move, depth, max, alpha, beta, lower, upper);
		HashSet(hash, key, &hash_info);
	}
	else{
		/* 合法手生成とパスの処理 */
		moves = CreateMoves(bk, wh, (int *)(&move_cnt));
		if(move_cnt == 0){
			if(pass_cnt == 1)
			{
				if(bk == 0)
				{
					return -64;
				}
				if(wh == 0)
				{
					return 64;
				}

				CNT_B = CountBit(bk);
				CNT_W = CountBit(wh);

				return CNT_B - CNT_W;
			}

			max = -pv_search_solve_exact(wh, bk, depth, -beta, -alpha, color ^ 1, hash, 1);
			return max;
		}

		/* 着手の適当な順序付け */
		if(move_cnt != 1){
			if(LIMIT_DEPTH - depth >= 12){
				move_ordering_end(pos_list, bk, wh, hash, 
					moves, rev_list, INF_DEPTH - depth);
			}
			else{
				move_ordering_middle(pos_list, bk, wh, hash, 
					moves, rev_list, INF_DEPTH - depth, alpha, beta, color);
			}
		}
		else{
			pos_list[0] = CountBit(moves - 1);
			rev_list[0] = GetRev[pos_list[0]](bk, wh);
		}

		// オーダリングの先頭の手を最善として探索
		p = pos_list[0];
		rev = rev_list[0];
		max_move = p;
		eval = -pv_search_solve_exact(wh^rev, bk^((one << p) | rev), depth - 1, 
			-beta, -a_window, color ^ 1, hash, 0);
		if(eval >= beta)
		{
			return beta;   // fail-soft beta-cutoff
		}
		a_window = max(a_window, eval);
		max = eval;
		if (eval > alpha) {
			pv_flag = true;
		}

		for(int i = 1; i < move_cnt; i++){

			p = pos_list[i];
			rev = rev_list[i];

			if(pv_flag == true){
				// PV値を取得できているのでnull-window探索
				eval = -pv_search_solve_exact(wh ^ rev, bk ^ ((one << p) | rev), depth - 1, 
					-(a_window + 1), -a_window, color ^ 1, hash, 0);
				if (eval > a_window && eval < beta){ // in fail-soft
					// re-search
					eval = -pv_search_solve_exact(wh ^ rev, bk ^ ((one << p) | rev), depth - 1, 
						-beta, -eval, color ^ 1, hash, 0);
				}
			}
			else {
				// PV値を取得できていないので通常幅での探索
				eval = -pv_search_solve_exact(wh^rev, bk^((one << p) | rev), depth - 1, 
					-beta, -a_window, color ^ 1, hash, 0);
			}

			if(eval >= beta)
			{
				return beta;   // fail-soft beta-cutoff
			}
			if(eval > max)
			{
				a_window = max(a_window, eval);
				max = eval;
				max_move = p;
				if (eval > alpha) {
					pv_flag = true;
				}
			}
		}
		/* 置換表に登録 */
		if(hash->data[key].locked == FALSE && ret != LOCKED)
		{
			hash_create(&hash_info, bk, wh, max_move, move_cnt, 
				depth, max, alpha, beta, lower, upper);
			HashSet(hash, key, &hash_info);
		}
	}

	return max;
}


//int _fastcall EndSolverExactShallow(BitBoard b_board, BitBoard w_board, char depth, int alpha, int beta, BitBoard blank, int pass_cnt){
//
//
//	/* アボート処理 */
//	if(Flag_Abort)
//	{
//		return ABORT;
//	}
//
//	COUNT_NODE++;
//
//	//int blankNum = CountBit(blank);
//	/* 空きマスの取得 */
//	if(depth == 0)
//	{
//		/* 空きマスがないとき */
//		CNT_B = CountBit(b_board);
//		return (CNT_B - 32) << 1;
//
//	}
//	/* 最終二手展開 */
//	//if(depth == 2){
//	//	BitBoard blank = ~(b_board | w_board);
//	//	return EndSolverExactFinalTwoMoves(b_board, w_board, blank, depth, alpha, beta, turn, pass_cnt);
//	//}
//	/* 最終一手展開 */
//	if(depth == 1){
//		return EndSolverExactFinalOneMove(b_board, w_board, blank, NULL, alpha, beta);
//	}
//
//
//	int max;		     //現在の最高評価値
//	int eval;                   //評価値の保存
//	BitBoard rev;
//	BitBoard moves = blank;
//	int pos;
//	BitBoard pos_bit;
//	max = -65;
//	//BitBoard blank = ~(b_board | w_board);
//	do{
//		/*
//		　 ここに来るのは６マス以下の空きなので、CreateMovesを呼ぶより
//		  反転データ取得と合法手を同時にチェックしたほうが圧倒的に速い)
//		  */
//
//		pos = CountBit((~moves) & (moves - 1));
//		pos_bit = one << pos;
//		rev = GetRev[pos](b_board, w_board);
//
//		/* 打てる？ */
//		if(rev)
//		{
//			/* ターンを進めて再帰処理へ */
//			eval = -EndSolverExactShallow(w_board^rev, b_board^(pos_bit ^ rev), depth - 1, -beta, -alpha, blank ^ pos_bit, 0);
//			if(beta <= eval){
//				return eval;
//			}
//			/* 今までより良い局面が見つかれば最善手の更新 */
//			if(eval > max)
//			{
//				max = eval;
//				if (alpha < eval) {
//					/* 下限を更新 */
//					alpha = eval;
//				}
//			}
//
//		}
//		/* 1が立っている桁に移動 01100100 なら 000011001 になる*/
//		moves ^= pos_bit;
//	}while(moves);
//
//	/* パスの処理(空きマス全部に打てなければmax = -65 のまま) */
//	if(max == -65){
//		if(b_board == 0)
//		{
//			return -64;
//		}
//		if(w_board == 0)
//		{
//			return 64;
//		}
//		if(pass_cnt == 1)
//		{
//
//			CNT_B = CountBit(b_board);
//			CNT_W = CountBit(w_board);
//			return CNT_B - CNT_W;
//		}
//		else{
//			max = -EndSolverExactShallow(w_board, b_board, depth, -beta, -alpha, blank, 1);
//		}
//	}
//
//	return max;
//
//}
//
//int EndSolverExactDeep(BitBoard b_board, BitBoard w_board, char depth, 
//	int alpha, int beta, int color, Hash *hash, int pass_cnt)
//{
//	/* アボート処理 */
//	if(Flag_Abort)
//	{
//		return ABORT;
//	}
//
//	COUNT_NODE++;
//
//	if(COUNT_NODE - COUNT_NODE2 > BASE_NODE_NUM){
//		end_t = timeGetTime();
//		double timer = (end_t - start_t) / 1000;
//		if(COUNT_NODE > 1000000000){
//			sprintf_s(AI_msg, "nodes:%.2f[Gn], NPS:%.1f[Kn]", COUNT_NODE / (double)1000000000, (COUNT_NODE / timer) / (double)1000);
//			BASE_NODE_NUM = 1000000;
//		}
//		else if(COUNT_NODE > 1000000){
//			sprintf_s(AI_msg, "nodes:%.1f[Mn], NPS:%.1f[Kn]", COUNT_NODE / (double)1000000, (COUNT_NODE / timer) / (double)1000);
//			BASE_NODE_NUM = 1000000;
//		}
//		else if(COUNT_NODE > 1000){
//			sprintf_s(AI_msg, "nodes:%.1f[Kn], NPS:%.1f[Kn]", COUNT_NODE / (double)1000, (COUNT_NODE / timer) / (double)1000);
//			BASE_NODE_NUM = 1000;
//		}
//
//		else{
//			sprintf_s(AI_msg, "nodes:%lld, NPS:%d[n]", COUNT_NODE, (int)((COUNT_NODE / (end_t - start_t)) / 1000));
//			BASE_NODE_NUM = 1;
//		}
//
//		SendMessage(hStatus, SB_SETTEXT, (WPARAM)0 | 0, (LPARAM)AI_msg);
//		COUNT_NODE2 = COUNT_NODE;
//	}
//
//	if(depth <= 6 && INF_DEPTH > 12)
//	{
//		BitBoard blank = ~(b_board | w_board);
//		return EndSolverExactShallow(b_board, w_board, depth, alpha, beta, blank, pass_cnt);
//	}
//
//	BOOL entry_flag;
//	int ret;
//	int lower, upper;
//	HashInfo hash_info;
//
//	/* キーを生成 */
//	int key = KEY_HASH_MACRO(b_board, w_board, 0);
//	/* 置換表を検索 */
//	if((ret = HashGet(hash, key, b_board, w_board, &hash_info)) == TRUE)
//	{
//		if(hash_info.depth - depth >= 0){
//			lower = hash_info.lower;
//			if(lower >= beta)
//			{
//				return lower;
//			}
//			upper = hash_info.upper;
//			if(upper <= alpha || upper == lower)
//			{
//				return upper;
//			}
//			alpha = max(alpha, lower);
//			beta = min(beta, upper);
//		}
//		else{
//			lower = -65;
//			upper = 65;
//		}
//		entry_flag = TRUE;
//	}
//	else
//	{
//		entry_flag = FALSE;
//		lower = -65;
//		upper = 65;
//	}
//
//	/* STABLITY CUT OFF （今後実装） */
//	/*if(depth < 20){
//	if ( alpha >= stability_threshold[depth] ) {
//	int stability_bound;
//
//	stability_bound = 64 - 2 * count_edge_stable( NowTurn ^ 1, w_board, b_board );
//	if ( stability_bound <= alpha )
//	return alpha;
//	stability_bound = 64 - 2 * count_stable(  NowTurn ^ 1, w_board, b_board );
//	if ( stability_bound < beta )
//	beta = stability_bound + 1;
//	if ( stability_bound <= alpha )
//	return alpha;
//	}
//	}*/
//
//	int max, max_move;
//	int eval;
//	BitBoard moves, rev;
//	max = -65;
//	int exchangePoint = 0;
//	int p;
//	int a_window = alpha;
//	BitBoard rev_list[35];
//	char *p_moves;
//	char pos_list[35];
//
//	if(entry_flag == TRUE)
//	{
//		hash_info.locked = TRUE;
//		p_moves = &hash_info.bestmove;
//		/* 最善手をorderingの結果の先頭に仮定 */
//		p = p_moves[0];
//		rev = GetRev[p](b_board, w_board);
//		/* PVS探索 */
//		max = -EndSolverExactDeep(w_board^rev, b_board^((one << p) | rev), 
//			depth - 1, -beta, -a_window, color ^ 1, hash, 0);
//
//		if(max >= beta){
//			max_move = p;
//			if (a_window < max) {
//				/* 下限を更新 */
//				a_window = max;
//			}
//		}
//		else {
//			/* とりあえず最大の評価値扱い */
//			max_move = p;
//			if (a_window < max) {
//				/* 下限を更新 */
//				a_window = max;
//			}
//
//			int i = 1;
//			int move_cnt = hash_info.move_cnt;
//
//			if(depth == INF_DEPTH)
//			{
//				sprintf_s(AI_msg, "@%+d %.0f%%", max, (1 / (double)move_cnt) * 100);
//				SendMessage(hStatus, SB_SETTEXT, (WPARAM)0 | 2, (LPARAM)AI_msg);
//			}
//
//			while(i < move_cnt){
//
//				p = p_moves[i];
//				rev = GetRev[p](b_board, w_board);
//
//				/* null window search */
//				eval = -EndSolverExactDeep(w_board^rev, b_board^((one << p) | rev), 
//					depth - 1, -(a_window + 1), -a_window, color ^ 1, hash, 0);
//
//				/* null window search failed */
//				if(a_window < eval)
//				{
//					/* 下限値の再設定 */
//					eval = -EndSolverExactDeep(w_board^rev, b_board^((one << p) | rev), 
//						depth - 1, -beta, -eval, color ^ 1, hash, 0);
//				}
//				/* この局面は選択されない */
//				if(beta <= eval)
//				{
//					max = eval;
//					max_move = p;
//					/* βカット */
//					exchangePoint = i;
//					break;
//				}
//				/* 今までより良い局面が見つかれば最善手の更新 */
//				if(eval > max)
//				{
//					max = eval;
//					max_move = p;
//					exchangePoint = i;
//					if (a_window < eval) {
//						/* 下限を更新 */
//						a_window = eval;
//					}
//				}
//				if(depth == INF_DEPTH)
//				{
//					sprintf_s(AI_msg, "@%+d %.0f%%", max, (i / (double)move_cnt) * 100);
//					SendMessage(hStatus, SB_SETTEXT, (WPARAM)0 | 2, (LPARAM)AI_msg);
//				}
//				i++;
//			}
//		}
//
//		hash_update(&hash_info, max_move, depth, max, a_window, beta, lower, upper);
//		HashSet(hash, key, &hash_info);
//	}
//	else{
//
//		int move_cnt;
//		/* 合法手生成とパスの処理 */
//		moves = CreateMoves(b_board, w_board, &move_cnt);
//		if(move_cnt == 0){
//			if(b_board == 0)
//			{
//				return -64;
//			}
//			if(w_board == 0)
//			{
//				return 64;
//			}
//			if(pass_cnt == 1)
//			{
//				CNT_B = CountBit(b_board);
//				CNT_W = CountBit(w_board);
//				return CNT_B - CNT_W;
//			}
//			max = -EndSolverExactDeep(w_board, b_board, depth, -beta, -alpha, color ^ 1, hash, 1);
//			return max;
//		}
//
//		/* 未出現の局面はmove ordering 関数を呼ぶ */
//		/* 着手の適当な順序付け */
//		if(move_cnt != 1){
//			if(LIMIT_DEPTH - depth >= 12){
//				move_cnt = move_ordering_end(pos_list, b_board, w_board, hash, 
//					moves, rev_list, INF_DEPTH - depth);
//			}
//			else{
//				move_cnt = move_ordering_middle(pos_list, b_board, w_board, hash, 
//					moves, rev_list, INF_DEPTH - depth, alpha, beta, color);
//			}
//		}
//		else{
//			pos_list[0] = CountBit(moves - 1);
//			rev_list[0] = GetRev[pos_list[0]](b_board, w_board);
//		}
//		/* 最善手をorderingの結果の先頭に仮定 */
//		p = pos_list[0];
//		rev = rev_list[0];
//		/* PVS探索 */
//		max = -EndSolverExactDeep(w_board^rev, b_board^((one << p) | rev), 
//			depth - 1, -beta, -alpha, color ^ 1, hash, 0);
//
//		if(max >= beta){
//			max_move = p;
//			if (a_window < max) {
//				/* 下限を更新 */
//				a_window = max;
//			}
//		}
//		else {
//			/* とりあえず最大の評価値扱い */
//			max_move = p;
//			if (a_window < max) {
//				/* 下限を更新 */
//				a_window = max;
//			}
//
//			int i = 1;
//			while(i < move_cnt){
//
//				p = pos_list[i];
//				rev = rev_list[i];
//
//				/* null window search */
//				eval = -EndSolverExactDeep(w_board^rev, b_board^((one << p) | rev), 
//					depth - 1, -(a_window + 1), -a_window, color ^ 1, hash, 0);
//
//				/* null window search failed */
//				if(a_window < eval)
//				{
//					/* 下限値の再設定 */
//					eval = -EndSolverExactDeep(w_board^rev, b_board^((one << p) | rev), 
//						depth - 1, -beta, -eval, color ^ 1, hash, 0);
//				}
//
//				/* この局面は選択されない */
//				if(beta <= eval)
//				{
//					max = beta;
//					max_move = p;
//					/* βカット */
//					exchangePoint = i;
//					break;
//				}
//				/* 今までより良い局面が見つかれば最善手の更新 */
//				if(eval > max)
//				{
//					max = eval;
//					max_move = p;
//					exchangePoint = i;
//
//					if (a_window < eval) {
//						/* 下限を更新 */
//						a_window = eval;
//					}
//
//				}
//				i++;
//			}
//		}
//
//		/* 置換表に登録 */
//		if(hash->data[key].locked == FALSE && ret != LOCKED)
//		{
//			hash_create(&hash_info, b_board, w_board, max_move, move_cnt, 
//				depth, max, a_window, beta, lower, upper);
//		}
//		HashSet(hash, key, &hash_info);
//
//	}
//
//	return max;
//
//}

/* CPU:AIの本体(終盤ソルバー最終石差探索) #FFO40 2秒程度 nps:16000k～20000k */
//ネガスカウト + move ordering + 置換表をノードの高さによって組み合わせる
int EndSolverExact(BitBoard b_board, BitBoard w_board, char depth, 
	int alpha, int beta, int color, Hash *hash, int pass_cnt)
{

	/* アボート処理 */
	if(Flag_Abort)
	{
		return ABORT;
	}

	int max;		     //現在の最高評価値

	//COUNT_NODE = 0;
	BASE_NODE_NUM = 0;
	/* 適当に下限値で初期化 */
	/* 6より上のノードではネガスカウト+ move ordering + 置換表 */
	/* それより末端ノードは単純アルファベータ */
	max = pv_search_solve_exact(b_board, w_board, depth, alpha, beta, color, hash, pass_cnt);

	COUNT_NODE2 = 0;

	return max;
}

int alpha_beta_winloss_final_move(BitBoard b_board, BitBoard w_board, BitBoard blank, int alpha, int beta){

	BitBoard rev = GetRev[CountBit(blank - 1)](b_board, w_board);
	/* 打てる？ */
	if(rev)
	{
		CNT_B = CountBit(b_board ^ (blank | rev)) - 32;
		if(CNT_B > 0){
			return WIN;
		}
		else if(CNT_B < 0)
		{
			return LOSS;
		}
		else
		{
			return DRAW;
		}
	}
	else{
		rev = GetRev[CountBit(blank - 1)](w_board, b_board);
		/* 打てる？ */
		if(rev)
		{
			CNT_B = CountBit(b_board ^ rev) - 32;
			if(CNT_B > 0)
			{
				return WIN;
			}
			else if(CNT_B < 0)
			{
				return LOSS;
			}
			else
			{
				return DRAW;
			}
		}
		else{
			CNT_B = CountBit(b_board) - CountBit(w_board);
			if(CNT_B > 0){
				return WIN;
			}
			else
			{
				return LOSS;
			}

		}
	}
}

int alpha_beta_solve_winloss(BitBoard b_board, BitBoard w_board, char depth, int alpha, int beta, BitBoard blank, int pass_cnt)
{
	/* アボート処理 */
	if(Flag_Abort)
	{
		return ABORT;
	}

	COUNT_NODE++;

	/* 空きマスの取得 */
	if(depth == 0)
	{
		/* 空きマスがないとき */
		CNT_B = CountBit(b_board);
		if(CNT_B > 32)
		{
			return WIN;
		}
		else if(CNT_B < 32)
		{
			return LOSS;
		}
		else
		{
			return DRAW;
		}

	}
	/* 最終一手展開 */
	//if(depth == 1){

	//	return EndSolverWinLossFinalOneMove(b_board, w_board, blank, alpha, beta);

	//}

	int max;		     //現在の最高評価値
	int eval;                   //評価値の保存
	BitBoard rev;
	int pos;
	BitBoard pos_bit;
	max = -2;
	BitBoard moves = blank;

	do{
		/*
		　 ここに来るのは６マス以下の空きなので、CreateMovesを呼ぶより
		  反転データ取得と合法手を同時にチェックしたほうが圧倒的に速い)
		  */
		//pos = get_order_position(blank);
		pos = CountBit((~moves) & (moves - 1));
		pos_bit = one << pos;
		rev = GetRev[pos](b_board, w_board);
		/* 打てる？ */
		if(rev)
		{
			/* ターンを進めて再帰処理へ */
			eval = -alpha_beta_solve_winloss(w_board^rev, b_board^(pos_bit ^ rev), 
				depth - 1, -beta, -alpha, blank ^ pos_bit, 0);
			if(eval >= beta){
				return eval;
			}
			/* 今までより良い局面が見つかれば最善手の更新 */
			if(eval > max)
			{
				max = eval;
				if (alpha < eval) {
					/* 下限を更新 */
					alpha = eval;
				}
			}
		}
		/* 1が立っている桁に移動 01100100 なら 000011001 になる*/
		moves ^= pos_bit;
	}while(moves);

	/* パスの処理(空きマス全部に打てなければmax = -2 のまま) */
	if(max == -2){
		if(pass_cnt == 1)
		{
			if(b_board == 0)
			{
				return LOSS;
			}
			if(w_board == 0)
			{
				return WIN;
			}
			CNT_B = CountBit(b_board);
			CNT_W = CountBit(w_board);
			if(CNT_B > CNT_W)
			{
				return WIN;
			}
			else if(CNT_B < CNT_W)
			{
				return LOSS;
			}
			else
			{
				return DRAW;
			}
		}
		max = -alpha_beta_solve_winloss(w_board, b_board, depth, -beta, -alpha, blank, 1);
	}

	return max;

}

int pv_search_solve_winloss(BitBoard bk, BitBoard wh, char depth, 
	int alpha, int beta, int color, Hash *hash, int pass_cnt)
{
	/* アボート処理 */
	if(Flag_Abort)
	{
		return ABORT;
	}

	COUNT_NODE++;

	if(depth <= 6 && INF_DEPTH > 12)
	{
		// 葉に近い探索
		return alpha_beta_solve_winloss(bk, wh, depth, alpha, beta, ~(bk | wh), pass_cnt);
	}

	if(COUNT_NODE - COUNT_NODE2 > BASE_NODE_NUM){
		end_t = timeGetTime();
		double timer = (end_t - start_t) / 1000;
		if(COUNT_NODE > 1000000000){
			sprintf_s(AI_msg, "nodes:%.2f[Gn], NPS:%.1f[Kn]", COUNT_NODE / (double)1000000000, (COUNT_NODE / timer) / (double)1000);
			BASE_NODE_NUM = 100000;
		}
		else if(COUNT_NODE > 1000000){
			sprintf_s(AI_msg, "nodes:%.1f[Mn], NPS:%.1f[Kn]", COUNT_NODE / (double)1000000, (COUNT_NODE / timer) / (double)1000);
			BASE_NODE_NUM = 100000;
		}
		else if(COUNT_NODE > 1000){
			sprintf_s(AI_msg, "nodes:%.1f[Kn], NPS:%.1f[Kn]", COUNT_NODE / (double)1000, (COUNT_NODE / timer) / (double)1000);
			BASE_NODE_NUM = 100;
		}

		else{
			sprintf_s(AI_msg, "nodes:%lld, NPS:%d[n]", COUNT_NODE, (int)((COUNT_NODE / (end_t - start_t)) / 1000));
			BASE_NODE_NUM = 1;
		}

		SendMessage(hStatus, SB_SETTEXT, (WPARAM)0 | 0, (LPARAM)AI_msg);
		COUNT_NODE2 = COUNT_NODE;
	}

	BOOL entry_flag;
	int ret;
	int lower, upper;
	HashInfo hash_info;

	/* キーを生成 */
	int key = KEY_HASH_MACRO(bk, wh);
	/* 置換表を検索 */
	if((ret = HashGet(hash, key, bk, wh, &hash_info)) == TRUE)
	{
		if(hash_info.depth - depth >= 0){
			lower = hash_info.lower;
			if(lower >= beta)
			{
				return lower;
			}
			upper = hash_info.upper;
			if(upper <= alpha || upper == lower)
			{
				return upper;
			}
			alpha = max(alpha, lower);
			beta = min(beta, upper);
		}
		else{
			//hash_info.depth = depth;
			lower = LOSS - 1;
			upper = WIN + 1;
		}
		entry_flag = TRUE;
	}
	else
	{
		entry_flag = FALSE;
		lower = LOSS - 1;
		upper = WIN + 1;
	}

	// 終盤用MPC探索(今後実装)
	/*if (depth >= MPC_DEPTH_MIN && depth <= 24) {
	int t_num = TurnNum + LIMIT_DEPTH - depth;
	if(t_num >= 36){
	MPC_CUT_VAL = 1.4;
	}
	else{
	MPC_CUT_VAL = 1.0;
	}
	MPCINFO *mpc_info = &MPCInfo[depth - MPC_DEPTH_MIN];
	int value = alpha - (mpc_info->deviation * MPC_CUT_VAL) - mpc_info->offset;
	int eval = CPU_AI_SHARROW_sharrow(bk, wh, mpc_info->depth, value - 1, value, t_num, pass_cnt);
	if (eval < value) {
	return alpha;
	}
	value = beta + (mpc_info->deviation * MPC_CUT_VAL) - mpc_info->offset;
	eval = CPU_AI_SHARROW_sharrow(bk, wh, mpc_info->depth, value, value + 1, t_num, pass_cnt);
	if (eval > value) {
	return beta;
	}
	}*/

	int max, max_move;
	int eval;
	int move_cnt;
	BitBoard moves, rev;
	max = -2;
	int p;
	int a_window = alpha;
	BitBoard rev_list[35];
	char pos_list[35];
	bool pv_flag = false;

	if(entry_flag == TRUE)
	{
		hash_info.locked = TRUE;
		/* 置換表から前の探索における最善手を取得 */
		p = hash_info.bestmove;
		rev = GetRev[p](bk, wh);
		/* PVS探索 */
		max_move = p;
		eval = -pv_search_solve_winloss(wh^rev, bk^((one << p) | rev), depth - 1, -beta, -a_window, color ^ 1, hash, 0);
		if(eval >= beta)
		{
			return beta;   // fail-soft beta-cutoff
		}

		a_window = max(a_window, eval);
		max = eval;
		if (eval > alpha) {
			pv_flag = true;
		}

		// 以降，前の探索の最善手が最善ではない可能性がある場合に通る
		moves = CreateMoves(bk, wh, (int *)(&move_cnt));
		// 置換表の最善手を除去
		moves ^= (one << p);
		move_cnt--;

		if(move_cnt != 0)
		{
			// 着手の適当な順序付け
			if(move_cnt > 1){
				if(LIMIT_DEPTH - depth >= 12){
					move_ordering_end(pos_list, bk, wh, hash, 
						moves, rev_list, INF_DEPTH - depth);
				}
				else{
					move_ordering_middle(pos_list, bk, wh, hash, 
						moves, rev_list, INF_DEPTH - depth, alpha, beta, color);
				}
			}
			else{
				pos_list[0] = CountBit(moves - 1);
				rev_list[0] = GetRev[pos_list[0]](bk, wh);
			}

			for(int i = 0; i < move_cnt; i++){

				p = pos_list[i];
				rev = rev_list[i];

				if(pv_flag == true){
					// PV値を取得できているのでnull-window探索
					eval = -pv_search_solve_winloss(wh ^ rev, bk ^ ((one << p) | rev), depth - 1, 
						-(a_window + 1), -a_window, color ^ 1, hash, 0);
					if (eval > a_window && eval < beta){ // in fail-soft
						// re-search
						eval = -pv_search_solve_winloss(wh ^ rev, bk ^ ((one << p) | rev), depth - 1, 
							-beta, -eval, color ^ 1, hash, 0);
					}
				}
				else {
					// PV値を取得できていないので通常幅での探索
					eval = -pv_search_solve_winloss(wh^rev, bk^((one << p) | rev), depth - 1, 
						-beta, -a_window, color ^ 1, hash, 0);
				}

				if(eval >= beta)
				{
					return beta;   // fail-soft beta-cutoff
				}
				if(eval > max)
				{
					a_window = max(a_window, eval);
					max = eval;
					max_move = p;
					if (eval > alpha) {
						pv_flag = true;
					}
				}
			}
		}
		// 置換表更新
		hash_update(&hash_info, max_move, depth, max, alpha, beta, lower, upper);
		HashSet(hash, key, &hash_info);
	}
	else{
		/* 合法手生成とパスの処理 */
		moves = CreateMoves(bk, wh, (int *)(&move_cnt));
		if(move_cnt == 0){
			if(pass_cnt == 1)
			{
				CNT_B = CountBit(bk);
				CNT_W = CountBit(wh);
				if(CNT_B > CNT_W)
				{
					return WIN;
				}
				if(CNT_B < CNT_W)
				{
					return LOSS;
				}
				else
				{
					return DRAW;
				}
			}

			max = -pv_search_solve_winloss(wh, bk, depth, -beta, -alpha, color ^ 1, hash, 1);
			return max;
		}

		/* 着手の適当な順序付け */
		if(move_cnt != 1){
			if(LIMIT_DEPTH - depth >= 12){
				move_ordering_end(pos_list, bk, wh, hash, 
					moves, rev_list, INF_DEPTH - depth);
			}
			else{
				move_ordering_middle(pos_list, bk, wh, hash, 
					moves, rev_list, INF_DEPTH - depth, alpha, beta, color);
			}
		}
		else{
			pos_list[0] = CountBit(moves - 1);
			rev_list[0] = GetRev[pos_list[0]](bk, wh);
		}

		// オーダリングの先頭の手を最善として探索
		p = pos_list[0];
		rev = rev_list[0];
		max_move = p;
		eval = -pv_search_solve_winloss(wh^rev, bk^((one << p) | rev), depth - 1, 
			-beta, -a_window, color ^ 1, hash, 0);
		if(eval >= beta)
		{
			return beta;   // fail-soft beta-cutoff
		}
		a_window = max(a_window, eval);
		max = eval;
		if (eval > alpha) {
			pv_flag = true;
		}

		for(int i = 1; i < move_cnt; i++){

			p = pos_list[i];
			rev = rev_list[i];

			if(pv_flag == true){
				// PV値を取得できているのでnull-window探索
				eval = -pv_search_solve_winloss(wh ^ rev, bk ^ ((one << p) | rev), depth - 1, 
					-(a_window + 1), -a_window, color ^ 1, hash, 0);
				if (eval > a_window && eval < beta){ // in fail-soft
					// re-search
					eval = -pv_search_solve_winloss(wh ^ rev, bk ^ ((one << p) | rev), depth - 1, 
						-beta, -eval, color ^ 1, hash, 0);
				}
			}
			else {
				// PV値を取得できていないので通常幅での探索
				eval = -pv_search_solve_winloss(wh^rev, bk^((one << p) | rev), depth - 1, 
					-beta, -a_window, color ^ 1, hash, 0);
			}

			if(eval >= beta)
			{
				return beta;   // fail-soft beta-cutoff
			}
			if(eval > max)
			{
				a_window = max(a_window, eval);
				max = eval;
				max_move = p;
				if (eval > alpha) {
					pv_flag = true;
				}
			}
		}

		/* 置換表に登録 */
		if(hash->data[key].locked == FALSE && ret != LOCKED)
		{
			hash_create(&hash_info, bk, wh, max_move, move_cnt, 
				depth, max, alpha, beta, lower, upper);
			HashSet(hash, key, &hash_info);
		}
	}

	return max;
}

//
//int pv_search_solve_winloss(BitBoard b_board, BitBoard w_board, char depth, 
//	int alpha, int beta, int color, Hash *hash, int pass_cnt){
//
//
//		/* アボート処理 */
//		if(Flag_Abort)
//		{
//			return ABORT;
//		}
//
//		COUNT_NODE++;
//
//		if(depth <= 6 && INF_DEPTH > 12)
//		{
//			BitBoard blank = ~(b_board | w_board);
//			return alpha_beta_solve_winloss(b_board, w_board, depth, alpha, beta, blank, pass_cnt);
//		}
//
//		if(COUNT_NODE - COUNT_NODE2 > BASE_NODE_NUM){
//			end_t = timeGetTime();
//			double timer = (end_t - start_t) / 1000;
//			if(COUNT_NODE > 1000000000){
//				sprintf_s(AI_msg, "nodes:%.2f[Gn], NPS:%.1f[Kn]", COUNT_NODE / (double)1000000000, (COUNT_NODE / timer) / (double)1000);
//				BASE_NODE_NUM = 1000000;
//			}
//			else if(COUNT_NODE > 1000000){
//				sprintf_s(AI_msg, "nodes:%.1f[Mn], NPS:%.1f[Kn]", COUNT_NODE / (double)1000000, (COUNT_NODE / timer) / (double)1000);
//				BASE_NODE_NUM = 1000000;
//			}
//			else if(COUNT_NODE > 1000){
//				sprintf_s(AI_msg, "nodes:%.1f[Kn], NPS:%.1f[Kn]", COUNT_NODE / (double)1000, (COUNT_NODE / timer) / (double)1000);
//				BASE_NODE_NUM = 1000;
//			}
//
//			else{
//				sprintf_s(AI_msg, "nodes:%lld, NPS:%d[n]", COUNT_NODE, (int)((COUNT_NODE / (end_t - start_t)) / 1000));
//				BASE_NODE_NUM = 1;
//			}
//
//			SendMessage(hStatus, SB_SETTEXT, (WPARAM)0 | 0, (LPARAM)AI_msg);
//			COUNT_NODE2 = COUNT_NODE;
//		}
//
//		BOOL entry_flag;
//		int ret;
//		int lower, upper;
//		HashInfo hash_info;
//
//		/* キーを生成 */
//		int key = KEY_HASH_MACRO(b_board, w_board, 0);
//		/* 置換表を検索 */
//		if((ret = HashGet(hash, key, b_board, w_board, &hash_info)) == TRUE)
//		{
//			if(hash_info.depth - depth >= 0){
//				lower = hash_info.lower;
//				if(lower >= beta)
//				{
//					return lower;
//				}
//				upper = hash_info.upper;
//				if(upper <= alpha || upper == lower)
//				{
//					return upper;
//				}
//				alpha = max(alpha, lower);
//				beta = min(beta, upper);
//			}
//			else{
//				lower = LOSS;
//				upper = WIN;
//			}
//			entry_flag = TRUE;
//		}
//		else
//		{
//			entry_flag = FALSE;
//			lower = LOSS;
//			upper = WIN;
//		}
//
//		//if (MPC_FLAG == TRUE && depth >= MPC_DEPTH_MIN && depth <= 24) {
//
//		//	MPCINFO *mpc_info = &MPCInfo[depth - MPC_DEPTH_MIN];
//		//	int value = alpha * EVAL_ONE_STONE - (mpc_info->deviation * 3) - mpc_info->offset;
//		//	int eval = CPU_AI_SHARROW_sharrow(b_board, w_board, mpc_info->depth, value - 1, value, turn, pass_cnt);
//		//	if (eval < value) {
//		//		return alpha;
//		//	}
//		//	value = beta * EVAL_ONE_STONE + (mpc_info->deviation * 3) - mpc_info->offset;
//		//	eval = CPU_AI_SHARROW_sharrow(b_board, w_board, mpc_info->depth, value, value + 1, turn, pass_cnt);
//		//	if (eval > value) {
//		//		return beta;
//		//	}
//		//}
//
//
//		int max, max_move;
//		int eval;
//		BitBoard moves, rev;
//		max = -2;
//		int exchangePoint = 0;
//		int p;
//		int a_window = alpha;
//		BitBoard rev_list[35];
//		char *p_moves;
//		char pos_list[35];
//
//		if(entry_flag == TRUE)
//		{
//			hash_info.locked = TRUE;
//			p_moves = &hash_info.bestmove;
//			/* 最善手をorderingの結果の先頭に仮定 */
//			p = p_moves[0];
//			rev = GetRev[p](b_board, w_board);
//			/* PVS探索 */
//			max = -pv_search_solve_winloss(w_board^rev, b_board^((one << p) | rev), 
//				depth - 1, -beta, -max(a_window, max), color ^ 1, hash, 0);
//
//			if(max >= beta){
//				max_move = p;
//				if (a_window < max) {
//					/* 下限を更新 */
//					a_window = max;
//				}
//			}
//			else {
//				/* とりあえず最大の評価値扱い */
//				max_move = p;
//				if (a_window < max) {
//					/* 下限を更新 */
//					a_window = max;
//				}
//
//				int i = 1;
//				int move_cnt = hash_info.move_cnt;
//
//				if(depth == INF_DEPTH)
//				{
//					sprintf_s(AI_msg, "@%s %.0f%%", wld_msg[max + 1], (1 / (double)move_cnt) * 100);
//					SendMessage(hStatus, SB_SETTEXT, (WPARAM)0 | 2, (LPARAM)AI_msg);
//				}
//
//				while(i < move_cnt){
//
//					p = p_moves[i];
//					rev = GetRev[p](b_board, w_board);
//
//					/* null window search */
//					eval = -EndSolverWinLossDeep(w_board^rev, b_board^((one << p) | rev), 
//						depth - 1, -(a_window + 1), -a_window, color ^ 1, hash, 0);
//
//					/* null window search failed */
//					if(a_window < eval)
//					{
//						/* 下限値の再設定 */
//						//a_window = eval;
//						eval = -EndSolverWinLossDeep(w_board^rev, b_board^((one << p) | rev), 
//							depth - 1, -beta, -eval, color ^ 1, hash, 0);
//
//					}
//					/* この局面は選択されない */
//					if(eval >= beta)
//					{
//						max = eval;
//						max_move = p;
//						/* βカット */
//						exchangePoint = i;
//						break;
//					}
//
//					/* 今までより良い局面が見つかれば最善手の更新 */
//					if(eval > max)
//					{
//						max = eval;
//						max_move = p;
//						exchangePoint = i;
//						if (a_window < eval) {
//							/* 下限を更新 */
//							a_window = eval;
//						}
//					}
//					if(depth == INF_DEPTH)
//					{
//						sprintf_s(AI_msg, "@%s %.0f%%", wld_msg[eval + 1], (i / (double)move_cnt) * 100);
//						SendMessage(hStatus, SB_SETTEXT, (WPARAM)0 | 2, (LPARAM)AI_msg);
//					}
//					i++;
//				}
//			}
//
//			hash_update(&hash_info, max_move, depth, max, a_window, beta, lower, upper);
//			HashSet(hash, key, &hash_info);
//		}
//		else{
//
//			int move_cnt;
//			/* 合法手生成とパスの処理 */
//			moves = CreateMoves(b_board, w_board, (int *)(&move_cnt));
//			if(move_cnt == 0){
//				if(pass_cnt == 1)
//				{
//					if(b_board == 0)
//					{
//						return LOSS;
//					}
//					if(w_board == 0)
//					{
//						return WIN;
//					}
//
//					int diff = CountBit(b_board) - CountBit(w_board);
//					if(diff > 0) return WIN;
//					if(diff < 0) return LOSS;
//					return diff;
//				}
//				max = -EndSolverWinLossDeep(w_board, b_board, depth, -beta, -alpha, color ^ 1, hash, 1);
//				return max;
//			}
//
//			/* 未出現の局面はmove ordering 関数を呼ぶ */
//			/* 着手の適当な順序付け */
//			if(move_cnt != 1){
//				if(LIMIT_DEPTH - depth >= 12){
//					move_cnt = move_ordering_end(pos_list, b_board, w_board, hash, 
//						moves, rev_list, INF_DEPTH - depth);
//				}
//				else{
//					move_cnt = move_ordering_middle(pos_list, b_board, w_board, hash, 
//						moves, rev_list, INF_DEPTH - depth, alpha, beta, color);
//				}
//			}
//			else{
//				pos_list[0] = CountBit(moves - 1);
//				rev_list[0] = GetRev[pos_list[0]](b_board, w_board);
//			}
//			/* 最善手をorderingの結果の先頭に仮定 */
//			p = pos_list[0];
//			rev = rev_list[0];
//			/* PVS探索 */
//			max = -EndSolverWinLossDeep(w_board^rev, b_board^((one << p) | rev), 
//				depth - 1, -beta, -max(a_window, max), color ^ 1, hash, 0);
//
//			if(max >= beta){
//				max_move = p;
//				if (a_window < max) {
//					/* 下限を更新 */
//					a_window = max;
//				}
//			}
//			else {
//				/* とりあえず最大の評価値扱い */
//				max_move = p;
//				if (a_window < max) {
//					/* 下限を更新 */
//					a_window = max;
//				}
//
//				int i = 1;
//				while(i < move_cnt){
//
//					p = pos_list[i];
//					rev = rev_list[i];
//
//					/* null window search */
//					eval = -EndSolverWinLossDeep(w_board^rev, b_board^((one << p) | rev), 
//						depth - 1, -(a_window + 1), -a_window, color ^ 1, hash, 0);
//
//					/* null window search failed */
//					if(a_window < eval)
//					{
//						/* 下限値の再設定 */
//						//a_window = eval;
//						eval = -EndSolverWinLossDeep(w_board^rev, b_board^((one << p) | rev), 
//							depth - 1, -beta, -eval, color ^ 1, hash, 0);
//
//					}
//					/* この局面は選択されない */
//					if(eval >= beta)
//					{
//						max = eval;
//						max_move = p;
//						/* βカット */
//						exchangePoint = i;
//						break;
//					}
//
//					/* 今までより良い局面が見つかれば最善手の更新 */
//					if(eval > max)
//					{
//						max = eval;
//						max_move = p;
//						exchangePoint = i;
//						if (a_window < eval) {
//							/* 下限を更新 */
//							a_window = eval;
//						}
//					}
//					i++;
//				}
//			}
//
//			/* 置換表に登録 */
//			if(hash->data[key].locked == FALSE && ret != LOCKED)
//			{
//				hash_create(&hash_info, b_board, w_board, max_move, move_cnt, 
//					depth, max, a_window, beta, lower, upper);
//			}
//			HashSet(hash, key, &hash_info);
//
//		}
//
//		return max;
//
//}

/* 終盤勝敗読み切り用 */
int EndSolverWinLoss(BitBoard b_board, BitBoard w_board, char depth, 
	int alpha, int beta, int color, Hash *hash, int pass_cnt)
{

	/* アボート処理 */
	if(Flag_Abort)
	{
		return ABORT;
	}

	int max;		     //現在の最高評価値

	//COUNT_NODE = 0;
	BASE_NODE_NUM = 0;
	/* 適当に下限値で初期化 */
	/* 6より上のノードではネガスカウト+ move ordering + 置換表 */
	/* それより末端ノードは単純アルファベータ */
	max = pv_search_solve_winloss(b_board, w_board, depth, alpha, beta, color, hash, pass_cnt);

	return max;
}