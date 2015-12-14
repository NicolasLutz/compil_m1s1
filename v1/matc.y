%{

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "quad.h"
#include "symbol.h"

void yyerror(char * s);
int yylex();
void lex_free();

QuadTab 			*g_qt=NULL;
SymbolTable 	*g_st=NULL;

int next_quad =0;

%}

%union{
	char 				*stringVal;
	int 				intVal;
	Symbol      *symbol_t;
	QuadList    *qlist_t;
	Quad        *quad_t;
	Instruction instruction_t;
	struct //%type <codeCondition> condition
	{
		QuadList *trueList;
		QuadList *falseList;
	} codeCondition_t;
	
	struct 
	{	
		Symbol      *result;
		Quad        *code;
	} codegen
}


%token YASSIGN YWHILE YDO YIF YELSE YRETURN
%token YEQUAL YNEQ YTRUE YFALSE YOR
%token YINT YFLOAT
%token <intVal> YNUM
%token <stringVal> YID

%type <codeCondition_t> condition
%type <symbol_t> expr
%type <symbol_t> var
%type <quad_t> tag //a label
%type <qlist_t> tagGoto //a goto
%type <instruction_t> rel

%nonassoc NO_ELSE
%nonassoc YELSE


%left YOR
%left YAND
%left YNOT

%right YASSIGN
%left '+' '-'
%left '*' '/'
%left UMINUS

%%
program : 
		|fn_declaration ;

fn_declaration	: type YID '('params ')' stmt_list ;

type : YINT|YFLOAT;

params : 
		|param_list ;
		
param_list: param_list ',' param 
			| param 
			;
param: type YID ;

return_stmt : YRETURN';'
            | YRETURN expr';'
			;

stmt :
	  expression_stmt
	 | if_stmt
	 | while_stmt
	 | '{'stmt_list'}'
	 |return_stmt
	 ;
	 
stmt_list : stmt
	| stmt stmt_list
	;
	
/*il ya pb de typage avec le moins et la parenthèse
	%type <codegen> expr
	
	| '-' expr %prec UMINUS {}
	| '(' expr')' {$$ =  $2;} 
*/
expr :
	YID
	{
		$$=ST_lookup(g_st, $1);
	}
	| YNUM
	{
		SymbolInfo si=SI_genInt($1);
		$$=ST_addTmp(g_st, &si);
	}
    | expr '-' expr
    | expr '+' expr
    | expr '*' expr
    | expr '/' expr
;

expression_stmt :
	expression ';'
	| ';'
  ;

expression :
	var YASSIGN expr
	{
		Quad q=Q_gen(AFF_I, $3, NULL, $1);
		printf("Added quad :\n\t");
		Q_print(QT_add(g_qt, &q));
	}
	| expr
  ;

var :
	YID
	{
		$$=ST_lookup(g_st, $1);
	}

while_stmt :
	YWHILE tag '(' condition ')' tag stmt
	{
		Quad qWhile	=Q_gen(GOTO_I, NULL, NULL, $2->res);
		Quad qDone 	=Q_genLabel(); //tag invisible

		QT_add(g_qt, &qWhile);

		QL_backpatch($4.trueList, $6);
		QL_backpatch($4.falseList, QT_add(g_qt, &qDone));
	}

if_stmt :
	YIF '(' condition ')' tag stmt %prec NO_ELSE
	{	// NO_ELSE est un token virtuel pour eviter les conflits
			Quad qDone 	=Q_genLabel(); //tag invisible
			QL_backpatch($3.trueList, $5);
			QL_backpatch($3.falseList, QT_add(g_qt, &qDone));
	}
  | YIF '(' condition ')' tag stmt YELSE tagGoto stmt
	{	// tu t'es trompé dans le placement de ton token.
		Quad qDone 	=Q_genLabel(); //tag invisible
		QL_backpatch($3.trueList, $5);
		QL_backpatch($3.falseList, $8);
		//QL_backpatch($9, QT_add(g_qt, &qDone));
	}
  ;

/*
  | YID  '['expr']'
	{
		$$=ST_lookup()
	}
*/
    ;

condition :
	YID rel YNUM
	{
		$$.trueList		= QL_gen();
		$$.falseList	= QL_gen();

		SymbolInfo si	= SI_genInt($3);

		Quad qTrue		= Q_gen($2, ST_lookup(g_st, $1), ST_addTmp(g_st, &si), GOTO_EMPTY);
		Quad qFalse		= Q_genGoto();

		QL_add($$.trueList, QT_add(g_qt, &qTrue));
		QL_add($$.falseList, QT_add(g_qt, &qFalse));
	}
   | YID rel YID
	{
		$$.trueList		= QL_gen();
		$$.falseList	= QL_gen();

		Quad qTrue		= Q_gen($2, ST_lookup(g_st, $1), ST_lookup(g_st, $3), GOTO_EMPTY);
		Quad qFalse		= Q_genGoto();

		QL_add($$.trueList, QT_add(g_qt, &qTrue));
		QL_add($$.falseList, QT_add(g_qt, &qFalse));
	}
	| YID
	{
		$$.trueList		= QL_gen();
		$$.falseList	= QL_gen();

		Quad qTrue		= Q_gen(B_NEQ_I, ST_lookup(g_st, $1), ST_lookup(g_st, "__zero"), GOTO_EMPTY);
		Quad qFalse		= Q_genGoto();

		QL_add($$.trueList, QT_add(g_qt, &qTrue));
		QL_add($$.falseList, QT_add(g_qt, &qFalse));
	}
  | YTRUE //TODO, if(458) for example
	{
		$$.trueList		= QL_gen();
		$$.falseList	= NULL;

		Quad qGoto		= Q_genGoto();
		QL_add($$.trueList, QT_add(g_qt, &qGoto));
	}
  | YFALSE //TODO, if(0)
	{
		$$.falseList	= QL_gen();
		$$.trueList		= NULL;

		Quad qGoto		= Q_genGoto();
		QL_add($$.falseList, QT_add(g_qt, &qGoto));
	}
  | condition YOR tag condition
	{
		QL_backpatch($1.falseList, $3);
		$$.trueList		= QL_concat($1.trueList, $4.trueList);
		$$.falseList	= $4.falseList;
	}
  | condition YAND tag condition
	{
		QL_backpatch($1.trueList, $3);
		$$.falseList	= QL_concat($1.falseList, $4.falseList);
		$$.trueList		= $4.trueList;
	}
  | YNOT condition
	{
		$$.trueList		= $2.falseList;
		$$.falseList	= $2.trueList;
	}
  |'(' condition ')'
	{
		$$=$2;
	}
  ;

tag :
	{
		Quad q=Q_genLabel();
		$$=QT_add(g_qt, &q);
	}
	;
tagGoto :
	{
		Quad q=Q_genGoto();
		$$=QL_gen();
		QL_add($$, QT_add(g_qt, &q));
	}
	;
rel :
	YEQUAL
	{
		$$=B_EQ_I;
	}
	| YNEQ
	{
		$$=B_NEQ_I;
	}
	| '<'
	{
		$$=B_L_I;
	}
	| '>'
	{
		$$=B_G_I;
	}
	;

/************************************************************************************/

/* //autres version qui marchent aussi
stmt : ';'
	 | expr ';'  
	 | var '=' expr ';'
	 | if_stmt
	 | while_stmt
	 | '{' stmt_list '}'
	 | return_stmt
;
*/

/* il ne sert à rien maintenant les accolades sont géres dans stmt
then_stmt :
	'{' stmt_list '}'
	| stmt
	;
*/
/*
axiom:
	stmt_list '\n'
	 {
	   printf("Match !! \n");
		 printf("===SYMBOL TABLE===\n");
		 ST_print(g_st);
		 printf("===QUAD TABLE===\n");
		 QT_print(g_qt);
	 }
*/


%%
int main()
{
	size_t qtSize=1024;
	g_qt=QT_gen(qtSize);
	g_st=ST_gen(128);
	yyparse();
  lex_free();
	return 0;
}

void yyerror(char *message)
{
	fprintf(stderr,"%s\n", message);
}
