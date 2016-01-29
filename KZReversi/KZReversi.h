#ifndef REVERSI_H
#define REVERSI_H 1

/*
##########################
#
# #include
#
###########################
*/

#include "stdio.h"
#include "stdlib.h"
#include "resource.h"
#include "process.h"
#include "mmsystem.h"
#include "commctrl.h"
#include "commdlg.h" 
#include "time.h"
#include "math.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "comctl32.lib")

/*
##########################
#
# #define
#
###########################
*/

#define MASS_SIZE 32
#define MASS_NUM 8
#define BOARD_SIZE (MASS_SIZE * MASS_NUM)
#define SYSTEM_SIZE 280
#define WINDOW_W (BOARD_SIZE + SYSTEM_SIZE)
#define WINDOW_H BOARD_SIZE + 70

#define WM_SET_TEXT_BAR (WM_APP + 1)

#define NEWGAME_BT 37000
#define CONTINUE_BT 37001
#define INTERRUPT_BT 37002
#define CHANGE_BT 37003
#define FIRST_BT 37004
#define UNDO_BT 37005
#define REDO_BT 37006
#define FINAL_BT 37007
#define ID_TOOLBAR 37008

#define IDC_COM1 200
#define IDC_COM2 201
#define IDC_COM3 202
#define IDC_COM4 203

#define WIN 1
#define LOSS -1
#define DRAW 0
#define CONTINUE -2

#define NEGAMAX 2000000
#define NEGAMIN -2000000

#define BLACK 0
#define WHITE 1

#define LEN 64
#define FILE_LEN 256

#define ID_BUT1 37000 

#define FIRST_BK 34628173824
#define FIRST_WH 68853694464

/* ボードはビットボードで実装 */
typedef UINT64 BitBoard;

/* ステータスバーID */
#define Status 32

/* LLU 1 */
#define one 1ULL

#define TABLE_SIZE 2097151//500009//999983//2000003
//#define TABLE_SIZE2 500009

#define KEY_HASH_MACRO(b, w) ((b ^ ((w) >> 1)) % TABLE_SIZE)

#define INDEX_NUM 6561
#define MOBILITY_NUM 64
#define POTENTIAL_MOBILITY_NUM 60
#define PARITY_NUM 4

/* 1石あたりの評価値 */
#define EVAL_ONE_STONE 10000

#define STACK_SIZE 30

/* 着手可能数計算用 */
typedef struct {
  unsigned long high;
  unsigned long low;
} st_bit;

typedef void (*INIT_MMX)(void);
typedef int (*BIT_MOB)(st_bit, st_bit, BitBoard *);

#define MPC_DEPTH_MIN 3

/*
##########################
#
# extern
#
###########################
*/

/* ビットボード */
extern BitBoard black;
extern BitBoard white;


/* 候補手リスト */

struct MOVELIST {
	char move;
	BitBoard rev;
};

/* 置換表定義 */
struct t_table
{
	BitBoard b_board;
	BitBoard w_board;
	int lower;
	int upper;
	char movelist[2];
	char depth;
	char move_cnt;
	char sharrow_entried;
	struct t_table *nextMove;

};

/* 置換表定義 */
typedef struct
{
	char move_list[36];
	char depth;
	char move_cnt;
	char sharrow_entried;
	int lower;
	int upper;
	int index_num[8];
	int color;
}t_table2;

/* 置換表 */
//extern t_table table[TABLE_SIZE];
//extern t_table2 table2[TABLE_SIZE];

struct MPCINFO{
	int depth;
	int offset;
	int deviation;
};

/* MPC */
extern MPCINFO MPCInfo[22];

/* インデックスボード */
extern int board[64];

/* 各座標の取得 */
extern BitBoard a1;			/* a1 */
extern BitBoard a2;			/* a2 */
extern BitBoard a3;			/* a3 */
extern BitBoard a4;			/* a4 */
extern BitBoard a5;			/* a5 */
extern BitBoard a6;			/* a6 */
extern BitBoard a7;			/* a7 */
extern BitBoard a8;			/* a8 */

extern BitBoard b1;			/* b1 */
extern BitBoard b2;			/* b2 */
extern BitBoard b3;			/* b3 */
extern BitBoard b4;			/* b4 */
extern BitBoard b5;			/* b5 */
extern BitBoard b6;			/* b6 */
extern BitBoard b7;			/* b7 */
extern BitBoard b8;			/* b8 */

extern BitBoard c1;			/* c1 */
extern BitBoard c2;			/* c2 */
extern BitBoard c3;			/* c3 */
extern BitBoard c4;			/* c4 */
extern BitBoard c5;			/* c5 */
extern BitBoard c6;			/* c6 */
extern BitBoard c7;			/* c7 */
extern BitBoard c8;			/* c8 */

extern BitBoard d1;			/* d1 */
extern BitBoard d2;			/* d2 */
extern BitBoard d3;			/* d3 */
extern BitBoard d4;			/* d4 */
extern BitBoard d5;			/* d5 */
extern BitBoard d6;			/* d6 */
extern BitBoard d7;			/* d7 */
extern BitBoard d8;			/* d8 */

extern BitBoard e1;			/* e1 */
extern BitBoard e2;			/* e2 */
extern BitBoard e3;			/* e3 */
extern BitBoard e4;			/* e4 */
extern BitBoard e5;			/* e5 */
extern BitBoard e6;			/* e6 */
extern BitBoard e7;			/* e7 */
extern BitBoard e8;			/* e8 */

extern BitBoard f1;			/* f1 */
extern BitBoard f2;			/* f2 */
extern BitBoard f3;			/* f3 */
extern BitBoard f4;			/* f4 */
extern BitBoard f5;			/* f5 */
extern BitBoard f6;			/* f6 */
extern BitBoard f7;			/* f7 */
extern BitBoard f8;			/* f8 */

extern BitBoard g1;			/* g1 */
extern BitBoard g2;			/* g2 */
extern BitBoard g3;			/* g3 */
extern BitBoard g4;			/* g4 */
extern BitBoard g5;			/* g5 */
extern BitBoard g6;			/* g6 */
extern BitBoard g7;			/* g7 */
extern BitBoard g8;			/* g8 */

extern BitBoard h1;			/* h1 */
extern BitBoard h2;			/* h2 */
extern BitBoard h3;			/* h3 */
extern BitBoard h4;			/* h4 */
extern BitBoard h5;			/* h5 */
extern BitBoard h6;			/* h6 */
extern BitBoard h7;			/* h7 */
extern BitBoard h8;			/* h8 */

/* デバッグモード */
extern BOOL AI_result;
extern BOOL OnTheWay;
extern BOOL AI_thinking;

/* 各パターンのテーブルを明示 */
extern BitBoard p_pattern[72];
extern BitBoard CStone;

//extern int m_list[64];
extern int eval_list[34];

/* 探索時間 */
extern double start_t, end_t;
/* 探索ノード数 */
extern long long int COUNT_NODE, COUNT_NODE2, BASE_NODE_NUM;
extern char CountNodeTime_str[64];

/* 前回の手 */
extern int move_x, move_y;

/*CPUの手*/
extern int CPU_MOVE;
extern int print_eval_flag;

/* 手を戻す時の参照用 */
extern BitBoard UndoBoard_B[120];
extern BitBoard UndoBoard_W[120];
extern int Undo_color[120];
extern int Undo_x[120], Undo_y[120];

/* 現在のターンの色 */
extern int NowTurn;
extern char turn_str[2][4];
/* CPUの難易度 */
extern int difficult[2];
extern char Diff_Str[24];

/* ウェインドウハンドル*/
extern HWND hWnd, child_hWnd, Edit_hWnd, hRebar;
/* リストボックスハンドル */
extern HWND hListBox;
/* ツールバーハンドル */
extern HWND hToolBar;
extern HMODULE hDLL;

extern INIT_MMX init_mmx;
extern BIT_MOB bit_mob;
extern st_bit bit_b_board;
extern st_bit bit_w_board;

/* ステータスバーの分割用 */
extern HWND hStatus;
extern int SbSize[4];

/* x * MASS_NUM + y から、座標に変換 */
extern char cordinates_table[64][3];

/* 評価値表示用 */
extern double key_hori_ver1[4];
extern double key_hori_ver2[4];
extern double key_hori_ver3[4];
extern double key_dia_ver1[2];
extern double key_dia_ver2[4];
extern double key_dia_ver3[4];
extern double key_dia_ver4[4];
extern double key_edge[4];
//extern double key_edge_cor[4];
extern double key_corner5_2[8];
extern double key_corner3_3[4];
extern double key_triangle[4];
extern double key_mobility;
extern double key_parity;
extern double key_constant;
//extern double key_pot_mobility;

extern double eval_sum;
//extern double mobility;

extern int NowStage;

extern int MAX_MOVE;

/*
##########################
#
# func
#
###########################
*/

/* 初期化関連 */
void InitSystem(void);
void InitBoard(void);
void ClearListBoxItem(int);

/* 置換表関連 */
void init_substitution_table(struct Hash *hash);
void init_substitution_table_eval(struct Hash *hash);
void init_substitution_table_winloss(struct Hash *hash);
void init_substitution_table_exact(struct Hash *hash);

/* 画面描写関数 */
HWND CreateStatusBar(HWND);
HWND MakeMyRebar(HWND);
HWND MakeDebugWindow(HWND, LPARAM, WNDCLASS* );
HWND MakeEditBox(HWND, LPARAM, WNDCLASS* );
HWND MakeBoardWindow(HWND);
HWND CreateListBox(HWND);
HWND CreateComBox1(HWND, int, int, int);
HWND CreateComBox2(HWND, int, int, int);

void auto_game(void *);
void cpy_stack(int[], int[], int);
void print_line(int[]);

void read_eval_table(int);

int n_move_expantion(BitBoard, BitBoard, int);
BitBoard CreateMoves(BitBoard, BitBoard, int *);

int CountBit2(int[], int);
int CountBit(BitBoard);
int CountPotMob(BitBoard, BitBoard);

void configCPU(int);

#endif