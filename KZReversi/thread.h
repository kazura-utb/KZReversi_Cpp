#pragma once

void open_book_thread(HINSTANCE);
void Learn_Thread(void);
void GameThread(LPARAM, int);
void auto_game_thread(void);
void start_disp_hint_thread(void *);
void T_disp_hint(HDC, BitBoard, BitBoard, BitBoard);

extern HANDLE hMutex;

#define HINTONLY 2
#define LBUTTON 1
#define STARTGAME 0

/* ƒqƒ“ƒgŠÖ˜A */
extern BOOL HINT_DISPED;
extern HANDLE SIG_AI_FINISHED;
extern HANDLE SIG_HINT_FINISHED;