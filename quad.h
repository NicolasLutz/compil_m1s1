#ifndef __QUAD__H__
#define __QUAD__H__

#include "symbol.h"

extern int compiledCorrectly;

#define P_DEBUG

//possible instructions
typedef enum Instruction
{
    AFF_I=0, ADD_I=1, SUB_I=2, MULT_I=3, DIV_I=4, NEG_I=5,  //=, +, -, /, *, unary -
    PRINT_I=6, PRINTF_I=7, PRINTMAT_I=8,                     //prints
    LABEL_I=9, GOTO_I=10,                                  //goto, label
    B_GEQ_I=11, B_LEQ_I=12, B_L_I=13, B_G_I=14, B_EQ_I=15, B_NEQ_I=16, //booleans (G=greater, L=lower, EQ=equal)
    NO_INSTRUCTION_I=17
} Instruction;

#define GOTO_EMPTY NULL

#ifdef P_DEBUG
extern const char *_g_instrDesc[18];
#define InstructionDesc(i) (_g_instrDesc[(unsigned int)(i)])
#else
#define InstructionDesc(i) ("(Release mode)")
#endif


typedef struct strQuad {
    Instruction op;
	Symbol* arg1;
	Symbol* arg2;
	Symbol* res;
	struct strQuad* next;
} Quad;

typedef struct
{
	Quad *head;
	Quad *tail;
} QuadList;

typedef struct strQuadTab
{
  Quad *tab;
  unsigned int index;
  size_t size;
  struct strQuadTab *next; //avoids overflow and realloc
} QuadTab;

Quad        Q_gen(Instruction op, Symbol* arg1, Symbol* arg2, Symbol* res);
Quad        Q_genGoto();
Quad        Q_genLabel();
Quad        *Q_concat(Quad *q1, Quad *q2);
void        Q_writeMIPS(const Quad *q, FILE *f);
void        Q_print(const Quad *q);
void        Q_free(Quad *q);
void        Q_backpatch(Quad *q, Symbol *label);

//================================================================================================

QuadList    *QL_gen();
void        QL_add (QuadList *ql, Quad *quad);
QuadList    *QL_concat (QuadList *ql1, QuadList *ql2);
void        QL_backpatch(QuadList *ql, Quad *labelQuad);
void        QL_print (const QuadList *ql);
void        QL_free(QuadList *ql);

//================================================================================================

QuadTab     *QT_gen();
Quad        *QT_add(QuadTab *qt, Quad *quad);
Quad        *QT_get(const QuadTab *qt, unsigned int index);
void        QT_print(const QuadTab *qt);
void        QT_writeMIPS(const QuadTab *qt, FILE *f);

//================================================================================================

void MATC_Compile(QuadTab *qt, SymbolTable *st, const char *filename);
//void MATC_Optimize(QuadTab *qt);
void MATC_error();
void MATC_warning();

#endif
