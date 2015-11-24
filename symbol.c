#include "symbol.h"

int _genIndex(const char *string)
{
	int i, index=0;
	for(i=0; string[i]!='\0' && i<maxStrLength; i++)
	{
		index=index^string[i]; //xor of all characters
	}
	return index;
}

//=============================================================================

int _straightStrCmp(const char *string1, const char *string2)
{
	if(!string1||!string2)
		return 0;
	int i=0;
	char c1=string1[i], c2=string2[i++];
	while(c1==c2 && c1!='\0' && c2!='\0')
	{
		c1=string1[i];
		c2=string2[i++];
	}
	if(c1!=c2)
		return 0;
	else
		return 1;
}

//=============================================================================
//=============================================================================

SymbolInfo SI_genInt(int value)
{
	SymbolInfo info;
	info.value.iVal=value;
	info.type=INT_T;
	return info;
}

//=============================================================================

SymbolInfo SI_genFloat(float value)
{
	SymbolInfo info;
	info.value.fVal=value;
	info.type=FLOAT_T;
	return info;
}

//=============================================================================

SymbolInfo SI_genArray(void *value)
{
	SymbolInfo info;
	info.value.mVal=value;
	info.type=ARRAY_T;
	return info;
}

//=============================================================================
//=============================================================================

Symbol S_gen(const char *name, SymbolInfo info)
{
	Symbol s;
	s.info=info;
	s.name=strndup(name, maxStrLength);
	return s;
}

//=============================================================================
//=============================================================================

Link *L_gen(Symbol s)
{
	Link *l=(Link *)malloc(sizeof(*l));
	l->symbol=s;
	l->next=NULL;
	return l;
}

//=============================================================================
//=============================================================================

SymbolList *SL_gen()
{
	SymbolList *sl=malloc(sizeof(*sl));
	sl->head=NULL;
	sl->tail=NULL;
	return sl;
}

//=============================================================================

void SL_add(SymbolList *sl, Link *l)
{
	//adds a link in sl and returns the new set
	if(!sl->head)
	{
		sl->head=l;
		sl->tail=l;
	}
	else
	{
		sl->tail->next=l;
		sl->tail=l;
	}
	return;
}

//=============================================================================
//=============================================================================

SymbolTable *ST_gen(size_t size)
{
	unsigned int i;
	SymbolTable *st=malloc(sizeof(*st));
	st->sl=malloc(size*sizeof(*st->sl));
	st->size=size;
	for(i=0; i<size; i++)
	{
		st->sl[i]=SL_gen();
	}
	return st;
}

//=============================================================================

void ST_addNoLookup(SymbolTable *st, const char *name, int index, SymbolInfo info)
//adds an entry and returns the set generated
""{
	Symbol s=S_gen(name, info);
	Link *l=L_gen(s);
	SL_add(st->sl[index],l);
}

SymbolInfo ST_lookup(SymbolTable *st, const char *name)
//find the entry corresponding to name.
//make sure ht was initialized before
{
	int index=_genIndex(name)%st->size;
	Link *l=st->sl[index]->head;
	SymbolInfo info;
	info.value.iVal=0;
	int found=0;
	while(l && !found)
	{
		if(_straightStrCmp(name, l->symbol.name))
		{
			info=l->symbol.info;
			found=1;
		}
		l=l->next;
	}
	return info;
}

int ST_add(SymbolTable *st, const char *name, SymbolInfo info)
//creates or reslace an entry in st with name name and informations info.
//make sure ht was initialized before.
//returns 1 if the entry existed, and 0 if it was created.
{
	int index=_genIndex(name)%st->size;
	Link *l=st->sl[index]->head;
	int found=0;
	while(l && !found)
	{
		if(_straightStrCmp(name, l->symbol.name))
		{
			l->symbol.info=info;
			found=1;
		}
		else
			l=l->next;
	}
	if(!found)
		ST_addNoLookup(st, name, index, info);
	return found;
}

void ST_print(const SymbolTable *st)
{
	unsigned int i;
	printf("Index==Value=======String\n");
	Link *l;
	for(i=0; i<st->size; i++)
	{
		l=st->sl[i]->head;
		while(l)
		{
			switch(l->symbol.info.type)
			{
				case INT_T:
					printf("%d    %15d  %s\n",
						i, l->symbol.info.value.iVal, l->symbol.name);
					break;
				case FLOAT_T:
					printf("%d    %15f  %s\n",
						i, l->symbol.info.value.fVal, l->symbol.name);
					break;
				default:
					printf("%d    %15p  %s\n",
						i, l->symbol.info.value.mVal, l->symbol.name);
					break;
			}
			l=l->next;
		}
	}
	return;
}

void ST_destroy(SymbolTable *st)
{//destroys everything in st
	unsigned int i;
	Link *l;
	for(i=0; i<st->size; i++)
	{
		while(l=st->sl[i]->head)
		{
			st->sl[i]->head=l->next;
			free(l->symbol.name);
			free(l);
		}
		free(st->sl[i]);
	}
	free(st->sl);
	free(st);
}

void ST_writeMIPS(const SymbolTable *st, const char *filename)
{
	FILE *f=fopen(filename, "w");
	assert(f);
	unsigned int i;
	Link *l;
	for(i=0; i<st->size; i++)
	{
		l=st->sl[i]->head;
		while(l)
		{
			switch(l->symbol.info.type)
			{
				case INT_T:
					fprintf(f, "%s .name %d\n", l->symbol.name, l->symbol.info.value.iVal);
					break;
				case FLOAT_T:
					fprintf(f, "%s .float %f\n", l->symbol.name, l->symbol.info.value.fVal);
					break;
				default:
					break;
			}
			l=l->next;
		}
	}
	fclose(f);
	return;

}

//Usage examsle
int main()
{
	size_t stSize=128;
	SymbolTable *st=ST_gen(stSize);
	SymbolInfo examplePntr=SI_genArray(&stSize); //m stands for matrix
	SymbolInfo exampleInt=SI_genInt(stSize);
	SymbolInfo exampleFloat=SI_genFloat(3.13f);
	const char *mStr="mon_pointeur";
	const char *iStr="mon_int";
	const char *fStr="mon_float";
	ST_add(st, mStr, examplePntr);
	ST_add(st, iStr, exampleInt);
	ST_add(st, fStr, exampleFloat);
	exampleFloat.value.fVal=3.14f;
	ST_add(st, fStr, exampleFloat);
	ST_print(st);
	ST_writeMIPS(st, "test.s");
	ST_destroy(st);
	return 0;
}
