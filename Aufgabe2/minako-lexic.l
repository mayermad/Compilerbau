%option noyywrap
%option yylineno

%{
#include <stdlib.h>
#include <string.h>
#include "minako.h"

char * strdup(const char *str1);
yystype yylval;


%}

%x COMMENT
WHITESPACE 	[ \t\n\r]
DIGIT		[0-9]
INTEGER 	{DIGIT}+
FLOAT		{INTEGER}"."{INTEGER}|"."{INTEGER}
LETTER		[a-zA-Z]

%%

"/*"				{BEGIN(COMMENT);}
<COMMENT>.|\n			{}
<COMMENT>"*/"			{BEGIN(INITIAL);}
"//".*				{}

"bool"				{return KW_BOOLEAN;}
"do"				{return KW_DO;}
"else"				{return KW_ELSE;}
"float"				{return KW_FLOAT;}
"for"				{return KW_FOR;}
"if"				{return KW_IF;}
"int"				{return KW_INT;}
"printf"			{return KW_PRINTF;}
"return"			{return KW_RETURN;}
"void"				{return KW_VOID;}
"while"				{return KW_WHILE;}



"+"				{return 43;}
"-"				{return 45;}
"*"				{return 42;}
"/"				{return 47;}
"="				{return 61;}
"=="				{return EQ;}
"!="				{return NEQ;}
"<"				{return LSS;}
">"				{return GRT;}
"<="				{return LEQ;}
">="				{return GEQ;}
"&&"				{return AND;}
"||"				{return OR;}



","				{return 44;}
";"				{return 59;}
"("				{return 40;}
")"				{return 41;}
"{"				{return 123;}
"}"				{return 125;}


{WHITESPACE}+ 			{}

{INTEGER}			{yylval.intValue = atoi(yytext); return CONST_INT;}
{FLOAT}([eE]([-+])?{INTEGER})?|{INTEGER}[eE]([-+])?{INTEGER}	{yylval.floatValue = atof(yytext); return CONST_FLOAT;}
"true"|"false"			{if(strcmp(strdup(yytext),"true")){yylval.intValue=0;}else{yylval.intValue=1;} return CONST_BOOLEAN;}

"\""[^\n\"]*"\""		{yylval.string = strdup(yytext); yylval.string = yylval.string+1; yylval.string[yyleng-2] = '\0'; return CONST_STRING;}
({LETTER})+({DIGIT}|{LETTER})*	{yylval.string = strdup(yytext); return ID;}


<<EOF>>				{return EOF;}
.				{printf("'%s' ist nicht Teil der Sprache\n",strdup(yytext)); exit(0);}

