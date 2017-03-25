//
//  Logic.h
//  ProjectCV
//
//  Created by Terna Kpamber on 2/25/16.
//  Copyright © 2016 Terna Kpamber. All rights reserved.
//

#ifndef Logic_h
#define Logic_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <assert.h>

#define ALONG_ROW 0
#define ALONG_COL 1

#define N_M -1
#define D_W 0
#define T_W 1
#define D_L 2
#define T_L 3
#define EMPTY '-'
const int LETTER_SCORES[] = {1,3,3,2,1, 4,2,4,1,8, 5,1,3,1,1, 3,10,1,1,1, 1,4,4,8,4, 10};
//							 A,B,C,D,E  F,G,H,I,J, k,L,M,N,O, P,Q,R,S,T,  U,V,W,X,Y, Z

/**
 * Square represents tile locations
 */
typedef struct {
	int tr; //the top-left pixels's row
	int tc; //the top-left pixels's column
	char letter; //the letter at the tile location
	int multiplier;
} Square;

typedef Square* Board; //array of squares

/**
 * [Board_init sets board up empty]
 * @param board [Board array to set-up]
 */
void Board_init(Board board, int* peaks){
	int i, j;
	int* vPeaks = peaks;
	int* hPeaks = peaks+16;
	int multipliers[] = {
		T_W,N_M,N_M,D_L,N_M,  N_M,N_M,T_W,N_M,N_M,  N_M,D_L,N_M,N_M,T_W,
		N_M,D_W,N_M,N_M,N_M,  T_L,N_M,N_M,N_M,T_L,  N_M,N_M,N_M,D_W,N_M,
		N_M,N_M,D_W,N_M,N_M,  N_M,D_L,N_M,D_L,N_M,  N_M,N_M,D_W,N_M,N_M,
		D_L,N_M,N_M,D_W,N_M,  N_M,N_M,D_L,N_M,N_M,  N_M,D_W,N_M,N_M,D_L,
		N_M,N_M,N_M,N_M,D_W,  N_M,N_M,N_M,N_M,N_M,  D_W,N_M,N_M,N_M,N_M,
		
		N_M,T_L,N_M,N_M,N_M,  T_L,N_M,N_M,N_M,T_L,  N_M,N_M,N_M,T_L,N_M,
		N_M,N_M,D_L,N_M,N_M,  N_M,D_L,N_M,D_L,N_M,  N_M,N_M,D_L,N_M,N_M,
		T_W,N_M,N_M,D_L,N_M,  N_M,N_M,N_M,N_M,N_M,  N_M,D_L,N_M,N_M,T_W,
		N_M,N_M,D_L,N_M,N_M,  N_M,D_L,N_M,D_L,N_M,  N_M,N_M,D_L,N_M,N_M,
		N_M,T_L,N_M,N_M,N_M,  T_L,N_M,N_M,N_M,T_L,  N_M,N_M,N_M,T_L,N_M,
		
		N_M,N_M,N_M,N_M,D_W,  N_M,N_M,N_M,N_M,N_M,  D_W,N_M,N_M,N_M,N_M,
		D_L,N_M,N_M,D_W,N_M,  N_M,N_M,D_L,N_M,N_M,  N_M,D_W,N_M,N_M,D_L,
		N_M,N_M,D_W,N_M,N_M,  N_M,D_L,N_M,D_L,N_M,  N_M,N_M,D_W,N_M,N_M,
		N_M,D_W,N_M,N_M,N_M,  T_L,N_M,N_M,N_M,T_L,  N_M,N_M,N_M,D_W,N_M,
		T_W,N_M,N_M,D_L,N_M,  N_M,N_M,T_W,N_M,N_M,  N_M,D_L,N_M,N_M,T_W
	};
	
	int mul = 0;
	for(i = 0 ; i < 15; i++){
		for(j = 0; j < 15; j++, mul++){
			board[i * 15 + j].tr = hPeaks[i];
			board[i * 15 + j].tc = vPeaks[j];
			board[i * 15 + j].letter = EMPTY;
			board[i * 15 + j].multiplier = multipliers[mul];
		}
	}
}


/**
 * [Board_print friendly representation of the board]
 * @param board [Board array]
 */
void Board_print(Board board){
	int i, j;
	
	printf("\t0\t");
	for(j = 1 ; j < 15; j++){
		printf("%d\t", j);
	}
	puts("");
	for(i = 0 ; i < 15; i++){
		printf("%2d:\t", i);
		for(j = 0; j < 15; j++){
			printf("%c\t", board[i * 15 + j].letter);
		}
		puts("");
	}
}


/**
 * [Board_setAt set a letter at a row-column location]
 * @param board [Board array]
 * @param row   [row]
 * @param col   [column]
 * @param letter     [the letter]
 */
void Board_setAt(Board board, int row, int col, char letter){
	board[row * 15 + col].letter = letter;
}


/**
* [Board_getAt get the letter at a row-column location]
* @param board [Board array]
* @param row   [row]
* @param col   [column]
* @return      [the value at the element]
*/
char Board_getAt(Board board, int row, int col){
	return board[row * 15 + col].letter;
}

/**
 * [Board_placeWord place a whole word unto the board]
 * @param board     [Board array]
 * @param word      [the word]
 * @param startRow  [origin row of the word]
 * @param startCol  [origin column of the word]
 * @param length    [length of the word]
 * @param direction [word direction: ALONG_ROW or ALONG_COL]
 */
void Board_placeWord(Board board, char* word, int startRow, int startCol, int length, int direction){
	int i,j;
	int startIndex = startRow * 15 + startCol;
	
	/** ASSERT BLOCK **/
//	int assertMult = direction == ALONG_ROW ? 1 : 15;
//	assert(startRow < 15 && startCol < 15);
//	assert(startIndex + (length-1) * assertMult < 225);
	/** END ASSERT BLOCK **/
	
	int toAdd = direction == ALONG_ROW ? 1 : 15;
	for(i = startIndex, j = 0 ; j < length; i += toAdd, j++){
		board[i].letter = word[j];
	}
	
}

int Board_getScore(Board board, int startRow, int startCol, int length, int direction){
	int toAdd = direction == ALONG_ROW ? 1 : 15;
	int startIndex = startRow * 15 + startCol;
	int score = 0;
	int multiplier = 1;
	int i,j;
	for(i = startIndex, j = 0 ; j < length; i += toAdd, j++){
		int alphaIndex = board[i].letter - 'A';
		switch(board[i].multiplier){
			case D_W: multiplier *= 2; score += LETTER_SCORES[alphaIndex]; break;
			case T_W: multiplier *= 3; score += LETTER_SCORES[alphaIndex]; break;
			case D_L: score += LETTER_SCORES[alphaIndex]*2; break;
			case T_L: score += LETTER_SCORES[alphaIndex]*3; break;
			default: score += LETTER_SCORES[alphaIndex];
		}
	}
	score *= multiplier;
	
	return score;
}

char* Board_getWord(Board board, int startRow, int startCol, int length, int direction){
	char* word;
//	word = "YEAR";

	int i,j;
	int toAddRow = direction == ALONG_COL ? 1 : 0;
	int toAddCol = direction == ALONG_ROW ? 1 : 0;
	static char str[16];
	for (i=0, j=0; i<length; i++){
		char c = Board_getAt(board, startRow+toAddRow*i, startCol+toAddCol*i);
		str[i] = c;
	}
	str[i] = '\0';
	word = &str[0];
	return word;
}

#endif /* Logic_h */
