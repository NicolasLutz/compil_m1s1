	#include "symbol.h"

	int _genIndex(const char *string)
	{
		assert(string!=NULL);
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

	Symbol S_gen(const char *name, SymbolInfo *info)
	{
		assert(name!=NULL);
		Symbol s;
		s.info=(info!=NULL ? *info : SI_genInt(0));
		s.name=strndup(name, maxStrLength);
		return s;
	}

	//=============================================================================
	//=============================================================================

	Link *L_gen(Symbol *s)
	{
		Link *l=(Link *)malloc(sizeof(*l));
	    l->symbol=*s;
		l->next=NULL;
		return l;
	}

	//=============================================================================
	//=============================================================================

	SymbolList *SL_gen()
	{
		SymbolList *sl=(SymbolList *)malloc(sizeof(*sl));
		sl->head=NULL;
		sl->tail=NULL;
		return sl;
	}

	//=============================================================================

	void SL_add(SymbolList *sl, Link *l)
	{
		assert(sl!=NULL && l!=NULL);
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
		assert(size>0);
		unsigned int i;
		SymbolTable *st=(SymbolTable *)malloc(sizeof(*st));
		st->sl=(SymbolList **)malloc(size*sizeof(*st->sl));
		st->size=size;
		for(i=0; i<size; i++)
		{
			st->sl[i]=SL_gen();
		}
		SymbolInfo siZero	=SI_genInt(0);
		SymbolInfo siOne 	=SI_genInt(1);
		ST_add(st, "__zero", &siZero);
		ST_add(st, "__one", &siOne);
		return st;
	}

	//=============================================================================

	Symbol *ST_lookup(SymbolTable *st, const char *name)
	//find the entry corresponding to name.
	//returns NULL if not found.
	{
		assert(st!=NULL && name!=NULL);
		int index=_genIndex(name)%st->size;
		Link *l=st->sl[index]->head;
		Symbol *s=NULL;
		int found=0;
		while(!found && l!=NULL)
		{
			if(_straightStrCmp(name, l->symbol.name))
			{
				s=&l->symbol;
				found=1;
			}
			l=l->next;
		}
		return s;
	}

Symbol *ST_add(SymbolTable *st, const char *name, SymbolInfo *info)
	//creates or reslace an entry in st with name name and informations info.
	//returns the symbol.
	{
		assert(st!=NULL && name!=NULL);
		int index=_genIndex(name)%st->size;
		Symbol s;
		Link *l=st->sl[index]->head;
		int found=0;
		while(l!=NULL && !found)
		{
			if(_straightStrCmp(name, l->symbol.name))
			{
				l->symbol.info=(info!=NULL ? *info : SI_genInt(0));
				found=1;
			}
			else
				l=l->next;
		}
		if(!found)
	    {
					s=S_gen(name, info);
	        l=L_gen(&s);
	        SL_add(st->sl[index],l);
	    }
		return &l->symbol;
	}

	Symbol *ST_addTmp(SymbolTable *st, SymbolInfo *info)
	{
	    static unsigned int count=0;
	    sprintf(st->_tmpName,"__tmp_%d",count++);
	    return ST_add(st,st->_tmpName,info);
	}

	void ST_print(const SymbolTable *st) //debug purposes only
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
						printf("%3d    %12d  %s\n",
							i, l->symbol.info.value.iVal, l->symbol.name);
						break;
					case FLOAT_T:
						printf("%3d    %12f  %s\n",
							i, l->symbol.info.value.fVal, l->symbol.name);
						break;
					default:
						printf("%3d    %12p  %s\n",
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
	        while((l=st->sl[i]->head))
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

	void ST_writeMIPS(const SymbolTable *st, FILE *f)
	{
		assert(f!=NULL);
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
		return;
	}
