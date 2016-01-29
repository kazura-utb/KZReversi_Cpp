#pragma once

extern BOOL UseBook;

extern BOOL b_Flag_not_change;
extern BOOL b_Flag_little_change;
extern BOOL b_Flag_change;

extern int max_change_num[2];

#define BOOK_MOVE NEGAMAX + 1

/* ローディング画面 */
extern HWND DlgHandle;
INT_PTR CALLBACK	load_books(HWND, UINT, WPARAM, LPARAM);

/* デバッグ用フラグ */
//int book_name_flag;

/* 指し手の回転・対称変換フラグ */
extern int TRANCE_MOVE;

typedef UINT64 BitBoard;

typedef struct node
{
	struct node *child;
	struct node *next;
	BitBoard bk;
	BitBoard wh;
	int eval;
	short move;
	short depth;
}BooksNode;

/* 前回の定石手を保存しておく */

extern BooksNode *BeforeNode;
extern BooksNode *Undo_Node[120];

extern BooksNode book_tree;

void open_book(void);
int search_books(BooksNode *, BitBoard, BitBoard, int);
BooksNode *search_book_info(BooksNode *, BooksNode *, BitBoard, BitBoard, int);