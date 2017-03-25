#include "main.h"

#define PLAYER_ONE 0
#define PLAYER_TWO 1
typedef struct{
	int turn;
	int P1_score;
	int P2_score;
}GameState;

void main( void )
 { 
 	
// 	char static_array[20];
//	setvbuf(stdout, static_array, _IOLBF, sizeof(static_array));

	GameState gameState;
	gameState.turn = PLAYER_ONE;
	gameState.P1_score = 0;
	gameState.P2_score = 0;
	
	varsInit();
	Int16 dip0, dip1, dip2, dip3;
	short singleOccur = 1;
	int corners[8];
	int peaks[32];
	int* vPeaks = peaks;
	int* hPeaks = peaks+16;
	int status;
//	
//	Mat_readGray(buffer_in, &frame);
//	drawVertLine(&frame, 550);
//	drawVertLine(&frame, 650);
//	drawHorizLine(&frame, 380);
//	Mat_writeGray(&frame, buffer_out);
	//Dictionary
//	Mat_readGray(buffer_in, &frame);
//			
//	char find[MAXWORDSIZE];
//	strcpy(find,"TARUNA");
//    	if (findword(find))
////			printf("Not Valid\n");
//			Mat_writeWord(&frame, "INVALID", 7, 50, 50);
//		else
//			Mat_writeWord(&frame, "VALID", 5, 50, 50);
////			printf("Valid\n");
//	Mat_writeGray(&frame, buffer_out);
	
        
    while(EVMDM6437_DIP_get( DIP_3 ) == DIP_DOWN){
    	Calibrate();
    }
    Uint32 response;
    // Run different procedures depending on the DIP switches pressed.
    	
	singleOccur = 0;
	captureFrame(&frame, &matTemp);
	Mat_writeGray(&frame, buffer_out);
	
	Mat_colorFilter(buffer_in, &matTemp);
	Mat_integralImage(&matTemp, &matTemp2);
	
	Mat_readGray(buffer_in, &frame);
	
	response = Mat_maxResponse(&matTemp, 21, 0, 50, 200, 200, &matTemp2);
	corners[0] = response/720;
	corners[1] = response%720;
	
	response = Mat_maxResponse(&matTemp, 21, 0, 500, 200, 200, &matTemp2);
	corners[2] = response/720;
	corners[3] = response%720;
	
	response = Mat_maxResponse(&matTemp, 21, 300, 50, 180, 200, &matTemp2);
	corners[4] = response/720;
	corners[5] = response%720;
	
	
	response = Mat_maxResponse(&matTemp, 21, 300, 500, 180, 200, &matTemp2);
	corners[6] = response/720;
	corners[7] = response%720;
		
	Mat_readGray(buffer_in, &frame);
	Mat_integralImage(&frame, &matTemp);
	Peak_findPeaks(&frame, corners, vPeaks, hPeaks, &matTemp);
	
	int i;
	for(i = 0; i < 16; i++){
		drawVertLine(&frame, vPeaks[i]);
		drawHorizLine(&frame, hPeaks[i]);
	}
	Mat_writeGray(&frame, buffer_out);
	_waitusec(500000);
	captureFrame(&frame, &matTemp);
	Board_init(board, peaks);	
    
   	char turn[] = "P1 ";
	turn[1] = gameState.turn == PLAYER_ONE ? '1' : '2'; 
	Mat_writeWord(&frame, turn, 3, 50, 50);
	Mat_writeWord(&frame, "GREEN WHEN READY", 16, 50, 110);
	Mat_writeGray(&frame, buffer_out);
	
	do{
		status = Mat_colorBox(buffer_in);
	}while(status != GREEN);
	_waitusec(500000);		
	
	while (1){		
        /* Will return DIP_DOWN or DIP_UP */
//        dip0 = EVMDM6437_DIP_get( DIP_0 );
//        dip1 = EVMDM6437_DIP_get( DIP_1 );
//        dip2 = EVMDM6437_DIP_get( DIP_2 );
//        dip3 = EVMDM6437_DIP_get( DIP_3 );

        // Get 1st frame
//        if ( EVMDM6437_DIP_get( DIP_1 ) == DIP_DOWN )
//        {
//        	
//        	Mat_writeGray(&frame, buffer_out);
//        	while(  EVMDM6437_DIP_get( DIP_1 ) == DIP_DOWN);
//        }

        // Get 2nd frame and find absolute difference from the first
        if ( EVMDM6437_DIP_get( DIP_2 ) == DIP_DOWN )
        {
        	//reset dimensions
        	captureFrame(&frame2, &matTemp);
        	
        	Mat_absDiff(&frame, &frame2, &diff);
        	letterCount = 0;
        	Uint32 startRow, startCol;
        	
			Mat* box = Mat_alloc(20,20);
			Mat* boxBinary = Mat_alloc(20,20);
			Mat* boxIntegral = Mat_alloc(20,20);
			
			int i,j;
			for (i = 0; i<SCRAB_BOX; i++)
			{
				for (j = 0; j<SCRAB_BOX; j++)
				{	
					startRow = board[i*SCRAB_BOX + j].tr + OFFSET;
					startCol = board[i*SCRAB_BOX + j].tc + OFFSET;
					int isEmpty = Board_getAt(board, i,j) == EMPTY;
					if (isEmpty)
					{
						Mat_getSubMat(&diff, startRow, startCol, BOX_SIZE, BOX_SIZE, box);
						Uint32 boxPercent = countPercent(box);
						
						Uint32 varSum = centerOfMass(box);
	//					printf("(%d, %d): %d, %d\n", i,j,boxPercent,varSum);
						if (boxPercent > NEW_LETTER_THRESH && varSum > COM_THRESH)
						{
							newLetterLoc[letterCount][ROW] = i;
							newLetterLoc[letterCount][COL] = j;
	
							Int32 ocrOutputs[78];
							int g, pos;
							for(g = -1, pos = 0; g < 2; g++, pos += 26){
								Mat_getSubMat(&frame2, startRow, startCol+g, BOX_SIZE, BOX_SIZE, box);
								Mat_integralImage(box, boxIntegral);
					        	Mat_adaptiveThresholdMeanInv(box, 19, 20, boxBinary, boxIntegral);
								Network_ocr(boxBinary->data, ocrOutputs+pos);
							}
							
							int max_index = 0;
							for(g = 0; g < 26; g++){
								ocrOutputs[g] = ocrOutputs[g]+ocrOutputs[g+26]+ocrOutputs[g+52];
								if(ocrOutputs[g] > ocrOutputs[max_index])
									max_index = g;
							}
				        	lettersRecognized[letterCount] = ALPHA[max_index];
	//						printf("%c, (%d, %d)\n", lettersRecognized[letterCount], i, j);
							Mat_writeLetter(&frame2, lettersRecognized[letterCount], startRow, startCol);
							Mat_writeGray(&frame2, buffer_out);
							letterCount++;
						//	Board_setAt(board, i, j, letterRecognized);
						}
					}
				}
			}
			Mat_free(&box);
			Mat_free(&boxBinary);
			Mat_free(&boxIntegral);		
			
			Mat_writeWord(&frame2, "IS THIS CORRECT", 15, 50, 50);
			Mat_writeGray(&frame2, buffer_out);
			
						
			do{
				status = Mat_colorBox(buffer_in);
			}while(status == NONE);
			
			if(status == RED){
				Mat_writeWord(&frame2, "NO             ", 15, 50, 50);
				Mat_writeGray(&frame2, buffer_out);
				while(Mat_colorBox(buffer_in) == RED);
				letterCount = 0;
			}
			else if(status == GREEN){
				Mat_writeWord(&frame2, "YES            ", 15, 50, 50);
				Mat_writeGray(&frame2, buffer_out);
				while(Mat_colorBox(buffer_in) == GREEN);
			}
			_waitusec(1000000);	
			
			
			if(letterCount > 0){
				int score = primaryWord();
				if (score > 0){
					int secondaryScore = secondaryWord();
					if(secondaryScore != -1){
						score += secondaryScore;
						Mat_writeNumber(&frame2, score, 20, 230);
						Mat_writeWord(&frame2,"POINTS -", 8, 20, 50);
						
						captureFrame(&frame, &matTemp);
					
						if(gameState.turn == PLAYER_ONE) gameState.P1_score += score;
						else if(gameState.turn == PLAYER_TWO) gameState.P2_score += score;
						gameState.turn = (gameState.turn == PLAYER_ONE) ? PLAYER_TWO : PLAYER_ONE;
					}
					else{
						int i;
						for (i=0; i<letterCount; i++){
							Board_setAt(board, newLetterLoc[i][ROW], newLetterLoc[i][COL], EMPTY);
						}
					}
				} 
			}
			
			turn[1] = gameState.turn == PLAYER_ONE ? '1' : '2'; 
			Mat_writeWord(&frame2, turn, 3, 50, 50);
			
			
//			Mat_writeWord(&frame2, "GREEN WHEN READY", 16, 200, 50);

			Mat_writeWord(&frame2, "GREEN WHEN READY", 16, 50, 110);
			
			Mat_writeWord(&frame2, "P1", 2, 100, 600);
			Mat_writeNumber(&frame2, gameState.P1_score, 130, 600);
			Mat_writeWord(&frame2, "P2", 2, 200, 600);
			Mat_writeNumber(&frame2, gameState.P2_score, 230, 600);
			
			
			Mat_writeGray(&frame2, buffer_out);
//			captureFrame(&frame, &matTemp);
			// Add new letters here
			
			do{
				status = Mat_colorBox(buffer_in);
			}while(status != GREEN);
//			do{
//				_waitusec(10000);	
//			}while(  EVMDM6437_DIP_get( DIP_1 ) == DIP_UP);
			_waitusec(500000);			
		}
	}
}
