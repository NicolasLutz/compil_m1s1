%{

//Ne pas editer ce fichier sur qt (problemes d'espace, reconnaissance de syntaxe, etc)
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "quad.h"
#include "y.tab.h"

int lineno=0; //<initialiser a 0

%}

%option nounput
%option noinput

blancs    [ \t\n]+
commentaire "/*"([^*]*[*][^*/])*[^*]*[*]+"/"

chiffre   [0-9]
entier    {chiffre}+
lettre 		[a-zA-Z_]
identificateur {lettre}({lettre}|{chiffre})*
string \"([^"]|\\\")*\"
exposant  [eE][+-]?{entier}
reel {entier}("."{entier})?{exposant}?

%%

[+-/()\[\]\{\}^~,;\>\<] {return(yytext[0]);}

"=="       return(YEQUAL);
"!="       return(YNEQ);
"<="			 return(YLEQ);
">="			 return(YGEQ);
"&&"	   	 return(YAND);
"||"	   	 return(YOR);
"!"	       return(YNOT);
"="        return(YASSIGN);
"while"    return(YWHILE);
"do"       return(YDO);
"if"       return(YIF);
"else"     return(YELSE);
"true"     return(YTRUE);
"false"    return(YFALSE);
"int"      return(YINT);
"float"    return(YFLOAT);
"print"		 return(YPRINT);
"printf"	 return(YPRINTF);
"printmat" return(YPRINTMAT);
"main"		 return(YMAIN);
"return"   return(YRETURN);

{identificateur} {yylval.stringVal=strdup(yytext);
		         return YID; }

{entier}     { yylval.intVal=atoi(yytext);
						  return(YNUM);}

{reel}     { yylval.floatVal=(float)atof(yytext);
						 return(YREEL);}

{string}	{yylval.stringVal=strdup(yytext);
					return YSTRING;}

{blancs}   { /* On ignore */ }
{commentaire} { /* On ignore */ }

.          {printf("ERROR LEX unknow character %s\n", yytext); return 0;}


%%
int yywrap(){return 1;}
//free memory allocated for lex
void lex_free(){
	yy_delete_buffer(YY_CURRENT_BUFFER);
	free(yy_buffer_stack);
}
