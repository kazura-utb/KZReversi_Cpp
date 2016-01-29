#pragma once

extern int COUNT_NO_PATTERN;
extern int COUNT_PATTERN;

extern int pos_eval[64];

/* 各座標 */
#define A1 0			/* A1 */
#define A2 1			/* A2 */
#define A3 2			/* A3 */
#define A4 3			/* A4 */
#define A5 4			/* A5 */
#define A6 5			/* A6 */
#define A7 6			/* A7 */
#define A8 7			/* A8 */

#define B1 8			/* B1 */
#define B2 9			/* B2 */
#define B3 10			/* B3 */
#define B4 11			/* B4 */
#define B5 12			/* B5 */
#define B6 13			/* B6 */
#define B7 14			/* B7 */
#define B8 15			/* B8 */

#define C1 16			/* C1 */
#define C2 17			/* C2 */
#define C3 18			/* C3 */
#define C4 19			/* C4 */
#define C5 20			/* C5 */
#define C6 21			/* C6 */
#define C7 22			/* C7 */
#define C8 23			/* C8 */

#define D1 24			/* D1 */
#define D2 25			/* D2 */
#define D3 26			/* D3 */
#define D4 27			/* D4 */
#define D5 28			/* D5 */
#define D6 29			/* D6 */
#define D7 30			/* D7 */
#define D8 31			/* D8 */

#define E1 32			/* E1 */
#define E2 33			/* E2 */
#define E3 34			/* E3 */
#define E4 35			/* E4 */
#define E5 36			/* E5 */
#define E6 37			/* E6 */
#define E7 38			/* E7 */
#define E8 39			/* E8 */

#define F1 40			/* F1 */
#define F2 41			/* F2 */
#define F3 42			/* F3 */
#define F4 43			/* F4 */
#define F5 44			/* F5 */
#define F6 45			/* F6 */
#define F7 46			/* F7 */
#define F8 47			/* F8 */

#define G1 48			/* G1 */
#define G2 49			/* G2 */
#define G3 50			/* G3 */
#define G4 51			/* G4 */
#define G5 52			/* G5 */
#define G6 53			/* G6 */
#define G7 54			/* G7 */
#define G8 55			/* G8 */

#define H1 56			/* H1 */
#define H2 57			/* H2 */
#define H3 58			/* H3 */
#define H4 59			/* H4 */
#define H5 60			/* H5 */
#define H6 61			/* H6 */
#define H7 62			/* H7 */
#define H8 63			/* H8 */

/* 評価パターンテーブル(おおもと) */
extern float hori_ver1_data[2][60][INDEX_NUM];
extern float hori_ver2_data[2][60][INDEX_NUM];
extern float hori_ver3_data[2][60][INDEX_NUM];
extern float dia_ver1_data[2][60][INDEX_NUM];
extern float dia_ver2_data[2][60][INDEX_NUM / 3];
extern float dia_ver3_data[2][60][INDEX_NUM / 9];
extern float dia_ver4_data[2][60][INDEX_NUM / 27];
extern float edge_data[2][60][INDEX_NUM * 9];
extern float edge_cor_data[2][60][INDEX_NUM * 9];
extern float corner5_2_data[2][60][INDEX_NUM * 9];
extern float corner3_3_data[2][60][INDEX_NUM * 3];
extern float triangle_data[2][60][INDEX_NUM * 9];
extern float mobility_data[2][60][MOBILITY_NUM];
extern float parity_data[2][60][PARITY_NUM];
extern float constant_data[2][60];

/* 評価パターンテーブル */
extern float *hori_ver1;
extern float *hori_ver2;
extern float *hori_ver3;
extern float *dia_ver1;
extern float *dia_ver2;
extern float *dia_ver3;
extern float *dia_ver4;
extern float *edge;
//extern float *edge_cor;
//extern float *corner4_2;
extern float *corner5_2;
extern float *corner3_3;
extern float *triangle;
extern float *mobility;
extern float *parity;

extern void ReadEvalDat(void);
extern void ReadEvalDat(int);

extern void readMPCInfo(void);

extern void init_index_board(BitBoard bk, BitBoard wh);
int GetEvalFromPattern(BitBoard, BitBoard, int, int);

unsigned long long get_some_potential_moves(const unsigned long long P, const int dir);
unsigned long long get_potential_moves(BitBoard P, BitBoard O, BitBoard blank);

double check_h_ver1();
double check_h_ver2();
double check_h_ver3();
double check_dia_ver1();
double check_dia_ver2();
double check_dia_ver3();
double check_dia_ver4();
double check_edge();
double check_corner3_3();
double check_corner5_2();
double check_triangle();
double check_mobility(BitBoard, BitBoard, int);
double check_potential_mobility(BitBoard);
double check_pality(BitBoard);

void write_h_ver1(FILE *);
void write_h_ver2(FILE *);
void write_h_ver3(FILE *);
void write_dia_ver1(FILE *);
void write_dia_ver2(FILE *);
void write_dia_ver3(FILE *);
void write_dia_ver4(FILE *);
void write_edge(FILE *);
void write_corner3_3(FILE *);
void write_corner5_2(FILE *);
void write_triangle(FILE *);
void write_pality(FILE *, BitBoard, int);