//#ifndef MAIN_
//#define MAIN_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdio.h"
#include "evmdm6437.h"
#include "evmdm6437_dip.h"
#include "evmdm6437_led.h"
#include "Mat.h"
#include "Logic.h"
#include "dictionary.h"

extern Int16 video_loopback_test();

// size for buffer_in: 720 * 480 / 2, the reason is explained below. 
#define Pixels 172800
#define SCRAB_BOX 15
#define NEW_LETTER_THRESH 60
#define COM_THRESH 20000
#define ROW 0
#define COL 1
#define BOX_SIZE 20
#define NUM_FRAMES 5
#define OFFSET 2
// Resolution 720 * 480 (NTSC mode)
#define vWidth 720
#define vHeight 480
#define BufferNum 30
#define MAX_WORDSIZE 20

// Define a space on memory for save the information input and output (Interface data)
Uint32 buffer_out[Pixels]; //from 0x80000000
Uint32 buffer_in[Pixels]; //from 0x800A8C00, which is the same as 4 (bytes for integer) * Pixels
Mat frame, frame2, matTemp, matTemp2, diff;//, box;//, frameIn;

//OCR
#include "Netparams.h"
#include "Network.h"

short newLetterLoc[SCRAB_BOX][2];
char lettersRecognized[SCRAB_BOX];
short letterCount = 0;
short origin[2] = { 0, 0 };
short length = 0;
short dir;
// Intermediate buffer to hold RGB output
//Uint8 Buffer_input[vWidth*vHeight*3];

// Define internal buffer
//Uint32 internal_buffer1[Pixels / BufferNum];
//Uint8 internal_buffer2[2 * Pixels / BufferNum *3];

Square boardObj[SCRAB_BOX*SCRAB_BOX];
Board board = boardObj;

// Define the position of the data (refer to linker.cmd)
// Internal memory L2RAM ".l2ram" 
// External memory DDR2 ".ddr2"
#pragma DATA_SECTION(buffer_out,".ddr2")
#pragma DATA_SECTION(buffer_in,".ddr2")
//#pragma DATA_SECTION(Buffer_input, ".ddr2")
#pragma DATA_SECTION(frame, ".custom")
#pragma DATA_SECTION(frame2, ".custom")
//#pragma DATA_SECTION(box, ".custom")
#pragma DATA_SECTION(matTemp, ".custom")
#pragma DATA_SECTION(matTemp2, ".custom")
#pragma DATA_SECTION(diff, ".custom")
#pragma DATA_SECTION(Network_weights1, ".custom")
#pragma DATA_SECTION(Network_weights2, ".custom")
#pragma DATA_SECTION(Network_biases1, ".custom")
#pragma DATA_SECTION(Network_biases2, ".custom")
#pragma DATA_SECTION(TEXT_BITMAP, ".custom")
#pragma DATA_SECTION(boardObj, ".custom")
#pragma DATA_SECTION(dict, ".custom")


// buffer_in represents one input frame which consists of two interleaved frames.
// Each 32 bit data has the information for two adjacent pixels in a row.
// Thus, the buffer holds 720/2 integer data points for each row of 2D image and there exist 480 rows.
//
// Format: yCbCr422 ( y1 | Cr | y0 | Cb )
// Each of y1, Cr, y0, Cb has 8 bits
// For each pixel in the frame, it has y, Cb, Cr components
//
// You can generate a lookup table for color conversion if you want to convert to different color space such as RGB.
// Could refer to http://www.fourcc.org/fccyvrgb.php for conversion between yCbCr and RGB
// 


// Copy data from input buffer to output buffer
void Copy(void){
	Int32 i, j;
//	Uint32 temp;
	
	// Copy data from input buffer to output buffer and 
	// draw green square box at the center of view
	for(i=0; i<vHeight; ++i)
		for (j=0; j<vWidth/2; ++j) {
			buffer_out[i * vWidth/2 + j] = buffer_in[i * vWidth/2 + j];
			/*if ( i >= 220 && i < 260 && j >= 170 && j < 190 )
				buffer_out[i * vWidth/2 + j] = 0;*/
		}
}


void Calibrate(void){
	Int32 i, j, index=vHeight * (vWidth/2) - 1;
	Int32 top = 15;
	Int32 bottom = 385;
	Int32 thickness = 5;
	// Copy data from input buffer to output buffer and 
	// draw green horizontal lines
	for(i=0; i<vHeight; i++)
		for (j=0; j<vWidth/2; j++) {
			buffer_out[index] = buffer_in[i * vWidth/2 + j];
			if ( i >= top-thickness && i < top || i > bottom && i <= bottom+thickness )
				buffer_out[index] = 0;
			
			index--;
		}
}


void varsInit()
{
	/* Initialize BSL */
	EVMDM6437_init();
	
    /* Initialize the DIP Switches & LEDs if needed */
    EVMDM6437_DIP_init( );
    EVMDM6437_LED_init( );
    	
	// Initialize video input/output 
	video_loopback_test();
	
	Mat_init(&frame, 480, 720);
	Mat_init(&frame2, 480, 720);
	Mat_init(&diff, 480, 720);
	Mat_init(&matTemp, 480, 720);
	Mat_init(&matTemp2, 480, 720);
	
	int i, j;
	for(i=0; i < SCRAB_BOX; i++){
		for(j=0; j < 2; j++){
			lettersRecognized[i] = EMPTY;
			newLetterLoc[i][j] = 0;
		}
	}
}

Uint32 countPercent(const Mat* boxMatr)
{
	Uint32 count=0;
	Uint32 thresh = 10;
	Uint32 i, j;
	for (i = 0; i < BOX_SIZE; i++)
	{
		for (j = 0; j < BOX_SIZE; j++)
		{
			if (boxMatr->data[i*boxMatr->cols + j] > thresh)
				count++;
		}
	}
	return count;
}

Uint32 centerOfMass(const Mat* boxMatr)
{
	Mat_integralImage(boxMatr, &matTemp);
	Mat_varianceFilter(boxMatr, 5, &matTemp2, &matTemp);
	
	Uint32 i, j, offset = 2;
	//TOP ROWS
	for (i = 0; i < offset; i++)
		for (j = 0; j < BOX_SIZE; j++)
			matTemp2.data[i*BOX_SIZE+j] = 0;
	//BOTTOM ROWS
	for (i = BOX_SIZE-offset; i < BOX_SIZE; i++)
		for (j = 0; j < BOX_SIZE; j++)
			matTemp2.data[i*BOX_SIZE+j] = 0;
	//LEFT COLUMNS
	for (i = offset; i < BOX_SIZE-offset; i++)
		for (j = 0; j < offset; j++)
			matTemp2.data[i*BOX_SIZE+j] = 0;
	//RIGHT COLUMNS
	for (i = offset; i < BOX_SIZE-offset; i++)
		for (j = BOX_SIZE-offset; j < BOX_SIZE; j++)
			matTemp2.data[i*BOX_SIZE+j] = 0;
					
	return Mat_sum(&matTemp2);
}

void drawVertLine(Mat* image, int col){
	int i;
	for(i = 0 ; i < image->rows; i++)
		Mat_setAt(image, i, col, 255);
}
void drawHorizLine(Mat* image, int row){
	int i;
	for(i = 0 ; i < image->cols; i++)
		Mat_setAt(image, row, i, 255);
}

//Capture multiple frames and average - to remove noise
void captureFrame(Mat* theFrame, Mat* tempFrame){
	
	//Make sure dimensions are reset
	theFrame->rows = 480;
	theFrame->cols = 720;
	tempFrame->rows = 480;
	tempFrame->cols = 720;
	
	Mat_readGray(buffer_in, theFrame);
//	int i;
//	for(i = 1; i < NUM_FRAMES; i++){
//		Mat_readGray(buffer_in, tempFrame);
//		Mat_addMat(theFrame, tempFrame, theFrame); 
//	}
//	Mat_divScalar(theFrame, NUM_FRAMES, theFrame);

//	Mat_readGray(buffer_in, theFrame);
//	for(i = 0; i<10; i++){
//		Mat_readGray(buffer_in, tempFrame);
//		Mat_multScalar(tempFrame,3,tempFrame);
//		Mat_multScalar(theFrame,7,theFrame);
//		Mat_addMat(theFrame, tempFrame, theFrame); 
//		Mat_divScalar(theFrame, 10, theFrame);
//	}



}

int primaryWord(){

	// Change in the vertical direction because rows changed
	if (newLetterLoc[letterCount - 1][ROW] - newLetterLoc[0][ROW])
	{
		origin[COL] = newLetterLoc[0][COL];
		dir = ALONG_COL;
		// Existing characters at boundary of letters will get neglected
		// Check for those
		char existing_char;

		// Check all above the first
		int traverseIndex = newLetterLoc[0][ROW] - 1;
		if (newLetterLoc[0][ROW] != 0)
		{
			existing_char = Board_getAt(board, traverseIndex, newLetterLoc[0][COL]);
			while (existing_char != '-' && traverseIndex >= 0)
			{
				traverseIndex--;
				if (traverseIndex >= 0)
					existing_char = Board_getAt(board, traverseIndex, newLetterLoc[0][COL]);
			}
		}
		origin[ROW] = traverseIndex + 1;
		length = (newLetterLoc[letterCount - 1][ROW] - origin[ROW]) + 1;

		// Check all below the last
		traverseIndex = newLetterLoc[letterCount - 1][ROW] + 1;
		if (newLetterLoc[letterCount - 1][ROW] != (SCRAB_BOX - 1))
		{
			existing_char = Board_getAt(board, traverseIndex, newLetterLoc[0][COL]);
			while (existing_char != '-' && traverseIndex < SCRAB_BOX)
			{
				traverseIndex++;
				if (traverseIndex < SCRAB_BOX)
					existing_char = Board_getAt(board, traverseIndex, newLetterLoc[0][COL]);
			}
		}
		length = (traverseIndex - origin[ROW]);
	}

	// Change in the horizontal direction because columns changed
	else
	{
		origin[ROW] = newLetterLoc[0][ROW];
		dir = ALONG_ROW;
		// Existing characters at boundary of letters will get neglected
		// Check for those
		char existing_char;

		// Check all to the left of the first
		int traverseIndex = newLetterLoc[0][COL] - 1;
		if (newLetterLoc[0][COL] != 0)
		{
			existing_char = Board_getAt(board, newLetterLoc[0][ROW], traverseIndex);
			while (existing_char != '-' && traverseIndex >= 0)
			{
				traverseIndex--;
				if (traverseIndex >= 0)
					existing_char = Board_getAt(board, newLetterLoc[0][ROW], traverseIndex);
			}
		}
		origin[COL] = traverseIndex + 1;
		length = (newLetterLoc[letterCount - 1][COL] - origin[COL]) + 1;

		// Check all to the left of the last
		traverseIndex = newLetterLoc[letterCount - 1][COL] + 1;
		if (newLetterLoc[letterCount - 1][COL] != (SCRAB_BOX - 1))
		{
			existing_char = Board_getAt(board, newLetterLoc[0][ROW], traverseIndex);
			while (existing_char != '-' && traverseIndex < SCRAB_BOX)
			{
				traverseIndex++;
				if (traverseIndex < SCRAB_BOX)
					existing_char = Board_getAt(board, newLetterLoc[0][ROW], traverseIndex);
			}
		}
		length = (traverseIndex - origin[COL]);
	}
	int i,j;
	int toAddRow = dir == ALONG_COL ? 1 : 0;
	int toAddCol = dir == ALONG_ROW ? 1 : 0;
	char word[16];
	for (i=0, j=0; i<length; i++){
		char c = Board_getAt(board, origin[ROW]+toAddRow*i, origin[COL]+toAddCol*i);
		if(c == EMPTY){
			word[i] =  lettersRecognized[j];
			j++;
		}
		else
			word[i] = c;
	}
	word[i] = '\0';
		
	int score = 0;
	short validWord = findword(word);
	if (validWord == 1){
		Mat_writeWord(&frame2, "INVALID", 7, 20, 50);
		score = 0;
	}
	else
	{
//		Mat_writeWord(&frame2, "VALID  ", 7, 50, 50);
		for (i=0; i<letterCount; i++){
			Board_setAt(board, newLetterLoc[i][ROW], newLetterLoc[i][COL], lettersRecognized[i]);
		}
		score = Board_getScore(board, origin[ROW], origin[COL], length, dir);
//		score += secondaryWord();
	}
	
	return score;
}

// PRE: To be called after primaryWord()
int secondaryWord(){
	int score = 0;
	int branchLength = 0;
	if (dir == ALONG_ROW)
	{
		char existing_char;
		int i;
		for (i = 0; i < letterCount; i++){

			// Check all above current
			int branchStart = newLetterLoc[i][ROW];
			int traverseIndex;
			if (newLetterLoc[i][ROW] != 0)
			{
				traverseIndex = branchStart - 1;
				existing_char = Board_getAt(board, traverseIndex, newLetterLoc[i][COL]);
				while (existing_char != EMPTY && traverseIndex >= 0)
				{
					branchStart = traverseIndex;
					traverseIndex--;
					if (traverseIndex >= 0)
						existing_char = Board_getAt(board, traverseIndex, newLetterLoc[i][COL]);
				}
			}

			// Check all below current
			int branchEnd = newLetterLoc[i][ROW];
			if (newLetterLoc[i][ROW] != (SCRAB_BOX - 1))
			{
				traverseIndex = branchEnd + 1;
				existing_char = Board_getAt(board, traverseIndex, newLetterLoc[i][COL]);
				while (existing_char != EMPTY && traverseIndex < SCRAB_BOX)
				{
					branchEnd = traverseIndex;
					traverseIndex++;
					if (traverseIndex < SCRAB_BOX)
						existing_char = Board_getAt(board, traverseIndex, newLetterLoc[i][COL]);
				}
			}
			branchLength = (branchEnd - branchStart);
			if (branchLength){
				char word[16];
				char* wordPtr = Board_getWord(board, branchStart, newLetterLoc[i][COL], branchLength+1, ALONG_COL);
				int k;
				for (k=0; k<(branchLength+1); k++){
					word[k] = wordPtr[k];
				}
				word[k] = '\0';
			
				short validWord = findword(word);
				if (validWord == 1){
					Mat_writeWord(&frame2, "INVALID", 7, 20, 50);
					score = -1;
					return score;
				}
				else{
					score += Board_getScore(board, branchStart, newLetterLoc[i][COL], branchLength+1, ALONG_COL);
				}
			}
		}
	}

	else
	{
		char existing_char;
		int i;
		for (i = 0; i < letterCount; i++){

			// Check all to the left of current
			int branchStart = newLetterLoc[i][COL];
			int traverseIndex;
			if (newLetterLoc[i][COL] != 0)
			{
				traverseIndex = branchStart - 1;
				existing_char = Board_getAt(board, newLetterLoc[i][ROW], traverseIndex);
				while (existing_char != EMPTY && traverseIndex >= 0)
				{
					branchStart = traverseIndex;
					traverseIndex--;
					if (traverseIndex >= 0)
						existing_char = Board_getAt(board, newLetterLoc[i][ROW], traverseIndex);
				}
			}

			// Check all to the right of current
			int branchEnd = newLetterLoc[i][COL];
			if (newLetterLoc[i][COL] != (SCRAB_BOX - 1))
			{
				traverseIndex = branchEnd + 1;
				existing_char = Board_getAt(board, newLetterLoc[i][ROW], traverseIndex);
				while (existing_char != EMPTY && traverseIndex < SCRAB_BOX)
				{
					branchEnd = traverseIndex;
					traverseIndex++;
					if (traverseIndex < SCRAB_BOX)
						existing_char = Board_getAt(board, newLetterLoc[i][ROW], traverseIndex);
				}
			}

			branchLength = (branchEnd - branchStart);
			if (branchLength){	
				char word[16];
				char* wordPtr = Board_getWord(board, newLetterLoc[i][ROW], branchStart, branchLength+1, ALONG_ROW);
				int k;
				for (k=0; k<(branchLength+1); k++){
					word[k] = wordPtr[k];
				}
				word[k] = '\0';
				
				short validWord = findword(word);
				if (validWord == 1){
					Mat_writeWord(&frame2, "INVALID", 7, 50, 50);
					score = -1;
					return score;
				}
				else{
					score += Board_getScore(board, newLetterLoc[i][ROW], branchStart, branchLength+1, ALONG_ROW);
				}
			}
		}
	}
	
	return score;
}

//#endif /*UTILITY_*/
