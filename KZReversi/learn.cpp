/*#
  # 学習アルゴリズム：最急降下法
  #  
  ##############################################
*/

#include "stdafx.h"
#include "KZReversi.h"
#include "GetRev.h"
#include "AI.h"
#include "GetPattern.h"
#include "ordering.h"
#include "learn.h"
#include "game.h"

#include "direct.h"

char DIR_PATH[64];
char TAIKYOKU_NAME[256];
char TABLE_NAME[256];
char MSEOUT_PATH[64];
char TRACE_PATH[64];

FILE *data_fp[14];

char kihu_list[74][32] = 
{
	"01E4.gam.1.new" ,"01E4.gam.2.new" ,"02E4.gam.1.new" ,"02E4.gam.2.new",
	"03E4.gam.1.new" ,"03E4.gam.2.new" ,"04E4.gam.1.new" ,"04E4.gam.2.new",
	"05E4.gam.1.new" ,"05E4.gam.2.new" ,"06E4.gam.1.new" ,"06E4.gam.2.new",
	"07E4.gam.1.new" ,"07E4.gam.2.new" ,"08E4.gam.1.new" ,"08E4.gam.2.new",
	"09E4.gam.1.new" ,"09E4.gam.2.new" ,"10E4.gam.1.new" ,"10E4.gam.2.new",

	"11E4.gam.1.new" ,"11E4.gam.2.new" ,"12E4.gam.1.new" ,"12E4.gam.2.new",
	"13E4.gam.1.new" ,"13E4.gam.2.new" ,"14E4.gam.1.new" ,"14E4.gam.2.new",
	"15E4.gam.1.new" ,"15E4.gam.2.new" ,"16E4.gam.1.new" ,"16E4.gam.2.new",
	"17E4.gam.1.new" ,"17E4.gam.2.new" ,"18E4.gam.1.new" ,"18E4.gam.2.new",
	"19E4.gam.1.new" ,"19E4.gam.2.new" ,"20E4.gam.1.new" ,"20E4.gam.2.new",
	
	"21E4.gam.1.new" ,"21E4.gam.2.new" ,"22E4.gam.1.new" ,"22E4.gam.2.new",
	"23E4.gam.1.new" ,"23E4.gam.2.new" ,"24E4.gam.1.new" ,"24E4.gam.2.new",
	"25E4.gam.1.new" ,"25E4.gam.2.new" ,"26E4.gam.1.new" ,"26E4.gam.2.new",
	"27E4.gam.1.new" ,"27E4.gam.2.new" ,"28E4.gam.1.new" ,"28E4.gam.2.new",
	"29E4.gam.1.new" ,"29E4.gam.2.new" ,"30E4.gam.1.new" ,"30E4.gam.2.new",
	
	"31E4.gam.1.new" ,"31E4.gam.2.new" ,"32E4.gam.1.new" ,"32E4.gam.2.new",
	"33E4.gam.1.new" ,"33E4.gam.2.new" ,"34E4.gam.1.new" ,"34E4.gam.2.new",
	"35E4.gam.1.new" ,"35E4.gam.2.new" ,"36E4.gam.1.new" ,"36E4.gam.2.new",
	"37E4.gam.1.new" ,"37E4.gam.2.new"
	
};

int pow(int x, int pow_num)
{
	int temp_pow = 1;
	while(pow_num > 0)
	{
		temp_pow *= (x + 1);
		pow_num--;
	}
	return x * temp_pow;
}

int ConvertToMove_U(char *buf){
	int i, max;

	i = (buf[0] - 'A') * 8;
	max = i + 8;
	buf[0] += 32;
	while(i < max)
	{
		if(strncmp(buf, cordinates_table[i], 2) == 0)
		{
			return i;
		}
		i++;
	}
	return -1;
}

void SaveModel(FILE *fp, double StoneDiff, BitBoard b_board, BitBoard w_board, int color)
{

	fprintf(fp, "%lf\n", StoneDiff);

	write_h_ver1(fp);
	write_h_ver2(fp);
	write_h_ver3(fp);
	write_dia_ver1(fp);
	write_dia_ver2(fp);
	write_dia_ver3(fp);
	write_dia_ver4(fp);
	write_edge(fp);
	write_corner3_3(fp);
	write_corner5_2(fp);
	write_triangle(fp);
	//write_mobility(fp, b_board, w_board);
	write_pality(fp, ~(b_board | w_board), color);
	//write_potmob(fp, b_board, w_board);
}

void SaveModel_zyouseki()
{
	FILE *fp;
	if(fopen_s(&fp, "eval_zyouseki.dat", "a") != 0)
	{
		return;
	}
	fprintf(fp, "%d\n", 0);

	write_h_ver1(fp);
	write_h_ver2(fp);
	write_h_ver3(fp);
	write_dia_ver1(fp);
	write_dia_ver2(fp);
	write_dia_ver3(fp);
	write_dia_ver4(fp);
	write_edge(fp);
	write_corner5_2(fp);
	write_triangle(fp);

	fclose(fp);
}

void CulcPatternEval(int sample_num)
{
	static float t[MAX];
	//double x[MAX][PATTERN_NUM];
	static unsigned short int key[MAX][PATTERN_NUM];
	unsigned short int *p_key;
	float e;
	char str[64];
	float E_t;
	float w;
	static float derivatives[MAX][PATTERN_NUM];
	float *p_deri;
	float alpha = (float )ALPHA_NUM;
	int i, j, l;
	float temp = 0;
	float ave_error;
	FILE *fp = NULL;

	_mkdir(DIR_PATH);

	if(fopen_s(&fp, TAIKYOKU_NAME, "r") != 0){
		printf("error.\n");
		exit(1);
	}

	NowStage = 3;

	ReadEvalDat(NowStage);

	hori_ver1 = &hori_ver1_data[NowStage][0];
	hori_ver2 = &hori_ver2_data[NowStage][0];
	hori_ver3 = &hori_ver3_data[NowStage][0];
	dia_ver1 =  &dia_ver1_data[NowStage][0];
	dia_ver2 =  &dia_ver2_data[NowStage][0];
	dia_ver3 =  &dia_ver3_data[NowStage][0];
	dia_ver4 =  &dia_ver4_data[NowStage][0];
	edge =		&edge_data[NowStage][0];
	corner3_3 = &corner3_3_data[NowStage][0];
	corner5_2 = &corner5_2_data[NowStage][0];
	//triangle =	&triangle_data[NowStage][0];
	//pality =	&pality_data[NowStage][0];

	/* 学習データの読み込み */
	for(i = 0; i < sample_num; i++){
		fscanf_s(fp, "%f", &(t[i]));
		
		/* hori_ver1 */
		fscanf_s(fp, "%d", &(key[i][0]));
		fscanf_s(fp, "%d", &(key[i][1]));
		fscanf_s(fp, "%d", &(key[i][2]));
		fscanf_s(fp, "%d", &(key[i][3]));

		/* hori_ver2 */
		fscanf_s(fp, "%d", &(key[i][4]));
		fscanf_s(fp, "%d", &(key[i][5]));
		fscanf_s(fp, "%d", &(key[i][6]));
		fscanf_s(fp, "%d", &(key[i][7]));

		/* hori_ver3 */
		fscanf_s(fp, "%d", &(key[i][8]));
		fscanf_s(fp, "%d", &(key[i][9]));
		fscanf_s(fp, "%d", &(key[i][10]));
		fscanf_s(fp, "%d", &(key[i][11]));

		/* dia_ver1 */
		fscanf_s(fp, "%d", &(key[i][12]));
		fscanf_s(fp, "%d", &(key[i][13]));

		/* dia_ver2 */
		fscanf_s(fp, "%d", &(key[i][14]));
		fscanf_s(fp, "%d", &(key[i][15]));
		fscanf_s(fp, "%d", &(key[i][16]));
		fscanf_s(fp, "%d", &(key[i][17]));

		/* dia_ver3 */
		fscanf_s(fp, "%d", &(key[i][18]));
		fscanf_s(fp, "%d", &(key[i][19]));
		fscanf_s(fp, "%d", &(key[i][20]));
		fscanf_s(fp, "%d", &(key[i][21]));

		/* dia_ver4 */
		fscanf_s(fp, "%d", &(key[i][22]));
		fscanf_s(fp, "%d", &(key[i][23]));
		fscanf_s(fp, "%d", &(key[i][24]));
		fscanf_s(fp, "%d", &(key[i][25]));

		/* edge */
		fscanf_s(fp, "%d", &(key[i][26]));
		fscanf_s(fp, "%d", &(key[i][27]));
		fscanf_s(fp, "%d", &(key[i][28]));
		fscanf_s(fp, "%d", &(key[i][29]));

		/* corner3_3 */
		fscanf_s(fp, "%d", &(key[i][30]));
		fscanf_s(fp, "%d", &(key[i][31]));
		fscanf_s(fp, "%d", &(key[i][32]));
		fscanf_s(fp, "%d", &(key[i][33]));

		/* corner5_2 */
		fscanf_s(fp, "%d", &(key[i][34]));
		fscanf_s(fp, "%d", &(key[i][35]));
		fscanf_s(fp, "%d", &(key[i][36]));
		fscanf_s(fp, "%d", &(key[i][37]));
		fscanf_s(fp, "%d", &(key[i][38]));
		fscanf_s(fp, "%d", &(key[i][39]));
		fscanf_s(fp, "%d", &(key[i][40]));
		fscanf_s(fp, "%d", &(key[i][41]));

		/* triangle */
		fscanf_s(fp, "%d", &(key[i][42]));
		fscanf_s(fp, "%d", &(key[i][43]));
		fscanf_s(fp, "%d", &(key[i][44]));
		fscanf_s(fp, "%d", &(key[i][45]));

		/* pality */
		fscanf_s(fp, "%d", &(key[i][46]));
	}

	fclose(fp);

	/* 最急降下法 */

	int MAX_LOOP = 200000;
	for(i = 0; i < MAX_LOOP; i++)
	{
		sprintf_s(str, "　　　　　　%d/%d(α = %lf)", i, MAX_LOOP, alpha);
		SendMessage(hStatus, SB_SETTEXT, (WPARAM)3|0, (LPARAM)str);
		temp = 0;
		for(l = 0; l < sample_num; l++)
		{
			p_key = &key[l][0];
			memset(derivatives[l], 0, sizeof(derivatives[l]));
			p_deri = &derivatives[l][0];
			/* 予想石差eを計算。t[l]が教師信号 */
			e =(float )
			  (hori_ver1[p_key[0]] +  hori_ver1[p_key[1]] +  hori_ver1[p_key[2]] +  hori_ver1[p_key[3]] +
			   hori_ver2[p_key[4]] +  hori_ver2[p_key[5]] +  hori_ver2[p_key[6]] +  hori_ver2[p_key[7]] +
			   hori_ver3[p_key[8]] +  hori_ver3[p_key[9]] +  hori_ver3[p_key[10]] + hori_ver3[p_key[11]] +
			   dia_ver1[p_key[12]] +  dia_ver1[p_key[13]] +
			   dia_ver2[p_key[14]] +  dia_ver2[p_key[15]] +  dia_ver2[p_key[16]] +  dia_ver2[p_key[17]] +
			   dia_ver3[p_key[18]] +  dia_ver3[p_key[19]] +  dia_ver3[p_key[20]] +  dia_ver3[p_key[21]] +
			   dia_ver4[p_key[22]] +  dia_ver4[p_key[23]] +  dia_ver4[p_key[24]] +  dia_ver4[p_key[25]] +
			   edge[p_key[26]]     +  edge[p_key[27]]     +  edge[p_key[28]]     +  edge[p_key[29]]     + 
			   corner3_3[p_key[30]] + corner3_3[p_key[31]] + corner3_3[p_key[32]] + corner3_3[p_key[33]] +
			   corner5_2[p_key[34]] + corner5_2[p_key[35]] + corner5_2[p_key[36]] + corner5_2[p_key[37]] +
			   corner5_2[p_key[38]] + corner5_2[p_key[39]] + corner5_2[p_key[40]] + corner5_2[p_key[41]]);
			/* 誤差を算出 */
			E_t = t[l] - e;

			temp += E_t * E_t;

			for(j = 0; j < PATTERN_NUM; j++)
			{
				p_deri[j] += E_t;
			}
			if(i % 1000 == 0)
			{
				if(l == 0)
				{
					fopen_s(&fp, MSEOUT_PATH, "w");
				}
				fprintf_s(fp, "%3d : t = %f, y = %f (err = %f)\n", l, t[l], e, E_t);

				if(l == sample_num - 1)
				{
					fclose(fp);
				}
			}
		}

		ave_error = temp / (float )sample_num;
		/* α値更新 */
		//alpha += alpha * (float )0.0001;
		for(l = 0; l < sample_num; l++)
		{
			p_key = &key[l][0];
			p_deri = &derivatives[l][0];
			for(j = 0; j < PATTERN_NUM; j++)
			{
				p_deri[j] = 2 * p_deri[j] / (float)sample_num;
			}

			/* 局面ナンバーlに出現する特徴の評価値を更新 */
			w = alpha * p_deri[0];
			hori_ver1[p_key[0]] += w;
			w = alpha * p_deri[1];
			hori_ver1[p_key[1]] += w;
			w = alpha * p_deri[2];
			hori_ver1[p_key[2]] += w;
			w = alpha * p_deri[3];
			hori_ver1[p_key[3]] += w;

			w = alpha * p_deri[4];
			hori_ver2[p_key[4]] += w;
			w = alpha * p_deri[5];
			hori_ver2[p_key[5]] += w;
			w = alpha * p_deri[6];
			hori_ver2[p_key[6]] += w;
			w = alpha * p_deri[7];
			hori_ver2[p_key[7]] += w;

			w = alpha * p_deri[8];
			hori_ver3[p_key[8]] += w;
			w = alpha * p_deri[9];
			hori_ver3[p_key[9]] += w;
			w = alpha * p_deri[10];
			hori_ver3[p_key[10]] += w;
			w = alpha * p_deri[11];
			hori_ver3[p_key[11]] += w;

			w = alpha * p_deri[12];
			dia_ver1[p_key[12]] += w;
			w = alpha * p_deri[13];
			dia_ver1[p_key[13]] += w;

			w = alpha * p_deri[14];
			dia_ver2[p_key[14]] += w;
			w = alpha * p_deri[15];
			dia_ver2[p_key[15]] += w;
			w = alpha * p_deri[16];
			dia_ver2[p_key[16]] += w;
			w = alpha * p_deri[17];
			dia_ver2[p_key[17]] += w;

			w = alpha * p_deri[18];
			dia_ver3[p_key[18]] += w;
			w = alpha * p_deri[19];
			dia_ver3[p_key[19]] += w;
			w = alpha * p_deri[20];
			dia_ver3[p_key[20]] += w;
			w = alpha * p_deri[21];
			dia_ver3[p_key[21]] += w;

			w = alpha * p_deri[22];
			dia_ver4[p_key[22]] += w;
			w = alpha * p_deri[23];
			dia_ver4[p_key[23]] += w;
			w = alpha * p_deri[24];
			dia_ver4[p_key[24]] += w;
			w = alpha * p_deri[25];
			dia_ver4[p_key[25]] += w;

			w = alpha * p_deri[26];
			edge[p_key[26]] += w;
			w = alpha * p_deri[27];
			edge[p_key[27]] += w;
			w = alpha * p_deri[28];
			edge[p_key[28]] += w;
			w = alpha * p_deri[29];
			edge[p_key[29]] += w;

			w = alpha * p_deri[30];
			corner3_3[p_key[30]] += w;
			w = alpha * p_deri[31];
			corner3_3[p_key[31]] += w;
			w = alpha * p_deri[32];
			corner3_3[p_key[32]] += w;
			w = alpha * p_deri[33];
			corner3_3[p_key[33]] += w;

			w = alpha * p_deri[34];
			corner5_2[p_key[34]] += w;
			w = alpha * p_deri[35];
			corner5_2[p_key[35]] += w;
			w = alpha * p_deri[36];
			corner5_2[p_key[36]] += w;
			w = alpha * p_deri[37];
			corner5_2[p_key[37]] += w;
			w = alpha * p_deri[38];
			corner5_2[p_key[38]] += w;
			w = alpha * p_deri[39];
			corner5_2[p_key[39]] += w;
			w = alpha * p_deri[40];
			corner5_2[p_key[40]] += w;
			w = alpha * p_deri[41];
			corner5_2[p_key[41]] += w;

			/*w = alpha * p_deri[42];
			triangle[p_key[42]] += w;
			w = alpha * p_deri[43];
			triangle[p_key[43]] += w;
			w = alpha * p_deri[44];
			triangle[p_key[44]] += w;
			w = alpha * p_deri[45];
			triangle[p_key[45]] += w;*/

			w = alpha * p_deri[46];
			//pality[p_key[46]] += w;

		}
		/* 平均誤差の出力 */
		if(i % 1000 == 0)
		{
			fopen_s(&fp, TRACE_PATH, "a");
			fprintf_s(fp, "平均２乗誤差= %f\n", ave_error);
			fclose(fp);
			if(i % 5000 == 0)
			{
				Eval_Write();
			}
		}

	}
	fclose(fp);
}

int RandomMove(BitBoard moves)
{
	int move;
	int move_cnt = rand()%CountBit(moves);
	int pos;

	while(1)
	{
		pos = CountBit((moves & (-(signed long long int)moves)) - 1);
		if(move_cnt == 0)
		{
			move = pos;
			break;
		}
		else
		{
			move_cnt--;
		}
		moves &= moves - 1;
	}
	return move;
}

void Start_Auto_Learn(int depth, int end_turn, int count, int situation)
{
	return;
}


BOOL Start_File_Learn(char *filename)
{
	//OPENFILENAME ofn;
	//char filename[FILE_LEN];
	char buf[3];

	//filename[0] = '\0';
	//memset(&ofn, 0, sizeof(OPENFILENAME));
	//ofn.lStructSize = sizeof(OPENFILENAME);
	//ofn.lpstrFilter = "kifu file(*.new)\0*.new\0all file(*.*)\0*.*\0\0";
	//ofn.lpstrFile = filename;
	//ofn.nMaxFile = sizeof(filename);
	//ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	//ofn.lpstrDefExt = "new";

	//if(!GetOpenFileName(&ofn))
	//{
	//	return FALSE;
	//}

	int error;
	FILE *fp;
	char dir_path[256], file_path[256];
	strcpy_s(dir_path, "kifu\\");
	sprintf_s(file_path, "%s%s", dir_path, filename);

	if((error = fopen_s(&fp, file_path, "r")) != 0){
		return FALSE;
	}

	int i = 0, color = BLACK;
	BitBoard bk, wh, rev;
	BitBoard first_bk = 34628173824, first_wh = 68853694464;
	BitBoard temp_b[60], temp_w[60];
	int teban[60];
	int turn;
	int move, stone_diff, cnt = 0, bad_cnt = 0;
	bool bad_data = false, end_flag = false;
	//HDC hdc;

	//MessageBox(hWnd, "エラー", "エラー", MB_OK);
	/* ファイルの終端？ */
	while(1)
	{
		bk = first_bk;
		wh = first_wh;
		color = BLACK;
		turn = 0;
		/* スペース？ */
   		while(1)
		{
			/* 英字を読み込む */
			buf[0] = (char )fgetc(fp);
			/* オシマイ */
			if(buf[0] == EOF)
			{
				end_flag = true;
				break;
			}
			/* スペースなら定石との切れ目か石差 */
			if(buf[0] == ' ')
			{
				/* スペースの次が数字だった場合は抜ける */
				while((buf[0] = (char )fgetc(fp)) == ' ');
				if(isdigit(buf[0]) || buf[0] == '-')
				{
					break;
				}
				/* 続きがある場合はそのまま続行 */
			}
			else if(!isalpha(buf[0]))
			{
				bad_data = true;
				break;
			}
			/* 数字を読み込む */
			buf[1] = (char )fgetc(fp);

			/****** ここからエラー処理 ******/
			/* なぜかおかしな棋譜データが結構ある */
			if(!isdigit(buf[1]))
			{
				bad_data = true;
				break;
			}
			move = ConvertToMove_U(buf);
			/* a9 とか明らかに間違った手を含んでいる棋譜がある */
			if(move == -1)
			{
				bad_data = true;
				break;
			}
			/* なぜかすでに置いたマスに置いている棋譜がある */
			if(bk & (one << move) || wh & (one << move))
			{
				bad_data = true;
				break;
			}
			/****** ここまで ******/

			if(color == BLACK)
			{
				rev = GetRev[move](bk, wh);
				/* 黒パス？ */
				if(rev == 0)
				{
					rev = GetRev[move](wh, bk);
					/* 一応合法手になっているかのチェック */
					if(rev == 0)
					{
						bad_data = true;
						break;
					}
					bk ^= rev;
					wh ^= ((one << move) | rev);
				}
				else
				{
					bk ^= ((one << move) | rev);
					wh ^= rev;
					color ^= 1;
				}
			}
			else
			{
				rev = GetRev[move](wh, bk);
				/* 白パス？ */
				if(rev == 0)
				{
					rev = GetRev[move](bk, wh);
					/* 一応合法手になっているかのチェック */
					if(rev == 0)
					{
						bad_data = true;
						break;
					}
					bk ^= ((one << move) | rev);
					wh ^= rev;
				}
				else
				{
					bk ^= rev;
					wh ^= ((one << move) | rev);
					color ^= 1;
				}
			}

			turn++;
			/* 対局データ生成 */
			temp_b[(turn - 1)] = bk;
			temp_w[(turn - 1)] = wh;
			teban[(turn - 1)] = color;
		}

		if(end_flag == true)
		{
			break;
		}
		if(!bad_data)
		{
			/* 石差 */
			buf[1] = (char )fgetc(fp);
			buf[2] = (char )fgetc(fp);
			sscanf_s(buf, "%d", &stone_diff);

			for(i = 0; i < 56; i++)
			{
				init_index_board(temp_b[i], temp_w[i]);
				SaveModel(data_fp[i / 4], stone_diff, temp_b[i], temp_w[i], teban[i]);
			}

		}
		else
		{
			bad_data = false;
			bad_cnt++;
		}
		/* 次の棋譜までスキップ */
		while(fgetc(fp) != 0x0A);
		/*Sleep(500);
		black = bk;
		white = wh;
		hdc = GetDC(hWnd);
		DrawBoard(hWnd, hdc, hStatus, move / 8, move % 8);
		ReleaseDC(hWnd, hdc);*/
		cnt++;
	}
	char str[512];
	sprintf_s(str, "[%s]\ngame:%d, bad:%d", filename, cnt, bad_cnt);
	PostMessage(hStatus, SB_SETTEXT, (WPARAM)0 | 3, (LPARAM)str);
	fclose(fp);

	return TRUE;
}

//BOOL Start_File_Learn(char *filename)
//{
//	//OPENFILENAME ofn;
//	//char filename[FILE_LEN];
//	char buf[3];
//
//	//filename[0] = '\0';
//	//memset(&ofn, 0, sizeof(OPENFILENAME));
//	//ofn.lStructSize = sizeof(OPENFILENAME);
//	//ofn.lpstrFilter = "kifu file(*.new)\0*.new\0all file(*.*)\0*.*\0\0";
//	//ofn.lpstrFile = filename;
//	//ofn.nMaxFile = sizeof(filename);
//	//ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
//	//ofn.lpstrDefExt = "new";
//
//	//if(!GetOpenFileName(&ofn))
//	//{
//	//	return FALSE;
//	//}
//
//	int error;
//	FILE *fp;
//	char dir_path[256], file_path[256];
//	strcpy_s(dir_path, "kifu\\");
//	sprintf_s(file_path, "%s%s", dir_path, filename);
//
//	if((error = fopen_s(&fp, file_path, "r")) != 0){
//		return FALSE;
//	}
//
//	int i = 0, color = BLACK;
//	BitBoard bk, wh, rev;
//	BitBoard first_bk = 34628173824, first_wh = 68853694464;
//	BitBoard temp_b[60], temp_w[60];
//	int teban[14];
//	int turn;
//	int move, stone_diff, cnt = 0, bad_cnt = 0;
//	bool bad_data = false, end_flag = false;
//	//HDC hdc;
//
//	//MessageBox(hWnd, "エラー", "エラー", MB_OK);
//	/* ファイルの終端？ */
//	while(1)
//	{
//		bk = first_bk;
//		wh = first_wh;
//		color = BLACK;
//		turn = 0;
//		/* スペース？ */
//   		while(1)
//		{
//			/* 英字を読み込む */
//			buf[0] = (char )fgetc(fp);
//			/* オシマイ */
//			if(buf[0] == EOF)
//			{
//				end_flag = true;
//				break;
//			}
//			/* スペースなら定石との切れ目か石差 */
//			if(buf[0] == ' ')
//			{
//				/* スペースの次が数字だった場合は抜ける */
//				while((buf[0] = (char )fgetc(fp)) == ' ');
//				if(isdigit(buf[0]) || buf[0] == '-')
//				{
//					break;
//				}
//				/* 続きがある場合はそのまま続行 */
//			}
//			else if(!isalpha(buf[0]))
//			{
//				bad_data = true;
//				break;
//			}
//			/* 数字を読み込む */
//			buf[1] = (char )fgetc(fp);
//
//			/****** ここからエラー処理 ******/
//			/* なぜかおかしな棋譜データが結構ある */
//			if(!isdigit(buf[1]))
//			{
//				bad_data = true;
//				break;
//			}
//			move = ConvertToMove_U(buf);
//			/* a9 とか明らかに間違った手を含んでいる棋譜がある */
//			if(move == -1)
//			{
//				bad_data = true;
//				break;
//			}
//			/* なぜかすでに置いたマスに置いている棋譜がある */
//			if(bk & (one << move) || wh & (one << move))
//			{
//				bad_data = true;
//				break;
//			}
//			/****** ここまで ******/
//
//			if(color == BLACK)
//			{
//				rev = GetRev[move](bk, wh, one << move);
//				/* 黒パス？ */
//				if(rev == 0)
//				{
//					rev = GetRev[move](wh, bk, one << move);
//					/* 一応合法手になっているかのチェック */
//					if(rev == 0)
//					{
//						bad_data = true;
//						break;
//					}
//					bk ^= rev;
//					wh ^= ((one << move) | rev);
//				}
//				else
//				{
//					bk ^= ((one << move) | rev);
//					wh ^= rev;
//					color ^= 1;
//				}
//			}
//			else
//			{
//				rev = GetRev[move](wh, bk, one << move);
//				/* 白パス？ */
//				if(rev == 0)
//				{
//					rev = GetRev[move](bk, wh, one << move);
//					/* 一応合法手になっているかのチェック */
//					if(rev == 0)
//					{
//						bad_data = true;
//						break;
//					}
//					bk ^= ((one << move) | rev);
//					wh ^= rev;
//				}
//				else
//				{
//					bk ^= rev;
//					wh ^= ((one << move) | rev);
//					color ^= 1;
//				}
//			}
//			turn++;
//			/* 対局データ生成 */
//			if(turn % 4 == 0 && turn != 60)
//			{
//				temp_b[(turn - 1) / 4] = bk;
//				temp_w[(turn - 1) / 4] = wh;
//				teban[(turn - 1) / 4] = color;
//			}
//		}
//
//		if(end_flag == true)
//		{
//			break;
//		}
//		if(!bad_data)
//		{
//			/* 石差 */
//			buf[1] = (char )fgetc(fp);
//			buf[2] = (char )fgetc(fp);
//			sscanf_s(buf, "%d", &stone_diff);
//
//			for(i = 0; i < 14; i++)
//			{
//				init_index_board(temp_b[i], temp_w[i]);
//				SaveModel(data_fp[i], stone_diff, temp_b[i], temp_w[i], teban[i]);
//			}
//
//		}
//		else
//		{
//			bad_data = false;
//			bad_cnt++;
//		}
//		/* 次の棋譜までスキップ */
//		while(fgetc(fp) != 0x0A);
//		/*Sleep(500);
//		black = bk;
//		white = wh;
//		hdc = GetDC(hWnd);
//		DrawBoard(hWnd, hdc, hStatus, move / 8, move % 8);
//		ReleaseDC(hWnd, hdc);*/
//		cnt++;
//	}
//	char str[512];
//	sprintf_s(str, "[%s]\ngame:%d, bad:%d", filename, cnt, bad_cnt);
//	PostMessage(hStatus, SB_SETTEXT, (WPARAM)0 | 3, (LPARAM)str);
//	fclose(fp);
//
//	return TRUE;
//}

void Eval_Write()
{
	int error;
	FILE *fp;

	if((error = fopen_s(&fp, TABLE_NAME, "w")) != 0){
		return;
	}

	int i;
	for(i = 0; i < 6561; i++)
	{
		fprintf(fp, "%lf\n", hori_ver1[i]);
		//fprintf(fp, "%lf %d\n", hori_ver1[i], key_count_hori1[i]);
	}
	for(i = 0; i < 6561; i++)
	{
		fprintf(fp, "%lf\n", hori_ver2[i]);
		//fprintf(fp, "%lf\n", hori_ver2[i], key_count_hori2[i]);
	}
	for(i = 0; i < 6561; i++)
	{
		fprintf(fp, "%lf\n", hori_ver3[i]);
		//fprintf(fp, "%lf\n", hori_ver3[i], key_count_hori3[i]);
	}
	for(i = 0; i < 6561; i++)
	{
		fprintf(fp, "%lf\n", dia_ver1[i]);
		//fprintf(fp, "%lf\n", dia_ver1[i], key_count_dia1[i]);
	}
	for(i = 0; i < 2187; i++)
	{
		fprintf(fp, "%lf\n", dia_ver2[i]);
		//fprintf(fp, "%lf\n", dia_ver2[i], key_count_dia2[i]);
	}
	for(i = 0; i < 729; i++)
	{
		fprintf(fp, "%lf\n", dia_ver3[i]);
		//fprintf(fp, "%lf\n", dia_ver3[i], key_count_dia3[i]);
		
	}
	for(i = 0; i < 243; i++)
	{
		fprintf(fp, "%lf\n", dia_ver4[i]);
		//fprintf(fp, "%lf\n", dia_ver4[i], key_count_dia4[i]);
	
	}
	for(i = 0; i < 59049; i++)
	{
		fprintf(fp, "%lf\n", edge[i]);
		//fprintf(fp, "%lf\n", edge[i], key_count_edge[i]);
		
	}
	for(i = 0; i < 19683; i++)
	{
		fprintf(fp, "%lf\n", corner3_3[i]);
		//fprintf(fp, "%lf\n", corner5_2[i], key_count_corner5_2[i]);
		
	}
	for(i = 0; i < 59049; i++)
	{
		fprintf(fp, "%lf\n", corner5_2[i]);
		//fprintf(fp, "%lf\n", corner5_2[i], key_count_corner5_2[i]);
		
	}
	//for(i = 0; i < 59049; i++)
	//{
	//	fprintf(fp, "%lf\n", triangle[i]);
	//	//fprintf(fp, "%lf\n", triangle[i], key_count_triangle[i]);
	//	
	//}
	//for(i = 0; i < PALITY_NUM; i++)
	//{
	//	fprintf(fp, "%lf\n", pality[i]);
	//	//fprintf(fp, "%lf\n", pality[i], key_count_pality[i]);
	//}
	fclose(fp);
}
