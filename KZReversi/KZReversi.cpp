/*#
# アプリケーション、データの初期化機能を提供します
#
###################################################
*/

// Reversi.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"


#include "KZReversi.h"
#include "rotate_and_symmetry.h"
#include "ordering.h"
#include "AI.h"
#include "hint.h"
#include "interface.h"
#include "game.h"
#include "book.h"
#include "GetPattern.h"
#include "thread.h"

#include "intrin.h"

BitBoard black;
BitBoard white;

int Interrupt_flag;
int SbSize[4];

char turn_str[2][4] = { "●", "○" };
char cordinates_table[64][3];

TBBUTTON tbb[] =
{
	{ 0, NEWGAME_BT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{ 1, CONTINUE_BT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{ 2, INTERRUPT_BT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{ 3, CHANGE_BT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{ 4, FIRST_BT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{ 5, UNDO_BT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{ 6, REDO_BT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{ 7, FINAL_BT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 }
};

TBBUTTON tb = { 0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0 };

int com_id[4] = { IDC_COM1, IDC_COM2, IDC_COM3, IDC_COM4 };

/* ウェインドウハンドル*/
HWND hWnd, child_hWnd, Edit_hWnd, hRebar;
HWND hComBox1, hComBox2, hComBox3, hComBox4;
HWND hListBox;
HWND DlgHandle;
HWND hStatus;
HWND hToolBar;

HMODULE hDLL;


INIT_MMX init_mmx;
BIT_MOB bit_mob;

st_bit bit_b_board;
st_bit bit_w_board;

#define MAX_LOADSTRING 100


// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{

	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: ここにコードを挿入してください。

	InitSystem();
	InitBoard();

	/*
	FFO Endgame Suite(http://radagast.se/othello/ffotest.html) のベンチマーク結果
	普通にプレイする分には、ここをいじらないでください。
	ここのコメントアウトを適宜外せば、ベンチマークできます。
	*/

	AI_result = TRUE;
	ghash = NULL;

	//ベンチマーク時は、以下のコメントアウトを外す
	/* 上：勝敗探索 下:石差探索 */
	//m_FlagSearchWin = TRUE;
	//m_FlagPerfectSearch = TRUE;

	//環境:CPU AMD Phenom(tm)Ⅱ X6 1045T 2.70GHz (6コアCPU)

	//FFO#40(black to move) (WinLoss:(a2:WIN) 0.170sec Exact:(a2:+38) 1.739sec)
	/* 以下3行をコメントアウト */
	//black = 9158069842325798912;
	//white = 11047339776155165;
	//CpuColor = BLACK;

	//FFO#41(black to move) (WinLoss:(h4:DRAW) 3.254sec Exact:(h4:+0) 3.212sec)
	//black = 616174399789064;
	//white = 39493460025648416;
	//NowTurn = BLACK;

	//FFO#42(black to move) (WinLoss:(g2:WIN) 2.185sec Exact:(g2:+6) 4.554sec)
	//white = 9091853944868375556;
	//black = 22586176447709200;
	//NowTurn = BLACK;

	//FFO#43(white to move) (WinLoss:(c7:LOSS) 0.998sec Exact:(c7:-12) 9.559sec)
	//black = 38808086923902976;
	//white = 13546258740034592;
	//NowTurn = WHITE;

	//FFO#44(white to move) (WinLoss:(d2:LOSS) 1.639sec  Exact:(d2:-14) 4.045ec)
	//black = 2494790880993312;
	//white = 1010251075753548824;
	//NowTurn = WHITE;

	//FFO#45(black to move) (WinLoss:(b2:WIN) 1.700sec Exact:(b2:+6) 36.337sec)
	/*black = 282828816915486;
	white = 9287318235258944;
	NowTurn = BLACK;*/

	//FFO#46(black to move) (WinLoss:(b7:LOSS) 3.782sec Exact:(b3:-8) 16.330sec)
	//black = 4052165999611379712;
	//white = 36117299622447104;
	//NowTurn = BLACK;

	//FFO#47(white to move) (WinLoss:(g2:WIN) 1.934sec: Exact:(g2:+4) 6.752sec)
	//black = 277938752194568;
	//white = 3536224466208;
	//NowTurn = WHITE;

	//FFO#48(white to move) (WinLoss:(f6:WIN) 1.096sec: Exact:(f6:+28) 55.048sec)
	/*black = 38519958422848574;
	white = 4725679339520;
	NowTurn = WHITE;*/

	//FFO#49(black to move) (WinLoss:(e1:WIN) 9.998sec: Exact:(e1:+16) 84.654sec)
	//black = 5765976742297600;
	//white = 4253833575484;
	//NowTurn = BLACK;

	//FFO#50(black to move) (WinLoss:(d8:WIN) 5.005sec: Exact:(d8:+10) 264.836sec)
	//black = 4504145659822080;
	//white = 4336117619740130304;
	//NowTurn = BLACK;

	//FFO#51(white to move) (WinLoss:(e2:WIN) 7.185sec: Exact:(e2:+6) 72.105sec)
	//black = 349834415978528;
	//white = 8664011788383158280;
	//NowTurn = WHITE;

	//FFO#52(white to move) (WinLoss:(a3:DRAW) 131.881sec: Exact:(a3:+0) 160.600sec)
	//black = 9096176176681728056;
	//white = 35409824317440;
	//NowTurn = WHITE;

	//FFO#53(black to move) (WinLoss:(d8:LOSS) 1281.192sec: Exact:(d8:-2) 3178.53sec)
	//black = 2515768979493888;
	//white = 8949795312300457984;
	//NowTurn = BLACK;

	//FFO#59(black to move) (WinLoss:(g8:WIN) 0.32sec: Exact:(g8:+64) 0.34sec)
	/*black = 17320879491911778304;
	white = 295223649004691488;
	NowTurn = BLACK;*/


	/* これ以降は一晩経っても終わらんので登録していない */
	/*-----------------------------------------------------------------------------------*/

	//FFO#54(black to move) (WinLoss:(WIN) 8.79sec: Exact:(g2:+4) 22.5sec)
	//black = 277938752194568;
	//white = 3536224466208;
	//NowTurn = BLACK;

	//FFO#55(white to move) (WinLoss:(WIN) 8.79sec: Exact:(g2:+4) 22.5sec)
	/*black = 4635799596172290;
	white = 289361502099486840;
	NowTurn = WHITE;
	*/
	//FFO#56(white to move) (WinLoss:(WIN) 8.79sec: Exact:(g2:+4) 22.5sec)
	/*black = 4925086697193472;
	white = 9007372734053408;
	NowTurn = WHITE;*/

	//FFO#57(black to move) (WinLoss:(WIN) 8.79sec: Exact:(g2:+4) 22.5sec)
	//black = 9060166336512000;
	//white = 8943248156475301888;
	//NowTurn = BLACK;

	//FFO#58(black to move) (WinLoss:(WIN) 8.79sec: Exact:(g2:+4) 22.5sec)
	//black = 4636039783186432;
	//white = 3383245044333600;
	//NowTurn = BLACK;

	MSG msg;
	HACCEL hAccelTable;

	// グローバル文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_REVERSI, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_REVERSI));

	// メイン メッセージ ループ:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
//  コメント:
//
//    この関数および使い方は、'RegisterClassEx' 関数が追加された
//    Windows 95 より前の Win32 システムと互換させる場合にのみ必要です。
//    アプリケーションが、関連付けられた
//    正しい形式の小さいアイコンを取得できるようにするには、
//    この関数を呼び出してください。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;

	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_REVERSI));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_REVERSI);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,
		CW_USEDEFAULT, 0, WINDOW_W, WINDOW_H + 46, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	long WinVal = GetWindowLong(hWnd, GWL_STYLE);
	SetWindowLong(hWnd, GWL_STYLE, WinVal - WS_MAXIMIZEBOX);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:  メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	static HWND boardWnd, hBut[2];       // レバーコントロールのハンドル
	INITCOMMONCONTROLSEX ic;  // INITCOMMONCONTROLSEX構造体
	REBARBANDINFO rbBand;     // REBARBANDINFO構造体
	switch (message)
	{
	case WM_CREATE:
		/* ボード表示ウィンドウの作成 */
		//boardWnd = MakeBoardWindow(hWnd);
		/* レバーコントロールの作成 */
		ic.dwSize = sizeof(INITCOMMONCONTROLSEX);
		ic.dwICC = ICC_COOL_CLASSES;
		InitCommonControlsEx(&ic);
		hRebar = MakeMyRebar(hWnd);
		hListBox = CreateListBox(hWnd);

		ZeroMemory(&rbBand, sizeof(REBARBANDINFO));
		rbBand.cbSize = sizeof(REBARBANDINFO);
		rbBand.fMask = RBBIM_TEXT | RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE;
		rbBand.fStyle = RBBS_CHILDEDGE | RBBS_NOGRIPPER;
		rbBand.cxMinChild = 90;
		rbBand.cyMinChild = 25;

		rbBand.hwndChild = CreateWindow("BUTTON", "新規対局", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0, hRebar, (HMENU)NEWGAME_BT, hInst, NULL);
		SendMessage(hRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);

		rbBand.hwndChild = CreateWindow("BUTTON", "継続対局", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0, hRebar, (HMENU)CONTINUE_BT, hInst, NULL);
		SendMessage(hRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);

		rbBand.hwndChild = CreateWindow("BUTTON", "中断", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0, hRebar, (HMENU)INTERRUPT_BT, hInst, NULL);
		rbBand.cxMinChild = 60;
		SendMessage(hRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);

		rbBand.hwndChild = CreateWindow("BUTTON", "手番変更", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0, hRebar, (HMENU)CHANGE_BT, hInst, NULL);
		rbBand.cxMinChild = 90;
		SendMessage(hRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);

		rbBand.cxMinChild = 30;
		rbBand.hwndChild = CreateWindow("BUTTON", "<<", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0, hRebar, (HMENU)FIRST_BT, hInst, NULL);
		SendMessage(hRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);
		rbBand.hwndChild = CreateWindow("BUTTON", "←", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0, hRebar, (HMENU)UNDO_BT, hInst, NULL);
		SendMessage(hRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);

		rbBand.hwndChild = CreateWindow("BUTTON", "→", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0, hRebar, (HMENU)REDO_BT, hInst, NULL);
		SendMessage(hRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);

		rbBand.hwndChild = CreateWindow("BUTTON", ">>", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0, hRebar, (HMENU)FINAL_BT, hInst, NULL);
		SendMessage(hRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);

		hComBox1 = CreateComBox1(hWnd, BOARD_SIZE + 10, 90, 0);
		SendMessage(hComBox1, CB_SETCURSEL, 0, 0);
		hComBox2 = CreateComBox1(hWnd, BOARD_SIZE + 10, 120, 1);
		SendMessage(hComBox2, CB_SETCURSEL, 1, 0);
		hComBox3 = CreateComBox2(hWnd, BOARD_SIZE + 100, 90, 2);
		ShowWindow(hComBox3, SW_HIDE);
		hComBox4 = CreateComBox2(hWnd, BOARD_SIZE + 100, 120, 3);
		player[0] = HUMAN;
		player[1] = CPU;
		/* 初期難易度 */
		Onnormal(hWnd, BLACK);
		Onnormal(hWnd, WHITE);

		ShowWindow(hListBox, SW_SHOW);	//リストボックスを表示
		UpdateWindow(hWnd);
		UpdateWindow(hRebar);

		//srand((unsigned int)time(NULL));
		/* ステータスバー作成 */
		hStatus = CreateStatusBar(hWnd);

		/* 定石データオープン */
		open_book_thread(hInst);

		break;
	case WM_LBUTTONDOWN:
		/* 左クリックイベント 打てるマスをクリックすればスレッドが起動 */
		if (!AI_THINKING && chk_range_board(lParam))
		{
			Flag_Abort = TRUE;
			WaitForSingleObject(SIG_HINT_FINISHED, INFINITE);
			Flag_Abort = FALSE;
			GameThread(lParam, LBUTTON);
		}
		SendMessage(hStatus, SB_SETTEXT, (WPARAM)1 | 0, (LPARAM)turn_str[NowTurn]);
		return 0;
	case WM_RBUTTONDOWN:
		RButtonClick(hWnd, hStatus, lParam);
		return 0;
	case WM_SET_TEXT_BAR:
		SendMessage(hStatus, SB_SETTEXT, (WPARAM)0 | 3, (LPARAM)lParam);
		return 0;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
			/* コンボボックス */
		case IDC_COM1:
			/* 変更された */
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				int index = SendMessage(hComBox1, CB_GETCURSEL, 0, 0);
				/* 人間を選択 */
				if (index == 0)
				{
					player[0] = HUMAN;
					CpuColor = WHITE;
					ShowWindow(hComBox3, SW_HIDE);
				}
				/* CPUを選択 */
				else
				{
					player[0] = CPU;
					CpuColor = BLACK;
					ShowWindow(hComBox3, SW_SHOW);
				}
			}
			break;
		case IDC_COM2:
			/* 変更された */
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				int index = SendMessage(hComBox2, CB_GETCURSEL, 0, 0);
				/* 人間を選択 */
				if (index == 0)
				{
					player[1] = HUMAN;
					CpuColor = BLACK;
					ShowWindow(hComBox4, SW_HIDE);
				}
				/* CPUを選択 */
				else
				{
					player[1] = CPU;
					CpuColor = WHITE;
					ShowWindow(hComBox4, SW_SHOW);
				}
			}
			break;
		case IDC_COM3:
			/* 変更された */
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				int index = SendMessage(hComBox3, CB_GETCURSEL, 0, 0);
				/* 難易度 */
				switch (index)
				{
				case 0:
					OnVeasy(hWnd, BLACK);
					break;
				case 1:
					Oneasy(hWnd, BLACK);
					break;
				case 2:
					Onnormal(hWnd, BLACK);
					break;
				case 3:
					Onhard(hWnd, BLACK);
					break;
				case 4:
					OnVhard(hWnd, BLACK);
					break;
				case 5:
					OnUhard(hWnd, BLACK);
					break;
				case 6:
					OnSUhard(hWnd, BLACK);
					break;
				case 7:
					OnSSUhard(hWnd, BLACK);
					break;
				case 8:
					OnHUhard(hWnd, BLACK);
					break;
				case 9:
					OnUltra(hWnd, BLACK);
					break;
				case 10:
					OnSUltra(hWnd, BLACK);
					break;
				case 11:
					OnSSUltra(hWnd, BLACK);
					break;
				case 12:
					OnAllSearchWin(hWnd, BLACK);
					break;
				case 13:
					OnAllSearchSD(hWnd, BLACK);
					break;
				}
			}
			break;
		case IDC_COM4:
			/* 変更された */
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				int index = SendMessage(hComBox4, CB_GETCURSEL, 0, 0);
				/* 難易度 */
				switch (index)
				{
				case 0:
					OnVeasy(hWnd, WHITE);
					break;
				case 1:
					Oneasy(hWnd, WHITE);
					break;
				case 2:
					Onnormal(hWnd, WHITE);
					break;
				case 3:
					Onhard(hWnd, WHITE);
					break;
				case 4:
					OnVhard(hWnd, WHITE);
					break;
				case 5:
					OnUhard(hWnd, WHITE);
					break;
				case 6:
					OnSUhard(hWnd, WHITE);
					break;
				case 7:
					OnSSUhard(hWnd, WHITE);
					break;
				case 8:
					OnHUhard(hWnd, WHITE);
					break;
				case 9:
					OnUltra(hWnd, WHITE);
					break;
				case 10:
					OnSUltra(hWnd, WHITE);
					break;
				case 11:
					OnSSUltra(hWnd, WHITE);
					break;
				case 12:
					OnAllSearchWin(hWnd, WHITE);
					break;
				case 13:
					OnAllSearchSD(hWnd, WHITE);
					break;
				}
			}
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case NEWGAME_BT:
		case StartNew:
			InitBoard();
			/* 置換表の取得 */
			HashDelete(ghash);
			ghash = HashNew(21);
			HashClear(ghash);
			/* 再描写 */
			hdc = GetDC(hWnd);
			DrawBoard(hdc, hStatus, -1, -1);
			ReleaseDC(hWnd, hdc);
			GameThread(lParam, STARTGAME);
			return DefWindowProc(hWnd, message, wParam, lParam);
		case CONTINUE_BT:
			/* 置換表の取得 */
			if (ghash == NULL){
				HashDelete(ghash);
				ghash = HashNew(21);
				HashClear(ghash);
			}
			if (NowTurn == BLACK)
			{
				/* 再描写 */
				hdc = GetDC(hWnd);
				DrawBoard(hdc, hStatus, -1, -1);
				ReleaseDC(hWnd, hdc);
				NowTurn = BLACK;
				GameThread(lParam, STARTGAME);
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			else
			{
				hdc = GetDC(hWnd);
				DrawBoard(hdc, hStatus, -1, -1);
				ReleaseDC(hWnd, hdc);
				NowTurn = WHITE;
				GameThread(lParam, STARTGAME);
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		case StartContinue_B:
			/* 再描写 */
			hdc = GetDC(hWnd);
			DrawBoard(hdc, hStatus, -1, -1);
			ReleaseDC(hWnd, hdc);
			NowTurn = BLACK;
			GameThread(lParam, STARTGAME);
			return DefWindowProc(hWnd, message, wParam, lParam);
		case StartContinue_W:
			hdc = GetDC(hWnd);
			DrawBoard(hdc, hStatus, -1, -1);
			ReleaseDC(hWnd, hdc);
			NowTurn = WHITE;
			GameThread(lParam, STARTGAME);
			return DefWindowProc(hWnd, message, wParam, lParam);
			/*case Veasy:
			Force_SearchWin = FALSE;
			Force_SearchSD = FALSE;
			OnVeasy(hWnd);
			break;
			case easy:
			Force_SearchWin = FALSE;
			Force_SearchSD = FALSE;
			Oneasy(hWnd);
			break;
			case normal:
			Force_SearchWin = FALSE;
			Force_SearchSD = FALSE;
			Onnormal(hWnd);
			break;
			case hard:
			Force_SearchWin = FALSE;
			Force_SearchSD = FALSE;
			Onhard(hWnd);
			break;
			case Vhard:
			Force_SearchWin = FALSE;
			Force_SearchSD = FALSE;
			OnVhard(hWnd);
			break;
			case Uhard:
			Force_SearchWin = FALSE;
			Force_SearchSD = FALSE;
			OnUhard(hWnd);
			break;
			case AllWinLoss:
			Force_SearchWin = TRUE;
			OnAllSearchWin(hWnd);
			break;
			case AllStoneDiff:
			Force_SearchSD = TRUE;
			Force_SearchWin = FALSE;
			OnAllSearchSD(hWnd);
			break;*/
		case book_switch:
			if (!UseBook)
			{
				UseBook = TRUE;
				m_FlagBook = TRUE;
				HMENU hMenu = GetMenu(hWnd);
				CheckMenuItem(hMenu, book_switch, MFS_CHECKED);
			}
			else
			{
				UseBook = FALSE;
				m_FlagBook = FALSE;
				HMENU hMenu = GetMenu(hWnd);
				CheckMenuItem(hMenu, book_switch, MFS_UNCHECKED);
			}
			break;
		case book_best:
			if (!b_Flag_not_change)
			{
				HMENU hMenu = GetMenu(hWnd);
				UncheckedAll_BookMenu(hMenu);
				b_Flag_not_change = TRUE;
				CheckMenuItem(hMenu, book_best, MFS_CHECKED);
			}
			break;
		case book_little_change:
			if (!b_Flag_little_change)
			{
				HMENU hMenu = GetMenu(hWnd);
				UncheckedAll_BookMenu(hMenu);
				b_Flag_little_change = TRUE;
				CheckMenuItem(hMenu, book_little_change, MFS_CHECKED);
			}
			break;
		case book_change:
			if (!b_Flag_change)
			{
				HMENU hMenu = GetMenu(hWnd);
				UncheckedAll_BookMenu(hMenu);
				b_Flag_change = TRUE;
				CheckMenuItem(hMenu, book_change, MFS_CHECKED);
			}
			break;
		case book_random:
		{
			HMENU hMenu = GetMenu(hWnd);
			UncheckedAll_BookMenu(hMenu);
			CheckMenuItem(hMenu, book_random, MFS_CHECKED);
		}
		break;
		/*case CorrectSt:
		PrintCorrectSt(hWnd);
		return DefWindowProc(hWnd, message, wParam, lParam);*/
		case rotate90:
			m_FlagInGame = FALSE;
			black = rotate_90(black);
			white = rotate_90(white);
			{
				BitBoard move = one << (move_x * 8 + move_y);
				move = rotate_90(move);
				int pos = CountBit((move & (-(signed long long int)move)) - 1);
				move_x = pos / 8;
				move_y = pos % 8;
			}
			break;
		case rotate180:
			m_FlagInGame = FALSE;
			black = rotate_180(black);
			white = rotate_180(white);
			{
				BitBoard move = one << (move_x * 8 + move_y);
				move = rotate_180(move);
				int pos = CountBit((move & (-(signed long long int)move)) - 1);
				move_x = pos / 8;
				move_y = pos % 8;
			}
			break;
		case rotate270:
			m_FlagInGame = FALSE;
			black = rotate_270(black);
			white = rotate_270(white);
			{
				BitBoard move = one << (move_x * 8 + move_y);
				move = rotate_270(move);
				int pos = CountBit((move & (-(signed long long int)move)) - 1);
				move_x = pos / 8;
				move_y = pos % 8;
			}
			break;
		case symX:
			m_FlagInGame = FALSE;
			black = symmetry_x(black);
			white = symmetry_x(white);
			{
				BitBoard move = one << (move_x * 8 + move_y);
				move = symmetry_x(move);
				int pos = CountBit((move & (-(signed long long int)move)) - 1);
				move_x = pos / 8;
				move_y = pos % 8;
			}
			break;
		case symY:
			m_FlagInGame = FALSE;
			black = symmetry_y(black);
			white = symmetry_y(white);
			{
				BitBoard move = one << (move_x * 8 + move_y);
				move = symmetry_y(move);
				int pos = CountBit((move & (-(signed long long int)move)) - 1);
				move_x = pos / 8;
				move_y = pos % 8;
			}
			break;
		case symBL:
			m_FlagInGame = FALSE;
			black = symmetry_b(black);
			white = symmetry_b(white);
			{
				BitBoard move = one << (move_x * 8 + move_y);
				move = symmetry_b(move);
				int pos = CountBit((move & (-(signed long long int)move)) - 1);
				move_x = pos / 8;
				move_y = pos % 8;
			}
			break;
		case symWL:
			m_FlagInGame = FALSE;
			black = symmetry_w(black);
			white = symmetry_w(white);
			{
				BitBoard move = one << (move_x * 8 + move_y);
				move = symmetry_w(move);
				int pos = CountBit((move & (-(signed long long int)move)) - 1);
				move_x = pos / 8;
				move_y = pos % 8;
			}
			break;
		case BoardInit:
			InitBoard();
			{
				/* メニューバーの各項目の有効無効化 */
				HMENU hMenu = GetMenu(hWnd);
				EnableMenuItem(hMenu, interrupt, MFS_GRAYED);
				EnableMenuItem(hMenu, EditBoard, MFS_ENABLED);
				EnableMenuItem(hMenu, ChangeColor, MFS_ENABLED);
			}
			break;
		case EditBoard:
			Flag_Edit = TRUE;
			break;
		case INTERRUPT_BT:
		case interrupt:
			if (AI_THINKING || m_FlagHint)
			{
				Flag_Abort = TRUE;
			}
			else
			{
				Interrupt_flag = TRUE;
				m_FlagInterrupt = TRUE;
				m_FlagInGame = FALSE;
			}
			{
				HMENU hMenu = GetMenu(hWnd);
				EnableMenuItem(hMenu, interrupt, MFS_GRAYED);
				hMenu = GetMenu(hWnd);
				EnableMenuItem(hMenu, EditBoard, MFS_ENABLED);
				EnableMenuItem(hMenu, ChangeColor, MFS_ENABLED);
			}
			break;
		case CHANGE_BT:
		case ChangeColor:
			CpuColor ^= 1;
			break;
		case print_result:
			if (!AI_result)
			{
				AI_result = TRUE;
				HMENU hMenu = GetMenu(hWnd);
				CheckMenuItem(hMenu, print_result, MFS_CHECKED);
			}
			else
			{
				AI_result = FALSE;
				HMENU hMenu = GetMenu(hWnd);
				CheckMenuItem(hMenu, print_result, MFS_UNCHECKED);
			}
			break;
		case print_AI_moves:
			/*if(!AI_thinking)
			{
			AI_thinking = TRUE;
			HMENU hMenu = GetMenu(hWnd);
			CheckMenuItem(hMenu, print_AI_moves, MFS_CHECKED);
			}
			else
			{
			AI_thinking = FALSE;
			HMENU hMenu = GetMenu(hWnd);
			CheckMenuItem(hMenu, print_AI_moves, MFS_UNCHECKED);
			}*/

			break;
		case print_thinking:
			/*	if(!OnTheWay)
			{
			OnTheWay = TRUE;
			HMENU hMenu = GetMenu(hWnd);
			CheckMenuItem(hMenu, print_thinking, MFS_CHECKED);
			}
			else
			{
			OnTheWay = FALSE;
			HMENU hMenu = GetMenu(hWnd);
			CheckMenuItem(hMenu, print_thinking, MFS_UNCHECKED);
			}*/
			break;
		case Auto_Learn:
			//InitBoard();
			//{
			//	int i = 0;
			//	while (i < 1)
			//	{
			//		Learn_Thread();
			//		//Start_Auto_Learn(4, 13, 100000, -1);
			//		i++;
			//	}
			//}
			break;
		case 32849:
		{
			//Learn_Thread();
		}
		break;
		case auto_g:
			m_FlagInterrupt = FALSE;
			Interrupt_flag = FALSE;
			auto_game_thread();
			break;
		case UNDO_BT:
		case Undo:
			if (turn == 0 || (UndoBoard_B[turn - 1] == 0 && UndoBoard_W[turn - 1] == 0))
			{
				break;
			}
			m_FlagInGame = FALSE;
			if (AI_THINKING || m_FlagHint)
			{
				Flag_Abort = TRUE;
			}
			/* ヒント用の評価データ破棄 */
			{
				for (int i = 0; i < 32; i++){ EvalData[i] = NEGAMAX; }
			}
			black = UndoBoard_B[turn - 1];
			white = UndoBoard_W[turn - 1];
			move_x = Undo_x[turn - 1];
			move_y = Undo_y[turn - 1];
			NowTurn = Undo_color[turn - 1];

			turn--;
			SendMessage(hListBox, LB_SETCURSEL, turn - 1, 0);
			if (turn - 1 < 0 || (UndoBoard_B[turn - 1] == 0 && UndoBoard_W[turn - 1] == 0))
			{
				EnableMenuItem(GetMenu(hWnd), Undo, MFS_GRAYED);
				EnableWindow(GetDlgItem(hRebar, UNDO_BT), FALSE);
			}
			if (!(UndoBoard_B[turn + 1] == 0 && UndoBoard_W[turn + 1] == 0) || turn + 1 <= 60)
			{
				EnableMenuItem(GetMenu(hWnd), Redo, MFS_ENABLED);
				EnableWindow(GetDlgItem(hRebar, REDO_BT), TRUE);
			}
			break;
		case REDO_BT:
		case Redo:
			if (turn == 60 || UndoBoard_B[turn + 1] == 0 && UndoBoard_W[turn + 1] == 0)
			{
				break;
			}
			m_FlagInGame = FALSE;
			/* ヒント用の評価データ破棄 */
			{
				for (int i = 0; i < 32; i++){ EvalData[i] = NEGAMAX; }
			}
			SendMessage(hListBox, LB_SETCURSEL, turn, 0);
			black = UndoBoard_B[turn + 1];
			white = UndoBoard_W[turn + 1];
			move_x = Undo_x[turn + 1];
			move_y = Undo_y[turn + 1];
			NowTurn = Undo_color[turn + 1];
			turn++;
			if ((UndoBoard_B[turn + 1] == 0 && UndoBoard_W[turn + 1] == 0) || turn + 1 > 60)
			{
				EnableMenuItem(GetMenu(hWnd), Redo, MFS_GRAYED);
				EnableWindow(GetDlgItem(hRebar, REDO_BT), FALSE);
			}
			if (turn - 1 >= 0 || !(UndoBoard_B[turn - 1] == 0 && UndoBoard_W[turn - 1] == 0))
			{
				EnableMenuItem(GetMenu(hWnd), Undo, MFS_ENABLED);
				EnableWindow(GetDlgItem(hRebar, UNDO_BT), TRUE);
			}
			break;
		case FIRST_BT:
		{
			int i;
			for (i = 0; i < 60; i++)
			{
				if (UndoBoard_B[i] != 0 || UndoBoard_W[i] != 0)
				{
					black = UndoBoard_B[i];
					white = UndoBoard_W[i];
					move_x = Undo_x[i];
					move_y = Undo_y[i];
					NowTurn = Undo_color[i];
					turn = i;
					/* ヒント用の評価データ破棄 */
					for (i = 0; i < 32; i++){ EvalData[i] = NEGAMAX; }
					m_FlagInGame = FALSE;
					break;
				}
			}
		}
		break;
		case FINAL_BT:
		{
			int i;
			for (i = 60; i >= 0; i--)
			{
				if (UndoBoard_B[i] != 0 || UndoBoard_W[i] != 0)
				{
					black = UndoBoard_B[i];
					white = UndoBoard_W[i];
					move_x = Undo_x[i];
					move_y = Undo_y[i];
					NowTurn = Undo_color[i];
					turn = i;
					/* ヒント用の評価データ破棄 */
					for (i = 0; i < 32; i++){ EvalData[i] = NEGAMAX; }
					m_FlagInGame = FALSE;
					break;
				}
			}
		}
		break;
		case Hint_0:
			OnHint_0(hWnd);
			HINT_DISPED = FALSE;
			GameThread(0, HINTONLY);
			break;
		case Hint_1:
			OnHint_1(hWnd);
			HINT_DISPED = FALSE;
			GameThread(0, HINTONLY);
			break;
		case Hint_2:
			OnHint_2(hWnd);
			HINT_DISPED = FALSE;
			GameThread(0, HINTONLY);
			break;
		case Hint_3:
			OnHint_3(hWnd);
			HINT_DISPED = FALSE;
			GameThread(0, HINTONLY);
			break;
		case Hint_4:
			OnHint_4(hWnd);
			HINT_DISPED = FALSE;
			GameThread(0, HINTONLY);
			break;
		case Hint_5:
			OnHint_5(hWnd);
			HINT_DISPED = FALSE;
			GameThread(0, HINTONLY);
			break;
		case Hint_6:
			OnHint_6(hWnd);
			HINT_DISPED = FALSE;
			GameThread(0, HINTONLY);
			break;
		case print_eval:
			//read_eval_table(CountBit(black | white) - 4);
			if (turn == 0){
				break;
			}
			//read_eval_table(turn - 1);
			if (NowTurn == BLACK){
				init_index_board(black, white);
				GetEvalFromPattern(black, white, turn - 1, NowTurn);
			}
			else{
				init_index_board(white, black);
				GetEvalFromPattern(white, black, turn - 1, NowTurn);
			}

			{
				char eval_msg[51][64];
				sprintf_s(eval_msg[0], "hori_ver1_1 = %f, ", key_hori_ver1[0]);
				sprintf_s(eval_msg[1], "hori_ver1_2 = %f\n", key_hori_ver1[1]);
				sprintf_s(eval_msg[2], "hori_ver1_3 = %f, ", key_hori_ver1[2]);
				sprintf_s(eval_msg[3], "hori_ver1_4 = %f\n", key_hori_ver1[3]);
				sprintf_s(eval_msg[4], "hori_ver2_1 = %f, ", key_hori_ver2[0]);
				sprintf_s(eval_msg[5], "hori_ver2_2 = %f\n", key_hori_ver2[1]);
				sprintf_s(eval_msg[6], "hori_ver2_3 = %f, ", key_hori_ver2[2]);
				sprintf_s(eval_msg[7], "hori_ver2_4 = %f\n", key_hori_ver2[3]);
				sprintf_s(eval_msg[8], "hori_ver3_1 = %f, ", key_hori_ver3[0]);
				sprintf_s(eval_msg[9], "hori_ver3_2 = %f\n", key_hori_ver3[1]);
				sprintf_s(eval_msg[10], "hori_ver3_3 = %f, ", key_hori_ver3[2]);
				sprintf_s(eval_msg[11], "hori_ver3_4 = %f\n", key_hori_ver3[3]);
				sprintf_s(eval_msg[12], "dia_ver1_1 = %f, ", key_dia_ver1[0]);
				sprintf_s(eval_msg[13], "dia_ver1_2 = %f\n", key_dia_ver1[1]);
				sprintf_s(eval_msg[14], "dia_ver2_1 = %f, ", key_dia_ver2[0]);
				sprintf_s(eval_msg[15], "dia_ver2_2 = %f\n", key_dia_ver2[1]);
				sprintf_s(eval_msg[16], "dia_ver2_3 = %f, ", key_dia_ver2[2]);
				sprintf_s(eval_msg[17], "dia_ver2_4 = %f\n", key_dia_ver2[3]);
				sprintf_s(eval_msg[18], "dia_ver3_1 = %f, ", key_dia_ver3[0]);
				sprintf_s(eval_msg[19], "dia_ver3_2 = %f\n", key_dia_ver3[1]);
				sprintf_s(eval_msg[20], "dia_ver3_3 = %f, ", key_dia_ver3[2]);
				sprintf_s(eval_msg[21], "dia_ver3_4 = %f\n", key_dia_ver3[3]);
				sprintf_s(eval_msg[22], "dia_ver4_1 = %f, ", key_dia_ver4[0]);
				sprintf_s(eval_msg[23], "dia_ver4_2 = %f\n", key_dia_ver4[1]);
				sprintf_s(eval_msg[24], "dia_ver4_3 = %f, ", key_dia_ver4[2]);
				sprintf_s(eval_msg[25], "dia_ver4_4 = %f\n", key_dia_ver4[3]);
				sprintf_s(eval_msg[26], "edge_1 = %f, ", key_edge[0]);
				sprintf_s(eval_msg[27], "edge_2 = %f\n", key_edge[1]);
				sprintf_s(eval_msg[28], "edge_3 = %f, ", key_edge[2]);
				sprintf_s(eval_msg[29], "edge_4 = %f\n", key_edge[3]);
				sprintf_s(eval_msg[30], "corner5_2_1 = %f, ", key_corner5_2[0]);
				sprintf_s(eval_msg[31], "corner5_2_2 = %f\n", key_corner5_2[1]);
				sprintf_s(eval_msg[32], "corner5_2_3 = %f, ", key_corner5_2[2]);
				sprintf_s(eval_msg[33], "corner5_2_4 = %f\n", key_corner5_2[3]);
				sprintf_s(eval_msg[34], "corner5_2_5 = %f, ", key_corner5_2[4]);
				sprintf_s(eval_msg[35], "corner5_2_6 = %f\n", key_corner5_2[5]);
				sprintf_s(eval_msg[36], "corner5_2_7 = %f, ", key_corner5_2[6]);
				sprintf_s(eval_msg[37], "corner5_2_8 = %f\n", key_corner5_2[7]);
				sprintf_s(eval_msg[38], "corner3_3_1 = %f, ", key_corner3_3[0]);
				sprintf_s(eval_msg[39], "corner3_3_2 = %f\n", key_corner3_3[1]);
				sprintf_s(eval_msg[40], "corner3_3_3 = %f, ", key_corner3_3[2]);
				sprintf_s(eval_msg[41], "corner3_3_4 = %f\n", key_corner3_3[3]);
				sprintf_s(eval_msg[42], "triangle_1 = %f, ", key_triangle[0]);
				sprintf_s(eval_msg[43], "triangle_2 = %f\n", key_triangle[1]);
				sprintf_s(eval_msg[44], "triangle_3 = %f, ", key_triangle[2]);
				sprintf_s(eval_msg[45], "triangle_4 = %f\n\n", key_triangle[3]);
				sprintf_s(eval_msg[46], "mobility = %f\n", key_mobility);

				sprintf_s(eval_msg[47], "parity = %f\n", key_parity);
				sprintf_s(eval_msg[48], "constant = %f\n", key_constant);

				sprintf_s(eval_msg[49], "\n評価値 = %f,", eval_sum);
				sprintf_s(eval_msg[50], "\nblack = 0x%llx\nwhite = 0x%llx\n", black, white);

				int i;
				char print_msg[8192];
				strcpy_s(print_msg, eval_msg[0]);
				for (i = 1; i < 51; i++)
				{
					strcat_s(print_msg, eval_msg[i]);
				}

				MessageBox(hWnd, print_msg, "評価値詳細一覧", MB_OK);
			}
			break;
			/* FFOメニュ－ */

		case FFO40:
			black = 9158069842325798912;
			white = 11047339776155165;
			configCPU(BLACK);
			break;

		case FFO41:
			black = 616174399789064;
			white = 39493460025648416;
			configCPU(BLACK);
			break;

		case FFO42:
			white = 9091853944868375556;
			black = 22586176447709200;
			configCPU(BLACK);
			break;

		case FFO43:
			black = 38808086923902976;
			white = 13546258740034592;
			configCPU(WHITE);
			break;

		case FFO44:
			black = 2494790880993312;
			white = 1010251075753548824;
			configCPU(WHITE);
			break;

		case FFO45:
			black = 282828816915486;
			white = 9287318235258944;
			configCPU(BLACK);
			break;

		case FFO46:
			black = 4052165999611379712;
			white = 36117299622447104;
			configCPU(BLACK);

			break;

		case FFO47:
			black = 277938752194568;
			white = 3536224466208;
			configCPU(WHITE);
			break;

		case FFO48:
			black = 38519958422848574;
			white = 4725679339520;
			configCPU(WHITE);
			break;

		case FFO49:
			black = 5765976742297600;
			white = 4253833575484;
			configCPU(BLACK);

			break;

		case FFO50:
			black = 4504145659822080;
			white = 4336117619740130304;
			configCPU(BLACK);
			break;

		case FFO51:
			black = 349834415978528;
			white = 8664011788383158280;
			configCPU(WHITE);
			break;

		case FFO52:
			black = 9096176176681728056;
			white = 35409824317440;
			configCPU(WHITE);
			break;

		case FFO53:
			black = 2515768979493888;
			white = 8949795312300457984;
			configCPU(BLACK);
			break;

		case FFO54:
			black = 277938752194568;
			white = 3536224466208;
			configCPU(BLACK);
			break;

		case FFO55:
			black = 4635799596172290;
			white = 289361502099486840;
			configCPU(BLACK);
			break;

		case FFO56:
			black = 4925086697193472;
			white = 9007372734053408;
			configCPU(WHITE);
			break;

		case FFO57:
			black = 9060166336512000;
			white = 8943248156475301888;
			configCPU(BLACK);

			break;

		case FFO58:
			black = 4636039783186432;
			white = 3383245044333600;
			configCPU(BLACK);
			break;

		case FFO59:
			black = 17320879491911778304;
			white = 295223649004691488;
			configCPU(BLACK);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		InvalidateRect(hWnd, NULL, FALSE);
		/* 再描写 */
		hdc = GetDC(hWnd);
		DrawBoard(hdc, hStatus, -1, -1);
		ReleaseDC(hWnd, hdc);
		UpdateWindow(hRebar);
		SendMessage(hStatus, SB_SETTEXT, (WPARAM)1 | 0, (LPARAM)turn_str[NowTurn]);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 描画コードをここに追加してください...
		hdc = GetDC(hWnd);
		DrawBoard(hdc, hStatus, move_x, move_y);
		ReleaseDC(hWnd, hdc);
		SendMessage(hStatus, SB_SETTEXT, (WPARAM)1 | 0, (LPARAM)turn_str[NowTurn]);
		//char str[64];
		//sprintf_s(str, "black = %llu", black);
		//SendMessage(hStatus, SB_SETTEXT, (WPARAM)3 | 0, (LPARAM)str);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:

		/* 置換表の解放 */
		//HashDelete(hashTable);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// バージョン情報ボックスのメッセージ ハンドラです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK load_books(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		DlgHandle = hDlg;
		{
			long DlgVal = GetWindowLong(DlgHandle, GWL_STYLE);
			SetWindowLong(DlgHandle, GWL_STYLE, DlgVal - WS_SYSMENU);
			/* プログレスバーの設定 */
			{
				HANDLE ProgressHandle = GetDlgItem(DlgHandle, IDC_PROGRESS1);
				PostMessage((HWND)ProgressHandle, PBM_SETRANGE, 0, MAKELPARAM(0, 1000));
				PostMessage((HWND)ProgressHandle, PBM_SETSTEP, (WPARAM)1, 0);
			}
			return (INT_PTR)TRUE;
		}
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void InitSystem()
{

	if ((hDLL = LoadLibrary("mobility.dll")) == NULL)
	{
		MessageBox(hWnd, "mobility.dllが見つかりません。", "dllファイルオープンエラー", MB_OK);
		exit(0);
	}

	init_mmx = (INIT_MMX)GetProcAddress(hDLL, "init_mmx");
	bit_mob = (BIT_MOB)GetProcAddress(hDLL, "bitboard_mobility");

	(init_mmx)();

	m_FlagInGame = FALSE;
	m_FlagPerfectSearch = FALSE;
	m_FlagHint = FALSE;
	m_FlagBook = FALSE;
	CpuColor = WHITE;
	PassCnt = 0;
	/* デバッグモードOFF */
	AI_result = FALSE;
	OnTheWay = FALSE;
	AI_thinking = FALSE;
	//DEBUG = TRUE;
	UseBook = TRUE;
	b_Flag_little_change = TRUE;
	INF_DEPTH = 64;
	/* ヒントOFF */
	Flag_Hint = FALSE;
	HINT_DISPED = FALSE;
	/* 先手は黒 */
	NowTurn = BLACK;
	/* 前回の手はない */
	move_x = move_y = -1;

	/* 難易度はLEVEL1 */
	//difficult = 3;
	//strcpy_s(Diff_Str, 16, "LEVEL 3");

	/* 終盤の読み手数 */
	difficult_table[0] = 12;
	difficult_table[1] = 14;
	difficult_table[2] = 16;
	difficult_table[3] = 18;
	difficult_table[4] = 18;
	difficult_table[5] = 20;
	difficult_table[6] = 20;
	difficult_table[7] = 22;
	difficult_table[8] = 22;
	difficult_table[9] = 24;
	difficult_table[10] = 24;
	difficult_table[11] = 26;

	int i;
	char cordinate[4];
	/* 指し手の座標表記変換用 */
	for (i = 0; i < 64; i++)
	{
		sprintf_s(cordinate, "%c%d", i / MASS_NUM + 'a', (i % MASS_NUM) + 1);
		strcpy_s(cordinates_table[i], cordinate);
	}
	for (i = 0; i < 64; i++)
	{
		p_pattern[i] = 0;
	}

}

void ClearUndoData()
{
	int i;
	for (i = 0; i < 120; i++)
	{
		UndoBoard_B[i] = 0;
		UndoBoard_W[i] = 0;
		Undo_color[i] = 0;
		Undo_x[i] = -1;
		Undo_y[i] = -1;
		Undo_Node[i] = NULL;
	}
}

void ClearListBoxItem(int delete_item_index)
{
	/* 項目の消去 */
	SendMessage(hListBox, LB_SETCURSEL, delete_item_index, 0);
	int item_count = SendMessage(hListBox, LB_GETCOUNT, 0, 0);

	while (item_count > delete_item_index)
	{
		SendMessage(hListBox, LB_DELETESTRING, (WPARAM)(item_count - 1), 0);
		UndoBoard_B[item_count] = 0;
		UndoBoard_W[item_count] = 0;
		Undo_color[item_count] = 0;
		Undo_x[item_count] = -1;
		Undo_y[item_count] = -1;
		Undo_Node[item_count] = NULL;
		item_count--;
	}
}

void InitBoard()
{
	int x, y;
	turn = 0;
	NowTurn = BLACK;
	MAXEVAL = 0;
	black = 0;
	white = 0;
	move_x = -1;
	move_y = -1;

	x = 3; y = 4;
	black ^= one << (x * MASS_NUM + y);
	x = 4; y = 3;
	black ^= one << (x * MASS_NUM + y);

	x = 3; y = 3;
	white ^= one << (x * MASS_NUM + y);
	x = 4; y = 4;
	white ^= one << (x * MASS_NUM + y);

	m_FlagInGame = FALSE;
	m_FlagForWhite = FALSE;
	m_FlagPerfectSearch = FALSE;
	m_FlagBook = FALSE;

	if (m_FlagHint)
	{
		Flag_Abort = TRUE;
	}

	Flag_Edit = FALSE;

	/* ヒント用の評価データ破棄 */
	for (int i = 0; i < 64; i++){ EvalData[i] = NEGAMAX; }
	SendMessage(hStatus, SB_SETTEXT, (WPARAM)0 | 0, (LPARAM)"");
	SendMessage(hStatus, SB_SETTEXT, (WPARAM)3 | 0, (LPARAM)"");
	SendMessage(hStatus, SB_SETTEXT, (WPARAM)4 | 0, (LPARAM)"");

	/* ボタンの初期化 */
	EnableWindow(GetDlgItem(hRebar, REDO_BT), FALSE);
	EnableWindow(GetDlgItem(hRebar, UNDO_BT), FALSE);

	ClearUndoData();

	ClearListBoxItem(0);
	max_change_num[0] = 0;
	max_change_num[1] = 0;
	srand((unsigned int)time(NULL));

}

void init_substitution_table_eval(Hash *hash)
{
	int i;
	for (i = 0; i < TABLE_SIZE; i++)
	{
		hash->data[i].lower = NEGAMIN;
		hash->data[i].upper = NEGAMAX;
	}
}


void init_substitution_table_winloss(Hash *hash)
{
	int i;

	for (i = 0; i < TABLE_SIZE; i++)
	{
		hash->data[i].lower = LOSS - 1;
		hash->data[i].upper = WIN + 1;
	}
}

void init_substitution_table_exact(Hash *hash)
{
	int i;
	for (i = 0; i < TABLE_SIZE; i++)
	{
		if (hash->data[i].lower == WIN && hash->data[i].upper == WIN)
		{
			hash->data[i].lower = WIN;
			hash->data[i].upper = 65;
		}
		else if (hash->data[i].lower == LOSS && hash->data[i].upper == LOSS)
		{
			hash->data[i].lower = -65;
			hash->data[i].upper = LOSS;
		}
		else if (hash->data[i].lower == DRAW && hash->data[i].upper == DRAW)
		{
			hash->data[i].lower = DRAW;
			hash->data[i].upper = DRAW;
		}
		else{
			hash->data[i].lower = -65;
			hash->data[i].upper = 65;
		}

	}
}

void init_substitution_table(Hash *hash)
{
	if (hash != NULL){
		memset(hash->data, 0, sizeof(HashInfo) * hash->num);
	}
}

//void refresh_index(BitBoard rev)
//{
//	do
//	{
//		board[CountBit((rev & (-(signed long long int)rev)) - 1)] ^= 3;
//		rev = rev & (rev - 1);
//	}while(rev);
//
//}

/* 1が立っているビットを算出 */
int CountBit(BitBoard bit)
{
	int l_moves = bit & 0x00000000FFFFFFFF;
	int h_moves = (bit & 0xFFFFFFFF00000000) >> 32;

	int count = _mm_popcnt_u32(l_moves);
	count += _mm_popcnt_u32(h_moves);

	/*l_moves -= (l_moves >> 1) & 0x55555555;
	l_moves = (l_moves & 0x33333333) + ((l_moves >> 2) & 0x33333333);
	l_moves = (l_moves + (l_moves >> 4)) & 0x0F0F0F0F;
	l_moves += l_moves >> 8;
	l_moves += l_moves >> 16;
	l_moves &= 0x0000003f;

	h_moves -= (h_moves >> 1) & 0x55555555;
	h_moves = (h_moves & 0x33333333) + ((h_moves >> 2) & 0x33333333);
	h_moves = (h_moves + (h_moves >> 4)) & 0x0F0F0F0F;
	h_moves += h_moves >> 8;
	h_moves += h_moves >> 16;
	h_moves &= 0x0000003f;

	return l_moves + h_moves;*/

	return count;
}

/* 将来、着手可能になる可能性の高いマスを評価 */
//int CountPotMob(BitBoard b_board, BitBoard w_board){
//	BitBoard blank = ~(b_board | w_board);
//	BitBoard pm_cnt = (
//					   ((w_board & 0x007e7e7e7e7e7e00) << 9) |
//					   ((w_board & 0x00ffffffffffff00) << 8) |
//					   ((w_board & 0x007e7e7e7e7e7e00) << 7) |
//					   ((w_board & 0x7e7e7e7e7e7e7e7e) << 1) |
//					   ((w_board & 0x7e7e7e7e7e7e7e7e) << 1) |
//					   ((w_board & 0x007e7e7e7e7e7e00) << 7) |
//					   ((w_board & 0x00ffffffffffff00) << 8) |
//					   ((w_board & 0x007e7e7e7e7e7e00) << 9)) & blank;
//
//	return CountBit(pm_cnt);
//}




//void PrintCorrectSt(HWND hWnd)
//{
//	CStone = SearchCorrectSt(black, white);
//	HDC hdc = GetDC(hWnd);
//
//	ReleaseDC(hWnd, hdc);
//}

//int CorrectSt_Count(BitBoard bk, BitBoard wh)
//{
//	int CStone = 0;
//
//	CStone += sumi1_To_sumi2_count(bk, wh);
//	CStone += sumi1_To_sumi3_count(bk, wh);
//
//	CStone += sumi2_To_sumi4_count(bk, wh);
//	CStone += sumi3_To_sumi4_count(bk, wh);
//
//	CStone += sumi3_To_sumi1_count(bk, wh);
//	CStone += sumi4_To_sumi2_count(bk, wh);
//	return CStone;
//}

HWND CreateStatusBar(HWND hWnd)
{
	InitCommonControls();
	//ステータスバーの作成
	HWND hStatus = CreateStatusWindow
		(
		WS_CHILD | CCS_BOTTOM | WS_VISIBLE,
		NULL, hWnd, Status
		);
	/* 4分割 */
	SbSize[3] = BOARD_SIZE + SYSTEM_SIZE;
	SbSize[2] = BOARD_SIZE + 50;
	SbSize[1] = BOARD_SIZE - 50;
	SbSize[0] = BOARD_SIZE - 70;
	SendMessage(hStatus, SB_SETPARTS, 4, (LPARAM)SbSize);
	return hStatus;
}

HWND MakeMyRebar(HWND hWnd)
{
	HWND hRebar;
	REBARINFO rbi;

	hRebar = CreateWindowEx(WS_EX_TOOLWINDOW, REBARCLASSNAME, NULL,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | RBS_BANDBORDERS | WS_CLIPCHILDREN | CCS_NODIVIDER,
		0, 0, 0, 0, hWnd, (HMENU)ID_COOL, hInst, NULL);
	
	ZeroMemory(&rbi, sizeof(REBARINFO));
	rbi.cbSize = sizeof(REBARINFO);
	SendMessage(hRebar, RB_SETBARINFO, (WPARAM)0, (LPARAM)&rbi);

	return hRebar;
}

HWND CreateListBox(HWND hWnd)
{
	HWND handle;
	handle = CreateWindowEx(0, "ListBox", "",
		WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
		BOARD_SIZE + 10,
		160,
		(SYSTEM_SIZE - 25) / 2,
		WINDOW_H / 2 - 160,
		hWnd, NULL, hInst, NULL);
	return handle;
}

HWND MakeBoardWindow(HWND hWnd)
{
	HWND boardWnd;
	boardWnd = CreateWindowEx(WS_EX_LEFT, REBARCLASSNAME, NULL,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | RBS_BANDBORDERS | WS_CLIPCHILDREN | CCS_NODIVIDER,
		0, 0, 0, 0, hWnd, NULL, hInst, NULL);

	return boardWnd;
}

HWND CreateComBox1(HWND hWnd, int x, int y, int num)
{
	HWND t_handle;

	t_handle = CreateWindow(
		"COMBOBOX",
		"text is here.",							 // 表示する文字列
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | CBS_DROPDOWNLIST, // ウインドウスタイル
		x, y, 										 // 左上の座標
		80, 100,										 // 幅と高さ
		hWnd,										 // 親ウインドウのウインドウハンドル
		(HMENU)com_id[num],										 // メニューハンドル。NULLでよい。
		hInst,										 // アプリケーションのインスタンスハンドル。
		NULL									     // ウインドウ作成データ。NULLでよい
		);

	SendMessage(t_handle, CB_ADDSTRING, 0, (LPARAM)"human");
	SendMessage(t_handle, CB_ADDSTRING, 0, (LPARAM)"CPU");

	ShowWindow(t_handle, SW_SHOW);

	return t_handle;
}

HWND CreateComBox2(HWND hWnd, int x, int y, int num)
{
	HWND t_handle;

	t_handle = CreateWindow(
		"COMBOBOX",
		"text is here.",							 // 表示する文字列
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | CBS_DROPDOWNLIST, // ウインドウスタイル
		x, y, 										 // 左上の座標
		100, 100,										 // 幅と高さ
		hWnd,										 // 親ウインドウのウインドウハンドル
		(HMENU)com_id[num],										 // メニューハンドル。NULLでよい。
		hInst,										 // アプリケーションのインスタンスハンドル。
		NULL									     // ウインドウ作成データ。NULLでよい
		);

	SendMessage(t_handle, CB_ADDSTRING, 0, (LPARAM)"LEVEL1");
	SendMessage(t_handle, CB_ADDSTRING, 0, (LPARAM)"LEVEL2");
	SendMessage(t_handle, CB_ADDSTRING, 0, (LPARAM)"LEVEL3");
	SendMessage(t_handle, CB_ADDSTRING, 0, (LPARAM)"LEVEL4");
	SendMessage(t_handle, CB_ADDSTRING, 0, (LPARAM)"LEVEL5");
	SendMessage(t_handle, CB_ADDSTRING, 0, (LPARAM)"LEVEL6");
	SendMessage(t_handle, CB_ADDSTRING, 0, (LPARAM)"LEVEL7");
	SendMessage(t_handle, CB_ADDSTRING, 0, (LPARAM)"LEVEL8");
	SendMessage(t_handle, CB_ADDSTRING, 0, (LPARAM)"LEVEL9");
	SendMessage(t_handle, CB_ADDSTRING, 0, (LPARAM)"LEVEL10");
	SendMessage(t_handle, CB_ADDSTRING, 0, (LPARAM)"LEVEL11");
	SendMessage(t_handle, CB_ADDSTRING, 0, (LPARAM)"LEVEL12");
	SendMessage(t_handle, CB_ADDSTRING, 0, (LPARAM)"WinLose");
	SendMessage(t_handle, CB_ADDSTRING, 0, (LPARAM)"Exact");

	SendMessage(t_handle, CB_SETCURSEL, 2, 0);

	ShowWindow(t_handle, SW_SHOW);

	return t_handle;
}

void UncheckedAll_BookMenu(HMENU hMenu)
{
	b_Flag_not_change = FALSE;
	b_Flag_little_change = FALSE;
	b_Flag_change = FALSE;
	CheckMenuItem(hMenu, book_best, MFS_UNCHECKED);
	CheckMenuItem(hMenu, book_little_change, MFS_UNCHECKED);
	CheckMenuItem(hMenu, book_change, MFS_UNCHECKED);
	CheckMenuItem(hMenu, book_random, MFS_UNCHECKED);
}

void configCPU(int color){

	CpuColor = color;
	NowTurn = color;
	OnAllSearchSD(hWnd, color);
	player[color] = CPU;
	player[color ^ 1] = HUMAN;

	if (color == BLACK){
		ShowWindow(hComBox3, SW_SHOW);
		ShowWindow(hComBox4, SW_HIDE);

		SendMessage(hComBox1, CB_SETCURSEL, 1, 0);
		SendMessage(hComBox2, CB_SETCURSEL, 0, 0);
		SendMessage(hComBox3, CB_SETCURSEL, 13, 0);
	}
	else{
		ShowWindow(hComBox4, SW_SHOW);
		ShowWindow(hComBox3, SW_HIDE);

		SendMessage(hComBox1, CB_SETCURSEL, 0, 0);
		SendMessage(hComBox2, CB_SETCURSEL, 1, 0);
		SendMessage(hComBox4, CB_SETCURSEL, 13, 0);

	}
}