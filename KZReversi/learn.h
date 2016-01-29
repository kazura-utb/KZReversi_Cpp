#pragma once

#define PATTERN_NUM 47
#define MAX 1896172

#define PROLOGUE 4
#define OPENING1 8
#define OPENING2 12
#define OPENING3 16
#define MIDDLE1  20
#define MIDDLE2  24
#define MIDDLE3  28
#define MIDDLE4  32
#define MIDDLE5  36
#define END1     40
#define END2     44
#define END3     48
#define END4     52
#define END5     56

#define STAGE "end5"
#define ALPHA_NUM 0.35

extern char DIR_PATH[64];
extern char TAIKYOKU_NAME[256];
extern char TABLE_NAME[256];
extern char MSEOUT_PATH[64];
extern char TRACE_PATH[64];
extern char kihu_list[74][32];
extern FILE *data_fp[14];

BOOL Start_File_Learn(char *filename);
void Start_Auto_Learn(int, int, int);
void CulcPatternEval(int sample_num);
void Eval_Write(void);
void ReadEvalDat_learn(char *, int);
int CPU_AI_LEARN(BitBoard, BitBoard, int, int, int, int, int);