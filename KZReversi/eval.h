#pragma once

typedef struct { 
	int code;
	int codeSize;
}CodeInfo;

struct TreeNode{
	char chr;
	int occurrence;
	int parent;
	int left;
	int right;
};

int decodeBookData(UCHAR *decodeData);
int decodeEvalData(UCHAR *evalData, FILE *fp);
void makeData();