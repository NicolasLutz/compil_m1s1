#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "quad.h"
#include "symbol.h"

Quad* Q_gen(Instruction op, Symbol* arg1, Symbol* arg2, Symbol* res)
{
    Quad* q = malloc(sizeof(*q));
    q->op       = op;
    q->arg1     = arg1;
    q->arg2     = arg2;
    q->res      = res;
    q->next     = NULL;
    return q;
}

Quad *Q_genGoto()
{
    Quad* q = malloc(sizeof(*q));
    q->op       = GOTO_I;
    q->arg1     = NULL;
    q->arg2     = NULL;
    q->res      = NULL;
    return q;
}

Quad *Q_concat(Quad *q1, Quad *q2)
{
    assert(q1->next==NULL);
    q1->next=q2;
    return q1;
}

Quad *Q_writeMIPS(Quad *q, FILE *f)
{
  assert(f!=NULL && q!=NULL);
  switch (q->op)
  {
    case AFF_I:
      break;
    case PLUS_I:
      break;
    case MINUS_I:
      break;
    case MULT_I:
      break;
    case DIV_I:
      break;
    case U_MINUS_I:
      break;
    case PRINT_I:
      break;
    case PRINTF_I:
      break;
    case PRINTMAT_I:
      break;
    case GOTO_I:
      break;
    case IF_GOTO_I:
      break;
    default:
      break;
  }
}

//================================================================================================

QuadList *QL_gen()
{
    QuadList *ql=malloc(sizeof(*ql));
    ql->head=NULL;
    ql->tail=NULL;
    return ql;
}

void QL_add (QuadList *ql, Quad* quad)
{
	assert(ql!=NULL);
	if(!ql->head)
	{
        ql->head=quad;
        ql->tail=quad;
	}
	else
	{
        ql->tail->next=quad;
        ql->tail=quad;
	}
	return;
}

void QL_print (QuadList *ql)
{
    assert(ql!=NULL);
	Quad *q=ql->head;
	while(q!=NULL)
	{
		printf("%c %7s %7s %7s\n",
                q->op,
                (q->arg1 ? q->arg1->name : "None"),
                (q->arg2 ? q->arg2->name : "None"),
                (q->res  ? q->res->name  : "None"));
		q=q->next;
	}
	return;
}

void QL_writeMIPS(QuadList *ql, const char *filename)
{
  assert(ql!=NULL);
  FILE *f=fopen(filename, "a");
  Quad *q=ql->head;
  while(q!=NULL)
  {
    Q_writeMIPS(q, f);
    q=q->next;
  }
}

//================================================================================================

//Usage example
int main()
{
  size_t stSize=128;
  SymbolTable *st=ST_gen(stSize);
  SymbolInfo examplePntr=SI_genArray(&stSize);
  SymbolInfo exampleInt=SI_genInt(stSize);
  SymbolInfo exampleFloat=SI_genFloat(3.13f);
  const char *mStr="mon_pointeur"; //m stands for matrix
  const char *iStr="mon_int";
  const char *fStr="mon_float";
  ST_add(st, mStr, &examplePntr);
  ST_add(st, iStr, &exampleInt);
  ST_add(st, fStr, &exampleFloat);

  exampleFloat.value.fVal=3.14f;
  ST_add(st, fStr, &exampleFloat);

  exampleFloat.value.fVal=7.08f;
  ST_addTmp(st, &exampleFloat);

  exampleInt.value.iVal=1;
  ST_addTmp(st, &exampleInt);

  ST_print(st);
  ST_writeMIPS(st, "test.s");

  //quads
  Quad *q1=Q_gen(AFF_I, ST_lookup(st, "__tmp_0"), NULL, ST_lookup(st, iStr));
  Quad *q2=Q_gen(ADD_I, ST_lookup(st, iStr), ST_lookup(st, "__tmp_1"), ST_lookup(st, iStr));

  ST_destroy(st);
  return 0;
}
