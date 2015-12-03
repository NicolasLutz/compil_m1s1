%{

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void yyerror(char * s);
int yylex();
void lex_free();

//struct symbol* symbol_table =NULL;
//struct quad* quad_list =NULL;
int next_quad =0;
%}

%union{
		char * string;
    	int value;	
   	  }

%token YASSIGN YWHILE YDO YIF  YELSE 
%token YEQUAL YNEQ YTRUE YFALSE YOR
%token YINT YFLOAT
%token <value> YNUM
%token <string> YID

%right YASSIGN
%left YOR
%left YAND
%left YNOT

%% 

axiom:
	stmt_list '\n'
	 {
	   printf("Match !! \n");
	   return 0;
	 }

stmt_list :
	       | stmt_list stmt 
	       ;

stmt : expression_stmt 
	 | selection_stmt
	 | iteration_stmt
	 ;

expression_stmt : expression ';' | ';'
                 ;
      
iteration_stmt : YWHILE condition  stmt      
	
selection_stmt : YIF condition stmt 
               | YIF condition '{' stmt '}' YELSE stmt  
               ;	

expression : var YASSIGN expr 
           | expr
                ;
var : YID 
    | YID  '['expr']'
    ;
expr : YID
     | YNUM
     ;

condition : YID rel YNUM
          | YID rel YID
		  | YID 
          | YTRUE
          | YFALSE
          | condition YOR condition 
          | condition YAND condition 
          | YNOT condition 
          |'(' condition ')' 
          ;
rel : YEQUAL | YNEQ | '<' | '>';

%%
int main(){
	yyparse();
  lex_free();
	return 0;
}

void yyerror(char *message){
	fprintf(stderr,"%s\n", message);
}



