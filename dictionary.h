#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include<string.h>
#include <math.h>
#include "DictionaryWords.h"

#define NUMWORDS 9999
#define MAXWORDSIZE 20
#define TABLESIZE 10007
#define RETABLESIZE 4973//6091//4973
#define POWER 7

int count =0;
//get hash key1
unsigned int getHashKey(char *str)
{
	int i;
	unsigned int result = 0;
	unsigned int sum = 0;
	for (i = 0; i < strlen(str); i++)
	{
		unsigned int c = str[i];
		result = result + pow(POWER,i)*c;
		sum += c;
	}
	return result % TABLESIZE;
}
//get hash key 2
unsigned int reGetHashKey(char *str)
{
	int i;
	unsigned int result = 0;
	unsigned int sum = 0;
	for (i = 0; i < strlen(str); i++)
	{
		unsigned int c = str[i];
		result = result + pow(POWER,i)*c;
		sum += c;
	}
	return result % RETABLESIZE;
}

//Remove word from dictionary
void removeWord(unsigned int hashkey){
	strcpy(dict[hashkey], "");
}
//Find word in dictionary
int findword(char *str){
	unsigned int hashkey = getHashKey(str);
	unsigned int rehashkey = reGetHashKey(str);
	if (strcmp(dict[hashkey],str)) //compare word in hastable
	{
		unsigned int temp = hashkey;
		do{
			hashkey += rehashkey;
			if (hashkey >= TABLESIZE)
				hashkey -= TABLESIZE;
		}while(strcmp(dict[hashkey],str) && (temp != hashkey));
	}
	if (strcmp(dict[hashkey], str)) //Word not found
	{
		return 1;
	}
	removeWord(hashkey); //remove word from hash table-since found-hence used in scrabble board
	return 0;
}

#endif /*DICTIONARY_H_*/
