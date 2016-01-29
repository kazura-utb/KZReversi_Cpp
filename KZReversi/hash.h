#pragma once

/*
##############################################
#
# 置換表関連の関数
#  
##############################################
*/

#include "stdafx.h"
#include "KZReversi.h"

/* 置換表定義 */
struct HashInfo
{
	BitBoard b_board;
	BitBoard w_board;
	int lower;
	int upper;
	char bestmove;
	char depth;
	char move_cnt;
	char locked;
	char bestLocked;
};

struct Hash
{
	int num;
	HashInfo *data;
	int getNum;
	int hitNum;
};

#define PREPARE_LOCKED 3
#define LOCKED 2

static void HashFinalize(Hash *hash);
void HashDelete(Hash *hash);

void HashClear(Hash *hash);

static int HashInitialize(Hash *hash, int in_size);
Hash *HashNew(int in_size);

void HashSet(Hash *hash, int hashValue, const HashInfo *in_info);
int HashGet(Hash *hash, int hashValue, BitBoard b_board, BitBoard w_board, HashInfo *out_info);
void HashClearInfo(Hash *hash);
int HashCountGet(Hash *hash);
int HashCountHit(Hash *hash);