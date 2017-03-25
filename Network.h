//
//  Network.h
//  ProjectCV
//
//  Created by Terna Kpamber on 2/23/16.
//  Copyright © 2016 Terna Kpamber. All rights reserved.
//

#ifndef Network_h
#define Network_h

//#include "Netparams.h"

#define EPS 0
#define SIDE 20
#define IM_SIZE SIDE*SIDE
#define HIDDEN 64
const char* ALPHA = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
//typedef signed Int32 int32;

/**
 * Network
 */
char Network_ocr(Uint32 *image, Int32* output){
	//loopers
	int i,j;
	
	//Perform element-wise multiplication for each output neuron's inputs
	int32 hidden_output[HIDDEN];
//	int32 output[26];
	
//	float sum = 0; uncomment to view probabilities instead
	
	//hidden layer
	for(i = 0 ; i < HIDDEN; i++){
		
		int32 prod_sum = 0;
		int i_index = i*IM_SIZE;
//		int k,j;
//		for(k = OFFSET; k < SIDE - OFFSET; k++){
//			for(j = OFFSET; j < SIDE - OFFSET; j++){
//				prod_sum += image[k*SIDE + j] * Network_weights1[i_index + k*SIDE + j];
//			}
//		}
		int j;
		for(j = 0 ; j < IM_SIZE; j++)
			prod_sum += image[j] * Network_weights1[i_index + j];
		prod_sum += Network_biases1[i];
		prod_sum = prod_sum > EPS ? prod_sum : 0;
		
//		prod_sum = exp(prod_sum); uncomment to view probabilities instead
		hidden_output[i] = prod_sum / FACTOR;
//		sum += prod_sum; uncomment to view probabilities instead
	}
	
	//output layer
	for(i = 0; i < 26; i++){
		
		int32 prod_sum = 0;
		int i_index = i*HIDDEN;
		for(j = 0; j < HIDDEN; j++){
			prod_sum += hidden_output[j] * Network_weights2[i_index + j];
		}
		prod_sum += Network_biases2[i];
		output[i] = prod_sum / FACTOR;
	}
	
	for (i = 0; i < 26; i++) {
//		outputs[i] = outputs[i]/sum; uncomment to view probabilities instead
//		printf("%c:%d\n", ALPHA[i], output[i]);
//		cout << ALPHA[i] << ": " << output[i] << endl;

	}
	
	//find max
	int max_index = 0;
	for(i = 1 ; i < 26; i++){
		if(output[i] > output[max_index])
			max_index = i;
	}
	
	return ALPHA[max_index];
}
/* END Network*/




#endif /* Network_h */
