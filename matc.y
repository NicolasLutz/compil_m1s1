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
int yywarp();
int yydebug=1;
extern FILE *yyin;

QuadTab 			*g_qt=NULL;
SymbolTable 	*g_st=NULL;

int compiledCorrectly=1;
int next_quad =0;

char *outFile;

%}

%union{
	char 				*stringVal;
	int 				intVal;
	float 			floatVal;
	Symbol      *symbol_t;
	Quad 				*quad_t;
	QuadList    *qlist_t;
	Instruction instruction_t;
	Typename type_t;
	struct //%type <codeCondition> condition
	{
		QuadList *trueList;
		QuadList *falseList;
	} codeCondition_t;
}


%token YASSIGN YWHILE YFOR YDO YIF YELSE
%token YEQUAL YNEQ YTRUE YFALSE YOR
%token YINT YFLOAT YPRINT YPRINTF YPRINTMAT
%token YLEQ YGEQ YMAIN YRETURN YEOF YPP YMM
%token <intVal> YNUM
%token <floatVal> YREEL
%token <stringVal> YID
%token <stringVal> YSTRING

%type <codeCondition_t> condition
%type <symbol_t> expr
%type <symbol_t> const
%type <symbol_t> var
%type <quad_t> tag //a label
%type <qlist_t> tagGoto //a goto
%type <type_t> type
%type <instruction_t> rel
%type <instruction_t> incr

%nonassoc NO_ELSE
%nonassoc YELSE

%right YASSIGN
%left '<' '>' YNEQ YEQUAL YLEQ YGEQ
%left YOR
%left YAND
%left '+' '-'
%left '*' '/'
%left UMINUS
%left YPP YMM
%left YNOT

%%

S :
	program
	{
		printf("===SYMBOL TABLE===\n");
		ST_print(g_st);
		printf("===QUAD TABLE===\n");
		QT_print(g_qt);
		if(compiledCorrectly)
		 MATC_Compile(g_qt, g_st, outFile);
		else
		 fprintf(stderr, "No output produced: Errors have been detected.\n");
	}
	;

program:
	| fn_declaration
	;

fn_declaration:
	type YID '(' params ')' '{' stmt_list '}'
	| YINT YMAIN '(' ')' '{' stmt_list '}'
	;

type:
	YINT {$$=INT_T;}
	|YFLOAT {$$=FLOAT_T;}
	;

params :

	|param_list
	;

param_list:
	param_list ',' param
	| param
	;

param:
	type YID
	;

return_stmt :
	YRETURN
	| YRETURN expr
	;

stmt :
	';'
 	| expression ';'
 	| var '=' expr ';'
 	| if_stmt
 	| while_stmt
	| for_stmt
 	| '{' stmt_list '}'
 	| return_stmt ';'

stmt_list :
	stmt
	| stmt stmt_list
	;

expression :
	//TODO : paranthesis
	aff_expr
	| YPRINT '(' expr ')'
	{
		Quad q=Q_gen(PRINT_I, $3, NULL, NULL);
		QT_add(g_qt, &q);
	}
	| YPRINTF '(' YSTRING ')'
	{
		SymbolInfo si=SI_genString($3);
		Quad q=Q_gen(PRINTF_I, ST_addTmp(g_st, &si), NULL, NULL);
		QT_add(g_qt, &q);
	}
	| expr
	;

expr :
	YID
	{
		$$=ST_lookup(g_st, $1);
		if($$==NULL)
		{
			MATC_error_undeclared($1);
		}
		free($1);
	}
	| const
	{
		$$=$1;
	}
	| expr '-' expr
	{
		SymbolInfo *p_si = $1!=NULL ? &$1->info : NULL;
		if(!S_castFloat($1, $3))
		{
			MATC_warning_unsafeOperation($1, $3);
		}
		$$=ST_addTmp(g_st, p_si);
		Quad q=Q_gen(SUB_I, $1, $3, $$);
		QT_add(g_qt, &q);
	}
	| expr '+' expr
	{
		SymbolInfo *p_si = $1!=NULL ? &$1->info : NULL;
		if(!S_castFloat($1, $3))
		{
			MATC_warning_unsafeOperation($1, $3);
		}
		$$=ST_addTmp(g_st, p_si);
		Quad q=Q_gen(ADD_I, $1, $3, $$);
		QT_add(g_qt, &q);
	}
	| expr '*' expr
	{
		SymbolInfo *p_si = $1!=NULL ? &$1->info : NULL;
		if(!S_castFloat($1, $3))
		{
			MATC_warning_unsafeOperation($1, $3);
		}
		$$=ST_addTmp(g_st, p_si);
		Quad q=Q_gen(MULT_I, $1, $3, $$);
		QT_add(g_qt, &q);
	}
	| expr '/' expr
	{
		SymbolInfo *p_si = $1!=NULL ? &$1->info : NULL;
		if(!S_castFloat($1, $3))
		{
			MATC_warning_unsafeOperation($1, $3);
		}
		$$=ST_addTmp(g_st, p_si);
		Quad q=Q_gen(DIV_I, $1, $3, $$);
		QT_add(g_qt, &q);
	}
	| expr incr
	{
		SymbolInfo *p_si = $1!=NULL ? &$1->info : NULL;
		$$=$1;
		if(p_si!=NULL && p_si->type!=INT_T)
		{
			MATC_error_unavailableOperator($$);
		}
		Quad q=Q_gen($2, $1, ST_lookup(g_st, "__one"), $$);
		QT_add(g_qt, &q);
	}
	;

incr:
	YPP 		{$$=ADD_I;}
	| YMM		{$$=SUB_I;}

const :
	YNUM
	{
		SymbolInfo si=SI_genInt($1);
		$$=ST_addTmp(g_st, &si);
	}
	| YREEL
	{
		SymbolInfo si=SI_genFloat($1);
		$$=ST_addTmp(g_st, &si);
	}
	;


var:
	YID
	{
		$$=ST_lookup(g_st, $1);
		if($$==NULL)
		{
			MATC_error_undeclared($1);
		}
		free($1);
	}
	//also YID YINDEX
	;

while_stmt :
	YWHILE tag '(' condition ')' tag stmt
	{
		Quad qWhile	=Q_gen(GOTO_I, NULL, NULL, $2->res);
		Quad qDone 	=Q_genLabel(); //tag invisible

		QT_add(g_qt, &qWhile);

		QL_backpatch($4.trueList, $6);
		QL_backpatch($4.falseList, QT_add(g_qt, &qDone));
	}

for_stmt :
	YFOR '(' aff_expr ';' tag condition ';' tag expr tagGoto ')' tag stmt
	{
		Quad qFor	=Q_gen(GOTO_I, NULL, NULL, $8->res);
		Quad qDone 	=Q_genLabel(); //tag invisible

		QT_add(g_qt, &qFor);

		QL_backpatch($10, $5); //apres expr -> tag de condition

		QL_backpatch($6.trueList, $12); //condition true -> avant-dernier tag
		QL_backpatch($6.falseList, QT_add(g_qt, &qDone));
	}

if_stmt :
	YIF '(' condition ')' tag stmt %prec NO_ELSE
	{
			Quad qDone 	= Q_genLabel(); //tag invisible
			QL_backpatch($3.trueList, $5);
			QL_backpatch($3.falseList, QT_add(g_qt, &qDone));
	}
  | YIF '(' condition ')' tag stmt YELSE tagGoto tag stmt
	{
		Quad qDone 	= Q_genLabel(); //tag invisible

		QL_backpatch($3.trueList, $5);
		QL_backpatch($3.falseList, $9);
		QL_backpatch($8, QT_add(g_qt, &qDone));
	}
  ;

aff_expr:
	type YID YASSIGN expr
	{
		SymbolInfo *p_si=($4!=NULL ? &$4->info : NULL);
		Symbol *s2=ST_add(g_st, $2, p_si);

		if(!S_castFloat(s2, $4))//checks if float casts have to be made and tries to do them
		{
			MATC_warning_unsafeAffectation(s2,$4);
		}
		Quad q=Q_gen(AFF_I, $4, NULL, s2);
		QT_add(g_qt, &q);
	}
	| var YASSIGN expr
	{
		if(!S_castFloat($1, $3))
		{
				MATC_warning_unsafeAffectation($1,$3);
		}
		Quad q=Q_gen(AFF_I, $3, NULL, $1);
		QT_add(g_qt, &q);
	}

condition :
	expr rel expr
	{
		$$.trueList		= QL_gen();
		$$.falseList	= QL_gen();

		if(!S_castFloat($1, $3))
		{
			MATC_warning_unsafeComparasion($1, $3);
		}

		Quad qTrue		= Q_gen($2, $1, $3, GOTO_EMPTY);
		Quad qFalse		= Q_genGoto();

		QL_add($$.trueList, QT_add(g_qt, &qTrue));
		QL_add($$.falseList, QT_add(g_qt, &qFalse));
	}
	| expr
	{
		$$.trueList		= QL_gen();
		$$.falseList	= QL_gen();

		Quad qTrue		= Q_gen(B_NEQ_I, $1, ST_lookup(g_st, "__zero"), GOTO_EMPTY);
		/*
			Note : 0 is 0 in float, so we can safely pretend we're reading an int.
			Same goes for a null adress.
		*/
		Quad qFalse		= Q_genGoto();

		QL_add($$.trueList, QT_add(g_qt, &qTrue));
		QL_add($$.falseList, QT_add(g_qt, &qFalse));
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

tagGoto :
	{
		Quad q=Q_genGoto();
		$$=QL_gen();
		QL_add($$, QT_add(g_qt, &q));
	}

rel :
	YEQUAL 	{$$=B_EQ_I;}
	| YNEQ 	{$$=B_NEQ_I;}
	| YLEQ 	{$$=B_LEQ_I;}
	| YGEQ 	{$$=B_GEQ_I;}
	| '<'		{$$=B_L_I;}
	| '>'		{$$=B_G_I;}
	;

%%
int main(int argc, char **argv)
{
	if(argc==1)
		outFile="out.s";
	else
		outFile=argv[1];
	if(argc>2 && !(yyin=fopen(argv[2],"r")))
	{
		fprintf(stderr,"failed to open %s\n", argv[2]);
		exit(EXIT_FAILURE);
	}
	g_qt=QT_gen(128);
	g_st=ST_gen(128);
	yyparse();
	ST_destroy(g_st);
	QT_destroy(g_qt);
	fclose(yyin);
  lex_free();
	return 0;
}

void yyerror(char *message)
{
	fprintf(stderr,"%s\n", message);
}

int yywarp()
{
	return 1;
}
