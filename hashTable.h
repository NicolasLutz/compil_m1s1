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
typedef struct {Value val; char *string;} Pair;
typedef struct strLink { Pair pair; struct strLink *next;} Link;
typedef struct { Link *head; Link *tail; } PairList;
typedef struct {PairList **pl; size_t size;} SymbolTable;

int 		_genIndex(const char *chaine);
int 		_straightStrCmp(const char *string1, const char *string2);

Pair 		P_gen(const char *chaine, Value val);

Link 		*L_gen(Pair p);

PairList 	*PL_gen();
void 		PL_add(PairList *pl, Link *l);

SymbolTable 	*ST_gen(size_t size);
void 		ST_addNoLookup(SymbolTable *st, const char *string, int index, Value val);
Value 		ST_lookup(SymbolTable *st, const char *string);
int 		ST_add(SymbolTable *st, const char *string, Value val);
void 		ST_print(SymbolTable *st);
void 		ST_destroy(SymbolTable *st);

/*
SymbolTable *st;
//TODO:
//Use this for yacc
*/

#endif
