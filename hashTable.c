#include "hashTable.h"

int genIndex(const char *chaine, size_t strlen)
{
	int i, index=0;
	for(i=0; i<strlen; i++)
	{
		index=index^chaine[i]; //xor of all characters
	}
	return index;
}

Pair genPair(const char *chaine, Value val, size_t strlen)
{
	Pair p;
	p.val=val;
	p.chaine=(char *)malloc((strlen)*sizeof(char));
	memcpy(p.chaine, chaine, strlen);
	return p;
}

Link *genLink(Pair p)
{
	Link *l=(Link *)malloc(sizeof(Link));
	l->pair=p;
	l->next=NULL;
	return l;
}

PairList *genPairList()
{
	PairList *pl=(PairList *)malloc(sizeof(PairList));
	pl->head=NULL;
	pl->tail=NULL;
}

void addPairList(PairList *pl, Link *l)
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

void initHashTable(PairList **ht, size_t size)
{
	int i;
	for(i=0;i<size;i++)
	{
		ht[i]=genPairList();
	}
}

void addHashTable(PairList **ht, const char *chaine, int index, Value val, size_t strlen)
//adds an entry and returns the set generated
{
	Pair p=genPair(chaine, val, strlen);
	Link *l=genLink(p);
	addPairList(ht[index],l);
}

Value findOnHashTable(PairList **ht, const char *chaine, size_t strlen, size_t size)
//find the entry corresponding to chaine.
//make sure ht was initialized before
{
	int lenNullTerminated=strlen+1;
	int index=genIndex(chaine, lenNullTerminated)%size;
	Link *l=ht[index]->head;
	Value val;
	val.iVal=0;
	int found=0;
	while(l && !found)
	{
		if(straightStrCmp(chaine, l->pair.chaine))
		{
			val=l->pair.val;
			found=1;
		}
		l=l->next;
	}
	return val;
}

int createOrReplaceOnHashTable(PairList **ht, const char *chaine, Value val, size_t strlen, size_t size)
//creates or replace an entry in ht (which is of size size) with name chaine of size strlen (use yyleng), and value val.
//make sure ht was initialized before.
//returns 1 if the entry existed, and 0 if it was created.
{
	size_t lenNullTerminated=strlen+1;
	int index=genIndex(chaine, lenNullTerminated)%size;
	Link *l=ht[index]->head;
	int found=0;
	while(l && !found)
	{
		if(straightStrCmp(chaine, l->pair.chaine))
		{
			l->pair.val=val;
			found=1;
		}
		else
			l=l->next;
	}
	if(!found)
		addHashTable(ht, chaine, index, val, lenNullTerminated);
	return found;
}

int straightStrCmp(const char *string1, const char *string2)
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

void debugPrintHashTable(PairList **ht, size_t size)
{
	printf("Index==Hex value=======Chaine\n");
	int i;
	Link *l;
	for(i=0; i<size; i++)
	{
		l=ht[i]->head;
		while(l)
		{
			printf("%d    %15p  %s\n", 
					i, l->pair.val.mVal, l->pair.chaine);
			l=l->next;
		}
	}
}

void freeAll(PairList **ht, size_t size)
{//destroys everything
	int i;
	Link *l;
	for(i=0; i<size; i++)
	{
		while(l=ht[i]->head)
		{
			ht[i]->head=l->next;
			free(l->pair.chaine);
			free(l);
		}
		free(ht[i]);
	}
}

//Usage example
int main()
{
	size_t htSize=128;
	PairList *ht[htSize];
	initHashTable(ht, htSize);
	Value examplePntr;
	examplePntr.mVal=&htSize; //m stands for matrix
	Value exampleInt;
	exampleInt.iVal=(int)htSize;
	Value exampleFloat;
	exampleFloat.fVal=3.13f;
	const char *mStr="mon_pointeur";
	const char *iStr="mon_int";
	const char *fStr="mon_float";
	createOrReplaceOnHashTable(ht, mStr, examplePntr, strlen(mStr), htSize); //in yacc use yyleng
	createOrReplaceOnHashTable(ht, iStr, exampleInt, strlen(iStr), htSize);
	createOrReplaceOnHashTable(ht, fStr, exampleFloat, strlen(fStr), htSize);
	exampleFloat.fVal=3.14f;
	createOrReplaceOnHashTable(ht, fStr, exampleFloat, strlen(fStr), htSize);
	debugPrintHashTable(ht, htSize);
	freeAll(ht, htSize);
	return 0;
}
