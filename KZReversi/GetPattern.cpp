/*#
  # 局面のパターンを管理、操作を行います
  #  
  ##############################################
*/

#include "stdafx.h"
#include "KZReversi.h"
#include "GetPattern.h"
#include "eval.h"


/* 評価パターンテーブル(現在のステージにより内容が変わるポインタ) */
float *hori_ver1;
float *hori_ver2;
float *hori_ver3;
float *dia_ver1;
float *dia_ver2;
float *dia_ver3;
float *dia_ver4;
float *edge;
float *corner5_2;
float *corner3_3;
float *triangle;
float *mobility;
float *parity;

/* 評価パターンテーブル(おおもと) */
float hori_ver1_data[2][60][INDEX_NUM];
float hori_ver2_data[2][60][INDEX_NUM];
float hori_ver3_data[2][60][INDEX_NUM];
float dia_ver1_data[2][60][INDEX_NUM];
float dia_ver2_data[2][60][INDEX_NUM / 3];
float dia_ver3_data[2][60][INDEX_NUM / 9];
float dia_ver4_data[2][60][INDEX_NUM / 27];
float edge_data[2][60][INDEX_NUM * 9];
float corner5_2_data[2][60][INDEX_NUM * 9];
float corner3_3_data[2][60][INDEX_NUM * 3];
float triangle_data[2][60][INDEX_NUM * 9];
float mobility_data[2][60][MOBILITY_NUM];
float parity_data[2][60][PARITY_NUM];
float constant_data[2][60];

int NowStage;

int COUNT_PATTERN;
int COUNT_NO_PATTERN;

int pow_table[10] = { 1, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683 };

double key_hori_ver1[4];
double key_hori_ver2[4];
double key_hori_ver3[4];
double key_dia_ver1[2];
double key_dia_ver2[4];
double key_dia_ver3[4];
double key_dia_ver4[4];
double key_edge[4];
double key_corner5_2[8];
double key_corner3_3[4];
double key_triangle[4];
double key_parity;
double key_mobility;
double key_constant;
double eval_sum;

BitBoard p_pattern[72];

BitBoard a1 =  one;					/* a1 */
BitBoard a2 = (one << 1);			/* a2 */
BitBoard a3 = (one << 2);			/* a3 */
BitBoard a4 = (one << 3);			/* a4 */
BitBoard a5 = (one << 4);			/* a5 */
BitBoard a6 = (one << 5);			/* a6 */
BitBoard a7 = (one << 6);			/* a7 */
BitBoard a8 = (one << 7);			/* a8 */

BitBoard b1 = (one << 8);			/* b1 */
BitBoard b2 = (one << 9);			/* b2 */
BitBoard b3 = (one << 10);			/* b3 */
BitBoard b4 = (one << 11);			/* b4 */
BitBoard b5 = (one << 12);			/* b5 */
BitBoard b6 = (one << 13);			/* b6 */
BitBoard b7 = (one << 14);			/* b7 */
BitBoard b8 = (one << 15);			/* b8 */

BitBoard c1 = (one << 16);			/* c1 */
BitBoard c2 = (one << 17);			/* c2 */
BitBoard c3 = (one << 18);			/* c3 */
BitBoard c4 = (one << 19);			/* c4 */
BitBoard c5 = (one << 20);			/* c5 */
BitBoard c6 = (one << 21);			/* c6 */
BitBoard c7 = (one << 22);			/* c7 */
BitBoard c8 = (one << 23);			/* c8 */

BitBoard d1 = (one << 24);			/* d1 */
BitBoard d2 = (one << 25);			/* d2 */
BitBoard d3 = (one << 26);			/* d3 */
BitBoard d4 = (one << 27);			/* d4 */
BitBoard d5 = (one << 28);			/* d5 */
BitBoard d6 = (one << 29);			/* d6 */
BitBoard d7 = (one << 30);			/* d7 */
BitBoard d8 = (one << 31);			/* d8 */

BitBoard e1 = (one << 32);			/* e1 */
BitBoard e2 = (one << 33);			/* e2 */
BitBoard e3 = (one << 34);			/* e3 */
BitBoard e4 = (one << 35);			/* e4 */
BitBoard e5 = (one << 36);			/* e5 */
BitBoard e6 = (one << 37);			/* e6 */
BitBoard e7 = (one << 38);			/* e7 */
BitBoard e8 = (one << 39);			/* e8 */

BitBoard f1 = (one << 40);			/* f1 */
BitBoard f2 = (one << 41);			/* f2 */
BitBoard f3 = (one << 42);			/* f3 */
BitBoard f4 = (one << 43);			/* f4 */
BitBoard f5 = (one << 44);			/* f5 */
BitBoard f6 = (one << 45);			/* f6 */
BitBoard f7 = (one << 46);			/* f7 */
BitBoard f8 = (one << 47);			/* f8 */

BitBoard g1 = (one << 48);			/* g1 */
BitBoard g2 = (one << 49);			/* g2 */
BitBoard g3 = (one << 50);			/* g3 */
BitBoard g4 = (one << 51);			/* g4 */
BitBoard g5 = (one << 52);			/* g5 */
BitBoard g6 = (one << 53);			/* g6 */
BitBoard g7 = (one << 54);			/* g7 */
BitBoard g8 = (one << 55);			/* g8 */

BitBoard h1 = (one << 56);			/* h1 */
BitBoard h2 = (one << 57);			/* h2 */
BitBoard h3 = (one << 58);			/* h3 */
BitBoard h4 = (one << 59);			/* h4 */
BitBoard h5 = (one << 60);			/* h5 */
BitBoard h6 = (one << 61);			/* h6 */
BitBoard h7 = (one << 62);			/* h7 */
BitBoard h8 = (one << 63);			/* h8 */

extern HWND DlgHandle;
int board[64];

char eval_table_list[14][32] = 
{
	"prologue", "opening1", "opening2", 
	"opening3", "middle1", "middle2",
	"middle3", "middle4", "middle5",
	"end1", "end2", "end3", "end4",
	"end5"
};

int pos_eval[64] = 
{
	8,1,5,6,6,5,1,8,
	1,0,2,3,3,2,0,1,
	5,2,7,4,4,7,2,5,
	6,3,4,0,0,4,3,6,
	6,3,4,0,0,4,3,6,
	5,2,7,4,4,7,2,5,
	1,0,2,3,3,2,0,1,
	8,1,5,6,6,5,1,8
};

/* A conversion table from the 2^8 edge values for one player to
   the corresponding base-3 value. */
static short base_conversion[256];

/* For each of the 3^8 edges, edge_stable[] holds an 8-bit mask
   where a bit is set if the corresponding disc can't be changed EVER. */
static short edge_stable[6561];

static union convert2indexBoard{
	BitBoard bitBoard;
	struct bitField{
		UCHAR bit63 : 1;
		UCHAR bit62 : 1;
		UCHAR bit61 : 1;
		UCHAR bit60 : 1;
		UCHAR bit59 : 1;
		UCHAR bit58 : 1;
		UCHAR bit57 : 1;
		UCHAR bit56 : 1;
		UCHAR bit55 : 1;
		UCHAR bit54 : 1;
		UCHAR bit53 : 1;
		UCHAR bit52 : 1;
		UCHAR bit51 : 1;
		UCHAR bit50 : 1;
		UCHAR bit49 : 1;
		UCHAR bit48 : 1;
		UCHAR bit47 : 1;
		UCHAR bit46 : 1;
		UCHAR bit45 : 1;
		UCHAR bit44 : 1;
		UCHAR bit43 : 1;
		UCHAR bit42 : 1;
		UCHAR bit41 : 1;
		UCHAR bit40 : 1;
		UCHAR bit39 : 1;
		UCHAR bit38 : 1;
		UCHAR bit37 : 1;
		UCHAR bit36 : 1;
		UCHAR bit35 : 1;
		UCHAR bit34 : 1;
		UCHAR bit33 : 1;
		UCHAR bit32 : 1;
		UCHAR bit31 : 1;
		UCHAR bit30 : 1;
		UCHAR bit29 : 1;
		UCHAR bit28 : 1;
		UCHAR bit27 : 1;
		UCHAR bit26 : 1;
		UCHAR bit25 : 1;
		UCHAR bit24 : 1;
		UCHAR bit23 : 1;
		UCHAR bit22 : 1;
		UCHAR bit21 : 1;
		UCHAR bit20 : 1;
		UCHAR bit19 : 1;
		UCHAR bit18 : 1;
		UCHAR bit17 : 1;
		UCHAR bit16 : 1;
		UCHAR bit15 : 1;
		UCHAR bit14 : 1;
		UCHAR bit13 : 1;
		UCHAR bit12 : 1;
		UCHAR bit11 : 1;
		UCHAR bit10 : 1;
		UCHAR bit9 : 1;
		UCHAR bit8 : 1;
		UCHAR bit7 : 1;
		UCHAR bit6 : 1;
		UCHAR bit5 : 1;
		UCHAR bit4 : 1;
		UCHAR bit3 : 1;
		UCHAR bit2 : 1;
		UCHAR bit1 : 1;
		UCHAR bit0 : 1;
	}bit;
}st_black, st_white;
	

void init_index_board(BitBoard bk, BitBoard wh)
{
	board[0] =  (int )(bk & a1)          + (int )((wh & a1) * 2);
	board[1] =  (int )((bk & a2) >> 1)  + (int )((wh & a2));
	board[2] =  (int )((bk & a3) >> 2)  + (int )((wh & a3) >> 1);
	board[3] =  (int )((bk & a4) >> 3)  + (int )((wh & a4) >> 2); 
	board[4] =  (int )((bk & a5) >> 4)  + (int )((wh & a5) >> 3);
	board[5] =  (int )((bk & a6) >> 5)  + (int )((wh & a6) >> 4);
	board[6] =  (int )((bk & a7) >> 6)  + (int )((wh & a7) >> 5);
	board[7] =  (int )((bk & a8) >> 7)  + (int )((wh & a8) >> 6);
	board[8] =  (int )((bk & b1) >> 8)  + (int )((wh & b1) >> 7);
	board[9] =  (int )((bk & b2) >> 9)  + (int )((wh & b2) >> 8);
	board[10] = (int )((bk & b3) >> 10) + (int )((wh & b3) >> 9);
	board[11] = (int )((bk & b4) >> 11) + (int )((wh & b4) >> 10);
	board[12] = (int )((bk & b5) >> 12) + (int )((wh & b5) >> 11);
	board[13] = (int )((bk & b6) >> 13) + (int )((wh & b6) >> 12);
	board[14] = (int )((bk & b7) >> 14) + (int )((wh & b7) >> 13);
	board[15] = (int )((bk & b8) >> 15) + (int )((wh & b8) >> 14);
	board[16] = (int )((bk & c1) >> 16) + (int )((wh & c1) >> 15);
	board[17] = (int )((bk & c2) >> 17) + (int )((wh & c2) >> 16);
	board[18] = (int )((bk & c3) >> 18) + (int )((wh & c3) >> 17);
	board[19] = (int )((bk & c4) >> 19) + (int )((wh & c4) >> 18);
	board[20] = (int )((bk & c5) >> 20) + (int )((wh & c5) >> 19);
	board[21] = (int )((bk & c6) >> 21) + (int )((wh & c6) >> 20);
	board[22] = (int )((bk & c7) >> 22) + (int )((wh & c7) >> 21);
	board[23] = (int )((bk & c8) >> 23) + (int )((wh & c8) >> 22);
	board[24] = (int )((bk & d1) >> 24) + (int )((wh & d1) >> 23);
	board[25] = (int )((bk & d2) >> 25) + (int )((wh & d2) >> 24);
	board[26] = (int )((bk & d3) >> 26) + (int )((wh & d3) >> 25);
	board[27] = (int )((bk & d4) >> 27) + (int )((wh & d4) >> 26); 
	board[28] = (int )((bk & d5) >> 28) + (int )((wh & d5) >> 27);
	board[29] = (int )((bk & d6) >> 29) + (int )((wh & d6) >> 28);
	board[30] = (int )((bk & d7) >> 30) + (int )((wh & d7) >> 29);
	board[31] = (int )((bk & d8) >> 31) + (int )((wh & d8) >> 30);
	board[32] = (int )((bk & e1) >> 32) + (int )((wh & e1) >> 31);
	board[33] = (int )((bk & e2) >> 33) + (int )((wh & e2) >> 32);
	board[34] = (int )((bk & e3) >> 34) + (int )((wh & e3) >> 33);
	board[35] = (int )((bk & e4) >> 35) + (int )((wh & e4) >> 34);
	board[36] = (int )((bk & e5) >> 36) + (int )((wh & e5) >> 35);
	board[37] = (int )((bk & e6) >> 37) + (int )((wh & e6) >> 36);
	board[38] = (int )((bk & e7) >> 38) + (int )((wh & e7) >> 37);
	board[39] = (int )((bk & e8) >> 39) + (int )((wh & e8) >> 38);
	board[40] = (int )((bk & f1) >> 40) + (int )((wh & f1) >> 39);
	board[41] = (int )((bk & f2) >> 41) + (int )((wh & f2) >> 40);
	board[42] = (int )((bk & f3) >> 42) + (int )((wh & f3) >> 41);
	board[43] = (int )((bk & f4) >> 43) + (int )((wh & f4) >> 42);
	board[44] = (int )((bk & f5) >> 44) + (int )((wh & f5) >> 43);
	board[45] = (int )((bk & f6) >> 45) + (int )((wh & f6) >> 44);
	board[46] = (int )((bk & f7) >> 46) + (int )((wh & f7) >> 45);
	board[47] = (int )((bk & f8) >> 47) + (int )((wh & f8) >> 46);
	board[48] = (int )((bk & g1) >> 48) + (int )((wh & g1) >> 47);
	board[49] = (int )((bk & g2) >> 49) + (int )((wh & g2) >> 48);
	board[50] = (int )((bk & g3) >> 50) + (int )((wh & g3) >> 49);
	board[51] = (int )((bk & g4) >> 51) + (int )((wh & g4) >> 50);
	board[52] = (int )((bk & g5) >> 52) + (int )((wh & g5) >> 51);
	board[53] = (int )((bk & g6) >> 53) + (int )((wh & g6) >> 52);
	board[54] = (int )((bk & g7) >> 54) + (int )((wh & g7) >> 53);
	board[55] = (int )((bk & g8) >> 55) + (int )((wh & g8) >> 54);
	board[56] = (int )((bk & h1) >> 56) + (int )((wh & h1) >> 55);
	board[57] = (int )((bk & h2) >> 57) + (int )((wh & h2) >> 56);
	board[58] = (int )((bk & h3) >> 58) + (int )((wh & h3) >> 57);
	board[59] = (int )((bk & h4) >> 59) + (int )((wh & h4) >> 58);
	board[60] = (int )((bk & h5) >> 60) + (int )((wh & h5) >> 59);
	board[61] = (int )((bk & h6) >> 61) + (int )((wh & h6) >> 60);
	board[62] = (int )((bk & h7) >> 62) + (int )((wh & h7) >> 61);
	board[63] = (int )((bk & h8) >> 63) + (int )((wh & h8) >> 62);
}

/**
 * @brief Get some potential moves.
 *
 * @param P bitboard with player's discs.
 * @param dir flipping direction.
 * @return some potential moves in a 64-bit unsigned integer.
 */
unsigned long long get_some_potential_moves(const unsigned long long P, const int dir)
{
	return (P << dir | P >> dir);
}

/**
 * @brief Get potential moves.
 *
 * Get the list of empty squares in contact of a player square.
 *
 * @param P bitboard with player's discs.
 * @param O bitboard with opponent's discs.
 * @return all potential moves in a 64-bit unsigned integer.
 */
unsigned long long get_potential_moves(BitBoard P, BitBoard O, BitBoard blank)
{
	return (get_some_potential_moves(O & 0x7E7E7E7E7E7E7E7Eull, 1) // horizontal
		| get_some_potential_moves(O & 0x00FFFFFFFFFFFF00ull, 8)   // vertical
		| get_some_potential_moves(O & 0x007E7E7E7E7E7E00ull, 7)   // diagonals
		| get_some_potential_moves(O & 0x007E7E7E7E7E7E00ull, 9))
		& blank; // mask with empties
}

double check_h_ver1()
{
	int key;
	double eval;

	/* a2 b2 c2 d2 e2 f2 g2 h2 */
	/* a7 b7 c7 d7 e7 f7 g7 h7 */
	/* b1 b2 b3 b4 b5 b6 b7 b8 */
	/* g1 g2 g3 g4 g5 g6 g7 g8 */

	key = board[A2];
	key += 3 * board[B2];
	key += 9 * board[C2];
	key += 27 * board[D2];
	key += 81 * board[E2];
	key += 243 * board[F2];
	key += 729 * board[G2];
	key += 2187 * board[H2];

	eval = hori_ver1[key];
	key_hori_ver1[0] = hori_ver1[key];

	//if(hori_ver1[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[A7];
	key += 3 * board[B7];
	key += 9 * board[C7];
	key += 27 * board[D7];
	key += 81 * board[E7];
	key += 243 * board[F7];
	key += 729 * board[G7];
	key += 2187 * board[H7];

	eval += hori_ver1[key];
	key_hori_ver1[1] = hori_ver1[key];

	//if(hori_ver1[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[B1];
	key += 3 * board[B2];
	key += 9 * board[B3];
	key += 27 * board[B4];
	key += 81 * board[B5];
	key += 243 * board[B6];
	key += 729 * board[B7];
	key += 2187 * board[B8];

	eval += hori_ver1[key];
	key_hori_ver1[2] = hori_ver1[key];

	//if(hori_ver1[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[G1];
	key += 3 * board[G2];
	key += 9 * board[G3];
	key += 27 * board[G4];
	key += 81 * board[G5];
	key += 243 * board[G6];
	key += 729 * board[G7];
	key += 2187 * board[G8];

	eval += hori_ver1[key];
	key_hori_ver1[3] = hori_ver1[key];
	//if(hori_ver1[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	return eval;
}

double check_h_ver2()
{
	int key;
	double eval;

	/* a3 b3 c3 d3 e3 f3 g3 h3 */
	/* a6 b6 c6 d6 e6 f6 g6 h6 */
	/* c1 c2 c3 c4 c5 c6 c7 c8 */
	/* f1 f2 f3 f4 f5 f6 f7 f8 */

	key = board[A3];
	key += 3 * board[B3];
	key += 9 * board[C3];
	key += 27 * board[D3];
	key += 81 * board[E3];
	key += 243 * board[F3];
	key += 729 * board[G3];
	key += 2187 * board[H3];

	eval = hori_ver2[key];
	key_hori_ver2[0] = hori_ver2[key];
	//if(hori_ver2[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[A6];
	key += 3 * board[B6];
	key += 9 * board[C6];
	key += 27 * board[D6];
	key += 81 * board[E6];
	key += 243 * board[F6];
	key += 729 * board[G6];
	key += 2187 * board[H6];

	eval += hori_ver2[key];
	key_hori_ver2[1] = hori_ver2[key];
	//if(hori_ver2[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[C1];
	key += 3 * board[C2];
	key += 9 * board[C3];
	key += 27 * board[C4];
	key += 81 * board[C5];
	key += 243 * board[C6];
	key += 729 * board[C7];
	key += 2187 * board[C8];

	eval += hori_ver2[key];
	key_hori_ver2[2] = hori_ver2[key];
	//if(hori_ver2[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[F1];
	key += 3 * board[F2];
	key += 9 * board[F3];
	key += 27 * board[F4];
	key += 81 * board[F5];
	key += 243 * board[F6];
	key += 729 * board[F7];
	key += 2187 * board[F8];

	eval += hori_ver2[key];
	key_hori_ver2[3] = hori_ver2[key];
	//if(hori_ver2[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	return eval;

}

double check_h_ver3()
{
	int key;
	double eval;

	/* a4 b4 c4 d4 e4 f4 g4 h4 */
	/* a5 b5 c5 d5 e5 f5 g5 h5 */
	/* d1 d2 d3 d4 d5 d6 d7 d8 */
	/* e1 e2 e3 e4 e5 e6 e7 e8 */

	key = board[A4];
	key += 3 * board[B4];
	key += 9 * board[C4];
	key += 27 * board[D4];
	key += 81 * board[E4];
	key += 243 * board[F4];
	key += 729 * board[G4];
	key += 2187 * board[H4];

	eval = hori_ver3[key];
	key_hori_ver3[0] = hori_ver3[key];
	//if(hori_ver3[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[A5];
	key += 3 * board[B5];
	key += 9 * board[C5];
	key += 27 * board[D5];
	key += 81 * board[E5];
	key += 243 * board[F5];
	key += 729 * board[G5];
	key += 2187 * board[H5];

	eval += hori_ver3[key];
	key_hori_ver3[1] = hori_ver3[key];
	//if(hori_ver3[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[D1];
	key += 3 * board[D2];
	key += 9 * board[D3];
	key += 27 * board[D4];
	key += 81 * board[D5];
	key += 243 * board[D6];
	key += 729 * board[D7];
	key += 2187 * board[D8];

	eval += hori_ver3[key];
	key_hori_ver3[2] = hori_ver3[key];
	//if(hori_ver3[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[E1];
	key += 3 * board[E2];
	key += 9 * board[E3];
	key += 27 * board[E4];
	key += 81 * board[E5];
	key += 243 * board[E6];
	key += 729 * board[E7];
	key += 2187 * board[E8];

	eval += hori_ver3[key];
	key_hori_ver3[3] = hori_ver3[key];
	//if(hori_ver3[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	return eval;

}

double check_dia_ver1()
{
	int key;
	double eval;

	/* a1 b2 c3 d4 e5 f6 g7 h8 */
	/* h1 g2 f3 e4 d5 c6 b7 a8  */

	key = board[A1];
	key += 3 * board[B2];
	key += 9 * board[C3];
	key += 27 * board[D4];
	key += 81 * board[E5];
	key += 243 * board[F6];
	key += 729 * board[G7];
	key += 2187 * board[H8];

	eval = dia_ver1[key];
	key_dia_ver1[0] = dia_ver1[key];

	//if(dia_ver1[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[A8];
	key += 3 * board[B7];
	key += 9 * board[C6];
	key += 27 * board[D5];
	key += 81 * board[E4];
	key += 243 * board[F3];
	key += 729 * board[G2];
	key += 2187 * board[H1];

	eval += dia_ver1[key];
	key_dia_ver1[1] = dia_ver1[key];
	//if(dia_ver1[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	return eval;
}

double check_dia_ver2()
{
	int key;
	double eval;

	/* a2 b3 c4 d5 e6 f7 g8 */
	/* b1 c2 d3 e4 f5 g6 h7 */
	/* h2 g3 f4 e5 d6 c7 b8 */
	/* g1 f2 e3 d4 c5 b6 a7 */

	key = board[A2];
	key += 3 * board[B3];
	key += 9 * board[C4];
	key += 27 * board[D5];
	key += 81 * board[E6];
	key += 243 * board[F7];
	key += 729 * board[G8];

	eval = dia_ver2[key];
	key_dia_ver2[0] = dia_ver2[key];
	//if(dia_ver2[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}
	
	key = board[B1];
	key += 3 * board[C2];
	key += 9 * board[D3];
	key += 27 * board[E4];
	key += 81 * board[F5];
	key += 243 * board[G6];
	key += 729 * board[H7];

	eval += dia_ver2[key];
	key_dia_ver2[1] = dia_ver2[key];
	//if(dia_ver2[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[B8];
	key += 3 * board[C7];
	key += 9 * board[D6];
	key += 27 * board[E5];
	key += 81 * board[F4];
	key += 243 * board[G3];
	key += 729 * board[H2];

	eval += dia_ver2[key];
	key_dia_ver2[2] = dia_ver2[key];
	//if(dia_ver2[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[A7];
	key += 3 * board[B6];
	key += 9 * board[C5];
	key += 27 * board[D4];
	key += 81 * board[E3];
	key += 243 * board[F2];
	key += 729 * board[G1];

	eval += dia_ver2[key];
	key_dia_ver2[3] = dia_ver2[key];
	//if(dia_ver2[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	return eval;
}

double check_dia_ver3()
{
	int key;
	double eval;

	key = board[A3];
	key += 3 * board[B4];
	key += 9 * board[C5];
	key += 27 * board[D6];
	key += 81 * board[E7];
	key += 243 * board[F8];

	eval = dia_ver3[key];
	key_dia_ver3[0] = dia_ver3[key];
	//if(dia_ver3[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[C1];
	key += 3 * board[D2];
	key += 9 * board[E3];
	key += 27 * board[F4];
	key += 81 * board[G5];
	key += 243 * board[H6];

	eval += dia_ver3[key];
	key_dia_ver3[1] = dia_ver3[key];
	//if(dia_ver3[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[C8];
	key += 3 * board[D7];
	key += 9 * board[E6];
	key += 27 * board[F5];
	key += 81 * board[G4];
	key += 243 * board[H3];
	
	eval += dia_ver3[key];
	key_dia_ver3[2] = dia_ver3[key];
	//if(dia_ver3[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[A6];
	key += 3 * board[B5];
	key += 9 * board[C4];
	key += 27 * board[D3];
	key += 81 * board[E2];
	key += 243 * board[F1];
	
	eval += dia_ver3[key];
	key_dia_ver3[3] = dia_ver3[key];
	//if(dia_ver3[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	return eval;
}

double check_dia_ver4()
{
	int key;
	double eval;

	key = board[A4];
	key += 3 * board[B5];
	key += 9 * board[C6];
	key += 27 * board[D7];
	key += 81 * board[E8];

	eval = dia_ver4[key];
	key_dia_ver4[0] = dia_ver4[key];
	//if(dia_ver4[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[D1];
	key += 3 * board[E2];
	key += 9 * board[F3];
	key += 27 * board[G4];
	key += 81 * board[H5];

	eval += dia_ver4[key];
	key_dia_ver4[1] = dia_ver4[key];
	//if(dia_ver4[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[D8];
	key += 3 * board[E7];
	key += 9 * board[F6];
	key += 27 * board[G5];
	key += 81 * board[H4];

	eval += dia_ver4[key];
	key_dia_ver4[2] = dia_ver4[key];
	//if(dia_ver4[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[A5];
	key += 3 * board[B4];
	key += 9 * board[C3];
	key += 27 * board[D2];
	key += 81 * board[E1];

	eval += dia_ver4[key];
	key_dia_ver4[3] = dia_ver4[key];
	//if(dia_ver4[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	return eval;
}

double check_edge()
{
	int key;
	double eval;

	key = board[A1];
	key += 3 * board[A2];
	key += 9 * board[A3];
	key += 27 * board[A4];
	key += 81 * board[A5];
	key += 243 * board[A6];
	key += 729 * board[A7];
	key += 2187 * board[A8];
	key += 6561 * board[B2];
	key += 19683 * board[B7];

	eval = edge[key];
	key_edge[0] = edge[key];
	//if(edge[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[H1];
	key += 3 * board[H2];
	key += 9 * board[H3];
	key += 27 * board[H4];
	key += 81 * board[H5];
	key += 243 * board[H6];
	key += 729 * board[H7];
	key += 2187 * board[H8];
	key += 6561 * board[G2];
	key += 19683 * board[G7];

	eval += edge[key];
	key_edge[1] = edge[key];
	//if(edge[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[A1];
	key += 3 * board[B1];
	key += 9 * board[C1];
	key += 27 * board[D1];
	key += 81 * board[E1];
	key += 243 * board[F1];
	key += 729 * board[G1];
	key += 2187 * board[H1];
	key += 6561 * board[B2];
	key += 19683 * board[G2];

	eval += edge[key];
	key_edge[2] = edge[key];
	//if(edge[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[A8];
	key += 3 * board[B8];
	key += 9 * board[C8];
	key += 27 * board[D8];
	key += 81 * board[E8];
	key += 243 * board[F8];
	key += 729 * board[G8];
	key += 2187 * board[H8];
	key += 6561 * board[B7];
	key += 19683 * board[G7];

	eval += edge[key];
	key_edge[3] = edge[key];
	//if(edge[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	return eval;
}

//double check_edge_cor()
//{
//	int key;
//	double eval;
//
//	key = board[B2];
//	key += 3 * board[A1];
//	key += 9 * board[A2];
//	key += 27 * board[A3];
//	key += 81 * board[A4];
//	key += 243 * board[A5];
//	key += 729 * board[B1];
//	key += 2187 * board[C1];
//	key += 6561 * board[D1];
//	key += 19683 * board[E1];
//
//	eval = edge_cor[key];
//	key_edge_cor[0] = edge_cor[key];
//
//	key = board[G2];
//	key += 3 * board[H1];
//	key += 9 * board[G1];
//	key += 27 * board[F1];
//	key += 81 * board[E1];
//	key += 243 * board[D1];
//	key += 729 * board[H2];
//	key += 2187 * board[H3];
//	key += 6561 * board[H4];
//	key += 19683 * board[H5];
//
//	eval += edge_cor[key];
//	key_edge_cor[1] = edge_cor[key];
//
//	key = board[G7];
//	key += 3 * board[H8];
//	key += 9 * board[H7];
//	key += 27 * board[H6];
//	key += 81 * board[H5];
//	key += 243 * board[H4];
//	key += 729 * board[G8];
//	key += 2187 * board[F8];
//	key += 6561 * board[E8];
//	key += 19683 * board[D8];
//
//	eval += edge_cor[key];
//	key_edge_cor[2] = edge_cor[key];
//
//	key = board[B7];
//	key += 3 * board[A8];
//	key += 9 * board[B8];
//	key += 27 * board[C8];
//	key += 81 * board[D8];
//	key += 243 * board[E8];
//	key += 729 * board[A7];
//	key += 2187 * board[A6];
//	key += 6561 * board[A5];
//	key += 19683 * board[A4];
//
//	eval += edge_cor[key];
//	key_edge_cor[3] = edge_cor[key];
//
//	return eval;
//}

//double check_corner4_2()
//{
//	int key;
//	double eval;
//
//	key = board[A1];
//	key += 3 * board[A8];
//	key += 9 * board[A3];
//	key += 27 * board[A4];
//	key += 81 * board[A5];
//	key += 243 * board[A6];
//	key += 729 * board[B3];
//	key += 2187 * board[B4];
//	key += 6561 * board[B5];
//	key += 19683 * board[B6];
//
//	eval = corner4_2[key];
//	key_corner4_2[0] = corner4_2[key];
//
//	key = board[H1];
//	key += 3 * board[A1];
//	key += 9 * board[F1];
//	key += 27 * board[E1];
//	key += 81 * board[D1];
//	key += 243 * board[C1];
//	key += 729 * board[F2];
//	key += 2187 * board[E2];
//	key += 6561 * board[D2];
//	key += 19683 * board[C2];
//
//	eval += corner4_2[key];
//	key_corner4_2[1] = corner4_2[key];
//
//	key = board[H1];
//	key += 3 * board[H8];
//	key += 9 * board[H3];
//	key += 27 * board[H4];
//	key += 81 * board[H5];
//	key += 243 * board[H6];
//	key += 729 * board[G3];
//	key += 2187 * board[G4];
//	key += 6561 * board[G5];
//	key += 19683 * board[G6];
//
//	eval += corner4_2[key];
//	key_corner4_2[2] = corner4_2[key];
//
//	key = board[A8];
//	key += 3 * board[H8];
//	key += 9 * board[C8];
//	key += 27 * board[D8];
//	key += 81 * board[E8];
//	key += 243 * board[F8];
//	key += 729 * board[C7];
//	key += 2187 * board[D7];
//	key += 6561 * board[E7];
//	key += 19683 * board[F7];
//
//	eval += corner4_2[key];
//	key_corner4_2[3] = corner4_2[key];
//
//	return eval;
//
//}

double check_corner3_3()
{
	int key;
	double eval;

	key = board[A1];
	key += 3 * board[A2];
	key += 9 * board[A3];
	key += 27 * board[B1];
	key += 81 * board[B2];
	key += 243 * board[B3];
	key += 729 * board[C1];
	key += 2187 * board[C2];
	key += 6561 * board[C3];

	eval = corner3_3[key];
	key_corner3_3[0] = corner3_3[key];

	key = board[H1];
	key += 3 * board[H2];
	key += 9 * board[H3];
	key += 27 * board[G1];
	key += 81 * board[G2];
	key += 243 * board[G3];
	key += 729 * board[F1];
	key += 2187 * board[F2];
	key += 6561 * board[F3];

	eval += corner3_3[key];
	key_corner3_3[1] = corner3_3[key];

	key = board[A8];
	key += 3 * board[A7];
	key += 9 * board[A6];
	key += 27 * board[B8];
	key += 81 * board[B7];
	key += 243 * board[B6];
	key += 729 * board[C8];
	key += 2187 * board[C7];
	key += 6561 * board[C6];

	eval += corner3_3[key];
	key_corner3_3[2] = corner3_3[key];

	key = board[H8];
	key += 3 * board[H7];
	key += 9 * board[H6];
	key += 27 * board[G8];
	key += 81 * board[G7];
	key += 243 * board[G6];
	key += 729 * board[F8];
	key += 2187 * board[F7];
	key += 6561 * board[F6];

	eval += corner3_3[key];
	key_corner3_3[3] = corner3_3[key];

	return eval;
}


double check_corner5_2()
{
	int key;
	double eval;

	key = board[A1];
	key += 3 * board[B1];
	key += 9 * board[C1];
	key += 27 * board[D1];
	key += 81 * board[E1];
	key += 243 * board[A2];
	key += 729 * board[B2];
	key += 2187 * board[C2];
	key += 6561 * board[D2];
	key += 19683 * board[E2];

	eval = corner5_2[key];
	key_corner5_2[0] = corner5_2[key];
	//if(corner5_2[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[A8];
	key += 3 * board[B8];
	key += 9 * board[C8];
	key += 27 * board[D8];
	key += 81 * board[E8];
	key += 243 * board[A7];
	key += 729 * board[B7];
	key += 2187 * board[C7];
	key += 6561 * board[D7];
	key += 19683 * board[E7];

	eval += corner5_2[key];
	key_corner5_2[1] = corner5_2[key];
	//if(corner5_2[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[H1];
	key += 3 * board[G1];
	key += 9 * board[F1];
	key += 27 * board[E1];
	key += 81 * board[D1];
	key += 243 * board[H2];
	key += 729 * board[G2];
	key += 2187 * board[F2];
	key += 6561 * board[E2];
	key += 19683 * board[D2];

	eval += corner5_2[key];
	key_corner5_2[2] = corner5_2[key];
	//if(corner5_2[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[H8];
	key += 3 * board[G8];
	key += 9 * board[F8];
	key += 27 * board[E8];
	key += 81 * board[D8];
	key += 243 * board[H7];
	key += 729 * board[G7];
	key += 2187 * board[F7];
	key += 6561 * board[E7];
	key += 19683 * board[D7];

	eval += corner5_2[key];
	key_corner5_2[3] = corner5_2[key];
	//if(corner5_2[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[A1];
	key += 3 * board[A2];
	key += 9 * board[A3];
	key += 27 * board[A4];
	key += 81 * board[A5];
	key += 243 * board[B1];
	key += 729 * board[B2];
	key += 2187 * board[B3];
	key += 6561 * board[B4];
	key += 19683 * board[B5];

	eval += corner5_2[key];
	key_corner5_2[4] = corner5_2[key];
	//if(corner5_2[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[H1];
	key += 3 * board[H2];
	key += 9 * board[H3];
	key += 27 * board[H4];
	key += 81 * board[H5];
	key += 243 * board[G1];
	key += 729 * board[G2];
	key += 2187 * board[G3];
	key += 6561 * board[G4];
	key += 19683 * board[G5];

	eval += corner5_2[key];
	key_corner5_2[5] = corner5_2[key];
	//if(corner5_2[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[A8];
	key += 3 * board[A7];
	key += 9 * board[A6];
	key += 27 * board[A5];
	key += 81 * board[A4];
	key += 243 * board[B8];
	key += 729 * board[B7];
	key += 2187 * board[B6];
	key += 6561 * board[B5];
	key += 19683 * board[B4];

	eval += corner5_2[key];
	key_corner5_2[6] = corner5_2[key];
	//if(corner5_2[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[H8];
	key += 3 * board[H7];
	key += 9 * board[H6];
	key += 27 * board[H5];
	key += 81 * board[H4];
	key += 243 * board[G8];
	key += 729 * board[G7];
	key += 2187 * board[G6];
	key += 6561 * board[G5];
	key += 19683 * board[G4];

	eval += corner5_2[key];
	key_corner5_2[7] = corner5_2[key];
	//if(corner5_2[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	return eval;
}

double check_triangle()
{
	int key;
	double eval;

	key = board[A1];
	key += 3 * board[A2];
	key += 9 * board[A3];
	key += 27 * board[A4];
	key += 81 * board[B1];
	key += 243 * board[B2];
	key += 729 * board[B3];
	key += 2187 * board[C1];
	key += 6561 * board[C2];
	key += 19683 * board[D1];

	eval = triangle[key];
	key_triangle[0] = triangle[key];
	//if(triangle[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[H1];
	key += 3 * board[H2];
	key += 9 * board[H3];
	key += 27 * board[H4];
	key += 81 * board[G1];
	key += 243 * board[G2];
	key += 729 * board[G3];
	key += 2187 * board[F1];
	key += 6561 * board[F2];
	key += 19683 * board[E1];

	eval += triangle[key];
	key_triangle[1] = triangle[key];
	//if(triangle[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[A8];
	key += 3 * board[A7];
	key += 9 * board[A6];
	key += 27 * board[A5];
	key += 81 * board[B8];
	key += 243 * board[B7];
	key += 729 * board[B6];
	key += 2187 * board[C8];
	key += 6561 * board[C7];
	key += 19683 * board[D8];

	eval += triangle[key];
	key_triangle[2] = triangle[key];
	//if(triangle[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	key = board[H8];
	key += 3 * board[H7];
	key += 9 * board[H6];
	key += 27 * board[H5];
	key += 81 * board[G8];
	key += 243 * board[G7];
	key += 729 * board[G6];
	key += 2187 * board[F8];
	key += 6561 * board[F7];
	key += 19683 * board[E8];

	eval += triangle[key];
	key_triangle[3] = triangle[key];
	//if(triangle[key] == 0)
	//{
	//	COUNT_NO_PATTERN++;
	//}
	//else
	//{
	//	COUNT_PATTERN++;
	//}

	return eval;
}

//double check_potential_mobility(BitBoard b_board, BitBoard w_board, BitBoard blank)
//{
//
//	int count = CountBit(get_potential_moves(b_board, w_board, blank));
//	key_pot_mobility = pot_mobilityNum[count];
//
//	return pot_mobilityNum[count];
//}

double check_parity(BitBoard blank)
{
	int p;

	p = CountBit(blank & 0x0f0f0f0f) % 2;
	p |= (CountBit(blank & 0xf0f0f0f0) % 2) << 1;
	p |= (CountBit(blank & 0x0f0f0f0f00000000) % 2) << 2;
	p |= (CountBit(blank & 0xf0f0f0f000000000) % 2) << 3;

	key_parity = parity[p];

	return parity[p];
}



double check_mobility(BitBoard b_board, BitBoard w_board)
{
	int mob1, mob2;

	CreateMoves(b_board, w_board, &mob1);
	CreateMoves(w_board, b_board, &mob2);
	key_mobility = mobility[mob1 - mob2 + 32];

	return key_mobility;
}

//int symmetry(int index_num)
//{
//	if(index_num == 0)
//	{
//		return 0;
//	}
//	return index_num;
//}
//
///* 線対称 */
//int convert_index_sym(int index_num, int num_table[])
//{
//	int i;
//	int s_index_num = 0;
//	int *p_num = &num_table[0];
//	for(i = 0; index_num != 0; i++)
//	{
//		s_index_num += symmetry(index_num % 3) * pow_table[p_num[i]]; 
//		index_num /= 3;
//	}
//	
//	return s_index_num;
//}

//int reverse(int index_num)
//{
//	if(index_num == 0)
//	{
//		return 0;
//	}
//	return index_num ^ 3;
//}
//
///* 白黒反転 */
//int convert_index_rev(int index_num)
//{
//	int i;
//	int r_index_num = 0;
//	for(i = 0; index_num != 0; i++)
//	{
//		r_index_num += reverse(index_num % 3) * pow_table[i]; 
//		index_num /= 3;
//	}
//	
//	return r_index_num;
//}

/* 対称変換テーブル */
//int hori_convert_table[8] =		 {7,6,5,4,3,2,1,0};
//int dia2_convert_table[7] =		 {6,5,4,3,2,1,0};
//int dia3_convert_table[6] =		 {5,4,3,2,1,0};
//int dia4_convert_table[8] =		 {4,3,2,1,0};
//int edge_convert_table[10] =	 {7,6,5,4,3,2,1,0,9,8};
//int corner3_3_convert_table[9] = {0,3,6,1,4,7,2,5,8};
//int triangle_convert_table[10] = {0,4,7,9,1,5,8,2,6,3};
//
//void write_h_ver1(FILE *fp)
//{
//	int key;
//	int sym_key;  //対称形を正規化する
//
//	/* a2 b2 c2 d2 e2 f2 g2 h2 */
//	/* a7 b7 c7 d7 e7 f7 g7 h7 */
//	/* b1 b2 b3 b4 b5 b6 b7 b8 */
//	/* g1 g2 g3 g4 g5 g6 g7 g8 */
//
//	key = board[A2];
//	key += 3 * board[B2];
//	key += 9 * board[C2];
//	key += 27 * board[D2];
//	key += 81 * board[E2];
//	key += 243 * board[F2];
//	key += 729 * board[G2];
//	key += 2187 * board[H2];
//
//	/* 対称形が存在するため、小さいインデックス番号に正規化(パターンの出現頻度を多くする) */
//	sym_key = convert_index_sym(key, hori_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[A7];
//	key += 3 * board[B7];
//	key += 9 * board[C7];
//	key += 27 * board[D7];
//	key += 81 * board[E7];
//	key += 243 * board[F7];
//	key += 729 * board[G7];
//	key += 2187 * board[H7];
//
//	sym_key = convert_index_sym(key, hori_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[B1];
//	key += 3 * board[B2];
//	key += 9 * board[B3];
//	key += 27 * board[B4];
//	key += 81 * board[B5];
//	key += 243 * board[B6];
//	key += 729 * board[B7];
//	key += 2187 * board[B8];
//
//	sym_key = convert_index_sym(key, hori_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[G1];
//	key += 3 * board[G2];
//	key += 9 * board[G3];
//	key += 27 * board[G4];
//	key += 81 * board[G5];
//	key += 243 * board[G6];
//	key += 729 * board[G7];
//	key += 2187 * board[G8];
//
//	sym_key = convert_index_sym(key, hori_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	
//}
//
//void write_h_ver2(FILE *fp)
//{
//	int key;
//	int sym_key;
//
//	/* a3 b3 c3 d3 e3 f3 g3 h3 */
//	/* a6 b6 c6 d6 e6 f6 g6 h6 */
//	/* c1 c2 c3 c4 c5 c6 c7 c8 */
//	/* f1 f2 f3 f4 f5 f6 f7 f8 */
//
//	key = board[A3];
//	key += 3 * board[B3];
//	key += 9 * board[C3];
//	key += 27 * board[D3];
//	key += 81 * board[E3];
//	key += 243 * board[F3];
//	key += 729 * board[G3];
//	key += 2187 * board[H3];
//
//	sym_key = convert_index_sym(key, hori_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[A6];
//	key += 3 * board[B6];
//	key += 9 * board[C6];
//	key += 27 * board[D6];
//	key += 81 * board[E6];
//	key += 243 * board[F6];
//	key += 729 * board[G6];
//	key += 2187 * board[H6];
//
//	sym_key = convert_index_sym(key, hori_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[C1];
//	key += 3 * board[C2];
//	key += 9 * board[C3];
//	key += 27 * board[C4];
//	key += 81 * board[C5];
//	key += 243 * board[C6];
//	key += 729 * board[C7];
//	key += 2187 * board[C8];
//
//	sym_key = convert_index_sym(key, hori_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[F1];
//	key += 3 * board[F2];
//	key += 9 * board[F3];
//	key += 27 * board[F4];
//	key += 81 * board[F5];
//	key += 243 * board[F6];
//	key += 729 * board[F7];
//	key += 2187 * board[F8];
//
//	sym_key = convert_index_sym(key, hori_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//}
//
//void write_h_ver3(FILE *fp)
//{
//	int key;
//	int sym_key;
//
//	/* a4 b4 c4 d4 e4 f4 g4 h4 */
//	/* a5 b5 c5 d5 e5 f5 g5 h5 */
//	/* d1 d2 d3 d4 d5 d6 d7 d8 */
//	/* e1 e2 e3 e4 e5 e6 e7 e8 */
//
//	key = board[A4];
//	key += 3 * board[B4];
//	key += 9 * board[C4];
//	key += 27 * board[D4];
//	key += 81 * board[E4];
//	key += 243 * board[F4];
//	key += 729 * board[G4];
//	key += 2187 * board[H4];
//
//	sym_key = convert_index_sym(key, hori_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[A5];
//	key += 3 * board[B5];
//	key += 9 * board[C5];
//	key += 27 * board[D5];
//	key += 81 * board[E5];
//	key += 243 * board[F5];
//	key += 729 * board[G5];
//	key += 2187 * board[H5];
//
//	sym_key = convert_index_sym(key, hori_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[D1];
//	key += 3 * board[D2];
//	key += 9 * board[D3];
//	key += 27 * board[D4];
//	key += 81 * board[D5];
//	key += 243 * board[D6];
//	key += 729 * board[D7];
//	key += 2187 * board[D8];
//
//	sym_key = convert_index_sym(key, hori_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[E1];
//	key += 3 * board[E2];
//	key += 9 * board[E3];
//	key += 27 * board[E4];
//	key += 81 * board[E5];
//	key += 243 * board[E6];
//	key += 729 * board[E7];
//	key += 2187 * board[E8];
//
//	sym_key = convert_index_sym(key, hori_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//}
//
//void write_dia_ver1(FILE *fp)
//{
//	int key;
//	int sym_key;
//	
//
//	/* a1 b2 c3 d4 e5 f6 g7 h8 */
//	/* h1 g2 f3 e4 d5 c6 b7 a8  */
//
//	key = board[A1];
//	key += 3 * board[B2];
//	key += 9 * board[C3];
//	key += 27 * board[D4];
//	key += 81 * board[E5];
//	key += 243 * board[F6];
//	key += 729 * board[G7];
//	key += 2187 * board[H8];
//
//	sym_key = convert_index_sym(key, hori_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[H1];
//	key += 3 * board[G2];
//	key += 9 * board[F3];
//	key += 27 * board[E4];
//	key += 81 * board[D5];
//	key += 243 * board[C6];
//	key += 729 * board[B7];
//	key += 2187 * board[A8];
//
//	sym_key = convert_index_sym(key, hori_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	
//}
//
//void write_dia_ver2(FILE *fp)
//{
//	int key;
//	int sym_key;
//
//	/* a2 b3 c4 d5 e6 f7 g8 */
//	/* b1 c2 d3 e4 f5 g6 h7 */
//	/* h2 g3 f4 e5 d6 c7 b8 */
//	/* g1 f2 e3 d4 c5 b6 a7 */
//
//	key = board[A2];
//	key += 3 * board[B3];
//	key += 9 * board[C4];
//	key += 27 * board[D5];
//	key += 81 * board[E6];
//	key += 243 * board[F7];
//	key += 729 * board[G8];
//
//	sym_key = convert_index_sym(key, dia2_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[B1];
//	key += 3 * board[C2];
//	key += 9 * board[D3];
//	key += 27 * board[E4];
//	key += 81 * board[F5];
//	key += 243 * board[G6];
//	key += 729 * board[H7];
//
//	sym_key = convert_index_sym(key, dia2_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[H2];
//	key += 3 * board[G3];
//	key += 9 * board[F4];
//	key += 27 * board[E5];
//	key += 81 * board[D6];
//	key += 243 * board[C7];
//	key += 729 * board[B8];
//
//	sym_key = convert_index_sym(key, dia2_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[G1];
//	key += 3 * board[F2];
//	key += 9 * board[E3];
//	key += 27 * board[D4];
//	key += 81 * board[C5];
//	key += 243 * board[B6];
//	key += 729 * board[A7];
//
//	sym_key = convert_index_sym(key, dia2_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	
//}
//
//void write_dia_ver3(FILE *fp)
//{
//	int key;
//	int sym_key;
//
//	/* a3 b4 c5 d6 e7 f8 */
//	/* c1 d2 e3 f4 g5 h6 */
//	/* h3 g4 f5 e6 d7 c8 */
//	/* f1 e2 d3 c4 b5 a6 */
//
//	key = board[A3];
//	key += 3 * board[B4];
//	key += 9 * board[C5];
//	key += 27 * board[D6];
//	key += 81 * board[E7];
//	key += 243 * board[F8];
//
//	sym_key = convert_index_sym(key, dia3_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[C1];
//	key += 3 * board[D2];
//	key += 9 * board[E3];
//	key += 27 * board[F4];
//	key += 81 * board[G5];
//	key += 243 * board[H6];
//
//	sym_key = convert_index_sym(key, dia3_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[H3];
//	key += 3 * board[G4];
//	key += 9 * board[F5];
//	key += 27 * board[E6];
//	key += 81 * board[D7];
//	key += 243 * board[C8];
//	
//	sym_key = convert_index_sym(key, dia3_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[F1];
//	key += 3 * board[E2];
//	key += 9 * board[D3];
//	key += 27 * board[C4];
//	key += 81 * board[B5];
//	key += 243 * board[A6];
//	
//	sym_key = convert_index_sym(key, dia3_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	
//}
//
//void write_dia_ver4(FILE *fp)
//{
//	int key;
//	int sym_key;
//
//	key = board[A4];
//	key += 3 * board[B5];
//	key += 9 * board[C6];
//	key += 27 * board[D7];
//	key += 81 * board[E8];
//
//	sym_key = convert_index_sym(key, dia4_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[D1];
//	key += 3 * board[E2];
//	key += 9 * board[F3];
//	key += 27 * board[G4];
//	key += 81 * board[H5];
//
//	sym_key = convert_index_sym(key, dia4_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[H4];
//	key += 3 * board[G5];
//	key += 9 * board[F6];
//	key += 27 * board[E7];
//	key += 81 * board[D8];
//
//	sym_key = convert_index_sym(key, dia4_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[E1];
//	key += 3 * board[D2];
//	key += 9 * board[C3];
//	key += 27 * board[B4];
//	key += 81 * board[A5];
//
//	sym_key = convert_index_sym(key, dia4_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	
//}
//
//void write_edge(FILE *fp)
//{
//	int key;
//	int sym_key;
//
//	key = board[A1];
//	key += 3 * board[A2];
//	key += 9 * board[A3];
//	key += 27 * board[A4];
//	key += 81 * board[A5];
//	key += 243 * board[A6];
//	key += 729 * board[A7];
//	key += 2187 * board[A8];
//	key += 6561 * board[B2];
//	key += 19683 * board[B7];
//
//	sym_key = convert_index_sym(key, edge_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[H1];
//	key += 3 * board[H2];
//	key += 9 * board[H3];
//	key += 27 * board[H4];
//	key += 81 * board[H5];
//	key += 243 * board[H6];
//	key += 729 * board[H7];
//	key += 2187 * board[H8];
//	key += 6561 * board[G2];
//	key += 19683 * board[G7];
//
//	sym_key = convert_index_sym(key, edge_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[A1];
//	key += 3 * board[B1];
//	key += 9 * board[C1];
//	key += 27 * board[D1];
//	key += 81 * board[E1];
//	key += 243 * board[F1];
//	key += 729 * board[G1];
//	key += 2187 * board[H1];
//	key += 6561 * board[B2];
//	key += 19683 * board[G2];
//
//	sym_key = convert_index_sym(key, edge_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[A8];
//	key += 3 * board[B8];
//	key += 9 * board[C8];
//	key += 27 * board[D8];
//	key += 81 * board[E8];
//	key += 243 * board[F8];
//	key += 729 * board[G8];
//	key += 2187 * board[H8];
//	key += 6561 * board[B7];
//	key += 19683 * board[G7];
//
//	sym_key = convert_index_sym(key, edge_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	
//}
//
//void write_corner5_2(FILE *fp)
//{
//	int key;
//
//	key = board[A1];
//	key += 3 * board[B1];
//	key += 9 * board[C1];
//	key += 27 * board[D1];
//	key += 81 * board[E1];
//	key += 243 * board[A2];
//	key += 729 * board[B2];
//	key += 2187 * board[C2];
//	key += 6561 * board[D2];
//	key += 19683 * board[E2];
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[A8];
//	key += 3 * board[B8];
//	key += 9 * board[C8];
//	key += 27 * board[D8];
//	key += 81 * board[E8];
//	key += 243 * board[A7];
//	key += 729 * board[B7];
//	key += 2187 * board[C7];
//	key += 6561 * board[D7];
//	key += 19683 * board[E7];
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[H1];
//	key += 3 * board[G1];
//	key += 9 * board[F1];
//	key += 27 * board[E1];
//	key += 81 * board[D1];
//	key += 243 * board[H2];
//	key += 729 * board[G2];
//	key += 2187 * board[F2];
//	key += 6561 * board[E2];
//	key += 19683 * board[D2];
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[H8];
//	key += 3 * board[G8];
//	key += 9 * board[F8];
//	key += 27 * board[E8];
//	key += 81 * board[D8];
//	key += 243 * board[H7];
//	key += 729 * board[G7];
//	key += 2187 * board[F7];
//	key += 6561 * board[E7];
//	key += 19683 * board[D7];
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[A1];
//	key += 3 * board[A2];
//	key += 9 * board[A3];
//	key += 27 * board[A4];
//	key += 81 * board[A5];
//	key += 243 * board[B1];
//	key += 729 * board[B2];
//	key += 2187 * board[B3];
//	key += 6561 * board[B4];
//	key += 19683 * board[B5];
//
//	fprintf(fp, "%d\n", key);
//	
//	key = board[H1];
//	key += 3 * board[H2];
//	key += 9 * board[H3];
//	key += 27 * board[H4];
//	key += 81 * board[H5];
//	key += 243 * board[G1];
//	key += 729 * board[G2];
//	key += 2187 * board[G3];
//	key += 6561 * board[G4];
//	key += 19683 * board[G5];
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[A8];
//	key += 3 * board[A7];
//	key += 9 * board[A6];
//	key += 27 * board[A5];
//	key += 81 * board[A4];
//	key += 243 * board[B8];
//	key += 729 * board[B7];
//	key += 2187 * board[B6];
//	key += 6561 * board[B5];
//	key += 19683 * board[B4];
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[H8];
//	key += 3 * board[H7];
//	key += 9 * board[H6];
//	key += 27 * board[H5];
//	key += 81 * board[H4];
//	key += 243 * board[G8];
//	key += 729 * board[G7];
//	key += 2187 * board[G6];
//	key += 6561 * board[G5];
//	key += 19683 * board[G4];
//
//	fprintf(fp, "%d\n", key);
//
//	
//}
//
//void write_corner3_3(FILE *fp)
//{
//	int key;
//	int sym_key;
//
//	key = board[A1];
//	key += 3 * board[A2];
//	key += 9 * board[A3];
//	key += 27 * board[B1];
//	key += 81 * board[B2];
//	key += 243 * board[B3];
//	key += 729 * board[C1];
//	key += 2187 * board[C2];
//	key += 6561 * board[C3];
//
//	sym_key = convert_index_sym(key, corner3_3_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//	
//	key = board[H1];
//	key += 3 * board[H2];
//	key += 9 * board[H3];
//	key += 27 * board[G1];
//	key += 81 * board[G2];
//	key += 243 * board[G3];
//	key += 729 * board[F1];
//	key += 2187 * board[F2];
//	key += 6561 * board[F3];
//
//	sym_key = convert_index_sym(key, corner3_3_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[A8];
//	key += 3 * board[A7];
//	key += 9 * board[A6];
//	key += 27 * board[B8];
//	key += 81 * board[B7];
//	key += 243 * board[B6];
//	key += 729 * board[C8];
//	key += 2187 * board[C7];
//	key += 6561 * board[C6];
//
//	sym_key = convert_index_sym(key, corner3_3_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[H8];
//	key += 3 * board[H7];
//	key += 9 * board[ H6];
//	key += 27 * board[G8];
//	key += 81 * board[G7];
//	key += 243 * board[G6];
//	key += 729 * board[F8];
//	key += 2187 * board[F7];
//	key += 6561 * board[F6];
//
//	sym_key = convert_index_sym(key, corner3_3_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//}
//
//
//
//void write_triangle(FILE *fp)
//{
//	int key;
//	int sym_key;
//
//	key = board[A1];
//	key += 3 * board[A2];
//	key += 9 * board[A3];
//	key += 27 * board[A4];
//	key += 81 * board[B1];
//	key += 243 * board[B2];
//	key += 729 * board[B3];
//	key += 2187 * board[C1];
//	key += 6561 * board[C2];
//	key += 19683 * board[D1];
//
//	sym_key = convert_index_sym(key, triangle_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[H1];
//	key += 3 * board[H2];
//	key += 9 * board[H3];
//	key += 27 * board[H4];
//	key += 81 * board[G1];
//	key += 243 * board[G2];
//	key += 729 * board[G3];
//	key += 2187 * board[F1];
//	key += 6561 * board[F2];
//	key += 19683 * board[E1];
//
//	sym_key = convert_index_sym(key, triangle_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[A8];
//	key += 3 * board[A7];
//	key += 9 * board[A6];
//	key += 27 * board[A5];
//	key += 81 * board[B8];
//	key += 243 * board[B7];
//	key += 729 * board[B6];
//	key += 2187 * board[C8];
//	key += 6561 * board[C7];
//	key += 19683 * board[D8];
//
//	sym_key = convert_index_sym(key, triangle_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	key = board[H8];
//	key += 3 * board[H7];
//	key += 9 * board[H6];
//	key += 27 * board[H5];
//	key += 81 * board[G8];
//	key += 243 * board[G7];
//	key += 729 * board[G6];
//	key += 2187 * board[F8];
//	key += 6561 * board[F7];
//	key += 19683 * board[E8];
//
//	sym_key = convert_index_sym(key, triangle_convert_table);
//	if(key > sym_key)
//	{
//		key = sym_key;
//	}
//
//	fprintf(fp, "%d\n", key);
//
//	
//}
//
//void write_pality(FILE *fp, BitBoard blank, int color)
//{
//	int pal;
//	
//	pal  = CountBit(blank & 0x0f0f0f0f) % 2;
//	pal += CountBit(blank & 0xf0f0f0f0) % 2;
//	pal += CountBit(blank & 0x0f0f0f0f00000000) % 2;
//	pal += CountBit(blank & 0xf0f0f0f000000000) % 2;
//	pal += (5 * color);
//
//	fprintf(fp, "%d\n", pal);
//}

//void print_h_ver1()
//{
//	p_pattern[0] = b1;
//	p_pattern[1] = b2;
//	p_pattern[2] = b3;
//	p_pattern[3] = b4;
//	p_pattern[4] = b5;
//	p_pattern[5] = b6;
//	p_pattern[6] = b7;
//	p_pattern[7] = b8;
//
//	p_pattern[8] = g1;
//	p_pattern[9] = g2;
//	p_pattern[10] = g3;
//	p_pattern[11] = g4;
//	p_pattern[12] = g5;
//	p_pattern[13] = g6;
//	p_pattern[14] = g7;
//	p_pattern[15] = g8;
//	
//	p_pattern[16] = a2;
//	p_pattern[17] = b2;
//	p_pattern[18] = c2;
//	p_pattern[19] = d2;
//	p_pattern[20] = e2;
//	p_pattern[21] = f2;
//	p_pattern[22] = g2;
//	p_pattern[23] = h2;
//
//	p_pattern[24] = a7;
//	p_pattern[25] = b7;
//	p_pattern[26] = c7;
//	p_pattern[27] = d7;
//	p_pattern[28] = e7;
//	p_pattern[29] = f7;
//	p_pattern[30] = g7;
//	p_pattern[31] = h7;
//}
//
//void print_h_ver2()
//{
//	p_pattern[0] = c1;
//	p_pattern[1] = c2;
//	p_pattern[2] = c3;
//	p_pattern[3] = c4;
//	p_pattern[4] = c5;
//	p_pattern[5] = c6;
//	p_pattern[6] = c7;
//	p_pattern[7] = c8;
//
//	p_pattern[8] = f1;
//	p_pattern[9] = f2;
//	p_pattern[10] = f3;
//	p_pattern[11] = f4;
//	p_pattern[12] = f5;
//	p_pattern[13] = f6;
//	p_pattern[14] = f7;
//	p_pattern[15] = f8;
//	
//	p_pattern[16] = a3;
//	p_pattern[17] = b3;
//	p_pattern[18] = c3;
//	p_pattern[19] = d3;
//	p_pattern[20] = e3;
//	p_pattern[21] = f3;
//	p_pattern[22] = g3;
//	p_pattern[23] = h3;
//
//	p_pattern[24] = a6;
//	p_pattern[25] = b6;
//	p_pattern[26] = c6;
//	p_pattern[27] = d6;
//	p_pattern[28] = e6;
//	p_pattern[29] = f6;
//	p_pattern[30] = g6;
//	p_pattern[31] = h6;
//}
//
//void print_h_ver3()
//{
//	p_pattern[0] = d1;
//	p_pattern[1] = d2;
//	p_pattern[2] = d3;
//	p_pattern[3] = d4;
//	p_pattern[4] = d5;
//	p_pattern[5] = d6;
//	p_pattern[6] = d7;
//	p_pattern[7] = d8;
//
//	p_pattern[8] = e1;
//	p_pattern[9] = e2;
//	p_pattern[10] = e3;
//	p_pattern[11] = e4;
//	p_pattern[12] = e5;
//	p_pattern[13] = e6;
//	p_pattern[14] = e7;
//	p_pattern[15] = e8;
//	
//	p_pattern[16] = a4;
//	p_pattern[17] = b4;
//	p_pattern[18] = c4;
//	p_pattern[19] = d4;
//	p_pattern[20] = e4;
//	p_pattern[21] = f4;
//	p_pattern[22] = g4;
//	p_pattern[23] = h4;
//
//	p_pattern[24] = a5;
//	p_pattern[25] = b5;
//	p_pattern[26] = c5;
//	p_pattern[27] = d5;
//	p_pattern[28] = e5;
//	p_pattern[29] = f5;
//	p_pattern[30] = g5;
//	p_pattern[31] = h5;
//}
//
//void print_dia_ver1()
//{
//	p_pattern[0] = a1;
//	p_pattern[1] = b2;
//	p_pattern[2] = c3;
//	p_pattern[3] = d4;
//	p_pattern[4] = e5;
//	p_pattern[5] = f6;
//	p_pattern[6] = g7;
//	p_pattern[7] = h8;
//
//	p_pattern[8] = a8;
//	p_pattern[9] = b7;
//	p_pattern[10] = c6;
//	p_pattern[11] = d5;
//	p_pattern[12] = e4;
//	p_pattern[13] = f3;
//	p_pattern[14] = g2;
//	p_pattern[15] = h1;
//}
//
//void print_dia_ver2()
//{
//	p_pattern[0] = b1;
//	p_pattern[1] = c2;
//	p_pattern[2] = d3;
//	p_pattern[3] = e4;
//	p_pattern[4] = f5;
//	p_pattern[5] = g6;
//	p_pattern[6] = h7;
//
//	p_pattern[8] = a2;
//	p_pattern[9] = b3;
//	p_pattern[10] = c4;
//	p_pattern[11] = d5;
//	p_pattern[12] = e6;
//	p_pattern[13] = f7;
//	p_pattern[14] = g8;
//
//	p_pattern[0] = g1;
//	p_pattern[1] = f2;
//	p_pattern[2] = e3;
//	p_pattern[3] = d4;
//	p_pattern[4] = c5;
//	p_pattern[5] = b6;
//	p_pattern[6] = a7;
//
//	p_pattern[8] = h2;
//	p_pattern[9] = g3;
//	p_pattern[10] = f4;
//	p_pattern[11] = e5;
//	p_pattern[12] = d6;
//	p_pattern[13] = c7;
//	p_pattern[14] = b8;
//}
//
//void print_dia_ver3()
//{
//	p_pattern[0] = c1;
//	p_pattern[1] = d2;
//	p_pattern[2] = e3;
//	p_pattern[3] = f4;
//	p_pattern[4] = g5;
//	p_pattern[5] = h6;
//
//	p_pattern[8] = a3;
//	p_pattern[9] = b4;
//	p_pattern[10] = c5;
//	p_pattern[11] = d6;
//	p_pattern[12] = e7;
//	p_pattern[13] = f8;
//
//	p_pattern[0] = f1;
//	p_pattern[1] = e2;
//	p_pattern[2] = d3;
//	p_pattern[3] = c4;
//	p_pattern[4] = b5;
//	p_pattern[5] = a6;
//
//	p_pattern[8] = h3;
//	p_pattern[9] = g4;
//	p_pattern[10] = f5;
//	p_pattern[11] = e6;
//	p_pattern[12] = d7;
//	p_pattern[13] = c8;
//}
//
//void print_dia_ver4()
//{
//	p_pattern[0] = d1;
//	p_pattern[1] = e2;
//	p_pattern[2] = f3;
//	p_pattern[3] = g4;
//	p_pattern[4] = h5;
//
//	p_pattern[8] = a4;
//	p_pattern[9] = b5;
//	p_pattern[10] = c6;
//	p_pattern[11] = d7;
//	p_pattern[12] = e8;
//
//	p_pattern[0] = e1;
//	p_pattern[1] = d2;
//	p_pattern[2] = c3;
//	p_pattern[3] = b4;
//	p_pattern[4] = a5;
//
//	p_pattern[8] = h4;
//	p_pattern[9] = g5;
//	p_pattern[10] = f6;
//	p_pattern[11] = e7;
//	p_pattern[12] = d8;
//}
//
//void print_edge()
//{
//	p_pattern[0] = a1;
//	p_pattern[1] = a2;
//	p_pattern[2] = a3;
//	p_pattern[3] = a4;
//	p_pattern[4] = a5;
//	p_pattern[5] = a6;
//	p_pattern[6] = a7;
//	p_pattern[7] = a8;
//	p_pattern[8] = b2;
//	p_pattern[9] = b7;
//
//	p_pattern[10] = h1;
//	p_pattern[11] = h2;
//	p_pattern[12] = h3;
//	p_pattern[13] = h4;
//	p_pattern[14] = h5;
//	p_pattern[15] = h6;
//	p_pattern[16] = h7;
//	p_pattern[17] = h8;
//	p_pattern[18] = b7;
//	p_pattern[19] = g7;
//
//	p_pattern[20] = a1;
//	p_pattern[21] = b1;
//	p_pattern[22] = c1;
//	p_pattern[23] = d1;
//	p_pattern[24] = e1;
//	p_pattern[25] = f1;
//	p_pattern[26] = g1;
//	p_pattern[27] = h1;
//	p_pattern[28] = b2;
//	p_pattern[29] = g2;
//
//	p_pattern[30] = a8;
//	p_pattern[31] = b8;
//	p_pattern[30] = c8;
//	p_pattern[31] = d8;
//	p_pattern[30] = e8;
//	p_pattern[30] = f8;
//	p_pattern[31] = g8;
//	p_pattern[31] = h8;
//	p_pattern[30] = b7;
//	p_pattern[31] = g7;
//}
//
//void print_corner5_2()
//{
//	p_pattern[0] = a1;
//	p_pattern[1] = a2;
//	p_pattern[2] = a3;
//	p_pattern[3] = a4;
//	p_pattern[4] = a5;
//	p_pattern[5] = b1;
//	p_pattern[6] = b2;
//	p_pattern[7] = b3;
//	p_pattern[8] = b4;
//	p_pattern[9] = b5;
//
//	p_pattern[10] = h1;
//	p_pattern[11] = h2;
//	p_pattern[12] = h3;
//	p_pattern[13] = h4;
//	p_pattern[14] = h5;
//	p_pattern[15] = g1;
//	p_pattern[16] = g2;
//	p_pattern[17] = g3;
//	p_pattern[18] = g4;
//	p_pattern[19] = g5;
//
//	p_pattern[20] = a1;
//	p_pattern[21] = b1;
//	p_pattern[22] = c1;
//	p_pattern[23] = d1;
//	p_pattern[24] = e1;
//	p_pattern[25] = a2;
//	p_pattern[26] = b2;
//	p_pattern[27] = c2;
//	p_pattern[28] = d2;
//	p_pattern[29] = e2;
//
//	p_pattern[30] = h1;
//	p_pattern[31] = g1;
//	p_pattern[30] = f1;
//	p_pattern[31] = e1;
//	p_pattern[30] = d1;
//	p_pattern[30] = h2;
//	p_pattern[31] = g2;
//	p_pattern[31] = f2;
//	p_pattern[30] = e2;
//	p_pattern[31] = d2;
//
//	p_pattern[32] = a8;
//	p_pattern[33] = a7;
//	p_pattern[34] = a6;
//	p_pattern[35] = a5;
//	p_pattern[36] = a4;
//	p_pattern[37] = b8;
//	p_pattern[38] = b7;
//	p_pattern[39] = b6;
//	p_pattern[40] = b5;
//	p_pattern[41] = b4;
//
//	p_pattern[42] = h8;
//	p_pattern[43] = h7;
//	p_pattern[44] = h6;
//	p_pattern[45] = h5;
//	p_pattern[46] = h4;
//	p_pattern[47] = g8;
//	p_pattern[48] = g7;
//	p_pattern[49] = g6;
//	p_pattern[50] = g5;
//	p_pattern[51] = g4;
//
//	p_pattern[52] = a8;
//	p_pattern[53] = b8;
//	p_pattern[54] = c8;
//	p_pattern[55] = d8;
//	p_pattern[56] = e8;
//	p_pattern[57] = a7;
//	p_pattern[58] = b7;
//	p_pattern[59] = c7;
//	p_pattern[60] = d7;
//	p_pattern[61] = e7;
//
//	p_pattern[62] = h8;
//	p_pattern[63] = g8;
//	p_pattern[64] = f8;
//	p_pattern[65] = e8;
//	p_pattern[66] = d8;
//	p_pattern[67] = h7;
//	p_pattern[68] = g7;
//	p_pattern[69] = f7;
//	p_pattern[70] = e7;
//	p_pattern[71] = d7;
//}
//
//void print_triangle()
//{
//	p_pattern[0] = a1;
//	p_pattern[1] = b1;
//	p_pattern[2] = c1;
//	p_pattern[3] = d1;
//	p_pattern[4] = a2;
//	p_pattern[5] = b2;
//	p_pattern[6] = c2;
//	p_pattern[7] = a3;
//	p_pattern[8] = b3;
//	p_pattern[9] = a4;
//
//	p_pattern[10] = a8;
//	p_pattern[11] = b8;
//	p_pattern[12] = c8;
//	p_pattern[13] = d8;
//	p_pattern[14] = a7;
//	p_pattern[15] = b7;
//	p_pattern[16] = c7;
//	p_pattern[17] = a6;
//	p_pattern[18] = b6;
//	p_pattern[19] = a5;
//
//	p_pattern[20] = h1;
//	p_pattern[21] = g1;
//	p_pattern[22] = f1;
//	p_pattern[23] = e1;
//	p_pattern[24] = h2;
//	p_pattern[25] = g2;
//	p_pattern[26] = f2;
//	p_pattern[27] = h3;
//	p_pattern[28] = g3;
//	p_pattern[29] = h4;
//
//	p_pattern[30] = h8;
//	p_pattern[31] = g8;
//	p_pattern[30] = f8;
//	p_pattern[31] = e8;
//	p_pattern[30] = h7;
//	p_pattern[30] = g7;
//	p_pattern[31] = f7;
//	p_pattern[31] = h6;
//	p_pattern[30] = g6;
//	p_pattern[31] = h5;
//
//}

//int GetIndexNum_COL_A()
//{
//	int index_num;
//
//	index_num  = board[A1];
//	index_num += board[A2] * 3;
//	index_num += board[A3] * 9;
//	index_num += board[A4] * 27;
//	index_num += board[A5] * 81;
//	index_num += board[A6] * 243;
//	index_num += board[A7] * 729;
//	index_num += board[A8] * 2187;
//
//	return index_num;
//}
//
//int GetIndexNum_COL_B()
//{
//	int index_num;
//
//	index_num  = board[B1];
//	index_num += board[B2] * 3;
//	index_num += board[B3] * 9;
//	index_num += board[B4] * 27;
//	index_num += board[B5] * 81;
//	index_num += board[B6] * 243;
//	index_num += board[B7] * 729;
//	index_num += board[B8] * 2187;
//
//	return index_num;
//}
//
//int GetIndexNum_COL_C()
//{
//	int index_num;
//
//	index_num  = board[C1];
//	index_num += board[C2] * 3;
//	index_num += board[C3] * 9;
//	index_num += board[C4] * 27;
//	index_num += board[C5] * 81;
//	index_num += board[C6] * 243;
//	index_num += board[C7] * 729;
//	index_num += board[C8] * 2187;
//
//	return index_num;
//}
//
//int GetIndexNum_COL_D()
//{
//	int index_num;
//
//	index_num  = board[D1];
//	index_num += board[D2] * 3;
//	index_num += board[D3] * 9;
//	index_num += board[D4] * 27;
//	index_num += board[D5] * 81;
//	index_num += board[D6] * 243;
//	index_num += board[D7] * 729;
//	index_num += board[D8] * 2187;
//
//	return index_num;
//}
//
//int GetIndexNum_COL_E()
//{
//	int index_num;
//
//	index_num  = board[E1];
//	index_num += board[E2] * 3;
//	index_num += board[E3] * 9;
//	index_num += board[E4] * 27;
//	index_num += board[E5] * 81;
//	index_num += board[E6] * 243;
//	index_num += board[E7] * 729;
//	index_num += board[E8] * 2187;
//
//	return index_num;
//}
//
//int GetIndexNum_COL_F()
//{
//	int index_num;
//
//	index_num  = board[F1];
//	index_num += board[F2] * 3;
//	index_num += board[F3] * 9;
//	index_num += board[F4] * 27;
//	index_num += board[F5] * 81;
//	index_num += board[F6] * 243;
//	index_num += board[F7] * 729;
//	index_num += board[F8] * 2187;
//
//	return index_num;
//}
//
//int GetIndexNum_COL_G()
//{
//	int index_num;
//
//	index_num  = board[G1];
//	index_num += board[G2] * 3;
//	index_num += board[G3] * 9;
//	index_num += board[G4] * 27;
//	index_num += board[G5] * 81;
//	index_num += board[G6] * 243;
//	index_num += board[G7] * 729;
//	index_num += board[G8] * 2187;
//
//	return index_num;
//}
//
//int GetIndexNum_COL_H()
//{
//	int index_num;
//
//	index_num  = board[H1];
//	index_num += board[H2] * 3;
//	index_num += board[H3] * 9;
//	index_num += board[H4] * 27;
//	index_num += board[H5] * 81;
//	index_num += board[H6] * 243;
//	index_num += board[H7] * 729;
//	index_num += board[H8] * 2187;
//
//	return index_num;
//}
//
//int GetIndexNum_ROW_1()
//{
//	int index_num;
//
//	index_num  = board[A1];
//	index_num  += board[B1] * 3;
//	index_num  += board[C1] * 9;
//	index_num  += board[D1] * 27;
//	index_num  += board[E1] * 81;
//	index_num  += board[F1] * 243;
//	index_num  += board[G1] * 729;
//	index_num  += board[H1] * 2187;
//
//	return index_num;
//}
//
//int GetIndexNum_ROW_2()
//{
//	int index_num;
//
//	index_num  = board[A2];
//	index_num  += board[B2] * 3;
//	index_num  += board[C2] * 9;
//	index_num  += board[D2] * 27;
//	index_num  += board[E2] * 81;
//	index_num  += board[F2] * 243;
//	index_num  += board[G2] * 729;
//	index_num  += board[H2] * 2187;
//
//	return index_num;
//}
//
//int GetIndexNum_ROW_3()
//{
//	int index_num;
//	index_num  = board[A3];
//	index_num  += board[B3] * 3;
//	index_num  += board[C3] * 9;
//	index_num  += board[D3] * 27;
//	index_num  += board[E3] * 81;
//	index_num  += board[F3] * 243;
//	index_num  += board[G3] * 729;
//	index_num  += board[H3] * 2187;
//
//	return index_num;
//}
//
//int GetIndexNum_ROW_4()
//{
//	int index_num;
//	index_num  = board[A4];
//	index_num  += board[B4] * 3;
//	index_num  += board[C4] * 9;
//	index_num  += board[D4] * 27;
//	index_num  += board[E4] * 81;
//	index_num  += board[F4] * 243;
//	index_num  += board[G4] * 729;
//	index_num  += board[H4] * 2187;
//
//	return index_num;
//}
//
//int GetIndexNum_ROW_5()
//{
//	int index_num;
//	index_num  = board[A5];
//	index_num  += board[B5] * 3;
//	index_num  += board[C5] * 9;
//	index_num  += board[D5] * 27;
//	index_num  += board[E5] * 81;
//	index_num  += board[F5] * 243;
//	index_num  += board[G5] * 729;
//	index_num  += board[H5] * 2187;
//
//	return index_num;
//}
//
//int GetIndexNum_ROW_6()
//{
//	int index_num;
//	index_num  = board[A6];
//	index_num  += board[B6] * 3;
//	index_num  += board[C6] * 9;
//	index_num  += board[D6] * 27;
//	index_num  += board[E6] * 81;
//	index_num  += board[F6] * 243;
//	index_num  += board[G6] * 729;
//	index_num  += board[H6] * 2187;
//
//	return index_num;
//}
//
//int GetIndexNum_ROW_7()
//{
//	int index_num;
//	index_num  = board[A7];
//	index_num  += board[B7] * 3;
//	index_num  += board[C7] * 9;
//	index_num  += board[D7] * 27;
//	index_num  += board[E7] * 81;
//	index_num  += board[F7] * 243;
//	index_num  += board[G7] * 729;
//	index_num  += board[H7] * 2187;
//
//	return index_num;
//}
//
//int GetIndexNum_ROW_8()
//{
//	int index_num;
//	index_num  = board[A8];
//	index_num  += board[B8] * 3;
//	index_num  += board[C8] * 9;
//	index_num  += board[D8] * 27;
//	index_num  += board[E8] * 81;
//	index_num  += board[F8] * 243;
//	index_num  += board[G8] * 729;
//	index_num  += board[H8] * 2187;
//
//	return index_num;
//}
//
//int GetIndexNum_DIA_1()
//{
//	int index_num;
//
//	index_num  = board[A1];
//	index_num  += board[B2] * 3;
//	index_num  += board[C3] * 9;
//	index_num  += board[D4] * 27;
//	index_num  += board[E5] * 81;
//	index_num  += board[F6] * 243;
//	index_num  += board[G7] * 729;
//	index_num  += board[H8] * 2187;
//
//	return index_num;
//}
//
//int GetIndexNum_DIA_2()
//{
//	int index_num;
//
//	index_num  = board[A2];
//	index_num  += board[B3] * 3;
//	index_num  += board[C4] * 9;
//	index_num  += board[D5] * 27;
//	index_num  += board[E6] * 81;
//	index_num  += board[F7] * 243;
//	index_num  += board[G8] * 729;
//
//	return index_num;
//}
//
//int GetIndexNum_DIA_3()
//{
//	int index_num;
//
//	index_num  = board[A3];
//	index_num  += board[B4] * 3;
//	index_num  += board[C5] * 9;
//	index_num  += board[D6] * 27;
//	index_num  += board[E7] * 81;
//	index_num  += board[F8] * 243;
//
//	return index_num;
//}
//
//int GetIndexNum_DIA_4()
//{
//	int index_num;
//
//	index_num  = board[A4];
//	index_num  += board[B5] * 3;
//	index_num  += board[C6] * 9;
//	index_num  += board[D7] * 27;
//	index_num  += board[E8] * 81;
//
//	return index_num;
//}
//
//int GetIndexNum_DIA_5()
//{
//	int index_num;
//
//	index_num  = board[A5];
//	index_num  += board[B6] * 3;
//	index_num  += board[C7] * 9;
//	index_num  += board[D8] * 27;
//
//	return index_num;
//}
//
//int GetIndexNum_DIA_6()
//{
//	int index_num;
//
//	index_num  = board[A6];
//	index_num  += board[B7] * 3;
//	index_num  += board[C8] * 9;
//
//	return index_num;
//}
//
//int GetIndexNum_DIA_7()
//{
//	int index_num;
//
//	index_num  = board[B1];
//	index_num  += board[C2] * 3;
//	index_num  += board[D3] * 9;
//	index_num  += board[E4] * 27;
//	index_num  += board[F5] * 81;
//	index_num  += board[G6] * 243;
//	index_num  += board[H7] * 729;
//
//	return index_num;
//}
//
//int GetIndexNum_DIA_8()
//{
//	int index_num;
//
//	index_num  = board[C1];
//	index_num  += board[D2] * 3;
//	index_num  += board[E3] * 9;
//	index_num  += board[F4] * 27;
//	index_num  += board[G5] * 81;
//	index_num  += board[H6] * 243;
//
//	return index_num;
//}
//
//int GetIndexNum_DIA_9()
//{
//	int index_num;
//
//	index_num  =  board[D1];
//	index_num  += board[E2] * 3;
//	index_num  += board[F3] * 9;
//	index_num  += board[G4] * 27;
//	index_num  += board[H5] * 243;
//
//	return index_num;
//}
//
//int GetIndexNum_DIA_10()
//{
//	int index_num;
//
//	index_num   = board[E1];
//	index_num  += board[F2] * 3;
//	index_num  += board[G3] * 9;
//	index_num  += board[H4] * 27;
//
//	return index_num;
//}
//
//int GetIndexNum_DIA_11()
//{
//	int index_num;
//
//	index_num   = board[F1];
//	index_num  += board[G2] * 3;
//	index_num  += board[H3] * 9;
//
//	return index_num;
//}
//
///* 右肩上がり */
//
//int GetIndexNum_DIA_12()
//{
//	int index_num;
//
//	index_num   = board[A3];
//	index_num  += board[B2] * 3;
//	index_num  += board[C1] * 9;
//
//	return index_num;
//}
//
//int GetIndexNum_DIA_13()
//{
//	int index_num;
//
//	index_num  = board[A4];
//	index_num  += board[B3] * 3;
//	index_num  += board[C2] * 9;
//	index_num  += board[D1] * 27;
//	
//	return index_num;
//}
//
//int GetIndexNum_DIA_14()
//{
//	int index_num;
//
//	index_num  = board[A5];
//	index_num  += board[B4] * 3;
//	index_num  += board[C3] * 9;
//	index_num  += board[D2] * 27;
//	index_num  += board[E1] * 81;
//
//	return index_num;
//}
//
//int GetIndexNum_DIA_15()
//{
//	int index_num;
//
//	index_num  = board[A6];
//	index_num  += board[B5] * 3;
//	index_num  += board[C4] * 9;
//	index_num  += board[D3] * 27;
//	index_num  += board[E2] * 81;
//	index_num  += board[F1] * 243;
//
//	return index_num;
//}
//
//int GetIndexNum_DIA_16()
//{
//	int index_num;
//
//	index_num  = board[A7];
//	index_num  += board[B6] * 3;
//	index_num  += board[C5] * 9;
//	index_num  += board[D4] * 27;
//	index_num  += board[E3] * 81;
//	index_num  += board[F2] * 243;
//	index_num  += board[G1] * 729;
//
//	return index_num;
//}
//
//int GetIndexNum_DIA_17()
//{
//	int index_num;
//
//	index_num  = board[A8];
//	index_num  += board[B7] * 3;
//	index_num  += board[C6] * 9;
//	index_num  += board[D5] * 27;
//	index_num  += board[E4] * 81;
//	index_num  += board[F3] * 243;
//	index_num  += board[G2] * 729;
//	index_num  += board[H1] * 2187;
//
//	return index_num;
//}
//
//
//int GetIndexNum_DIA_18()
//{
//	int index_num;
//
//	index_num  = board[B8];
//	index_num  += board[C7] * 3;
//	index_num  += board[D6] * 9;
//	index_num  += board[E5] * 27;
//	index_num  += board[F4] * 81;
//	index_num  += board[G3] * 243;
//	index_num  += board[H2] * 729;
//
//	return index_num;
//}
//
//int GetIndexNum_DIA_19()
//{
//	int index_num;
//
//	index_num  = board[C8];
//	index_num  += board[D7] * 3;
//	index_num  += board[E6] * 9;
//	index_num  += board[F5] * 27;
//	index_num  += board[G4] * 81;
//	index_num  += board[H3] * 243;
//
//	return index_num;
//}
//
//int GetIndexNum_DIA_20()
//{
//	int index_num;
//
//	index_num  = board[D8];
//	index_num  += board[E7] * 3;
//	index_num  += board[F6] * 9;
//	index_num  += board[G5] * 27;
//	index_num  += board[H4] * 81;
//
//	return index_num;
//}
//
//int GetIndexNum_DIA_21()
//{
//	int index_num;
//
//	index_num  = board[E8];
//	index_num  += board[F7] * 3;
//	index_num  += board[G6] * 9;
//	index_num  += board[H5] * 27;
//
//	return index_num;
//}
//
//int GetIndexNum_DIA_22()
//{
//	int index_num;
//
//	index_num  = board[F8];
//	index_num  += board[G7] * 3;
//	index_num  += board[H6] * 9;
//
//	return index_num;
//}
//
//BitBoard GetRevFromIndex_a1(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_A(bk, wh);
//	index_num = GetIndexNum_COL_A();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][0];
//
//	init_index_row_1(bk, wh);
//	index_num = GetIndexNum_ROW_1();
//	rev ^= rev_table_ROW[index_num][0];
//
//	init_index_dia_1(bk, wh);
//	index_num = GetIndexNum_DIA_1();
//	rev ^= rev_table_DIA_down[index_num][0];
//
//	return rev;
//}
//
//
//
//BitBoard GetRevFromIndex_a2(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_A(bk, wh);
//	index_num = GetIndexNum_COL_A();
//	rev = rev_table_COL[index_num][1];
//
//	init_index_row_2(bk, wh);
//	index_num = GetIndexNum_ROW_2();
//	rev ^= rev_table_ROW[index_num][0] << 1;
//
//	init_index_dia_2(bk, wh);
//	index_num = GetIndexNum_DIA_2();
//	rev ^= rev_table_DIA_down[index_num][0] << 1;
//
//	return rev;
//}
//
//
//
//BitBoard GetRevFromIndex_a3(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_A(bk, wh);
//	index_num = GetIndexNum_COL_A();
//	rev = rev_table_COL[index_num][2];
//
//	init_index_row_3(bk, wh);
//	index_num = GetIndexNum_ROW_3();
//	rev ^= rev_table_ROW[index_num][0] << 2;
//
//	init_index_dia_3(bk, wh);
//	index_num = GetIndexNum_DIA_3();
//	rev ^= rev_table_DIA_down[index_num][0] << 2;
//
//	init_index_dia_12(bk, wh);
//	index_num = GetIndexNum_DIA_12();
//	rev ^= rev_table_DIA_up[index_num][0] >> 5;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_a4(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_A(bk, wh);
//	index_num = GetIndexNum_COL_A();
//	rev = rev_table_COL[index_num][3];
//
//	init_index_row_4(bk, wh);
//	index_num = GetIndexNum_ROW_4();
//	rev ^= rev_table_ROW[index_num][0] << 3;
//
//	init_index_dia_4(bk, wh);
//	index_num = GetIndexNum_DIA_4();
//	rev ^= rev_table_DIA_down[index_num][0] << 3;
//
//	init_index_dia_13(bk, wh);
//	index_num = GetIndexNum_DIA_13();
//	rev ^= rev_table_DIA_up[index_num][0] >> 4;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_a5(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_A(bk, wh);
//	index_num = GetIndexNum_COL_A();
//	rev = rev_table_COL[index_num][4];
//
//	init_index_row_5(bk, wh);
//	index_num = GetIndexNum_ROW_5();
//	rev ^= rev_table_ROW[index_num][0] << 4;
//
//	init_index_dia_5(bk, wh);
//	index_num = GetIndexNum_DIA_5();
//	rev ^= rev_table_DIA_down[index_num][0] << 4;
//
//	init_index_dia_14(bk, wh);
//	index_num = GetIndexNum_DIA_14();
//	rev ^= rev_table_DIA_up[index_num][0] >> 3;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_a6(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_A(bk, wh);
//	index_num = GetIndexNum_COL_A();
//	rev = rev_table_COL[index_num][5];
//
//	init_index_row_6(bk, wh);
//	index_num = GetIndexNum_ROW_6();
//	rev ^= rev_table_ROW[index_num][0] << 5;
//
//	init_index_dia_6(bk, wh);
//	index_num = GetIndexNum_DIA_6();
//	rev ^= rev_table_DIA_down[index_num][0] << 5;
//
//	init_index_dia_15(bk, wh);
//	index_num = GetIndexNum_DIA_15();
//	rev ^= rev_table_DIA_up[index_num][0] >> 2;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_a7(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_A(bk, wh);
//	index_num = GetIndexNum_COL_A();
//	rev = rev_table_COL[index_num][6];
//
//	init_index_row_7(bk, wh);
//	index_num = GetIndexNum_ROW_7();
//	rev ^= rev_table_ROW[index_num][0] << 6;
//
//	init_index_dia_16(bk, wh);
//	index_num = GetIndexNum_DIA_16();
//	rev ^= rev_table_DIA_up[index_num][0] >> 1;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_a8(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_A(bk, wh);
//	index_num = GetIndexNum_COL_A();
//	rev = rev_table_COL[index_num][7];
//
//	init_index_row_8(bk, wh);
//	index_num = GetIndexNum_ROW_8();
//	rev ^= rev_table_ROW[index_num][0] << 7;
//
//	init_index_dia_17(bk, wh);
//	index_num = GetIndexNum_DIA_17();
//	rev ^= rev_table_DIA_up[index_num][0];
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_b1(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_B(bk, wh);
//	index_num = GetIndexNum_COL_B();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][0] << 8;
//
//	init_index_row_1(bk, wh);
//	index_num = GetIndexNum_ROW_1();
//	rev ^= rev_table_ROW[index_num][1];
//
//	init_index_dia_7(bk, wh);
//	index_num = GetIndexNum_DIA_7();
//	rev ^= rev_table_DIA_down[index_num][0] << 8;
//
//	return rev;
//}
//
//
//
//BitBoard GetRevFromIndex_b2(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_B(bk, wh);
//	index_num = GetIndexNum_COL_B();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][1] << 8;
//
//	init_index_row_2(bk, wh);
//	index_num = GetIndexNum_ROW_2();
//	rev ^= rev_table_ROW[index_num][1] << 1;
//
//	init_index_dia_1(bk, wh);
//	index_num = GetIndexNum_DIA_1();
//	rev ^= rev_table_DIA_down[index_num][1];
//	
//	init_index_dia_12(bk, wh);
//	index_num = GetIndexNum_DIA_12();
//	rev ^= rev_table_DIA_up[index_num][1] >> 5;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_b3(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_B(bk, wh);
//	index_num = GetIndexNum_COL_B();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][2] << 8;
//
//	init_index_row_3(bk, wh);
//	index_num = GetIndexNum_ROW_3();
//	rev ^= rev_table_ROW[index_num][1] << 2;
//
//	init_index_dia_2(bk, wh);
//	index_num = GetIndexNum_DIA_2();
//	rev ^= rev_table_DIA_down[index_num][1] << 1;
//
//	init_index_dia_13(bk, wh);
//	index_num = GetIndexNum_DIA_13();
//	rev ^= rev_table_DIA_up[index_num][1] >> 4;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_b4(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_B(bk, wh);
//	index_num = GetIndexNum_COL_B();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][3] << 8;
//
//	init_index_row_4(bk, wh);
//	index_num = GetIndexNum_ROW_4();
//	rev ^= rev_table_ROW[index_num][1] << 3;
//
//	init_index_dia_3(bk, wh);
//	index_num = GetIndexNum_DIA_3();
//	rev ^= rev_table_DIA_down[index_num][1] << 2;
//
//	init_index_dia_14(bk, wh);
//	index_num = GetIndexNum_DIA_14();
//	rev ^= rev_table_DIA_up[index_num][1] >> 3;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_b5(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_B(bk, wh);
//	index_num = GetIndexNum_COL_B();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][4] << 8;
//
//	init_index_row_5(bk, wh);
//	index_num = GetIndexNum_ROW_5();
//	rev ^= rev_table_ROW[index_num][1] << 4;
//
//	init_index_dia_4(bk, wh);
//	index_num = GetIndexNum_DIA_4();
//	rev ^= rev_table_DIA_down[index_num][1] << 3;
//
//	init_index_dia_15(bk, wh);
//	index_num = GetIndexNum_DIA_15();
//	rev ^= rev_table_DIA_up[index_num][1] >> 2;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_b6(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_B(bk, wh);
//	index_num = GetIndexNum_COL_B();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][5] << 8;
//
//	init_index_row_6(bk, wh);
//	index_num = GetIndexNum_ROW_6();
//	rev ^= rev_table_ROW[index_num][1] << 5;
//
//	init_index_dia_5(bk, wh);
//	index_num = GetIndexNum_DIA_5();
//	rev ^= rev_table_DIA_down[index_num][1] << 4;
//
//	init_index_dia_16(bk, wh);
//	index_num = GetIndexNum_DIA_16();
//	rev ^= rev_table_DIA_up[index_num][1] >> 1;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_b7(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_B(bk, wh);
//	index_num = GetIndexNum_COL_B();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][6] << 8;
//
//	init_index_row_7(bk, wh);
//	index_num = GetIndexNum_ROW_7();
//	rev ^= rev_table_ROW[index_num][1] << 6;
//
//	init_index_dia_17(bk, wh);
//	index_num = GetIndexNum_DIA_17();
//	rev ^= rev_table_DIA_up[index_num][1];
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_b8(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_B(bk, wh);
//	index_num = GetIndexNum_COL_B();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][7] << 8;
//
//	init_index_row_8(bk, wh);
//	index_num = GetIndexNum_ROW_8();
//	rev ^= rev_table_ROW[index_num][1] << 7;
//
//	init_index_dia_18(bk, wh);
//	index_num = GetIndexNum_DIA_18();
//	rev ^= rev_table_DIA_up[index_num][0] << 8;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_c1(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_C(bk, wh);
//	index_num = GetIndexNum_COL_C();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][0] << 16;
//
//	init_index_row_1(bk, wh);
//	index_num = GetIndexNum_ROW_1();
//	rev ^= rev_table_ROW[index_num][2];
//
//	init_index_dia_8(bk, wh);
//	index_num = GetIndexNum_DIA_8();
//	rev ^= rev_table_DIA_down[index_num][0] << 16;
//
//	init_index_dia_12(bk, wh);
//	index_num = GetIndexNum_DIA_12();
//	rev ^= rev_table_DIA_up[index_num][2] >> 5;
//
//	return rev;
//}
//
//
//
//BitBoard GetRevFromIndex_c2(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_C(bk, wh);
//	index_num = GetIndexNum_COL_C();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][1] << 16;
//
//	init_index_row_2(bk, wh);
//	index_num = GetIndexNum_ROW_2();
//	rev ^= rev_table_ROW[index_num][2] << 1;
//
//	init_index_dia_7(bk, wh);
//	index_num = GetIndexNum_DIA_7();
//	rev ^= rev_table_DIA_down[index_num][1] << 8;
//	
//	init_index_dia_13(bk, wh);
//	index_num = GetIndexNum_DIA_13();
//	rev ^= rev_table_DIA_up[index_num][2] >> 4;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_c3(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_C(bk, wh);
//	index_num = GetIndexNum_COL_C();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][2] << 16;
//
//	init_index_row_3(bk, wh);
//	index_num = GetIndexNum_ROW_3();
//	rev ^= rev_table_ROW[index_num][2] << 2;
//
//	init_index_dia_1(bk, wh);
//	index_num = GetIndexNum_DIA_1();
//	rev ^= rev_table_DIA_down[index_num][2];
//	
//	init_index_dia_14(bk, wh);
//	index_num = GetIndexNum_DIA_14();
//	rev ^= rev_table_DIA_up[index_num][2] >> 3;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_c4(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_C(bk, wh);
//	index_num = GetIndexNum_COL_C();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][3] << 16;
//
//	init_index_row_4(bk, wh);
//	index_num = GetIndexNum_ROW_4();
//	rev ^= rev_table_ROW[index_num][2] << 3;
//
//	init_index_dia_2(bk, wh);
//	index_num = GetIndexNum_DIA_2();
//	rev ^= rev_table_DIA_down[index_num][2] << 1;
//	
//	init_index_dia_15(bk, wh);
//	index_num = GetIndexNum_DIA_15();
//	rev ^= rev_table_DIA_up[index_num][2] >> 2;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_c5(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_C(bk, wh);
//	index_num = GetIndexNum_COL_C();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][4] << 16;
//
//	init_index_row_5(bk, wh);
//	index_num = GetIndexNum_ROW_5();
//	rev ^= rev_table_ROW[index_num][2] << 4;
//
//	init_index_dia_3(bk, wh);
//	index_num = GetIndexNum_DIA_3();
//	rev ^= rev_table_DIA_down[index_num][2] << 2;
//	
//	init_index_dia_16(bk, wh);
//	index_num = GetIndexNum_DIA_16();
//	rev ^= rev_table_DIA_up[index_num][2] >> 1;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_c6(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_C(bk, wh);
//	index_num = GetIndexNum_COL_C();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][5] << 16;
//
//	init_index_row_6(bk, wh);
//	index_num = GetIndexNum_ROW_6();
//	rev ^= rev_table_ROW[index_num][2] << 5;
//
//	init_index_dia_4(bk, wh);
//	index_num = GetIndexNum_DIA_4();
//	rev ^= rev_table_DIA_down[index_num][2] << 3;
//	
//	init_index_dia_17(bk, wh);
//	index_num = GetIndexNum_DIA_17();
//	rev ^= rev_table_DIA_up[index_num][2];
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_c7(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_C(bk, wh);
//	index_num = GetIndexNum_COL_C();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][6] << 16;
//
//	init_index_row_7(bk, wh);
//	index_num = GetIndexNum_ROW_7();
//	rev ^= rev_table_ROW[index_num][2] << 6;
//
//	init_index_dia_5(bk, wh);
//	index_num = GetIndexNum_DIA_5();
//	rev ^= rev_table_DIA_down[index_num][2] << 4;
//	
//	init_index_dia_18(bk, wh);
//	index_num = GetIndexNum_DIA_18();
//	rev ^= rev_table_DIA_up[index_num][1] << 8;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_c8(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_C(bk, wh);
//	index_num = GetIndexNum_COL_C();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][7] << 16;
//
//	init_index_row_8(bk, wh);
//	index_num = GetIndexNum_ROW_8();
//	rev ^= rev_table_ROW[index_num][2] << 7;
//
//	init_index_dia_6(bk, wh);
//	index_num = GetIndexNum_DIA_6();
//	rev ^= rev_table_DIA_down[index_num][2] << 5;
//	
//	init_index_dia_19(bk, wh);
//	index_num = GetIndexNum_DIA_19();
//	rev ^= rev_table_DIA_up[index_num][0] << 16;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_d1(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_D(bk, wh);
//	index_num = GetIndexNum_COL_D();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][0] << 24;
//
//	init_index_row_1(bk, wh);
//	index_num = GetIndexNum_ROW_1();
//	rev ^= rev_table_ROW[index_num][3];
//
//	init_index_dia_9(bk, wh);
//	index_num = GetIndexNum_DIA_9();
//	rev ^= rev_table_DIA_down[index_num][0] << 24;
//
//	init_index_dia_13(bk, wh);
//	index_num = GetIndexNum_DIA_13();
//	rev ^= rev_table_DIA_up[index_num][3] >> 4;
//
//	return rev;
//}
//
//
//
//BitBoard GetRevFromIndex_d2(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_D(bk, wh);
//	index_num = GetIndexNum_COL_D();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][1] << 24;
//
//	init_index_row_2(bk, wh);
//	index_num = GetIndexNum_ROW_2();
//	rev ^= rev_table_ROW[index_num][3] << 1;
//
//	init_index_dia_8(bk, wh);
//	index_num = GetIndexNum_DIA_8();
//	rev ^= rev_table_DIA_down[index_num][1] << 16;
//
//	init_index_dia_14(bk, wh);
//	index_num = GetIndexNum_DIA_14();
//	rev ^= rev_table_DIA_up[index_num][3] >> 3;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_d3(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_D(bk, wh);
//	index_num = GetIndexNum_COL_D();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][2] << 24;
//
//	init_index_row_3(bk, wh);
//	index_num = GetIndexNum_ROW_3();
//	rev ^= rev_table_ROW[index_num][3] << 2;
//
//	init_index_dia_7(bk, wh);
//	index_num = GetIndexNum_DIA_7();
//	rev ^= rev_table_DIA_down[index_num][2] << 8;
//
//	init_index_dia_15(bk, wh);
//	index_num = GetIndexNum_DIA_15();
//	rev ^= rev_table_DIA_up[index_num][3] >> 2;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_d4(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_D(bk, wh);
//	index_num = GetIndexNum_COL_D();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][3] << 24;
//
//	init_index_row_4(bk, wh);
//	index_num = GetIndexNum_ROW_4();
//	rev ^= rev_table_ROW[index_num][3] << 3;
//
//	init_index_dia_1(bk, wh);
//	index_num = GetIndexNum_DIA_1();
//	rev ^= rev_table_DIA_down[index_num][3];
//
//	init_index_dia_16(bk, wh);
//	index_num = GetIndexNum_DIA_16();
//	rev ^= rev_table_DIA_up[index_num][3] >> 1;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_d5(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_D(bk, wh);
//	index_num = GetIndexNum_COL_D();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][4] << 24;
//
//	init_index_row_5(bk, wh);
//	index_num = GetIndexNum_ROW_5();
//	rev ^= rev_table_ROW[index_num][3] << 4;
//
//	init_index_dia_2(bk, wh);
//	index_num = GetIndexNum_DIA_2();
//	rev ^= rev_table_DIA_down[index_num][3] << 1;
//
//	init_index_dia_17(bk, wh);
//	index_num = GetIndexNum_DIA_17();
//	rev ^= rev_table_DIA_up[index_num][3];
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_d6(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_D(bk, wh);
//	index_num = GetIndexNum_COL_D();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][5] << 24;
//
//	init_index_row_6(bk, wh);
//	index_num = GetIndexNum_ROW_6();
//	rev ^= rev_table_ROW[index_num][3] << 5;
//
//	init_index_dia_3(bk, wh);
//	index_num = GetIndexNum_DIA_3();
//	rev ^= rev_table_DIA_down[index_num][3] << 2;
//
//	init_index_dia_18(bk, wh);
//	index_num = GetIndexNum_DIA_18();
//	rev ^= rev_table_DIA_up[index_num][2] << 8;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_d7(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_D(bk, wh);
//	index_num = GetIndexNum_COL_D();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][6] << 24;
//
//	init_index_row_7(bk, wh);
//	index_num = GetIndexNum_ROW_7();
//	rev ^= rev_table_ROW[index_num][3] << 6;
//
//	init_index_dia_4(bk, wh);
//	index_num = GetIndexNum_DIA_4();
//	rev ^= rev_table_DIA_down[index_num][3] << 3;
//
//	init_index_dia_19(bk, wh);
//	index_num = GetIndexNum_DIA_19();
//	rev ^= rev_table_DIA_up[index_num][1] << 16;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_d8(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_D(bk, wh);
//	index_num = GetIndexNum_COL_D();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][7] << 24;
//
//	init_index_row_8(bk, wh);
//	index_num = GetIndexNum_ROW_8();
//	rev ^= rev_table_ROW[index_num][3] << 7;
//
//	init_index_dia_5(bk, wh);
//	index_num = GetIndexNum_DIA_5();
//	rev ^= rev_table_DIA_down[index_num][3] << 4;
//
//	init_index_dia_20(bk, wh);
//	index_num = GetIndexNum_DIA_20();
//	rev ^= rev_table_DIA_up[index_num][0] << 24;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_e1(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_E(bk, wh);
//	index_num = GetIndexNum_COL_E();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][0] << 32;
//
//	init_index_row_1(bk, wh);
//	index_num = GetIndexNum_ROW_1();
//	rev ^= rev_table_ROW[index_num][4];
//
//	init_index_dia_10(bk, wh);
//	index_num = GetIndexNum_DIA_10();
//	rev ^= rev_table_DIA_down[index_num][0] << 32;
//
//	init_index_dia_14(bk, wh);
//	index_num = GetIndexNum_DIA_14();
//	rev ^= rev_table_DIA_up[index_num][4] >> 3;
//
//	return rev;
//}
//
//
//
//BitBoard GetRevFromIndex_e2(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_E(bk, wh);
//	index_num = GetIndexNum_COL_E();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][1] << 32;
//
//	init_index_row_2(bk, wh);
//	index_num = GetIndexNum_ROW_2();
//	rev ^= rev_table_ROW[index_num][4] << 1;
//
//	init_index_dia_9(bk, wh);
//	index_num = GetIndexNum_DIA_9();
//	rev ^= rev_table_DIA_down[index_num][1] << 24;
//
//	init_index_dia_15(bk, wh);
//	index_num = GetIndexNum_DIA_15();
//	rev ^= rev_table_DIA_up[index_num][4] >> 2;
//	
//	return rev;
//}
//
//BitBoard GetRevFromIndex_e3(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_E(bk, wh);
//	index_num = GetIndexNum_COL_E();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][2] << 32;
//
//	init_index_row_3(bk, wh);
//	index_num = GetIndexNum_ROW_3();
//	rev ^= rev_table_ROW[index_num][4] << 2;
//
//	init_index_dia_8(bk, wh);
//	index_num = GetIndexNum_DIA_8();
//	rev ^= rev_table_DIA_down[index_num][2] << 16;
//
//	init_index_dia_16(bk, wh);
//	index_num = GetIndexNum_DIA_16();
//	rev ^= rev_table_DIA_up[index_num][4] >> 1;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_e4(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_E(bk, wh);
//	index_num = GetIndexNum_COL_E();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][3] << 32;
//
//	init_index_row_4(bk, wh);
//	index_num = GetIndexNum_ROW_4();
//	rev ^= rev_table_ROW[index_num][4] << 3;
//
//	init_index_dia_7(bk, wh);
//	index_num = GetIndexNum_DIA_7();
//	rev ^= rev_table_DIA_down[index_num][3] << 8;
//
//	init_index_dia_17(bk, wh);
//	index_num = GetIndexNum_DIA_17();
//	rev ^= rev_table_DIA_up[index_num][4];
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_e5(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_E(bk, wh);
//	index_num = GetIndexNum_COL_E();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][4] << 32;
//
//	init_index_row_5(bk, wh);
//	index_num = GetIndexNum_ROW_5();
//	rev ^= rev_table_ROW[index_num][4] << 4;
//
//	init_index_dia_1(bk, wh);
//	index_num = GetIndexNum_DIA_1();
//	rev ^= rev_table_DIA_down[index_num][4];
//
//	init_index_dia_18(bk, wh);
//	index_num = GetIndexNum_DIA_18();
//	rev ^= rev_table_DIA_up[index_num][3] << 8;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_e6(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_E(bk, wh);
//	index_num = GetIndexNum_COL_E();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][5] << 32;
//
//	init_index_row_6(bk, wh);
//	index_num = GetIndexNum_ROW_6();
//	rev ^= rev_table_ROW[index_num][4] << 5;
//
//	init_index_dia_2(bk, wh);
//	index_num = GetIndexNum_DIA_2();
//	rev ^= rev_table_DIA_down[index_num][4] << 1;
//
//	init_index_dia_19(bk, wh);
//	index_num = GetIndexNum_DIA_19();
//	rev ^= rev_table_DIA_up[index_num][2] << 16;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_e7(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_E(bk, wh);
//	index_num = GetIndexNum_COL_E();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][6] << 32;
//
//	init_index_row_7(bk, wh);
//	index_num = GetIndexNum_ROW_7();
//	rev ^= rev_table_ROW[index_num][4] << 6;
//
//	init_index_dia_3(bk, wh);
//	index_num = GetIndexNum_DIA_3();
//	rev ^= rev_table_DIA_down[index_num][4] << 2;
//
//	init_index_dia_20(bk, wh);
//	index_num = GetIndexNum_DIA_20();
//	rev ^= rev_table_DIA_up[index_num][1] << 24;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_e8(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_E(bk, wh);
//	index_num = GetIndexNum_COL_E();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][7] << 32;
//
//	init_index_row_8(bk, wh);
//	index_num = GetIndexNum_ROW_8();
//	rev ^= rev_table_ROW[index_num][4] << 7;
//
//	init_index_dia_4(bk, wh);
//	index_num = GetIndexNum_DIA_4();
//	rev ^= rev_table_DIA_down[index_num][4] << 3;
//
//	init_index_dia_21(bk, wh);
//	index_num = GetIndexNum_DIA_21();
//	rev ^= rev_table_DIA_up[index_num][0] << 32;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_f1(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_F(bk, wh);
//	index_num = GetIndexNum_COL_F();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][0] << 40;
//
//	init_index_row_1(bk, wh);
//	index_num = GetIndexNum_ROW_1();
//	rev ^= rev_table_ROW[index_num][5];
//
//	init_index_dia_11(bk, wh);
//	index_num = GetIndexNum_DIA_11();
//	rev ^= rev_table_DIA_down[index_num][0] << 40;
//
//	init_index_dia_15(bk, wh);
//	index_num = GetIndexNum_DIA_15();
//	rev ^= rev_table_DIA_up[index_num][5] >> 2;
//
//	return rev;
//}
//
//
//
//BitBoard GetRevFromIndex_f2(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_F(bk, wh);
//	index_num = GetIndexNum_COL_F();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][1] << 40;
//
//	init_index_row_2(bk, wh);
//	index_num = GetIndexNum_ROW_2();
//	rev ^= rev_table_ROW[index_num][5] << 1;
//
//	init_index_dia_10(bk, wh);
//	index_num = GetIndexNum_DIA_10();
//	rev ^= rev_table_DIA_down[index_num][1] << 32;
//
//	init_index_dia_16(bk, wh);
//	index_num = GetIndexNum_DIA_16();
//	rev ^= rev_table_DIA_up[index_num][5] >> 1;
//	
//	return rev;
//}
//
//BitBoard GetRevFromIndex_f3(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_F(bk, wh);
//	index_num = GetIndexNum_COL_F();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][2] << 40;
//
//	init_index_row_3(bk, wh);
//	index_num = GetIndexNum_ROW_3();
//	rev ^= rev_table_ROW[index_num][5] << 2;
//
//	init_index_dia_9(bk, wh);
//	index_num = GetIndexNum_DIA_9();
//	rev ^= rev_table_DIA_down[index_num][2] << 24;
//
//	init_index_dia_17(bk, wh);
//	index_num = GetIndexNum_DIA_17();
//	rev ^= rev_table_DIA_up[index_num][5];
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_f4(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_F(bk, wh);
//	index_num = GetIndexNum_COL_F();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][3] << 40;
//
//	init_index_row_4(bk, wh);
//	index_num = GetIndexNum_ROW_4();
//	rev ^= rev_table_ROW[index_num][5] << 3;
//
//	init_index_dia_8(bk, wh);
//	index_num = GetIndexNum_DIA_8();
//	rev ^= rev_table_DIA_down[index_num][3] << 16;
//
//	init_index_dia_18(bk, wh);
//	index_num = GetIndexNum_DIA_18();
//	rev ^= rev_table_DIA_up[index_num][4] << 8;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_f5(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_F(bk, wh);
//	index_num = GetIndexNum_COL_F();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][4] << 40;
//
//	init_index_row_5(bk, wh);
//	index_num = GetIndexNum_ROW_5();
//	rev ^= rev_table_ROW[index_num][5] << 4;
//
//	init_index_dia_7(bk, wh);
//	index_num = GetIndexNum_DIA_7();
//	rev ^= rev_table_DIA_down[index_num][4] << 8;
//
//	init_index_dia_19(bk, wh);
//	index_num = GetIndexNum_DIA_19();
//	rev ^= rev_table_DIA_up[index_num][3] << 16;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_f6(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_F(bk, wh);
//	index_num = GetIndexNum_COL_F();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][5] << 40;
//
//	init_index_row_6(bk, wh);
//	index_num = GetIndexNum_ROW_6();
//	rev ^= rev_table_ROW[index_num][5] << 5;
//
//	init_index_dia_1(bk, wh);
//	index_num = GetIndexNum_DIA_1();
//	rev ^= rev_table_DIA_down[index_num][5];
//
//	init_index_dia_20(bk, wh);
//	index_num = GetIndexNum_DIA_20();
//	rev ^= rev_table_DIA_up[index_num][2] << 24;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_f7(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_F(bk, wh);
//	index_num = GetIndexNum_COL_F();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][6] << 40;
//
//	init_index_row_7(bk, wh);
//	index_num = GetIndexNum_ROW_7();
//	rev ^= rev_table_ROW[index_num][5] << 6;
//
//	init_index_dia_2(bk, wh);
//	index_num = GetIndexNum_DIA_2();
//	rev ^= rev_table_DIA_down[index_num][5] << 1;
//
//	init_index_dia_21(bk, wh);
//	index_num = GetIndexNum_DIA_21();
//	rev ^= rev_table_DIA_up[index_num][1] << 32;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_f8(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_F(bk, wh);
//	index_num = GetIndexNum_COL_F();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][7] << 40;
//
//	init_index_row_8(bk, wh);
//	index_num = GetIndexNum_ROW_8();
//	rev ^= rev_table_ROW[index_num][5] << 7;
//
//	init_index_dia_3(bk, wh);
//	index_num = GetIndexNum_DIA_3();
//	rev ^= rev_table_DIA_down[index_num][5] << 2;
//
//	init_index_dia_22(bk, wh);
//	index_num = GetIndexNum_DIA_22();
//	rev ^= rev_table_DIA_up[index_num][0] << 40;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_g1(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_G(bk, wh);
//	index_num = GetIndexNum_COL_G();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][0] << 48;
//
//	init_index_row_1(bk, wh);
//	index_num = GetIndexNum_ROW_1();
//	rev ^= rev_table_ROW[index_num][6];
//
//	init_index_dia_16(bk, wh);
//	index_num = GetIndexNum_DIA_16();
//	rev ^= rev_table_DIA_up[index_num][6] >> 1;
//
//	return rev;
//}
//
//
//
//BitBoard GetRevFromIndex_g2(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_G(bk, wh);
//	index_num = GetIndexNum_COL_G();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][1] << 48;
//
//	init_index_row_2(bk, wh);
//	index_num = GetIndexNum_ROW_2();
//	rev ^= rev_table_ROW[index_num][6] << 1;
//
//	init_index_dia_11(bk, wh);
//	index_num = GetIndexNum_DIA_11();
//	rev ^= rev_table_DIA_down[index_num][1] << 40;
//
//	init_index_dia_17(bk, wh);
//	index_num = GetIndexNum_DIA_17();
//	rev ^= rev_table_DIA_up[index_num][6];
//	
//	return rev;
//}
//
//BitBoard GetRevFromIndex_g3(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_G(bk, wh);
//	index_num = GetIndexNum_COL_G();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][2] << 48;
//
//	init_index_row_3(bk, wh);
//	index_num = GetIndexNum_ROW_3();
//	rev ^= rev_table_ROW[index_num][6] << 2;
//
//	init_index_dia_10(bk, wh);
//	index_num = GetIndexNum_DIA_10();
//	rev ^= rev_table_DIA_down[index_num][2] << 32;
//
//	init_index_dia_18(bk, wh);
//	index_num = GetIndexNum_DIA_18();
//	rev ^= rev_table_DIA_up[index_num][5] << 8;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_g4(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_G(bk, wh);
//	index_num = GetIndexNum_COL_G();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][3] << 48;
//
//	init_index_row_4(bk, wh);
//	index_num = GetIndexNum_ROW_4();
//	rev ^= rev_table_ROW[index_num][6] << 3;
//
//	init_index_dia_9(bk, wh);
//	index_num = GetIndexNum_DIA_9();
//	rev ^= rev_table_DIA_down[index_num][3] << 24;
//
//	init_index_dia_19(bk, wh);
//	index_num = GetIndexNum_DIA_19();
//	rev ^= rev_table_DIA_up[index_num][4] << 16;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_g5(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_G(bk, wh);
//	index_num = GetIndexNum_COL_G();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][4] << 48;
//
//	init_index_row_5(bk, wh);
//	index_num = GetIndexNum_ROW_5();
//	rev ^= rev_table_ROW[index_num][6] << 4;
//
//	init_index_dia_8(bk, wh);
//	index_num = GetIndexNum_DIA_8();
//	rev ^= rev_table_DIA_down[index_num][4] << 16;
//
//	init_index_dia_20(bk, wh);
//	index_num = GetIndexNum_DIA_20();
//	rev ^= rev_table_DIA_up[index_num][3] << 24;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_g6(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_G(bk, wh);
//	index_num = GetIndexNum_COL_G();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][5] << 48;
//
//	init_index_row_6(bk, wh);
//	index_num = GetIndexNum_ROW_6();
//	rev ^= rev_table_ROW[index_num][6] << 5;
//
//	init_index_dia_7(bk, wh);
//	index_num = GetIndexNum_DIA_7();
//	rev ^= rev_table_DIA_down[index_num][5] << 8;
//
//	init_index_dia_21(bk, wh);
//	index_num = GetIndexNum_DIA_21();
//	rev ^= rev_table_DIA_up[index_num][2] << 32;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_g7(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_G(bk, wh);
//	index_num = GetIndexNum_COL_G();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][6] << 48;
//
//	init_index_row_7(bk, wh);
//	index_num = GetIndexNum_ROW_7();
//	rev ^= rev_table_ROW[index_num][6] << 6;
//
//	init_index_dia_1(bk, wh);
//	index_num = GetIndexNum_DIA_1();
//	rev ^= rev_table_DIA_down[index_num][6];
//
//	init_index_dia_22(bk, wh);
//	index_num = GetIndexNum_DIA_22();
//	rev ^= rev_table_DIA_up[index_num][1] << 40;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_g8(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_G(bk, wh);
//	index_num = GetIndexNum_COL_G();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][7] << 48;
//
//	init_index_row_8(bk, wh);
//	index_num = GetIndexNum_ROW_8();
//	rev ^= rev_table_ROW[index_num][6] << 7;
//
//	init_index_dia_2(bk, wh);
//	index_num = GetIndexNum_DIA_2();
//	rev ^= rev_table_DIA_down[index_num][6] << 1;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_h1(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_H(bk, wh);
//	index_num = GetIndexNum_COL_H();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][0] << 56;
//
//	init_index_row_1(bk, wh);
//	index_num = GetIndexNum_ROW_1();
//	rev ^= rev_table_ROW[index_num][7];
//
//	init_index_dia_17(bk, wh);
//	index_num = GetIndexNum_DIA_17();
//	rev ^= rev_table_DIA_up[index_num][7];
//
//	return rev;
//}
//
//
//
//BitBoard GetRevFromIndex_h2(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_H(bk, wh);
//	index_num = GetIndexNum_COL_H();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][1] << 56;
//
//	init_index_row_2(bk, wh);
//	index_num = GetIndexNum_ROW_2();
//	rev ^= rev_table_ROW[index_num][7] << 1;
//
//	init_index_dia_18(bk, wh);
//	index_num = GetIndexNum_DIA_18();
//	rev ^= rev_table_DIA_up[index_num][6] << 8;
//	
//	return rev;
//}
//
//BitBoard GetRevFromIndex_h3(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_H(bk, wh);
//	index_num = GetIndexNum_COL_H();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][2] << 56;
//
//	init_index_row_3(bk, wh);
//	index_num = GetIndexNum_ROW_3();
//	rev ^= rev_table_ROW[index_num][7] << 2;
//
//	init_index_dia_11(bk, wh);
//	index_num = GetIndexNum_DIA_11();
//	rev ^= rev_table_DIA_down[index_num][2] << 40;
//
//	init_index_dia_19(bk, wh);
//	index_num = GetIndexNum_DIA_19();
//	rev ^= rev_table_DIA_up[index_num][5] << 16;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_h4(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_H(bk, wh);
//	index_num = GetIndexNum_COL_H();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][3] << 56;
//
//	init_index_row_4(bk, wh);
//	index_num = GetIndexNum_ROW_4();
//	rev ^= rev_table_ROW[index_num][7] << 3;
//
//	init_index_dia_10(bk, wh);
//	index_num = GetIndexNum_DIA_10();
//	rev ^= rev_table_DIA_down[index_num][3] << 32;
//
//	init_index_dia_20(bk, wh);
//	index_num = GetIndexNum_DIA_20();
//	rev ^= rev_table_DIA_up[index_num][4] << 24;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_h5(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_H(bk, wh);
//	index_num = GetIndexNum_COL_H();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][4] << 56;
//
//	init_index_row_5(bk, wh);
//	index_num = GetIndexNum_ROW_5();
//	rev ^= rev_table_ROW[index_num][7] << 4;
//
//	init_index_dia_9(bk, wh);
//	index_num = GetIndexNum_DIA_9();
//	rev ^= rev_table_DIA_down[index_num][4] << 24;
//
//	init_index_dia_21(bk, wh);
//	index_num = GetIndexNum_DIA_21();
//	rev ^= rev_table_DIA_up[index_num][3] << 32;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_h6(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_H(bk, wh);
//	index_num = GetIndexNum_COL_H();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][5] << 56;
//
//	init_index_row_6(bk, wh);
//	index_num = GetIndexNum_ROW_6();
//	rev ^= rev_table_ROW[index_num][7] << 5;
//
//	init_index_dia_8(bk, wh);
//	index_num = GetIndexNum_DIA_8();
//	rev ^= rev_table_DIA_down[index_num][5] << 16;
//
//	init_index_dia_22(bk, wh);
//	index_num = GetIndexNum_DIA_22();
//	rev ^= rev_table_DIA_up[index_num][2] << 40;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_h7(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_H(bk, wh);
//	index_num = GetIndexNum_COL_H();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][6] << 56;
//
//	init_index_row_7(bk, wh);
//	index_num = GetIndexNum_ROW_7();
//	rev ^= rev_table_ROW[index_num][7] << 6;
//
//	init_index_dia_7(bk, wh);
//	index_num = GetIndexNum_DIA_7();
//	rev ^= rev_table_DIA_down[index_num][6] << 8;
//
//	return rev;
//}
//
//BitBoard GetRevFromIndex_h8(BitBoard bk, BitBoard wh)
//{
//	BitBoard rev;
//	int index_num;
//
//	/* インデックス番号の取得 */
//	init_index_col_H(bk, wh);
//	index_num = GetIndexNum_COL_H();
//	/* インデックス番号から反転する石のビット列を取得 */
//	rev = rev_table_COL[index_num][7] << 56;
//
//	init_index_row_8(bk, wh);
//	index_num = GetIndexNum_ROW_8();
//	rev ^= rev_table_ROW[index_num][7] << 7;
//
//	init_index_dia_1(bk, wh);
//	index_num = GetIndexNum_DIA_1();
//	rev ^= rev_table_DIA_down[index_num][7];
//
//	return rev;
//}
//
//BitBoard (*GetRevIndex[64])(BitBoard, BitBoard) = 
//{
//	GetRevFromIndex_a1,  GetRevFromIndex_a2,  GetRevFromIndex_a3,  GetRevFromIndex_a4,  
//	GetRevFromIndex_a5,  GetRevFromIndex_a6,  GetRevFromIndex_a7,  GetRevFromIndex_a8,
//
//	GetRevFromIndex_b1,  GetRevFromIndex_b2,  GetRevFromIndex_b3,  GetRevFromIndex_b4,  
//	GetRevFromIndex_b5,  GetRevFromIndex_b6,  GetRevFromIndex_b7,  GetRevFromIndex_b8,
//
//	GetRevFromIndex_c1,  GetRevFromIndex_c2,  GetRevFromIndex_c3,  GetRevFromIndex_c4,  
//	GetRevFromIndex_c5,  GetRevFromIndex_c6,  GetRevFromIndex_c7,  GetRevFromIndex_c8,
//
//	GetRevFromIndex_d1,  GetRevFromIndex_d2,  GetRevFromIndex_d3,  GetRevFromIndex_d4,  
//	GetRevFromIndex_d5,  GetRevFromIndex_d6,  GetRevFromIndex_d7,  GetRevFromIndex_d8,
//
//	GetRevFromIndex_e1,  GetRevFromIndex_e2,  GetRevFromIndex_e3,  GetRevFromIndex_e4,  
//	GetRevFromIndex_e5,  GetRevFromIndex_e6,  GetRevFromIndex_e7,  GetRevFromIndex_e8,
//
//	GetRevFromIndex_f1,  GetRevFromIndex_f2,  GetRevFromIndex_f3,  GetRevFromIndex_f4,  
//	GetRevFromIndex_f5,  GetRevFromIndex_f6,  GetRevFromIndex_f7,  GetRevFromIndex_f8,
//
//	GetRevFromIndex_g1,  GetRevFromIndex_g2,  GetRevFromIndex_g3,  GetRevFromIndex_g4,  
//	GetRevFromIndex_g5,  GetRevFromIndex_g6,  GetRevFromIndex_g7,  GetRevFromIndex_g8,
//
//	GetRevFromIndex_h1,  GetRevFromIndex_h2,  GetRevFromIndex_h3,  GetRevFromIndex_h4,  
//	GetRevFromIndex_h5,  GetRevFromIndex_h6,  GetRevFromIndex_h7,  GetRevFromIndex_h8
//};

int GetEvalFromPattern(BitBoard b_board, BitBoard w_board, int color, int stage)
{
	double eval;

	/* 現在の色とステージでポインタを指定 */
	hori_ver1 = hori_ver1_data[color][stage];
	hori_ver2 = hori_ver2_data[color][stage];
	hori_ver3 = hori_ver3_data[color][stage];
	dia_ver1 =  dia_ver1_data[color][stage];
	dia_ver2 =  dia_ver2_data[color][stage];
	dia_ver3 =  dia_ver3_data[color][stage];
	dia_ver4 =  dia_ver4_data[color][stage];
	edge =		edge_data[color][stage];
	corner5_2 = corner5_2_data[color][stage];
	corner3_3 = corner3_3_data[color][stage];
	triangle = triangle_data[color][stage];
	mobility = mobility_data[0][stage];
	parity = parity_data[0][stage];

	eval  = check_h_ver1();
	eval += check_h_ver2();
	eval += check_h_ver3();

	eval += check_dia_ver1();
	eval += check_dia_ver2();
	eval += check_dia_ver3();
	eval += check_dia_ver4();

	eval += check_edge();
	eval += check_corner5_2();
	eval += check_corner3_3();
	eval += check_triangle();

	eval += check_mobility(b_board, w_board);
	eval += constant_data[color][stage];

	eval_sum = eval;
	eval *= EVAL_ONE_STONE;

	return (int )eval;
}

int opponent_feature(int l, int d)
{
	const int o[] = {0, 2 ,1};
	int f = o[l % 3];

	if (d > 1) f += opponent_feature(l / 3, d - 1) * 3;

	return f;
}

void ReadEvalDat()
{
	HWND progress_handle = GetDlgItem(DlgHandle, IDC_PROGRESS1);
	FILE *fp;
	int stage = 0;
	int i, evalSize;
	int bufSize = (int)(1.5 * 1024 * 1024);
	char *buf = (char *)malloc(bufSize);
	char *line, *ctr;
	char file_path[32];

	float *p_table, *p_table_op;

	if(fopen_s(&fp, "src\\eval.bin", "rb") != 0){
		char msg[128];
		sprintf_s(msg, "%s を開けません。プログラムを終了します。", file_path);
		MessageBox(hWnd, msg, "評価テーブルオープンエラー", MB_OK);
		exit(0);
		//stage++;
		//continue;
	}

	PostMessage(progress_handle, PBM_SETSTEP, 12, 0);

	while(stage < 60)
	{
		//fread(buf, sizeof(char), bufSize, fp);
		evalSize = decodeEvalData((UCHAR *)buf, fp);
		line = strtok_s(buf, "\n", &ctr);

		/* horizon_ver1 */
		p_table = hori_ver1_data[0][stage];
		p_table_op = hori_ver1_data[1][stage];
		for(i = 0; i < 6561; i++)
		{
			p_table[i] = atof(line);
			/* opponent */
			p_table_op[opponent_feature(i, 8)] = -p_table[i];
			line = strtok_s(NULL, "\n", &ctr);
		}

		/* horizon_ver2 */
		p_table = hori_ver2_data[0][stage];
		p_table_op = hori_ver2_data[1][stage];
		for(i = 0; i < 6561; i++)
		{
			p_table[i] = atof(line);
			p_table_op[opponent_feature(i, 8)] = -p_table[i];
			line = strtok_s(NULL, "\n", &ctr);
		}

		/* horizon_ver3 */
		p_table = hori_ver3_data[0][stage];
		p_table_op = hori_ver3_data[1][stage];
		for(i = 0; i < 6561; i++)
		{
			p_table[i] = atof(line);
			p_table_op[opponent_feature(i, 8)] = -p_table[i];
			line = strtok_s(NULL, "\n", &ctr);
		}

		/* diagram_ver1 */
		p_table = dia_ver1_data[0][stage];
		p_table_op = dia_ver1_data[1][stage];
		for(i = 0; i < 6561; i++)
		{
			p_table[i] = atof(line);
			p_table_op[opponent_feature(i, 8)] = -p_table[i];
			line = strtok_s(NULL, "\n", &ctr);
		}

		/* diagram_ver2 */
		p_table = dia_ver2_data[0][stage];
		p_table_op = dia_ver2_data[1][stage];
		for(i = 0; i < 2187; i++)
		{
			p_table[i] = atof(line);
			p_table_op[opponent_feature(i, 7)] = -p_table[i];
			line = strtok_s(NULL, "\n", &ctr);
		}

		/* diagram_ver3 */
		p_table = dia_ver3_data[0][stage];
		p_table_op = dia_ver3_data[1][stage];
		for(i = 0; i < 729; i++)
		{
			p_table[i] = atof(line);
			p_table_op[opponent_feature(i, 6)] = -p_table[i];
			line = strtok_s(NULL, "\n", &ctr);
		}

		/* diagram_ver4 */
		p_table = dia_ver4_data[0][stage];
		p_table_op = dia_ver4_data[1][stage];
		for(i = 0; i < 243; i++)
		{
			p_table[i] = atof(line);
			p_table_op[opponent_feature(i, 5)] = -p_table[i];
			line = strtok_s(NULL, "\n", &ctr);
		}

		/* edge */
		p_table = edge_data[0][stage];
		p_table_op = edge_data[1][stage];
		for(i = 0; i < 59049; i++)
		{
			p_table[i] = atof(line);
			p_table_op[opponent_feature(i, 10)] = -p_table[i];
			line = strtok_s(NULL, "\n", &ctr);
		}

		/* corner5 + 2X */
		p_table = corner5_2_data[0][stage];
		p_table_op = corner5_2_data[1][stage];
		for (i = 0; i < 59049; i++)
		{
			p_table[i] = atof(line);
			p_table_op[opponent_feature(i, 10)] = -p_table[i];
			line = strtok_s(NULL, "\n", &ctr);
		}

		/* corner3_3 */
		p_table = corner3_3_data[0][stage];
		p_table_op = corner3_3_data[1][stage];
		for(i = 0; i < 19683; i++)
		{
			p_table[i] = atof(line);
			p_table_op[opponent_feature(i, 9)] = -p_table[i];
			line = strtok_s(NULL, "\n", &ctr);
		}

		/* triangle */
		p_table = triangle_data[0][stage];
		p_table_op = triangle_data[1][stage];
		for (i = 0; i < 59049; i++)
		{
			p_table[i] = atof(line);
			p_table_op[opponent_feature(i, 10)] = -p_table[i];
			line = strtok_s(NULL, "\n", &ctr);
		}

		/* mobility */
		p_table = mobility_data[0][stage];
		p_table_op = mobility_data[1][stage];
		for (i = 0; i < MOBILITY_NUM; i++)
		{
			p_table[i] = atof(line);
			p_table_op[i] = -p_table[i];
			line = strtok_s(NULL, "\n", &ctr);
		}

		/* parity */
		p_table = parity_data[0][stage];
		p_table_op = parity_data[1][stage];
		for (i = 0; i < PARITY_NUM; i++)
		{
			p_table[i] = atof(line);
			p_table_op[i] = -p_table[i];
			line = strtok_s(NULL, "\n", &ctr);
		}

		/* constant */
		constant_data[0][stage] = atof(line);
		line = strtok_s(NULL, "\n", &ctr);

		stage++;
		/* プログレスバーの更新 */
		PostMessage(progress_handle, PBM_STEPIT, 0, 0);
	}

	fclose(fp);
	free(buf);
}

void readMPCInfo(){

	FILE *fp;
	char filename[32];

	sprintf_s(filename, sizeof(filename), "src\\mpc.dat");
	if(fopen_s(&fp, filename, "r")){
		MessageBox(hWnd, "mpc.dat を開けません。プログラムを終了します。", 
			"MPCテーブルオープンエラー", MB_OK);
		exit(1);
	}

	memset(MPCInfo, 0, sizeof(MPCInfo));

	for(int j = 0; j < 22; j++){
		fscanf_s(fp, "%d", &(MPCInfo[j].depth));
		fscanf_s(fp, "%d", &(MPCInfo[j].offset));
		fscanf_s(fp, "%d", &(MPCInfo[j].deviation));
	}
	fclose(fp);
}

