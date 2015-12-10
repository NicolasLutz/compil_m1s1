#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "quad.h"
#include "symbol.h"

const char *_g_instrDesc[18]={"Affectation", "Addition", "Substraction", "Multiplication",
              "Division", "Negation", "Print", "Printf", "Printmat",
              "Label", "Goto", "Geq", "Leq", "Lower", "Greater", "Equal", "Not equal",
              "Instruction deleted"};

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

Quad Q_genGoto()
{
  Quad q;
  q.op       = GOTO_I;
  q.arg1     = NULL;
  q.arg2     = NULL;
  q.res      = NULL;
  q.next     = NULL;
  return q;
}

Quad Q_genLabel()
{
  static unsigned int labelNum=1;
  Quad q;
  q.op       = LABEL_I;
  q.arg1     = NULL;
  q.arg2     = NULL;
  q.res      = (Symbol *)labelNum++; //look away
  q.next     = NULL;
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
  q->res=label; //they'll never know
}

void Q_writeMIPS(const Quad *q, FILE *f)
{
  assert(f!=NULL && q!=NULL);
  switch (q->op)
  {
    case AFF_I:
      fprintf(f, "lw $t0, %s\nsw $t0, %s\n",
        q->arg1->name, q->res->name);
      break;
    case ADD_I:
      fprintf(f, "lw $t1, %s\nlw $t2, %s\nadd $t0, $t1, $t2\nsw $t0, %s\n",
        q->arg1->name, q->arg2->name, q->res->name);
      break;
    case SUB_I:
      fprintf(f, "lw $t1, %s\nlw $t2, %s\nsub $t0, $t1, $t2\nsw $t0, %s\n",
        q->arg1->name, q->arg2->name, q->res->name);
      break;
    case MULT_I:
      fprintf(f, "lw $t1, %s\nlw $t2, %s\nmult $t1, $t2\nsw $LO, %s\n",
        q->arg1->name, q->arg2->name, q->res->name);
      break;
    case DIV_I:
      fprintf(f, "lw $t1, %s\nlw $t2, %s\ndiv $t1, $t2\nsw $LO, %s\n",
        q->arg1->name, q->arg2->name, q->res->name);
      break;
    case NEG_I:
      fprintf(f, "lw $t1, %s\nneg $t0, $t1\nsw $t0, %s\n",
        q->res->name, q->res->name);
        break;
    case PRINT_I:
      switch(q->arg1->info.type)
      {
        case INT_T:
          fprintf(f, "li $v0, 1\nlw $a0, %s\nsyscall\n",
            q->arg1->name);
          break;
        case FLOAT_T:
          fprintf(f, "li $v0, 2\nlw $f12, %s\nsyscall\n",
            q->arg1->name);
          break;
        default:
          //TODO
          break;
      }
      break;
    case PRINTF_I:
      //TODO
      break;
    case PRINTMAT_I:
      //TODO
      break;
    case LABEL_I:
      fprintf(f, "l%d: ", //TODO : make a l0 for error handling
        (unsigned int)q->res);
      break;
    case GOTO_I:
      fprintf(f, "j %d\n",
        (unsigned int)q->res);
      break;
    case B_GEQ_I:
      fprintf(f, "lw $t1, %s\nlw $t2, %s\nbge $t1, $t2, l%d\n",
        q->arg1->name, q->arg2->name, (unsigned int)q->res);
      break;
    case B_LEQ_I:
      fprintf(f, "lw $t1, %s\nlw $t2, %s\nble $t1, $t2, l%d\n",
        q->arg1->name, q->arg2->name, (unsigned int)q->res);
      break;
    case B_G_I:
      fprintf(f, "lw $t1, %s\nlw $t2, %s\nbgt $t1, $t2, l%d\n",
        q->arg1->name, q->arg2->name, (unsigned int)q->res);
      break;
    case B_L_I:
      fprintf(f, "lw $t1, %s\nlw $t2, %s\nblt $t1, $t2, l%d\n",
        q->arg1->name, q->arg2->name, (unsigned int)q->res);
      break;
    case B_EQ_I:
      fprintf(f, "lw $t1, %s\nlw $t2, %s\nbeq $t1, $t2, l%d\n",
        q->arg1->name, q->arg2->name, (unsigned int)q->res);
      break;
    case B_NEQ_I:
      fprintf(f, "lw $t1, %s\nlw $t2, %s\nbne $t1, $t2, l%d\n",
        q->arg1->name, q->arg2->name, (unsigned int)q->res);
      break;
    default:
    //TODO
    break;
  }
}

void Q_print(const Quad *q)
{
  if(q->op>9)
  {
    printf("%12s %12s %12s %12s%d\n",
      InstructionDesc(q->op),
      (q->arg1 ? q->arg1->name : ""),
      (q->arg2 ? q->arg2->name : ""),
      (q->res ? "Label " : "GOTO_EMPTY"),
      (unsigned int)q->res
    );
  }
  else
    switch(q->op)
    {
      case LABEL_I:
        printf("Label %d:\n", (unsigned int)q->res);
        break;
      default:
        printf("%12s %12s %12s %12s\n",
          InstructionDesc(q->op),
          (q->arg1 ? q->arg1->name : ""),
          (q->arg2 ? q->arg2->name : ""),
          (q->res  ? q->res->name  : "")
        );
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
  if(ql1==NULL)
    return ql2;
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
  free(ql2); //TODO
  return ql1;
}

void QL_backpatch(QuadList *ql, Quad *labelQuad)
{
    if(ql==NULL)
      return;
    Quad *q=ql->head;
    while(q!=NULL)
    {
      Q_backpatch(q, labelQuad->res);
      q=q->next;
    }
}

void QL_print (const QuadList *ql)
{
  assert(ql!=NULL);
  Quad *q=ql->head;
  while(q!=NULL)
  {
    Q_print(q);
    q=q->next;
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
  qt->size=size;
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

Quad *QT_get(const QuadTab *qt, unsigned int index)
{
  assert(qt!=NULL);
  while(qt->size<=index)
  {
    index-=qt->size;
    qt=qt->next;
  }
  return &qt->tab[index];
}

void QT_print(const QuadTab *qt)
{
  assert(qt!=NULL);
  printf("%12s=%12s=%12s=%12s\n",
    " Instruction",
    "==Argument 1",
    "==Argument 2",
    "Return/Label");
  unsigned int headIndex=0;
  while(headIndex!=qt->index)
  {
    Q_print(&qt->tab[headIndex++]);
    if(headIndex>=qt->size)
    {
      qt=qt->next;
      headIndex=0;
    }
  }
}

void QT_writeMIPS(const QuadTab *qt, const char *filename)
{
  assert(qt!=NULL);
  FILE *f=fopen(filename, "w");
  assert(f!=NULL);
  unsigned int headIndex=0;
  while(headIndex!=qt->index)
  {
    Q_writeMIPS(&qt->tab[headIndex++], f);
    if(headIndex>=qt->size)
    {
      qt=qt->next;
      headIndex=0;
    }
  }
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
