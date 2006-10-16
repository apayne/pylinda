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
*    along with Minimal; if not, write to the Free Software
*    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

%{
#include "minimal_internal.h"

#define YYSTYPE Minimal_SyntaxTree

#include "yy.lex.h"

Minimal_SyntaxTree yy_result;

void yyerror(char* s) {
    fprintf(stderr, "%s\n", s);
}
%}

%token YY_ID YY_INTEGER YY_TYPESPEC YY_FUNCTION YY_OPERATOR YY_SEMICOLON

%% /* Grammar rules and actions follow */

input:  { $$.type = BLANK;
          yy_result = $$; }
     | input typespec_def YY_SEMICOLON {
                        $$.branch1 = Minimal_SyntaxTree_copy(&$1); Minimal_SyntaxTree_clear(&$1);
                        $$.branch2 = Minimal_SyntaxTree_copy(&$2); Minimal_SyntaxTree_clear(&$2);
                        $$.type = SEQENTIAL_DEFS;
                        yy_result = $$;
                        }
     | input definition YY_SEMICOLON {
                        $$.branch1 = Minimal_SyntaxTree_copy(&$1); Minimal_SyntaxTree_clear(&$1);
                        $$.branch2 = Minimal_SyntaxTree_copy(&$2); Minimal_SyntaxTree_clear(&$2);
                        $$.type = SEQENTIAL_DEFS;
                        yy_result = $$;
                        }
;

typespec_def: YY_ID YY_TYPESPEC typespec {
    $$.type_name = (char*)malloc(strlen($1.string)+1); strcpy($$.type_name, $1.string); Minimal_SyntaxTree_clear(&$1);
    $$.type_def  = Minimal_SyntaxTree_copy(&$3); Minimal_SyntaxTree_clear(&$3);
    $$.type = TYPE_SPEC;
    }
;

typespec: YY_ID { $$ = $1 }
        | typespec YY_FUNCTION typespec {
                        $$.branch1 = Minimal_SyntaxTree_copy(&$1); Minimal_SyntaxTree_clear(&$1);
                        $$.branch2 = Minimal_SyntaxTree_copy(&$3); Minimal_SyntaxTree_clear(&$3);
                        $$.type = TYPE_FUNCTION;
                        }
;

definition: expr { $$ = $1 }
;

value: YY_ID { $$ = $1; }
     | YY_INTEGER { $$ = $1; }
;

expr: value { $$ = $1; }
    | expr YY_OPERATOR expr { $$ = $1; }
;

%%
