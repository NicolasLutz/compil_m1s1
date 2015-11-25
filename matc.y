%{

#include "global.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

%}


%token YIDENTIFIER
%token YMAIN
%token YNUM
%token STRING
%token YINT YFLOAT YMATRIX YMAIN YPRINTF YPRINTMAT YIF YELSE YWHILE YFOR YRETURN
%token ASSIGN

%nonassoc 	IF
%nonassoc	ELSE


%right ASSIGN 
%nonassoc YNEQ 
%left  '+' '-'
%left  '* '/'
%left  PLUSPLUS MINUSMINUS
%left NEG

%start program

%%
program : /*empty*/
	program declaration_list
	;
declaration_list : declaration_list 
                 | declaration
                 ;
declaration : var_declaration | fn_declaration
			;

var_declaration : TYPE var_decl_list ';'
                ;
scoped_var_declaration : TYPE var_decl_list';

var_decl_list : var_decl_list ',' var_decl_id | var_decl_id
			  ;
var_decl_id : IDENTIFIER | IDENTIFIER '['YNUM']'
            ;

TYPE : INT|FLOAT
	;

fn_declaration	: TYPE ID '('
	         params ')' statement
        |ID (params) statement
	    ;
params : 
	|param_list
	;
param_list: param_list ';' param_type_list | param_type_list
          ;
param_type_list: TYPE param_id_list
				;
param_id_list : param_id_list ',' param_id | param_id
			 ;
param_id : IDENTIFIER | IDENTIFIER '[' ']'
         ;

statement :	expression_stmt 
	      | compound_stmt
	      | selection_stmt
	      | iteration_stmt
	      | return_stmt
	      ;
compound_stmt : '{'statement_list'}'
		      ;
statement_list : 
               |statement_list statement
               ;
expression_stmt : expression ';' | ';'
				;
selection_stmt : YIF '('simple_expr')' statement
			   | YIF '('simple_expr')' statement YELSE statement
			   ;
iteration_stmt : YWHILE '(' simple_expr ')' statement      
				| YFOR '(' simple_expr';'simple_expr';'simple_expr ')'
                ;
return_stmt : YRETURN';'
            | YRETURN expr';'
            ;
expr : IDENTIFIER
     | YNUM
     | '(' expr')'
     | IDENTIFIER ASSIGN expr
     | '-' expr %prec NEG
     | expr '-' expr
     | expr '+' expr
     | expr '*' expr
     | expr YNEQ expr
     | expr '/' expr
     | IDENTIFIER '(' expr_list ')'
     ;
 expr_list : 
           | expr
           | expr ',' expr_list
           ;
%%
int main(){
	yyparse();
	return 0;
}

#include "lex.yy.c"

yyerror(char *message){
	extern int lineno;
	extern char* yytext;
	fprintf(stder, "%d" ; %s at %s\n", lineno, message, yytext);
}

