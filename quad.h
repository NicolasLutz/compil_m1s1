#ifndef __QUAD__H__
#define __QUAD__H__

#include "symbol.h"

typedef enum Instruction
{
    GOTO_I, PLUS_I, MINUS_I, MULT_I, DIV_I, U_MINUS_I,
    PRINT_I, PRINTF_I, PRINTMAT_I,
    AFF_I
} Instruction;


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



Quad *Q_gen(Instruction op, Symbol* arg1, Symbol* arg2, Symbol* res);
Quad *Q_concat(Quad *q1, Quad *q2);

void QL_add (QuadList *ql, Quad* quad);
void Q_print (QuadList *ql);



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
