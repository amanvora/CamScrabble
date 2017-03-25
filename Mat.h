#ifndef MAT_
#define MAT_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TextBitmap.h"
//#include <assert.h>

#define PEAK_ARR_LEN 480
#define RED 2
#define GREEN 1
#define NONE 0

typedef Int32 int32;
// typedef unsigned long Uint32;

/**
 * Mat struct.
 */
typedef struct{
	Uint32* data;
	Uint32 rows;
	Uint32 cols;
} Mat;

/** PROTOTYPES **/

void Mat_init(Mat* mat, Uint32 rows, Uint32 cols);
void Mat_free(Mat** mat);
Mat* Mat_alloc(int rows, int cols);
void Mat_constant(Mat* mat, Uint32 constant);

void Mat_addMat(const Mat* mat1, const Mat* mat2, Mat* result);
void Mat_subtMat(const Mat* mat1, const Mat* mat2, Mat* result);
void Mat_absDiff(const Mat* mat1, const Mat* mat2, Mat* result);
void Mat_multMat(const Mat* mat1, const Mat* mat2, Mat* result);
void Mat_divMat(const Mat* mat1, const Mat* mat2, Mat* result);


void Mat_addScalar(const Mat* mat1, Uint32 scalar, Mat* result);
void Mat_subtScalar(const Mat* mat1, Uint32 scalar, Mat* result);
void Mat_multScalar(const Mat* mat1, Uint32 scalar, Mat* result);
void Mat_divScalar(const Mat* mat1, Uint32 scalar, Mat* result);

Uint32 Mat_bufferInGetAt(Uint32 bufferIn[], Uint32 row, Uint32 col);
Uint32 Mat_getAt(const Mat* mat, Uint32 row, Uint32 col);
void Mat_setAt(Mat* mat, Uint32 row, Uint32 col, Uint32 value);
void Mat_getSubMat(const Mat* mat, Uint32 row_start, Uint32 col_start, Uint32 num_rows, Uint32 num_cols, Mat* result);
void Mat_overWriteSubMat(const Mat* boxMat, Uint32 row_start, Uint32 col_start, Uint32 num_rows, Uint32 num_cols, Mat* mat);
Uint32 Mat_sum(const Mat* mat);

void Mat_writeGray(const Mat* mat, Uint32 bufferOut[]);
void Mat_readGray(Uint32* bufferIn, Mat* output);

void Mat_integralImage(const Mat* source, Mat* dest);
void Mat_adaptiveThresholdMeanInv(const Mat* gray, int ksize, int meanC, Mat* result, Mat* integralTemp);
void Mat_varianceFilter(const Mat* gray, int ksize, Mat* result, Mat* integralTemp);
Uint32 Mat_maxResponse(const Mat* gray, int winSize, int rowStart, int colStart, int numRows, int numCols, const Mat* integralImg);
void Mat_colorFilter(Uint32* bufferIn, Mat* filtered);
void Mat_mapping(int value[4][2], int row, int col, int *rowLoc, int *colLoc);
void Mat_writeLetter(Mat* gray, char letter, int row, int col);
void Mat_writeNumber(Mat* gray, int number, int row, int col);
void Mat_writeScores(Mat* gray, int p1Score, int p2Score);
void Mat_writeWord(Mat* gray, char* word, int len, int row, int col);


Uint32 Mat_integralSum(const Mat* integralImg, int start_row, int startCol, int height, int width);
void Peak_averageFilter(int* hist, int* result, int len, int winSize);
void Peak_subtract(int* left, int* right, int* result, int* maxVal, int len, int zeroLessZero);
void Peak_findPeaks(const Mat* frame, int* corners, int* vPeaks, int* hPeaks, Mat* integral);



/** DEFINITIONS **/

void Mat_init(Mat* mat, Uint32 rows, Uint32 cols){
	mat->data = calloc(rows*cols, sizeof(Uint32));
	if(mat->data == NULL)
		abort();
	mat->rows = rows;
	mat->cols = cols;
}

Mat* Mat_alloc(int rows, int cols){
	Mat* mat = calloc(rows*cols, sizeof(Mat));
	if(mat == NULL)
		abort();
	Mat_init(mat, rows, cols);
	return mat;
}

void Mat_free(Mat** mat){
	free((*mat)->data);
	free((*mat));
}

/**
 * [Mat_Mat constuctor with constant]
 * @param  r [number of rows]
 * @param  c [number of columns]
 * @return   [pointer to Mat]
 */
void Mat_constant(Mat* mat, Uint32 constant){
	Uint32 i;
	for(i = 0 ; i < mat->rows*mat->cols; i++){
		mat->data[i] = constant;
	}
}

/**
 * [Mat_addMat sum two matrices]
 * @param mat1   [the first matrix]
 * @param mat2   [the second matric]
 * @param result [the result]
 */
void Mat_addMat(const Mat* mat1, const Mat* mat2, Mat* result){
//	assert(mat1->rows == mat2->rows && mat1->cols == mat2->cols);
	
	Uint32 i;
	Uint32* data = result->data;
	for(i = 0 ; i < mat1->rows * mat1->cols; i++){
		data[i] = mat1->data[i] + mat2->data[i];
	}
}


/**
 * [Mat_subtMat subtract a second matrix from a first]
 * @param mat1   [the first matrix]
 * @param mat2   [the second matric]
 * @param result [the result]
 */
void Mat_subtMat(const Mat* mat1, const Mat* mat2, Mat* result){
//	assert(mat1->rows == mat2->rows && mat1->cols == mat2->cols);
	
	Uint32 i;
	Uint32* data = result->data;
	for(i = 0 ; i < mat1->rows * mat1->cols; i++){
		data[i] = mat1->data[i] - mat2->data[i];
	}
}

/**
 * [Mat_absDiff Get absolute difference between two matrices]
 * @param mat1   [the first matrix]
 * @param mat2   [the second matric]
 * @param result [the result]
 */
void Mat_absDiff(const Mat* mat1, const Mat* mat2, Mat* result){
//	assert(mat1->rows == mat2->rows && mat1->cols == mat2->cols);
	
	Uint32 i;
	Uint32* data = result->data;
	for(i = 0 ; i < mat1->rows * mat1->cols; i++){
		if (mat1->data[i] > mat2->data[i])
			data[i] = mat1->data[i] - mat2->data[i];
		else
			data[i] = mat2->data[i] - mat1->data[i];
	}
}

/**
 * [Mat_multMat multiply two matrices]
 * @param mat1   [the first matrix]
 * @param mat2   [the second matric]
 * @param result [the result]
 */
void Mat_multMat(const Mat* mat1, const Mat* mat2, Mat* result){
//	assert(mat1->rows == mat2->rows && mat1->cols == mat2->cols);
	
	Uint32 i;
	Uint32* data = result->data;
	for(i = 0 ; i < mat1->rows * mat1->cols; i++){
		data[i] = mat1->data[i] * mat2->data[i];
	}
}


/**
 * [Mat_divMat divide a first matrix by the second]
 * @param mat1   [the first matrix]
 * @param mat2   [the second matric]
 * @param result [the result]
 */
void Mat_divMat(const Mat* mat1, const Mat* mat2, Mat* result){
//	assert(mat1->rows == mat2->rows && mat1->cols == mat2->cols);
	
	Uint32 i;
	Uint32* data = result->data;
	for(i = 0 ; i < mat1->rows * mat1->cols; i++){
		data[i] = mat1->data[i] / mat2->data[i];
	}
}



/**
 * [Mat_addMat sum a scalar to a matrix]
 * @param mat1   [the matrix]
 * @param s   	[the scalar]
 * @param result [the result]
 */
void Mat_addScalar(const Mat* mat1, Uint32 scalar, Mat* result){
	Uint32 i;
	Uint32* data = result->data;
	for(i = 0 ; i < mat1->rows * mat1->cols; i++){
		data[i] = mat1->data[i] + scalar;
	}
}


/**
 * [Mat_subtMat subtract a scalar from a matrix]
 * @param mat1   [the matrix]
 * @param s   	[the scalar]
 * @param result [the result]
 */
void Mat_subtScalar(const Mat* mat1, Uint32 scalar, Mat* result){
	Uint32 i;
	Uint32* data = result->data;
	for(i = 0 ; i < mat1->rows * mat1->cols; i++){
		data[i] = mat1->data[i] - scalar;
	}
}


/**
 * [Mat_multMat multiply a matrix by a scalar]
 * @param mat1   [the matrix]
 * @param s   	[the scalar]
 * @param result [the result]
 */
void Mat_multScalar(const Mat* mat1, Uint32 scalar, Mat* result){
	Uint32 i;
	Uint32* data = result->data;
	for(i = 0 ; i < mat1->rows * mat1->cols; i++){
		data[i] = mat1->data[i] * scalar;
	}
}


/**
 * [Mat_divMat divide a matrix by a scalar]
 * @param mat1   [the matrix]
 * @param s   	[the scalar]
 * @param result [the result]
 */
void Mat_divScalar(const Mat* mat1, Uint32 scalar, Mat* result){
	Uint32 i;
//	Uint32* data = result->data;
	for(i = 0 ; i < mat1->rows * mat1->cols; i++){
		result->data[i] = mat1->data[i] / scalar;
	}
}


/**
 * [Mat_bufferInGetAt get luminance from buuffer_in at (row,col)]
 * @param  bufferIn [the buffer_in arrray]
 * @param  row [row]
 * @param  col [column]
 * @return 	[the value of the element]
 */
Uint32 Mat_bufferInGetAt(Uint32 bufferIn[], Uint32 row, Uint32 col){
	//    assert(row < mat->rows && col < mat->cols);
	Uint32 index = (480 - row - 1)*360 + (360 - (col>>1) - 1);
	Uint32 pixels = bufferIn[index];
	if((360 - col - 1)%2 == 0)
		return (pixels >> 8) & 0x000000FF;
	else
		return (pixels >> 24) & 0x000000FF;
}

/**
 * [Mat_getAt get element at (row,col)]
 * @param  mat [the matrix]
 * @param  row [row]
 * @param  col [column]
 * @return 	[the value of the element]
 */
Uint32 Mat_getAt(const Mat* mat, Uint32 row, Uint32 col){
	//    assert(row < mat->rows && col < mat->cols);
	return mat->data[row*mat->cols + col];
	
}


/**
 * [Mat_setAt set element at (row,col)]
 * @param  mat [the matrix]
 * @param  row [row]
 * @param  col [column]
 * @param value [the new element value]
 */
void Mat_setAt(Mat* mat, Uint32 row, Uint32 col, Uint32 value){
//	assert(row < mat->rows && col < mat->cols);
	mat->data[row * mat->cols + col] = value;
}

/**
 * [Mat_getSubMat get sub-matrix from a Mat]
 * @param  mat       [the matrix]
 * @param  row_start [the top-row]
 * @param  col_start [the left-column]
 * @param  num_rows  [the number of rows]
 * @param  num_cols  [the number of columns]
 */
void Mat_getSubMat(const Mat* mat, Uint32 row_start, Uint32 col_start, Uint32 num_rows, Uint32 num_cols, Mat* result) {
//	assert(row_start + num_rows <= mat->rows && row_start < mat->rows);
//	assert(col_start + num_cols <= mat->cols && col_start < mat->cols);
	
	Uint32 i,j,index=0;
	for(i=0; i<num_rows; i++)
	{
		for(j=0; j<num_cols; j++)
		{
			result->data[index++] = Mat_getAt(mat, (row_start+i), (col_start+j));
		}
	}
	result->rows = num_rows;
	result->cols = num_cols;
}


/**
 * [Mat_overWriteSubMat Overwrite sub-matrix of a Mat]
 * @param  mat       [the matrix to overwrite]
 * @param  row_start [the top-row]
 * @param  col_start [the left-column]
 * @param  num_rows  [the number of rows]
 * @param  num_cols  [the number of columns]
 */
void Mat_overWriteSubMat(const Mat* boxMat, Uint32 row_start, Uint32 col_start, Uint32 num_rows, Uint32 num_cols, Mat* mat) {
//	assert(row_start + num_rows <= mat->rows && row_start < mat->rows);
//	assert(col_start + num_cols <= mat->cols && col_start < mat->cols);
	
	Uint32 i,j;
	int startIndex =  (row_start)*720+(col_start);
	for(i=0; i<boxMat->rows; i++)
	{
		for(j=0; j<boxMat->cols; j++)
		{
			int index = startIndex + i*720 + j;
			mat->data[index] = Mat_getAt(boxMat, i, j);
		}
	}
}

/**
 * [Mat_sum sum all elements in a matrix]
 * @param  mat [the matrix]
 * @return     [the sum, a Uint32]
 */
Uint32 Mat_sum(const Mat* mat){
	Uint32 i,j;
	Uint32 sum = 0;
	
	for(i = 0 ; i < mat->rows; i++)
		for(j = 0 ; j < mat->cols; j++)
			sum += mat->data[i*mat->cols + j];
	
	return sum;
}

/**
 * [Mat_integralImage obtains the integral image of an input]
 * @param  source 	[the input]
 * @param  dest 	[the integral image]
 */
void Mat_integralImage(const Mat* source, Mat* dest){
	
	Uint32* integral = dest->data;
	
	int32 rows = source->rows;
	int32 cols = source->cols;
	
	int32 i,j;
	
	//Integral image
	int32 sum = source->data[0];
	integral[0] = sum;
	for(j = 1 ; j < cols; j++){
		sum += source->data[j];
		integral[j] = sum;
	}
	
	sum = source->data[0];
	for(i = source->cols; i < rows*cols; i += cols){
		sum += source->data[i];
		integral[i] = sum;
	}
	
	for(i = cols; i < rows*cols; i += cols){
		for(j = 1 ; j < cols; j++){
			integral[i + j] = source->data[i + j] + integral[i + j - 1] + integral[i - cols + j] - integral[i- cols + j - 1];
		}
	}
	
	dest->rows = rows;
	dest->cols = cols;
}



/**
 * [Mat_adaptiveThresholdMeanInv2 faster adaptive mean thresholding using integral image]
 * @param gray   [gray image]
 * @param ksize  [length of kernel side]
 * @param meanC  [constant to subtract from mean]
 * @param result [the thresholded output]
 * @param integralTemp [used to store the integral image]
 */
void Mat_adaptiveThresholdMeanInv(const Mat* gray, int ksize, int meanC, Mat* result, Mat* integralTemp){
	Uint32* integral = integralTemp->data;
	
	int32 i,j,mStart,mEnd,nStart,nEnd;
	int32 kMid = ksize/2;
	int32 index = 0;
	int32 sum;
	int outerMeanC = meanC;
	
	
	for(i = 0; i < gray->rows; i++){
		mStart = i - kMid;
		mEnd = i + kMid;
		mEnd = mEnd >= gray->rows ? gray->rows-1 : mEnd;
		mStart = mStart < 0 ? 0 : mStart;
		
		for(j = 0 ; j < gray->cols ; j++){
			if(i<5 || i > 22 || j < 5 || j > 22)
				meanC = outerMeanC - 5;
			else
				meanC = outerMeanC;
			nStart = j - kMid;
			nEnd = j + kMid;
			nEnd = nEnd >= gray->cols ? gray->cols-1 : nEnd;
			nStart = nStart < 0 ? 0 : nStart;
			
			
			int32 x2y2 = mEnd*gray->cols + nEnd;
			int32 x2y1_1 = (mStart-1)*gray->cols + nEnd;
			int32 x1_1y2 = mEnd*gray->cols + nStart - 1;
			int32 x1_1y_1 = (mStart-1)*gray->cols + nStart - 1;
			
			x2y1_1 = x2y1_1 < 0 ? 0 : x2y1_1;
			x1_1y2 = x1_1y2 < 0 ? 0 : x1_1y2;
			x1_1y_1 = x1_1y_1 < 0 ? 0 : x1_1y_1;
			
			int32 integ_x2y2 = integral[x2y2];
			int32 integ_x2y1_1 = integral[x2y1_1];
			int32 integ_x1_1y2 = integral[x1_1y2];
			int32 integ_x1_1y_1 = integral[x1_1y_1];
			
			if (mStart <= 0) {
				integ_x1_1y_1 = 0;
				integ_x2y1_1 = 0;
			}
			if (nStart <= 0) {
				integ_x1_1y_1 = 0;
				integ_x1_1y2 = 0;
			}
			
			sum = integ_x2y2 + integ_x1_1y_1 - integ_x2y1_1 - integ_x1_1y2 ;
			
			int32 kLen = (mEnd-mStart+1) * (nEnd-nStart+1);
			
			Uint32 avg = (sum / kLen) - meanC;
			result->data[index] =  gray->data[index] < avg ? 255 : 0;
			index++;
		}
	}
	
	result->rows = gray->rows;
	result->cols = gray->cols;
}

/**
 * [Mat_varianceFilter variance filter using integral image]
 * @param gray   [gray image]
 * @param ksize  [length of kernel side]
 * @param meanC  [constant to subtract from mean]
 * @param result [the thresholded output]
 * @param integralTemp [used to store the integral image]
 */
void Mat_varianceFilter(const Mat* gray, int ksize, Mat* result, Mat* integralTemp){
//	int VAR_THRESH = 100;
	Mat_integralImage(gray, integralTemp);
	Uint32* integral = integralTemp->data;//(Uint32*) calloc(gray->rows * gray->cols, sizeof(Uint32));
	if(integral == NULL){
		puts("error");
	}
	
	int32 i,j,mStart,mEnd,nStart,nEnd;
	int32 kMid = ksize/2;
	int32 index = 0;
	int32 sum;
	
	
	for(i = 0; i < gray->rows; i++){
		mStart = i - kMid;
		mEnd = i + kMid;
		mEnd = mEnd >= gray->rows ? gray->rows-1 : mEnd;
		mStart = mStart < 0 ? 0 : mStart;
		
		for(j = 0 ; j < gray->cols ; j++){
			nStart = j - kMid;
			nEnd = j + kMid;
			nEnd = nEnd >= gray->cols ? gray->cols-1 : nEnd;
			nStart = nStart < 0 ? 0 : nStart;
			
			
			int32 x2y2 = mEnd*gray->cols + nEnd;
			int32 x2y1_1 = (mStart-1)*gray->cols + nEnd;
			int32 x1_1y2 = mEnd*gray->cols + nStart - 1;
			int32 x1_1y_1 = (mStart-1)*gray->cols + nStart - 1;
			
			x2y1_1 = x2y1_1 < 0 ? 0 : x2y1_1;
			x1_1y2 = x1_1y2 < 0 ? 0 : x1_1y2;
			x1_1y_1 = x1_1y_1 < 0 ? 0 : x1_1y_1;
			
			int32 integ_x2y2 = integral[x2y2];
			int32 integ_x2y1_1 = integral[x2y1_1];
			int32 integ_x1_1y2 = integral[x1_1y2];
			int32 integ_x1_1y_1 = integral[x1_1y_1];
			
			if (mStart <= 0) {
				integ_x1_1y_1 = 0;
				integ_x2y1_1 = 0;
			}
			if (nStart <= 0) {
				integ_x1_1y_1 = 0;
				integ_x1_1y2 = 0;
			}
			
			sum = integ_x2y2 + integ_x1_1y_1 - integ_x2y1_1 - integ_x1_1y2 ;
			int32 kLen = (mEnd-mStart+1) * (nEnd-nStart+1);
			Uint32 avg = ((sum << 2) / kLen) >> 2;

			mStart = mStart < 0 ? 0 : mStart;
			nStart = nStart < 0 ? 0 : nStart;

			int k,l,loc,variance = 0;
			// printf("(%d,%d,%d,%d)\t", mStart, nStart, mEnd, nEnd);

			for(k = mStart; k < mEnd+1; k++){
				for(l = nStart; l < nEnd+1; l++){
					loc= k*gray->cols + l;
					int diff = (int)gray->data[loc] - avg;
					// printf("(%d,%d)\t", diff, avg);
					variance += diff*diff;
				}
			}
			// puts("");
			variance /= kLen;

			result->data[index] =  variance > 255 ? 255 : variance;
//			if(gray->data[loc] > VAR_THRESH)
//				result->data[index] = 0;
			index++;
		}
		
	}

	result->rows = gray->rows;
	result->cols = gray->cols;
}

/**
 * [Mat_maxResponse get center pixel location with max response]
 * @param gray   	[gray image]
 * @param winSize  	[length of the window side: should be odd number]
 * @param rowStart  [top-left row of search area]
 * @param colStart  [top-left column of search area]
 * @param numRows  	[height of search area]
 * @param numCols  	[width of search area]
 * @param result 	[the thresholded output]
 * @param integralImg [the integral image: note - this should has been pre-computed before this function is called]
 * @return     [the pixel's location index = row*col + col (row => return value/cols, col => return value % cols)]
 */
Uint32 Mat_maxResponse(const Mat* gray, int winSize, int rowStart, int colStart, int numRows, int numCols, const Mat* integralImg){
	
//	assert(rowStart + numRows <= gray->rows && rowStart >=0 && rowStart < gray->rows);
//	assert(colStart + numCols <= gray->cols && colStart >= 0 && colStart < gray->cols);
//	assert(winSize%2 == 1);
	
	
	const Uint32* integral = integralImg->data;//(Uint32*) calloc(gray->rows * gray->cols, sizeof(Uint32));
	if(integral == NULL){
		puts("error");
	}
	
	int32 maxRow = rowStart + numRows;
	int32 maxCol = colStart + numCols;
	
	int32 i,j,mStart,mEnd,nStart,nEnd;
	int32 wMid = winSize/2; // half the window size
	int32 sum;
	int32 max = 0;
	Uint32 maxLoc = rowStart*gray->cols + colStart;
	
	
	for(i = rowStart; i < rowStart + numRows; i++){
		mStart = i - wMid;
		mEnd = i + wMid;
		mEnd = mEnd >= maxRow ? maxRow-1 : mEnd;
		
		for(j = colStart ; j < colStart + numCols ; j++){
			nStart = j - wMid;
			nEnd = j + wMid;
			nEnd = nEnd >= maxCol ? maxCol-1 : nEnd;
			
			
			int32 x2y2 = mEnd*gray->cols + nEnd; //bottom right
			int32 x2y1_1 = (mStart-1)*gray->cols + nEnd; //top-right
			int32 x1_1y2 = mEnd*gray->cols + nStart - 1; //bottom-left
			int32 x1_1y_1 = (mStart-1)*gray->cols + nStart - 1; //top-left
			
			
			int32 integ_x2y2 = integral[x2y2];
			int32 integ_x2y1_1 = integral[x2y1_1];
			int32 integ_x1_1y2 = integral[x1_1y2];
			int32 integ_x1_1y_1 = integral[x1_1y_1];
			
			//bounds checking
			if (mStart <= 0) {
				integ_x1_1y_1 = 0;
				integ_x2y1_1 = 0;
			}
			if (nStart <= 0) {
				integ_x1_1y_1 = 0;
				integ_x1_1y2 = 0;
			}
			
			//the sum of pixels in the window
			sum = integ_x2y2 + integ_x1_1y_1 - integ_x2y1_1 - integ_x1_1y2 ;
			
			// printf("%ld,%ld : %ld \n", i, j, sum);
			if(sum > max){
				max = sum;
				maxLoc = i*gray->cols + j;
			}
			
		}
		
	}
	
	return maxLoc;
}

/**
 * [Mat_colorFilter color filter an image in YCbCr color space]
 * @param ycrcb    [the YCbCr image: w/2-by-h image]
 * @param filtered [a grayscale image: w-by-h image]
 */
void Mat_colorFilter(Uint32* bufferIn, Mat* filtered){
	
	Uint32* data = filtered->data;
	
	Uint32  pixel1, pixel2, y1, y2, cr;//, cb;
	
	Uint32 i,j,bufferIndex =0;
	Uint32 index = 720*480 - 1;
	
	for(i = 0 ; i < 240; i++){
		for(j = 0; j < 720; j++){
			y2 = ((bufferIn[bufferIndex] >> 24) & 0xFF);
			y1 = ((bufferIn[bufferIndex] >> 8) & 0xFF);
			cr = ((bufferIn[bufferIndex] >> 16) & 0xFF);
//			cb = (bufferIn[bufferIndex] & 0xFF);
			
			if ((y1 > 60 && y1 < 135) && (cr > 155 && cr <180) /*&& (cb > 95 && cb < 140)*/)
				pixel1 = 255;
			else
				pixel1 = 0;
			
			if ((y2 > 60 && y2 < 135) && (cr > 155 && cr <180)/* && (cb > 95 && cb < 140)*/)
				pixel2 = 255;
			else
				pixel2 = 0;
			
			data[index] = pixel1;
			
			data[index-1] = pixel2;
			bufferIndex++;
			index -= 2;
		}
	}
}

/**
 * [Mat_colorBox color filter an image in YCbCr color space]
 * @param ycrcb    [the YCbCr image: w/2-by-h image]
 * @param filtered [a grayscale image: w-by-h image]
 */
int Mat_colorBox(Uint32* bufferIn){
	
	//Uint32* data = filtered->data;
	
	Uint32 y1, y2, cr;//, cb;
	
	Uint32 i,j,countR=0,countG=0,bufferIndex = 0, status =NONE;
	//Uint32 index = 720*480 - 1;
	
	for(i = 0; i < 100; i++){
		for(j = 35; j < 135; j++){
			bufferIndex = i*360+j;
			y2 = ((bufferIn[bufferIndex] >> 24) & 0xFF);
			y1 = ((bufferIn[bufferIndex] >> 8) & 0xFF);
			cr = ((bufferIn[bufferIndex] >> 16) & 0xFF);
//			cb = (bufferIn[bufferIndex] & 0xFF);
			
			if ((y1 > 60 && y1 < 220) && (cr > 155 && cr <255) /*&& (cb > 95 && cb < 140)*/)
				countR += 2;
			if ((y1 > 60 && y1 < 220) && (cr > 0 && cr <115))
				countG += 2;

		}
	}
	if (countR > 5000)//THRESHOLD)
	{
		status = RED;
	}
	else if (countG > 5000)//THRESHOLD)
	{
		status = GREEN;
	}
	return status;
}



/**
 * [Mat_read read Y-channel data from bufferIn]
 * @param bufferIn
 * @return
 */
void Mat_readGray(Uint32* bufferIn, Mat* output){
	Uint32 width = 720;
	Uint32 height = 480;
	
	Uint32 i,j, bufferIndex = 0;
	int index = 720*480-1;
	for (i = 0; i < height ; i++){
		for (j = 0; j < width/2 ; j++){
			
			Uint32 y2 = ( bufferIn[bufferIndex] >> 24 ) & 0x000000FF;
			Uint32 y1 = ( bufferIn[bufferIndex] >> 8) & 0x000000FF;
			
			output->data[index] = y1;
			output->data[index-1] = y2;
			
			index -= 2;
			bufferIndex++;
		}
	}
}

/**
 * [Mat_write] write mat to bufferOut
 * @param mat the mat to write to bufferOut
 * @param bufferOut
 */
void Mat_writeGray(const Mat* mat, Uint32 bufferOut[]){
	Uint32 width = mat->cols;
	Uint32 height = mat->rows;
	Uint32 i,j, index = 0;
	
	for (i = 0; i < height ; i++){
		for (j = 0; j < width/2 ; j++){
			
			Uint32 pixel2 = mat->data[index];// (Uint32)(( line >> 24 ) & 0xFF);
			Uint32 pixel1 = mat->data[index+1]; //(Uint32)(( line >> 8) & 0xFF);
			
			bufferOut[i*360+j] = (pixel1 << 24) | (128 << 16) | (pixel2 << 8) | 128;
			
			index += 2;
			// bufferIndex++;
		}
	}
	
}



Uint32 Mat_integralSum(const Mat* integralImg, int start_row, int start_col, int height, int width){

	int32 x2y2 = (start_row+height-1)*integralImg->cols + (start_col+width-1);
	int32 x2y1_1 = (start_row-1)*integralImg->cols + (start_col+width-1);
	int32 x1_1y2 = (start_row+height-1)*integralImg->cols + start_col - 1;
	int32 x1_1y_1 = (start_row-1)*integralImg->cols + start_col - 1;
	
	x2y1_1 = x2y1_1 < 0 ? 0 : x2y1_1;
	x1_1y2 = x1_1y2 < 0 ? 0 : x1_1y2;
	x1_1y_1 = x1_1y_1 < 0 ? 0 : x1_1y_1;

	int32 integ_x2y2 = integralImg->data[x2y2];
	int32 integ_x2y1_1 = integralImg->data[x2y1_1];
	int32 integ_x1_1y2 = integralImg->data[x1_1y2];
	int32 integ_x1_1y_1 = integralImg->data[x1_1y_1];
	
	if (start_row <= 0) {
		integ_x1_1y_1 = 0;
		integ_x2y1_1 = 0;
	}
	if (start_col <= 0) {
		integ_x1_1y_1 = 0;
		integ_x1_1y2 = 0;
	}

	return  integ_x2y2 + integ_x1_1y_1 - integ_x2y1_1 - integ_x1_1y2 ;
}

void Mat_writeLetter(Mat* gray, char letter, int row, int col){
	Uint8* textBitmap;
	int space = 0;
	if(letter >= 0 && letter <= 9)
		textBitmap = &TEXT_BITMAP[(int)letter][0];
	else if(letter >= 'A' && letter <= 'Z')
		textBitmap = &TEXT_BITMAP[letter-'A'+10][0];
	else if(letter >= '0' && letter <= '9')
		textBitmap = &TEXT_BITMAP[letter-'0'][0];
	else if(letter == '-')
		textBitmap = &TEXT_BITMAP[36][0];
	else if(letter == ' ') space = 1;
	else
		return;

	int i,j,index;
	int startIndex = row*720+col;
	if(space){
		for(i=0, index=0 ; i < 20; i++)
			for(j=0; j < 20; j++, index++)
				gray->data[startIndex + i*720 + j] = 0;
		return;
	}
	for(i=0, index=0 ; i < 20; i++)
	{
		for(j=0; j < 20; j++, index++)
		{
			gray->data[startIndex + i*720 + j] = (Uint32)textBitmap[index];
		}
	}
}

void Mat_writeNumber(Mat* gray, int number, int row, int col){

	if(number == 0){
		Mat_writeLetter(gray,0,row,col);
		return;
	}
	int digits[3];
	int len;
	if(number < 10) len = 1;
	else if(number < 100) len = 2;
	else if(number < 1000) len = 3;

	int i = 0;
	while (number > 0) {
	    digits[len-1-i] = number % 10;
	    number = number/10;
	    i++;
	}

	for(i = 0 ; i < len; i++){
		Mat_writeLetter(gray,digits[i],row,col+i*20);
	}
}


void Mat_writeWord(Mat* gray, char* word, int len, int row, int col){
	int i;
	for(i = 0 ; i < len; i++){
		Mat_writeLetter(gray, word[i],row,col+i*20);
	}
}

void Mat_writeScores(Mat* gray, int p1Score, int p2Score){
	Mat_writeWord(gray, "P1", 2, 20, 200);
	Mat_writeWord(gray, "P2", 2, 20, 520);
	// Mat_writeLetter(gray, 'P', 20, 200);
	// Mat_writeNumber(gray, '1', 20, 220);
	// Mat_writeLetter(gray, 'P', 20, 520);
	// Mat_writeLetter(gray, '2', 20, 540);
	Mat_writeNumber(gray, p1Score, 45, 200);
	Mat_writeNumber(gray, p2Score, 45, 520);
}
/* END Mat */



void Peak_averageFilter(int* hist, int* result, int len, int winSize){
	int i;
	int halfWin = winSize >> 1;
	int cummulative[PEAK_ARR_LEN];
	cummulative[0] = hist[0];

	for(i = 1; i < len; i++)
		cummulative[i] = hist[i] + cummulative[i-1];

	for(i = 0; i < len; i++){
		int left = i - halfWin;
		left = left <= 0 ? 0 : left;

		int right = i + halfWin;
		right = right > len-1 ? len-1 : right;


		int x0 = left <= 0 ? 0 : cummulative[left-1];
		int x1 = right > len-1  ? cummulative[len-1] : cummulative[right];
		// printf("%d, %d:-> %d, %d\n", left, right, x0, x1);

		result[i] = (((x1 - x0) << 2 ) / (right - left + 1)) >> 2 ;
	}
}

void Peak_subtract(int* left, int* right, int* result, int* maxVal, int len, int zeroLessZero){
	int i;
	if(!zeroLessZero){
		*maxVal = left[0] - right[0];
		for(i = 0 ; i < len; i++){
			result[i] = left[i] - right[i]; 
			if(result[i] > *maxVal)
				*maxVal = result[i];
		}
	}
	else{
		*maxVal = left[0] - right[0];
		for(i = 0 ; i < len; i++){
			result[i] = left[i] - right[i]; 
			result[i] = result[i] < 0 ? 0 : result[i];
			if(result[i] > *maxVal)
				*maxVal = result[i];
		}
	}
}


void Peak_findPeaks(const Mat* frame, int* corner, int* vPeaks, int* hPeaks, Mat* integral){

	int i,j;

	//DETECT GRID
	int ALLOWANCE = 0;
	int GRID_WIDTH = corner[3] - corner[1]  + 2*ALLOWANCE;
	int GRID_HEIGHT = corner[6] - corner[0]  + 2*ALLOWANCE;
	int origin_row = corner[0] - ALLOWANCE;
	int origin_col = corner[1] - ALLOWANCE;
	int SMOOTH = 3;
	int DC = 15;



	//get histograms
	int histVert[PEAK_ARR_LEN];// =  (int*)calloc(GRID_WIDTH, sizeof(int));
	int histVertDC[PEAK_ARR_LEN];// = (int*)calloc(GRID_WIDTH, sizeof(int));

	int histHoriz[PEAK_ARR_LEN];// = (int*)calloc(GRID_HEIGHT, sizeof(int));
	int histHorizDC[PEAK_ARR_LEN];// = (int*)calloc(GRID_HEIGHT, sizeof(int));

	for(i = origin_col ; i < origin_col + GRID_WIDTH; i++){
		histVert[i-origin_col] = Mat_integralSum(integral, origin_row, i, GRID_HEIGHT, 1);
		// printf("%d\n", histVert[i-origin_col]);
		histVert[i-origin_col] = histVert[i-origin_col];
	}
	for(i = origin_row ; i < origin_row + GRID_HEIGHT; i++){
		histHoriz[i-origin_row] = Mat_integralSum(integral, i, origin_col, 1, GRID_WIDTH);
		histHoriz[i-origin_row] = histHoriz[i-origin_row];
		// cout << histHoriz[i-origin_row] << endl;
	}


	Peak_averageFilter(histVert, histVertDC, GRID_WIDTH, DC);
	Peak_averageFilter(histVert, histVert, GRID_WIDTH, SMOOTH);

	Peak_averageFilter(histHoriz, histHorizDC, GRID_HEIGHT, DC);
	Peak_averageFilter(histHoriz, histHoriz, GRID_HEIGHT, SMOOTH);

	int maxV, maxH;
	Peak_subtract(histVert, histVertDC, histVert, &maxV, GRID_WIDTH, 1);
	Peak_subtract(histHoriz, histHorizDC, histHoriz, &maxH, GRID_HEIGHT, 1);

	// fprintf(stderr, "%d\n", origin_col);
	for(i = 0 ; i < GRID_WIDTH; i++){
//		fprintf(stdout, "%d\n", histHoriz[i]);
	}

	for(i = 1, j = 0; i < GRID_WIDTH - 1 && j < 14; i++){
		if(histVert[i] > histVert[i-1] && histVert[i] > histVert[i+1] && histVert[i] > (maxV / 3)){
			vPeaks[j+1] = origin_col + i;

			// fprintf(stderr, "%d\n", origin_col + i );
			j++;
		}
	}
	vPeaks[0] = vPeaks[1] - (vPeaks[2] - vPeaks[1]);
	vPeaks[15] = vPeaks[14] + (vPeaks[14] - vPeaks[13]);
	for(i = 1, j = 0; i < GRID_HEIGHT - 1 && j < 14; i++){
		if(histHoriz[i] > histHoriz[i-1] && histHoriz[i] > histHoriz[i+1] && histHoriz[i] > (maxH / 3)){
			hPeaks[j+1] = origin_row + i;

//			fprintf(stderr, "%d: %d\n", j+1, i);
			j++;
		}
	}
	hPeaks[0] = hPeaks[1] - (hPeaks[2] - hPeaks[1]);
	hPeaks[15] = hPeaks[14] + (hPeaks[14] - hPeaks[13]);


//	fprintf(stderr, "\n\n");
	
	// for(int i = 0; i < GRID_WIDTH; i++)
	// 	printf("%d\n", histHoriz[i]);
}


#endif /*MAT*/
