/*
##############################################
#
# ’uŠ·•\ŠÖ˜A‚ÌŠÖ”
#  
##############################################
*/

#include "stdafx.h"
#include "KZReversi.h"
#include "hash.h"
#include "rotate_and_symmetry.h"

int freeFlag = TRUE;

static void HashFinalize(Hash *hash)
{
	if(freeFlag == TRUE){
		return;
	}
	if (hash->data) {
		free(hash->data);
	}
	freeFlag = TRUE;
}

void HashDelete(Hash *hash)
{
	HashFinalize(hash);
	free(hash);
}

void HashClear(Hash *hash)
{
	memset(hash->data, 0, sizeof(HashInfo) * hash->num);
	hash->getNum = 0;
	hash->hitNum = 0;
}

static int HashInitialize(Hash *hash, int in_size)
{
	memset(hash, 0, sizeof(Hash));
	hash->num = 1 << in_size;
	hash->data = (HashInfo *)malloc(sizeof(HashInfo) * hash->num);
	if (!hash->data) {
		return FALSE;
	}

	HashClear(hash);

	return TRUE;
}

Hash *HashNew(int in_size)
{
	Hash *hash;
	freeFlag = FALSE;
	hash = (Hash *)malloc(sizeof(Hash));
	if (hash) {
		if (!HashInitialize(hash, in_size)) {
			HashDelete(hash);
			hash = NULL;
		}
	}
	return hash;
}

void HashSet(Hash *hash, int hashValue, const HashInfo *in_info)
{
	memcpy(&hash->data[hashValue], in_info, sizeof(HashInfo));
}

int HashGet(Hash *hash, int hashValue, BitBoard b_board, BitBoard w_board, HashInfo *out_info)
{
	if(hash == NULL){
		return FALSE;
	}
	//hash->getNum++;
	if (hash->data[hashValue].b_board == b_board && hash->data[hashValue].w_board == w_board) 
	{
		memcpy(out_info, &hash->data[hashValue], sizeof(HashInfo));
		//hash->hitNum++;
		return TRUE;
	}
	else if(hash->data[hashValue].bestLocked == LOCKED){
		return 2;
	}
	else if(hash->data[hashValue].bestLocked == PREPARE_LOCKED){
		return 3;
	}
	return FALSE;
}

void HashClearInfo(Hash *hash)
{
	hash->getNum = 0;
	hash->hitNum = 0;
}

int HashCountGet(Hash *hash)
{
	return hash->getNum;
}

int HashCountHit(Hash *hash)
{
	return hash->hitNum;
}