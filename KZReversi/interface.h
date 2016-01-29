#pragma once

void DrawBoard(HDC, HWND, int, int);
BOOL CanDropDown(BitBoard, BitBoard, int x, int y);
void DropDownStone(BitBoard, BitBoard, int x, int y);
void disp_eval(void);
void disp_hint(HDC, BitBoard, BitBoard, BitBoard, int, int);
void save_undoboard(BitBoard, BitBoard, int, int, int);
void Print_HintEval(HDC, double, int, int, int);
void sort_eval_data(int);

/* インタフェース関数 */
BOOL chk_range_board(LPARAM);
void LButtonClick(HWND, HWND,LPARAM);
void RButtonClick(HWND, HWND, LPARAM);
void OnStartGame(HWND, HWND);
void OnVeasy(HWND, int);
void Oneasy(HWND, int);
void Onnormal(HWND, int);
void Onhard(HWND, int);
void OnVhard(HWND, int);
void OnUhard(HWND, int);
void OnSUhard(HWND, int);
void OnSSUhard(HWND, int);
void OnHUhard(HWND, int);
void OnUltra(HWND, int);
void OnSUltra(HWND, int);
void OnSSUltra(HWND, int);

void OnAllSearchWin(HWND, int);
void OnAllSearchSD(HWND, int);
void Edit_Board(HWND, HWND, LPARAM);
void DispWinLoss(HWND, HWND);
void UncheckedAll_BookMenu(HMENU);
/* ヒントレベル */
void OnHint_0(HWND);
void OnHint_1(HWND);
void OnHint_2(HWND);
void OnHint_3(HWND);
void OnHint_4(HWND);
void OnHint_5(HWND);
void OnHint_6(HWND);