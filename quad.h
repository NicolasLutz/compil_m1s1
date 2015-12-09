#ifndef __QUAD__H__
#define __QUAD__H__

#include "symbol.h"

#define P_DEBUG

//possible instructions
typedef enum Instruction
{
    AFF_I=0, ADD_I=1, SUB_I=2, MULT_I=3, DIV_I=4, NEG_I=5,  //=, +, -, /, *, unary -
    PRINT_I=6, PRINTF_I=7, PRINTMAT_I=8,                     //prints
    LABEL_I=9, GOTO_I=10,                                  //goto, label
    B_GEQ_I=11, B_LEQ_I=12, B_L_I=13, B_G_I=14, B_EQ_I=15, B_NEQ_I=16 //booleans (G=greater, L=lower, EQ=equal)

} Instruction;

#define GOTO_EMPTY NULL

#ifdef P_DEBUG
extern const char *_g_instrDesc[17];
#define InstructionDesc(i) (_g_instrDesc[(int)(i)])
#else
#define InstructionDesc(i) (NULL)
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

/*
%union
{
	struct //%type <codeCondition> condition
	{
		Quad *code;
		QuadList *trueList;
		QuadList *falseList;
	} codeCondition;
	struct //%type <codeStatement> statement
	{
		Quad *code;
		QuadList *nextList;
	} codeStatement;
	Symbol *quad; //%type <quad> tag
	struct	//%type <codeGoto> tagoto
	{
		Symbol *quad;
		Quad *code;
		QuadList *nextList;
	} codeGoto;
}*/

/*SOME YACC CODE
tag: //< tag ..?
	{
		$$ = C_gen(nextQuad); //new constant
	};
tagoto: //< marque un futur goto
	{
		$$.code=GOTO_gen(NULL);
		$$.quad=C_gen(nextQuad);
		$$.nextList=QL_gen(tagoto.code);
	};
list: list1 ';' tag statement //< combine les statements
	{
		QL_completeTag(list1.nextList, $3);
		$$.nextList=$4.nextList;
		$$.code=Q_concat($1.code, $4.code);
	};
statement: IF '(' cond ')' tag statement1 //< if then
	{
		QL_completeTag($3.truelist, $5);
		$$.nextList = QL_concat($6.nextList, $3.falseList);
		$$.code = Q_concat($3.code, $6.code);
	}
	| IF '(' cond ')' tag statemen1 ELSE tagoto statement1
	{
		QL_completeTag($3.trueList, $5);
		QL_complete($3.falseList, $8.quad);
		$$.nextList = QL_concat(QL_concat($6.nextList, $9.nextList), $8.nextList);
		$$.code = Q_concat(Q_concat(Q_concat($3.code, $6.code), $8.code), $9.code);
	}
	| WHILE tag1 '(' cond ')' tag2 statement1
	{
		QL_completeTag($4.trueList, $6);
		QL_completeTag($7.nextList, $2);
		$$.nextList=$4.falseList;
		$$.code = Q_concat(Q_concat($4.code, $7.code),GOTO_gen($2));
	};
*/


#endif
