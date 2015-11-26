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
program : declaration_list ;
declaration_list : declaration_list declaration | declaration ;
declaration : var_declaration | fn_declaration;

var_declaration : type YIDENTIFIER ';'| type YIDENTIFIER '['YNUM']' ';'                ;
type : YINT|YFLOAT|YMATRIX;

fn_declaration	: type ID '('params ')' statement | ID '('params')' statement  ;
params : |param_list ;
param_list: param_list ',' param | param ;
param: type YIDENTIFIER ;

compound_stmt : | '{'local_declaration statement_list'}';
statement_list :  |statement_list statement ;

statement :   expression_stmt 
	      | compound_stmt
	      | selection_stmt
	      | iteration_stmt
	      | return_stmt
	      ;
expression_stmt : expression ';' | ';'
			
selection_stmt : YIF '('expression')' statement
               | YIF '('expression')' statement YELSE statement ;
iteration_stmt : YWHILE '(' expression ')' statement      
		| YFOR '(' expression';'expression';'expression ')'
                ;
return_stmt : YRETURN';'
            | YRETURN simple_expression';'
            ;
expression : var ASSIGN simple_expression | simple_expression ;
var : YIDENTIFIER | YIDENTIFIER'['expresion']';

simple_expression : YIDENTIFIER
     | YNUM
     | '(' simple_expression')'
     | 
     | '-' simple_expression %prec NEG
     | simple_expression '-' simple_expression
     | simple_expression '+' simple_expression
     | simple_expression '*' simple_expression
     | simple_expression YNEQ simple_expression
     | simple_expression '/' simple_expression
     | IDENTIFIER '(' expr_list ')'
     ;
 expr_list : 
           | simple_expression
           | simple_expression ',' expr_list
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
