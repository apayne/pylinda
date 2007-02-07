/*
*    Copyright 2006 Andrew Wilkinson <aw@cs.york.ac.uk>.
*
*    This file is part of Minimal (http://www-users.cs.york.ac.uk/~aw/pylinda)
*
*    Minimal is free software; you can redistribute it and/or modify
*    it under the terms of the GNU Lesser General Public License as published by
*    the Free Software Foundation; either version 2.1 of the License, or
*    (at your option) any later version.
*
*    Minimal is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with MiniMinimal_SyntaxElement_tmal; if not, write to the Free Software
*    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

%{
#include <math.h>

#include "minimal_internal.h"

#define YYSTYPE Minimal_SyntaxTree

#include "minimal_parser.h"
%}

CDEFINE    [#].*"\n"
DIGIT      [0-9]
ID         [a-zA-Z][a-zA-Z0-9]*
DELIM      [" "\n\t]
WHITESPACE {DELIM}+
ARROW      "->"
TYPESPEC   "::"
SEMICOLON  ";"
COMMA      ","
OPERATOR   [+-\/%\^*]
EQUAL      [=]
OPENB      [(]
CLOSEB     [)]

%%

{DIGIT}+ {
        yylval = Minimal_SyntaxTree_createInteger(atoi(yytext));
        return YY_INTEGER;
         }

{ID} {
        yylval = Minimal_SyntaxTree_createID(yytext);
        return YY_ID;
     }

{ARROW} { return YY_FUNCTION; }
{TYPESPEC} { return YY_TYPESPEC; }
{SEMICOLON} { return  YY_SEMICOLON; }
{COMMA} { return  YY_COMMA; }
{OPERATOR} {
        yylval = Minimal_SyntaxTree_createOperator(yytext);
        return YY_OPERATOR;
           }
{EQUAL} { return YY_EQ; }
{OPENB} { return YY_OPENB; }
{CLOSEB} { return YY_CLOSEB; }

{WHITESPACE} { }
{CDEFINE} { }

#ifndef WIN32
void Minimal_nevercall() {
    unput('a');
}
#endif
