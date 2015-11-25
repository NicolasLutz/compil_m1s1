%{

#include "global.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "y.tab.h"
int lineno;

%}

blancs    [ \t]+
commentaire "/*"([^]|("*"+[^/*]))*"*"+"/"

chiffre   [0-9]
entier    {chiffre}+
exposant  [eE][+-]?{entier}
lettre [a-zA-Z]
identificateur {lettre}({lettre}|{chiffre})*

reel    {entier}("."{entier})?{exposant}?

affectation "="

%%

[+-/()\[\]{}^;~,] {return(yytext[0]);}
!=         {return(YNEQ);} 
"++"       {return(PLUSPLUS);} 
"--"       {return(MINUSMINUS);} 

"int"      return(YINT);
"float"    return(YFLOAT);
"matrix"   return(YMATRIX);
"main"     return(YMAIN);
"printf"   return(YPRINTF);
"printmat" return(YPRINTMAT);
"if"       return(YIF);
"else"     return(YELSE);
"while"    return(YWHILE);
"for"      return(YFOR);
"return"   return(YRETURN);

{identificateur} {yylval= *yytext;
		         return IDENTIFIER; }
{affectation}    return ASSIGN;
{separateur}     return SEPARATOR;

{reel}     { yylval=atof(yytext);
      	   return(YNUM);}

\n  lineno++; 

{blancs}   { /* On ignore */ }
{commentaire} ;

%%
yywrap(){return 1;}

