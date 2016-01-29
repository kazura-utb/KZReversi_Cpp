#pragma once

#include "hash.h"

/* ordering ä÷êî */
int move_ordering(int[], BitBoard, BitBoard, BitBoard);
char move_ordering_middle(char *, BitBoard, BitBoard, Hash *, BitBoard, BitBoard[], 
	char depth, int, int, int);
char move_ordering_end(char *pos_list, BitBoard b_board, BitBoard w_board, 
	Hash *hash, BitBoard moves, BitBoard rev_list[], char depth);
char move_ordering_end(MOVELIST *, BitBoard, BitBoard, BitBoard);
int get_order_position(BitBoard);

/* É\Å[Égä÷êî */
//void sort_list(MOVELIST *, int[], int);
void sort_move_list(char *, int[], int);
void sort_move_list(MOVELIST *, int[], int);