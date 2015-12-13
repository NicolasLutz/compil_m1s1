#ifndef __SYMBOL__H__
#define __SYMBOL__H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define maxStrLength 255 //defines how long a symbol name can be

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

extern const char *_g_typeDesc[4];

typedef union Value
{
    float fVal;
    int iVal;
    void *mVal;
    char *sVal;
} Value;

typedef enum Typename {INT_T=0, FLOAT_T=1, ARRAY_T=2, STRING_T=3} Typename;

typedef struct
{
    Value value;
    Typename type;
    bool cst;
} SymbolInfo;

typedef struct
{
    SymbolInfo info;
    char *name;
} Symbol;

typedef struct strLink
{
    Symbol symbol;
    struct strLink *next;
} Link;

typedef struct
{
    Link *head;
    Link *tail;
} SymbolList;

typedef struct
{
    SymbolList **sl;
    size_t size;
    char _tmpName[maxStrLength]; ///<_tmpName gives a place for building tmp symbol names
} SymbolTable;

int 		    _genIndex(const char *string);
int 		    _straightStrCmp(const char *string1, const char *string2);

SymbolInfo	    SI_genInt(int value);
SymbolInfo	    SI_genFloat(float value);
SymbolInfo	    SI_genArray(void *value);
SymbolInfo	    SI_genString(char *value);//cleaner  !
const char      *SI_typeToString(Typename t);

Symbol 		    S_gen(const char *name, SymbolInfo *info);

Link 		    *L_gen(Symbol *s);

SymbolList 	    *SL_gen();
void 		     SL_add(SymbolList *sl, Link *l);

SymbolTable 	*ST_gen(size_t size);
Symbol 		    *ST_add(SymbolTable *st, const char *name, SymbolInfo *info);
Symbol 		    *ST_addTmp(SymbolTable *st, SymbolInfo *info);
Symbol 	      *ST_lookup(SymbolTable *st, const char *name);
void 		       ST_print(const SymbolTable *st);
void		       ST_writeMIPS(const SymbolTable *st, FILE *f);
void 		       ST_destroy(SymbolTable *st);

#endif
