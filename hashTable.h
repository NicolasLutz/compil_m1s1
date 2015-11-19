#ifndef __HT__H__
#define __HT__H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define maxStrLength 255

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

union Value {float fVal; int iVal; void *mVal;};
typedef union Value Value;
typedef struct {Value val; char *chaine;} Pair;
typedef struct strLink { Pair pair; struct strLink *next;} Link;
typedef struct { Link *head; Link *tail; } PairList;

int 		genIndex(const char *chaine, size_t strlen);
Pair 		genPair(const char *chaine, Value val, size_t strlen);
Link 		*genLink(Pair p);
PairList 	*genPairList();
void 		addPairList(PairList *pl, Link *l);
void 		initHashTable(PairList **ht, size_t size);
void 		addHashTable(PairList **ht, const char *chaine, int index, Value val, size_t strlen);
Value 		findOnHashTable(PairList **ht, const char *chaine, size_t strlen, size_t size);
int 		createOrReplaceOnHashTable(PairList **ht, const char *chaine, Value val, size_t strlen, size_t size);
int 		straightStrCmp(const char *string1, const char *string2);
void 		debugPrintHashTable(PairList **ht, size_t size);
void 		freeAll(PairList **ht, size_t size);

/*
size_t htSize;
PairList **hashTable;
//TODO:
//Use those for yacc
*/

#endif
