#include "hashTable.h"

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

Pair P_gen(const char *string, Value val)
{
	Pair p;
	p.val=val;
	p.string=strndup(string, maxStrLength);
	return p;
}

//=============================================================================

Link *L_gen(Pair p)
{
	Link *l=(Link *)malloc(sizeof(*l));
	l->pair=p;
	l->next=NULL;
	return l;
}

//=============================================================================

PairList *PL_gen()
{
	PairList *pl=(PairList *)malloc(sizeof(*pl));
	pl->head=NULL;
	pl->tail=NULL;
	return pl;
}

//=============================================================================

void PL_add(PairList *pl, Link *l)
{
	//adds a link in pl and returns the new set
	if(!pl->head)
	{
		pl->head=l;
		pl->tail=l;
	}
	else
	{
		pl->tail->next=l;
		pl->tail=l;
	}
	return;
}

//=============================================================================

SymbolTable *ST_gen(size_t size)
{
	unsigned int i;
	SymbolTable *st=malloc(sizeof(*st));
	st->pl=malloc(size*sizeof(*st->pl));
	st->size=size;
	for(i=0; i<size; i++)
	{
		st->pl[i]=PL_gen();
	}
	return st;
}

//=============================================================================

void ST_addNoLookup(SymbolTable *st, const char *string, int index, Value val)
//adds an entry and returns the set generated
{
	Pair p=P_gen(string, val);
	Link *l=L_gen(p);
	PL_add(st->pl[index],l);
}

Value ST_lookup(SymbolTable *st, const char *string)
//find the entry corresponding to string.
//make sure ht was initialized before
{
	int index=_genIndex(string)%st->size;
	Link *l=st->pl[index]->head;
	Value val;
	val.iVal=0;
	int found=0;
	while(l && !found)
	{
		if(_straightStrCmp(string, l->pair.string))
		{
			val=l->pair.val;
			found=1;
		}
		l=l->next;
	}
	return val;
}

int ST_add(SymbolTable *st, const char *string, Value val)
//creates or replace an entry in st with name string and value val.
//make sure ht was initialized before.
//returns 1 if the entry existed, and 0 if it was created.
{
	int index=_genIndex(string)%st->size;
	Link *l=st->pl[index]->head;
	int found=0;
	while(l && !found)
	{
		if(_straightStrCmp(string, l->pair.string))
		{
			l->pair.val=val;
			found=1;
		}
		else
			l=l->next;
	}
	if(!found)
		ST_addNoLookup(st, string, index, val);
	return found;
}

void ST_print(SymbolTable *st)
{
	unsigned int i;
	printf("Index==Hex value=======String\n");
	Link *l;
	for(i=0; i<st->size; i++)
	{
		l=st->pl[i]->head;
		while(l)
		{
			printf("%d    %15p  %s\n", 
					i, l->pair.val.mVal, l->pair.string);
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
		while(l=st->pl[i]->head)
		{
			st->pl[i]->head=l->next;
			free(l->pair.string);
			free(l);
		}
		free(st->pl[i]);
	}
	free(st->pl);
	free(st);
}

//Usage example
int main()
{
	size_t stSize=128;
	SymbolTable *st=ST_gen(stSize);
	Value examplePntr;
	examplePntr.mVal=&stSize; //m stands for matrix
	Value exampleInt;
	exampleInt.iVal=(int)stSize;
	Value exampleFloat;
	exampleFloat.fVal=3.13f;
	const char *mStr="mon_pointeur";
	const char *iStr="mon_int";
	const char *fStr="mon_float";
	ST_add(st, mStr, examplePntr); 
	ST_add(st, iStr, exampleInt);
	ST_add(st, fStr, exampleFloat);
	exampleFloat.fVal=3.14f;
	ST_add(st, fStr, exampleFloat);
	ST_print(st);
	ST_destroy(st);
	return 0;
}
