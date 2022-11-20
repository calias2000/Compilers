%option c++ prefix="fir_scanner_" outfile="fir_scanner.cpp"
%option stack noyywrap yylineno 8bit
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <sstream>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "fir_parser.tab.h"

static std::ostringstream strlit;
// don't change this
#define yyerror LexerError
%}

SPACE     ([ \t\r]|\n)

%x X_COMMENT
%x X_STRING  X_BACKSLASH
%%

 /* Comments */

"!!".*                        ;

"(*"                          yy_push_state(X_COMMENT);
<X_COMMENT>"*)"               yy_pop_state();
<X_COMMENT>.|\n               ;


 /* KeyWords */

"int"                         return tTYPE_INT;
"float"				          return tTYPE_FLOAT;
"string"			          return tTYPE_STRING;
"null"			              return tNULL;
"void"			              return tVOID;
"sizeof"                      return tSIZEOF;
[<>]                          return *yytext;


 /* Conditions */

"if"                          return tIF; 
"then"                        return tTHEN;
"else"                        return tELSE; 


 /* Cycle / termination */

"while"                       return tWHILE;
"for"                         return tFOR;
"do"                          return tDO;
"leave"                       return tLEAVE;
"restart"                     return tRESTART;
"finally"                     return tFINALLY;


 /* Operators */

[\[\](),?]                    return *yytext;
[-+*/%~<>@?=]                 return *yytext;

">="                          return tGE; 
"<="                          return tLE;

"=="                          return tEQ; 
"!="                          return tNE;

"&&"                          return tAND;
"||"                          return tOR;


 /* Read and Write */

[#\[\](){};:!,]               return *yytext;
"return"                      return tRETURN;
"write"                       return tWRITE;
"writeln"                     return tWRITELN;


 /* Integer and Octal */

0[1-7]+               { try { yylval.i = std::stoi(yytext, nullptr, 8); return tINTEGER;
                            } catch (std::out_of_range& e) { yyerror("Integer out of range.");} }

0[0-9]+               yyerror("invalid octal number.");

0|[1-9][0-9]*         { try { yylval.i = std::stoi(yytext, nullptr, 10); return tINTEGER;
                            } catch (std::out_of_range& e) { yyerror("Integer out of range.");} }


 /* Float */

([0-9]*\.[0-9]+|[0-9]+\.[0-9]*)([eE][-+]?[0-9]+)? { yylval.d = strtod(yytext, NULL); return tFLOAT; }


 /* ids */

[A-Za-z][A-Za-z0-9_]*          yylval.s = new std::string(yytext); return tID;


 /* strings */

"\'"                           yy_push_state(X_STRING);
<X_STRING>~                    yy_push_state(X_BACKSLASH);
<X_STRING>"\'"                 {
                                 yylval.s = new std::string(strlit.str());
                                 strlit.str("");
                                 yy_pop_state();
                                 return tSTRING;
                               }

<X_STRING>\n                   yyerror("Error in string");
<X_STRING>.                    strlit << *yytext;

<X_BACKSLASH>n                 strlit << '\n'; yy_pop_state();
<X_BACKSLASH>r                 strlit << '\r'; yy_pop_state();
<X_BACKSLASH>t                 strlit << '\t'; yy_pop_state();
<X_BACKSLASH>~                 strlit << '~';  yy_pop_state();

<X_BACKSLASH>[[:xdigit:]]{1,2} { strlit << (char)(unsigned char)strtoul(yytext, NULL, 16); yy_pop_state(); }
<X_BACKSLASH>.                 strlit << *yytext; yy_pop_state();


 /* other characters */

[[:space:]] 
.                              yyerror(yytext);

%%