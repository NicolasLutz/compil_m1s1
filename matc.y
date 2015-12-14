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
int yydebug=1;

QuadTab 			*g_qt=NULL;
SymbolTable 	*g_st=NULL;

int compiledCorrectly=1;
int next_quad =0;

%}

%union{
	char 				*stringVal;
	int 				intVal;
	float 			floatVal;
	Symbol      *symbol_t;
	QuadList    *qlist_t;
	Quad        *quad_t;
	Instruction instruction_t;
	struct //%type <codeCondition> condition
	{
		QuadList *trueList;
		QuadList *falseList;
	} codeCondition_t;
}


%token YASSIGN YWHILE YDO YIF YELSE
%token YEQUAL YNEQ YTRUE YFALSE YOR
%token YINT YFLOAT YPRINT YPRINTF YPRINTMAT
%token YLEQ YGEQ
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
%type <instruction_t> rel


%right YASSIGN

%left '<' '>' YNEQ YEQUAL YLEQ YGEQ
%left YOR
%left YAND
%left YNOT

%right YTHEN YELSE

%%

axiom:
	stmt_list '\n'
	 {
	   	printf("Match !! \n");
		 	printf("===SYMBOL TABLE===\n");
		 	ST_print(g_st);
		 	printf("===QUAD TABLE===\n");
		 	QT_print(g_qt);
		 	if(compiledCorrectly)
		 		MATC_Compile(g_qt, g_st, "out.s");
		 	else
		 		fprintf(stderr, "No output produced: Errors have been detected.\n");
			ST_destroy(g_st);
		  QT_destroy(g_qt);
	 }

stmt_list :
	| stmt stmt_list
	;

stmt :
	expression_stmt
	 | if_stmt
	 | while_stmt
	 ;

expression_stmt :
	expression ';'
	| ';'
  ;

while_stmt :
	YWHILE tag '(' condition ')' tag then_stmt
	{
		Quad qWhile	=Q_gen(GOTO_I, NULL, NULL, $2->res);
		Quad qDone 	=Q_genLabel(); //tag invisible

		QT_add(g_qt, &qWhile);

		QL_backpatch($4.trueList, $6);
		QL_backpatch($4.falseList, QT_add(g_qt, &qDone));
	}

if_stmt :
	YIF '(' condition ')' tag then_stmt %prec YTHEN
	{
			Quad qDone 	= Q_genLabel(); //tag invisible
			QL_backpatch($3.trueList, $5);
			QL_backpatch($3.falseList, QT_add(g_qt, &qDone));
	}
  | YIF '(' condition ')' tag then_stmt YELSE tagGoto tag then_stmt
	{
		Quad qDone 	= Q_genLabel(); //tag invisible

		QL_backpatch($3.trueList, $5);
		QL_backpatch($3.falseList, $9);
		QL_backpatch($8, QT_add(g_qt, &qDone));
	}
  ;

then_stmt :
	'{' stmt_list '}'
	| stmt
	;

expression :
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

aff_expr:
	YINT YID YASSIGN expr
	{
		SymbolInfo si=SI_genInt(0);
		Symbol *s2=ST_add(g_st, $2, &si);

		if($4!=NULL && $4->info.type!=INT_T)
		{
			MATC_warning();
			fprintf(stderr, "incompatible affectation from %s of type %s to %s of type int\n",
				$4->name, SI_typeToString($4->info.type), $2);
		}

		Quad q=Q_gen(AFF_I, $4, NULL, s2);
		QT_add(g_qt, &q);
	}
	| YFLOAT YID YASSIGN expr
	{
		SymbolInfo si=SI_genFloat(0);
		Symbol *s2=ST_add(g_st, $2, &si);

		if($4!=NULL && $4->info.type!=FLOAT_T)
		{
			if(!SI_castFloat(&$4->info))
			{//clearly not something you want to dry-cast
				MATC_warning();
				fprintf(stderr, "incompatible affectation from %s of type %s to %s of type float\n",
					$4->name, SI_typeToString($4->info.type), $2);
			}
		}
		Quad q=Q_gen(AFF_I, $4, NULL, s2);
		QT_add(g_qt, &q);
	}
	| var YASSIGN expr
	{
		if($1!=NULL && $3!=NULL && $1->info.type!=$3->info.type)
		{
			if($1->info.type==FLOAT_T && !SI_castFloat(&$3->info))
			{
				MATC_warning();
				fprintf(stderr, "incompatible affectation from %s of type %s to %s of type %s\n",
					$3->name, SI_typeToString($3->info.type), $1->name, SI_typeToString($1->info.type));
			}
		}
		Quad q=Q_gen(AFF_I, $3, NULL, $1);
		QT_add(g_qt, &q);
	}

var:
	YID
	{
		$$=ST_lookup(g_st, $1);
		if($$==NULL)
		{
			MATC_error();
			fprintf(stderr, "%s undeclared (first use in function)\n",
				$1);
			free($1);
		}
	}
	//also YID YINDEX
  ;
expr :
	YID
	{
		$$=ST_lookup(g_st, $1);
		if($$==NULL)
		{
			MATC_error();
			fprintf(stderr, "%s undeclared (first use in function)\n",
				$1);
			free($1);
		}
	}
	| const
	{
		$$=$1;
	}
  ;

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

condition :
	expr rel expr
	{
		$$.trueList		= QL_gen();
		$$.falseList	= QL_gen();

		bool warning=false;

		if($1!=NULL && $3!=NULL && $1->info.type!=$3->info.type)
		{
			if($1->info.type==FLOAT_T)
			{
				if(!SI_castFloat(&$3->info))
					warning=true;
			}
			else if($3->info.type==FLOAT_T)
			{
				if(!SI_castFloat(&$1->info))
					warning=true;
			}
			if(warning)
			{
				MATC_warning();
				fprintf(stderr, "incompatible comparasion between %s of type %s and %s of type %s\n",
					$1->name, SI_typeToString($1->info.type), $3->name, SI_typeToString($3->info.type));
			}
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
  |'(' condition ')'	{$$=$2;}
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
	char *outFile;
	if(argc==1)
	{
		outFile="out.s";
	}
	else
	{
		outFile=argv[1];
	}
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
