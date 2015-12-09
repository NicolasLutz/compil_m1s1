#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "quad.h"
#include "symbol.h"

const char *_g_instrDesc[17]={"Affectation", "Addition", "Substraction", "Multiplication",
              "Division", "Negation", "Print", "Printf", "Printmat",
              "Goto", "Label", "Geq", "Leq", "Lower", "Greater", "Equal", "Not equal"};

Quad Q_gen(Instruction op, Symbol* arg1, Symbol* arg2, Symbol* res)
{
  Quad q;
  q.op       = op;
  q.arg1     = arg1;
  q.arg2     = arg2;
  q.res      = res;
  q.next     = NULL;
  return q;
}

Quad *Q_genGoto()
{
  Quad* q = (Quad *)malloc(sizeof(*q));
  q->op       = GOTO_I;
  q->arg1     = NULL;
  q->arg2     = NULL;
  q->res      = NULL;
  return q;
}

Quad *Q_genLabel()
{
  static unsigned int labelNum=0;
  Quad *q = (Quad *)malloc(sizeof(*q));
  q->op       = LABEL_I;
  q->arg1     = NULL;
  q->arg2     = NULL;
  q->res      = (Symbol *)labelNum++; //look away
  return q;
}

Quad *Q_concat(Quad *q1, Quad *q2)
{
  assert(q1->next==NULL);
  q1->next=q2;
  return q1;
}

void Q_backpatch(Quad *q, Symbol *label)
{
    if(q->op==GOTO_I)
      q->res=label; //they'll never know
}

void Q_writeMIPS(const Quad *q, FILE *f)
{
  assert(f!=NULL && q!=NULL);
  switch (q->op)
  {
    case AFF_I:
    break;
    case ADD_I:
    break;
    case SUB_I:
    break;
    case MULT_I:
    break;
    case DIV_I:
    break;
    case NEG_I:
    break;
    case PRINT_I:
    break;
    case PRINTF_I:
    break;
    case PRINTMAT_I:
    break;
    case GOTO_I:
    break;
    case LABEL_I:
    break;
    case B_GEQ_I:
    break;
    case B_LEQ_I:
    break;
    case B_G_I:
    break;
    case B_L_I:
    break;
    case B_EQ_I:
    break;
    case B_NEQ_I:
    break;
    default:
    break;
  }
}

void Q_free(Quad *q)
{
  free(q);
  return;
}

//================================================================================================

QuadList *QL_gen()
{
  QuadList *ql=(QuadList *)malloc(sizeof(*ql));
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

int QL_empty(QuadList *ql)
{
  assert(ql!=NULL);
  return (ql->head==NULL);
}

QuadList *QL_concat(QuadList *ql1, QuadList* ql2)
{
  assert(ql1!=NULL);
  if(ql2==NULL || QL_empty(ql2))
    return ql1;
  if(ql1->tail==NULL)
  {
    ql1->tail=ql2->tail;
    ql1->head=ql2->head;
  }
  else
  {
    ql1->tail->next=ql2->head;
    ql1->tail=ql2->tail;
  }
  free(ql2);
  return ql1;
}

void QL_backpatch(QuadList *ql, Symbol *label)
{
    if(ql==NULL)
      return;
    Quad *q=ql->head;
    while(q!=NULL)
    {
      Q_backpatch(q, label);
      q=q->next;
    }
}

void QL_print (const QuadList *ql)
{
  assert(ql!=NULL);
  static int labelNum=0; //lalalalala I can't hear you
  Quad *q=ql->head;
  while(q!=NULL)
  {
    switch(q->op)
    {
      case LABEL_I:
        printf("L%d:\n", labelNum++);
        break;
      default:
        printf("%s %7s %7s %7s\n",
        InstructionDesc(q->op),
        (q->arg1 ? q->arg1->name : "None"),
        (q->arg2 ? q->arg2->name : "None"),
        (q->res  ? q->res->name  : "None"));
        q=q->next;
        break;
    }

  }
  return;
}

void QL_free(QuadList *ql)
{
  assert(ql!=NULL);
  free(ql);
  return;
}


//================================================================================================

QuadTab *QT_gen(size_t size)
{
  assert(size>0);
  QuadTab *qt=(QuadTab *)malloc(sizeof(*qt));
  qt->tab=(Quad *)malloc(size*sizeof(*qt->tab));
  qt->index=0;
  qt->next=NULL;
  return qt;
}

Quad *QT_add(QuadTab *qt, Quad *quad)
{
  assert(qt!=NULL);
  Quad *res;
  while(qt->index>=qt->size && qt->next!=NULL)
  {
    qt->index++;
    qt=qt->next;
  }
  if(qt->index<qt->size)
  {
    qt->tab[qt->index]=*quad;
    res=&qt->tab[qt->index++];
  }
  else
  {
    qt->next=QT_gen(qt->size);
    qt->next->tab[qt->next->index]=*quad;
    res=&qt->next->tab[qt->next->index++];
  }
  return res;
}

Quad *QT_get(QuadTab *qt, unsigned int index)
{
  assert(qt!=NULL);
  while(qt->size<=index)
  {
    index-=qt->size;
    qt=qt->next;
  }
  return &qt->tab[index];
}

//Usage example
/*
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
}*/
