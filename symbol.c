	#include "symbol.h"

	const char *_g_typeDesc[4]={"int", "float", "matrix", "string {aka (char *)}"};

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
		info.cst=false;
		return info;
	}

	//=============================================================================

	SymbolInfo SI_genFloat(float value)
	{
		SymbolInfo info;
		info.value.fVal=value;
		info.type=FLOAT_T;
		info.cst=false;
		return info;
	}

	//=============================================================================

	SymbolInfo SI_genString(char *value)//no copy
	{
		SymbolInfo info;
		info.value.sVal=value;
		info.type=STRING_T;
		info.cst=false; //not THAT kind of cst!
		return info;
	}

//=============================================================================

	SymbolInfo SI_genArray(void *value)
	{
		SymbolInfo info;
		info.value.mVal=value;
		info.type=ARRAY_T;
		info.cst=false;
		return info;
	}

const char *SI_typeToString(Typename t)
{
	return _g_typeDesc[t];
}

bool SI_castFloat(SymbolInfo *si)
{
	if(si==NULL)
		return false;
	if(si->type==FLOAT_T)
		return true;
	if(si->cst && (si->type==INT_T))
	{
		si->value.fVal=(float)si->value.iVal;
		si->type=FLOAT_T;
		return true;
	}
	return false;
}

	//=============================================================================
	//=============================================================================

	Symbol S_gen(char *name, SymbolInfo *info)
	{
		assert(name!=NULL);
		Symbol s;
		s.info=(info!=NULL ? *info : SI_genInt(0));
		s.name=name;//strndup(name, maxStrLength);
		return s;
	}

	bool S_castFloat(Symbol *s1, Symbol *s2)
	{
		bool castFine=true;
		if(s1!=NULL && s2!=NULL && s1->info.type!=s2->info.type)
		{
			if(s2->info.cst && s1->info.type==FLOAT_T)
			{
				if(!SI_castFloat(&s2->info))
					castFine=false;
			}
			else if(s1->info.cst && s2->info.type==FLOAT_T)
			{
				if(!SI_castFloat(&s1->info))
					castFine=false;
			}
			else
				castFine=false;
		}
		return castFine;
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
		ST_add(st, strndup("__zero",maxStrLength), &siZero);
		ST_add(st, strndup("__one",maxStrLength),  &siOne);
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

Symbol *ST_add(SymbolTable *st, char *name, SymbolInfo *info)
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
			info->cst=true; //it's also a cst

	    sprintf(st->_tmpName,"__tmp_%d",count++);
	    return ST_add(st,strndup(st->_tmpName, maxStrLength),info);
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
					case STRING_T:
						printf("%3d (const string)%s : %s\n",
							i, l->symbol.name, l->symbol.info.value.sVal);
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
						fprintf(f, "%s_var: .word %d\n", l->symbol.name, l->symbol.info.value.iVal);
						break;
					case FLOAT_T:
						fprintf(f, "%s_var: .float %f\n", l->symbol.name, l->symbol.info.value.fVal);
						break;
					case STRING_T:
						fprintf(f, "%s_var: .asciiz %s\n", l->symbol.name, l->symbol.info.value.sVal);
						break;
					default:
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
				if(l->symbol.info.type==ARRAY_T)
					free(l->symbol.info.value.mVal);
				else if(l->symbol.info.type==STRING_T)
					free(l->symbol.info.value.sVal);
				free(l);
			}
			free(st->sl[i]);
		}
		free(st->sl);
		free(st);
	}
