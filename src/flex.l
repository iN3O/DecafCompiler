%{
#include <iostream>
#include <list>
using namespace std;
#include "AstStructure.hpp"

#include "bison.tab.hpp"


%}

%option noyywrap
PLUS_EQUAL      ("+=")
MINUS_EQUAL     ("-=")
EQUAL		("=")
EXCLAMATORY	("!")
PERCENT		("%")
DIVISION        ("/")
MULTIPLICATION  ("*")
ADDITION        ("+")
SUBSTRACTION    ("-")
ID              [a-zA-Z][a-zA-Z0-9]*
DIGIT    	[0-9]*
HEX_DIGIT	({DIGIT}|[a-fA-F])+   
HEX_LITERAL     (0x{HEX_DIGIT}{HEX_DIGIT}*)+
DECIMAL         ({DIGIT}+{DIGIT}*)
INT_LIT         ({DECIMAL})
SEMICOLON	(";")
COMMA		(",")
OPENING_CURLY	("{")
ENDING_CURLY	("}")
OPENING_ROUN	("(")
ENDING_ROUN	(")")
OPENING_SQUAR	("[")
ENDING_SQUAR	("]")
SINGLE_QUOTE	("'")
DOUBLE_QUOTE	("\"")


%%
class			return CLASS;
Program			return PROGRAM;
void			{yylval.string_val = strdup(yytext); return VOID;}
int			{yylval.string_val = strdup(yytext); return INT;}
boolean			{yylval.string_val = strdup(yytext); return BOOLEAN;}
if			return IF;
else			return ELSE;
for			return FOR;
return			return RETURN;
break			return BREAK;
continue		return CONTINUE;
callout			return CALLOUT;
{PLUS_EQUAL}		return PLUS_EQ;
{MINUS_EQUAL}		return MINUS_EQ;
{EXCLAMATORY}		return EXCLAMATORY;
{PERCENT}		return PERCENTAGE;
{DIVISION}		return DIVIDE;
{MULTIPLICATION}	return MULTIPLY;
{ADDITION}		return ADD;
{SUBSTRACTION}		return SUBSTRACT;
{EQUAL}			return EQUAL;
==			return EQUALEQUAL;
!=			return NOTEQUAL;
true			return TRUE_TOKEN;
false			return FALSE_TOKEN;
{INT_LIT}               {yylval.int_val = atoi(yytext); return INT_LITERAL;}
{HEX_LITERAL}		{yylval.string_val = strdup(yytext); return HEX_LITERAL;}
{ID}			{ yylval.string_val = strdup(yytext); return ID;}
{SEMICOLON}		return SEMICOLON;
{COMMA}			return COMMA;
&&			return AND;
\|\|			return OR;
\<=			return LESS_EQUAL;
\<			return LESS;
\>=			return GREATER_EQUAL;
\>			return GREATER;
{OPENING_CURLY}		return OPENING_CURLY;
{ENDING_CURLY}		return ENDING_CURLY;
{OPENING_ROUN}		return OPENING_ROUND;
{ENDING_ROUN}		return ENDING_ROUND;
{OPENING_SQUAR}		return OPENING_SQUARE;
{ENDING_SQUAR}		return ENDING_SQUARE;		
\n                      /* ignore end of line */;
[ \n\t]+                { }/* ignore whitespace */;      
<<EOF>>  		return 0;
\/\/.*$     		{ }/* eat up one-line comments */
\"(\\.|[^\\"])*\"       { yylval.string_val = strdup(yytext); return STRING_LITERAL;}
\'(\\.|[^\\'])*\'       { yylval.char_val = strdup(yytext); return CHAR_LITERAL;}
.        		{ cerr << "Unrecognized token!" << endl; exit(1); }	
	
%%

