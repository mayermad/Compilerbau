%option noyywrap
%option yylineno

%{
#include "urlscanner.h"
#include <stdio.h>
#include <string.h>
extern char * strdup(const char *str1);
extern int fileno(FILE *stream);
%}

%x READURL
%x READLINKTEXT
%x CLOSETAG
WHITESPACE 	[ \t\n\r]

%%

"\<a"				{BEGIN(READURL);}
<READURL>"href=\""[^\"]*\"	{BEGIN(READLINKTEXT); yylval = strdup(yytext); yylval = yylval+6; yylval[yyleng-7] = '\0'; return TOKEN_URL;}	//zum Beispiel: href="abc"
<READURL>.|\n			{}

<READLINKTEXT>\>[^\<]*"\<\/a"	{BEGIN(CLOSETAG); yylval = strdup(yytext); yylval = yylval+1; yylval[yyleng-4] = '\0'; return TOKEN_TEXT;}	//zum Beispiel: >soso, ein spektakel</a
<READLINKTEXT>.|\n		{}

<CLOSETAG>\>			{BEGIN(INITIAL);}
<CLOSETAG>{WHITESPACE}*		{}

<<EOF>>				{return MYEOF;}
.|\n				{}


