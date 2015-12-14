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
    case AFF_I: //no need to distinct between int and float
      fprintf(f, "lw $t0, %s_var\nsw $t0, %s_var\n",
        q->arg1->name, q->res->name);
      break;
    case ADD_I:
      switch(q->arg1->info.type)
      {
        case INT_T:
          fprintf(f, "lw $t1, %s_var\nlw $t2, %s_var\nadd $t0, $t1, $t2\nsw $t0, %s_var\n",
            q->arg1->name, q->arg2->name, q->res->name);
          break;
        case FLOAT_T:
          fprintf(f, "l.s $f1, %s_var\nl.s $f2, %s_var\nadd.s $f0, $f1, $f2\ns.s $f0, %s_var\n",
            q->arg1->name, q->arg2->name, q->res->name);
          break;
        default:
          break;
      }
      break;
    case SUB_I:
      switch(q->arg1->info.type)
      {
        case INT_T:
          fprintf(f, "lw $t1, %s_var\nlw $t2, %s_var\nsub $t0, $t1, $t2\nsw $t0, %s_var\n",
            q->arg1->name, q->arg2->name, q->res->name);
          break;
        case FLOAT_T:
          fprintf(f, "l.s $f1, %s_var\nl.s $f2, %s_var\nsub.s $f0, $f1, $f2\ns.s $f0, %s_var\n",
            q->arg1->name, q->arg2->name, q->res->name);
          break;
        default:
          break;
      }
      break;
    case MULT_I:
      switch(q->arg1->info.type)
      {
        case INT_T:
          fprintf(f, "lw $t1, %s_var\nlw $t2, %s_var\nmult $t1, $t2\nsw $LO, %s_var\n",
            q->arg1->name, q->arg2->name, q->res->name);
          break;
        case FLOAT_T:
          fprintf(f, "l.s $f1, %s_var\nl.s $f2, %s_var\nmul.s $f0, $f1, $f2\ns.s $f0, %s_var\n",
            q->arg1->name, q->arg2->name, q->res->name);
          break;
        default:
          break;
      }
      break;
    case DIV_I:
      switch(q->arg1->info.type)
      {
        case INT_T:
          fprintf(f, "lw $t1, %s_var\nlw $t2, %s_var\ndiv $t1, $t2\nsw $LO, %s_var\n",
            q->arg1->name, q->arg2->name, q->res->name);
          break;
        case FLOAT_T:
          fprintf(f, "l.s $f1, %s_var\nl.s $f2, %s_var\ndiv $f0, $f1, $f2\ns.s $f0, %s_var\n",
            q->arg1->name, q->arg2->name, q->res->name);
          break;
        default:
          break;
      }
      break;
    case NEG_I:
      switch(q->arg1->info.type)
      {
        case INT_T:
          fprintf(f, "lw $t1, %s_var\nneg $t0, $t1\nsw $t0, %s_var\n",
            q->res->name, q->res->name);
            break;
        case FLOAT_T:
          fprintf(f, "l.s $f1, %s_var\nneg.s $f0, $f1\ns.s $f0, %s_var\n",
            q->res->name, q->res->name);
          break;
        default:
          break;
      }
    case PRINT_I:
      switch(q->arg1->info.type)
      {
        case INT_T:
          fprintf(f, "li $v0, 1\nlw $a0, %s_var\nsyscall\n",
            q->arg1->name);
          break;
        case FLOAT_T:
          fprintf(f, "li $v0, 2\nl.s $f12, %s_var\nsyscall\n",
            q->arg1->name);
          break;
        default:
          //TODO
          break;
      }
      break;
    case PRINTF_I:
      fprintf(f, "la $a0, %s_var\nli $v0, 4\nsyscall\n",
        q->arg1->name);
      break;
    case PRINTMAT_I:
      //TODO
      break;
    case LABEL_I:
      fprintf(f, "l%d: ", //TODO : make a l0 for error handling
        (unsigned int)q->res);
      break;
    case GOTO_I:
      fprintf(f, "j l%d\n",
        (unsigned int)q->res);
      break;
    case B_GEQ_I:
      switch(q->arg1->info.type)
      {
        case INT_T:
          fprintf(f, "lw $t1, %s_var\nlw $t2, %s_var\nbge $t1, $t2, l%d\n",
            q->arg1->name, q->arg2->name, (unsigned int)q->res);
          break;
        case FLOAT_T:
          fprintf(f, "l.s $f1, %s_var\nl.s $f2, %s_var\nc.lt.s $f2, $f1, l%d\n", //really now mips ?
            q->arg1->name, q->arg2->name, (unsigned int)q->res);
          break;
        default:
          break;
      }
      break;
    case B_LEQ_I:
      switch(q->arg1->info.type)
      {
        case INT_T:
          fprintf(f, "lw $t1, %s_var\nlw $t2, %s_var\nble $t1, $t2, l%d\n",
            q->arg1->name, q->arg2->name, (unsigned int)q->res);
          break;
        case FLOAT_T:
          fprintf(f, "l.s $f1, %s_var\nl.s $f2, %s_var\nc.le.s $f1, $f2, l%d\n",
            q->arg1->name, q->arg2->name, (unsigned int)q->res);
          break;
        default:
          break;
      }
      break;
    case B_G_I:
      switch(q->arg1->info.type)
      {
        case INT_T:
          fprintf(f, "lw $t1, %s_var\nlw $t2, %s_var\nbgt $t1, $t2, l%d\n",
            q->arg1->name, q->arg2->name, (unsigned int)q->res);
          break;
        case FLOAT_T:
          fprintf(f, "l.s $f1, %s_var\nl.s $f2, %s_var\nc.le.s $f2, $f1, l%d\n",
            q->arg1->name, q->arg2->name, (unsigned int)q->res);
        default:
          break;
      }
      break;
    case B_L_I:
      switch(q->arg1->info.type)
      {
        case INT_T:
          fprintf(f, "lw $t1, %s_var\nlw $t2, %s_var\nblt $t1, $t2, l%d\n",
            q->arg1->name, q->arg2->name, (unsigned int)q->res);
          break;
        case FLOAT_T:
          fprintf(f, "l.s $f1, %s_var\nl.s $f2, %s_var\nc.lt.s $f1, $f2, l%d\n",
            q->arg1->name, q->arg2->name, (unsigned int)q->res);
          break;
        default:
          break;
      }
      break;
    case B_EQ_I:
      fprintf(f, "lw $t1, %s_var\nlw $t2, %s_var\nbeq $t1, $t2, l%d\n",
        q->arg1->name, q->arg2->name, (unsigned int)q->res);
      break;
    case B_NEQ_I:
      fprintf(f, "lw $t1, %s_var\nlw $t2, %s_var\nbne $t1, $t2, l%d\n",
        q->arg1->name, q->arg2->name, (unsigned int)q->res);
      break;
    default:
    //TODO
    break;
  }
}

void Q_print(const Quad *q)
{
  if(q->op>9 && q->op<17) //gotos range
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
  if(ql2==NULL)
    return ql1;
  if(QL_empty(ql2))
  {
    QL_free(ql2);
    return ql1;
  }
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
  QL_free(ql2);
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
    QL_free(ql); //we don't need ql anymore
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

void QT_writeMIPS(const QuadTab *qt, FILE *f)
{
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

void QT_destroy(QuadTab *qt)
{
  QuadTab *next;
  while(qt!=NULL)
  {
    next=qt->next;
    free(qt->tab);
    free(qt);
    qt=next;
  }
}

//================================================================================================

void MATC_Compile(QuadTab *qt, SymbolTable *st, const char *filename)
{
  assert(qt!=NULL && st!=NULL && filename!=NULL);
  FILE *f=fopen(filename, "w");
  assert(f!=NULL);
  fprintf(f, ".data\n");
  ST_writeMIPS(st, f);
  fprintf(f, ".text\nmain:\n");
  QT_writeMIPS(qt, f);
  fprintf(f, "jr $ra\n");
  fclose(f);
}

void MATC_error()
{
  compiledCorrectly=0;
  fprintf(stderr, "Error: ");
}

void MATC_error_undeclared(const char *string)
{
  fprintf(stderr, "%s undeclared (first use in function)\n",
    string);
}

void MATC_warning()
{
  fprintf(stderr, "Warning: ");
}

//for the following, please ensure parameters aren't null !

void MATC_warning_unsafeAffectation(Symbol *s1, Symbol *s2)
{
  assert(s1 && s2);
  fprintf(stderr, "Unsafe affectation from %s of type %s to %s of type %s\n",
    s2->name, SI_typeToString(s2->info.type), s1->name, SI_typeToString(s1->info.type));
}

void MATC_warning_unsafeOperation(Symbol *s1, Symbol *s2)
{
  assert(s1 && s2);
  fprintf(stderr, "Unsafe operation (%s of type %s and %s of type %s)\n",
    s1->name, SI_typeToString(s1->info.type), s2->name, SI_typeToString(s2->info.type));
}

void MATC_warning_unsafeComparasion(Symbol *s1, Symbol *s2)
{
  assert(s1 && s2);
  fprintf(stderr, "Unsafe comparasion between %s of type %s and %s of type %s\n",
    s1->name, SI_typeToString(s1->info.type), s2->name, SI_typeToString(s2->info.type));
}
