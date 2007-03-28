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

#define YYSTYPE MinimalValue

#include "yy.lex.h"

MinimalValue yy_result;

void yyerror(char* s) {
}
%}

%token YY_ID YY_INTEGER YY_TYPESPEC YY_FUNCTION YY_OPERATOR YY_SEMICOLON YY_COMMA YY_EQ YY_OPENB YY_CLOSEB YY_IF YY_THEN YY_ELSE YY_ENDIF YY_TESTOP

%% /* Grammar rules and actions follow */

input: { $$ = Minimal_SyntaxTree_createBlank(); yy_result = $$; }
     | input typespec_def YY_SEMICOLON {
                        if($1->syntax_tree->type == ST_BLANK) {
                            $$ = $2;
                            Minimal_delReference($1);
                        } else if($2->syntax_tree->type == ST_BLANK) {
                            $$ = $1;
                            Minimal_delReference($2);
                        } else {
                            if($1->syntax_tree->type != ST_SEQENTIAL_DEFS) {
                                $$ = Minimal_SyntaxTree_createSequence2($1, $2);
                            } else {
                                $$ = Minimal_SyntaxTree_addToSequence($1, $2);
                            }
                        }
                        yy_result = $$;
                        }
     | input definition YY_SEMICOLON {
                        if($1->syntax_tree->type == ST_BLANK) {
                            Minimal_delReference($1);
                            $$ = $2;
                        } else if($2->syntax_tree->type == ST_BLANK) {
                            Minimal_delReference($2);
                            $$ = $1;
                        } else {
                            if($1->syntax_tree->type != ST_SEQENTIAL_DEFS) {
                                $$ = Minimal_SyntaxTree_createSequence2($1, $2);
                            } else {
                                $$ = Minimal_SyntaxTree_addToSequence($1, $2);
                            }
                        }
                        yy_result = $$;
                        }
     | input expr YY_SEMICOLON {
                        if($1->syntax_tree->type == ST_BLANK) {
                            Minimal_delReference($1);
                            $$ = $2;
                        } else if($2->syntax_tree->type == ST_BLANK) {
                            Minimal_delReference($2);
                            $$ = $1;
                        } else {
                            if($1->syntax_tree->type != ST_SEQENTIAL_DEFS) {
                                $$ = Minimal_SyntaxTree_createSequence2($1, $2);
                            } else {
                                $$ = Minimal_SyntaxTree_addToSequence($1, $2);
                            }
                        }
                        yy_result = $$;
                        }
;

typespec_def: YY_ID YY_TYPESPEC typespec {
    $$ = Minimal_SyntaxTree_createTypeSpec($1, $3);
    }
;

typespec: YY_ID { if(strcmp($1->syntax_tree->string, "Nil") == 0) {
                    Minimal_delReference($1);
                    $$ = Minimal_SyntaxTree_createNil();
                  } else {
                    $$ = $1;
                  }
                }
        | typespec YY_FUNCTION typespec {
                        $$ = Minimal_SyntaxTree_createFunction($1, $3);
                        }
        | typespec YY_OPERATOR typespec {
                        if(strcmp($2->syntax_tree->string, "*") == 0) {
                            if($1->syntax_tree->type == ST_PRODUCT_TYPE) {
                                Minimal_SyntaxTree_addToProductType($1, $3);
                                $$ = $1;
                            } else if($2->syntax_tree->type == ST_PRODUCT_TYPE) {
                                Minimal_SyntaxTree_prependToProductType($1, $3);
                                $$ = $1;
                            } else {
                                $$ = Minimal_SyntaxTree_createProductType();
                                Minimal_SyntaxTree_addToProductType($$, $1);
                                Minimal_SyntaxTree_addToProductType($$, $3);
                            }
                        } else if(strcmp($2->syntax_tree->string, "+") == 0) {
                            if($1->syntax_tree->type == ST_SUM_TYPE) {
                                Minimal_SyntaxTree_addToSumType($1, $3);
                                $$ = $1;
                            } else if($2->syntax_tree->type == ST_SUM_TYPE) {
                                Minimal_SyntaxTree_prependToSumType($1, $3);
                                $$ = $1;
                            } else {
                                $$ = Minimal_SyntaxTree_createSumType($1, $3);
                            }
                        } else {
                            fprintf(stderr, "Error: Type operator '%s' is not defined.\n", $2->syntax_tree->string);
                            $$ = Minimal_SyntaxTree_createBlank();
                        }
                        Minimal_delReference($2);
                        }
        | typespec YY_OPERATOR value {
                        if($3->syntax_tree->type != ST_INTEGER) {
                            fprintf(stderr, "Error: Type multiplication must involve an integer.\n");
                            $$ = Minimal_SyntaxTree_createBlank();
                            Minimal_delReference($1); Minimal_delReference($2); Minimal_delReference($3);
                        } else if(strcmp($2->syntax_tree->string, "*") == 0) {
                            int i;
                            if($3->syntax_tree->integer == 0) {
                                Minimal_delReference($1); Minimal_delReference($3);
                                $$ = Minimal_SyntaxTree_createProductType();
                            } else {
                                $$ = Minimal_SyntaxTree_createProductType();
                                for(i = 0; i < $3->syntax_tree->integer; i++) {
                                    Minimal_addReference($1);
                                    Minimal_SyntaxTree_addToProductType($$, $1);
                                }
                            }
                            Minimal_delReference($3);
                        } else if(strcmp($2->syntax_tree->string, "+") == 0) {
                            int i;
                            if($3->syntax_tree->integer == 0) {
                                Minimal_delReference($1); Minimal_delReference($3);
                                $$ = Minimal_SyntaxTree_createSumType();
                            } else {
                                $$ = Minimal_SyntaxTree_createSumType();
                                for(i = 0; i < $3->syntax_tree->integer; i++) {
                                    Minimal_addReference($1);
                                    $$ = Minimal_SyntaxTree_addToSumType($$, $1);
                                }
                            }
                            Minimal_delReference($3);
                        } else {
                            fprintf(stderr, "Error: Type operator '%s' is not for a value as the second operator.\n", $2->syntax_tree->string);
                            $$ = Minimal_SyntaxTree_createBlank();
                            Minimal_delReference($1); Minimal_delReference($3);
                        }
                        Minimal_delReference($2);
                        }
        | YY_OPENB typespec YY_CLOSEB {
                        $$ = Minimal_SyntaxTree_createBracket($2);
                        }
        | YY_ID YY_OPENB YY_ID YY_CLOSEB {
                        if(strcmp($1->syntax_tree->string, "ptr") == 0) {
                            $$ = Minimal_SyntaxTree_createPointer($3);
                        } else {
                            fprintf(stderr, "Error: %s is an invalid type function.\n", $1->syntax_tree->string);
                            $$ = Minimal_SyntaxTree_createBlank();
                        }
                        Minimal_delReference($1); Minimal_delReference($3);
                        }
;

definition: YY_ID parameter_list YY_EQ expr {
                                              $$ = Minimal_SyntaxTree_createFunctionDef($1, $2, $4);
                                            }
;


expr: value { $$ = $1; }
    | YY_OPENB expr YY_CLOSEB { $$ = $2; }
    | expr YY_OPERATOR expr   { $$ = Minimal_SyntaxTree_createOperator($2, $1, $3); }
    | function_call           { $$ = $1; }
    | tuple                   { $$ = $1; }
    | if_expr                 { $$ = $1; }
    | test_expr               { $$ = $1; }
;

value: YY_ID { if(strcmp($1->syntax_tree->string, "Nil") == 0) {
                    Minimal_delReference(&$1);
                    $$ = Minimal_SyntaxTree_createNil();
                  } else {
                    $$ = $1;
                  }
             }
     | YY_INTEGER { $$ = $1; }
;

parameter_list: { $$ = Minimal_SyntaxTree_createBlank(); }
              | parameter_list YY_ID { if($1->syntax_tree->type == ST_BLANK) {
                                        $$ = Minimal_SyntaxTree_createParameterList1($2);
                                        Minimal_delReference($1);
                                       } else {
                                        $$ = Minimal_SyntaxTree_createParameterList2($1, $2);
                                       }
                                     }
;

function_call: expr YY_OPENB argument_list YY_CLOSEB {
        $$ = Minimal_SyntaxTree_createFunctionCall($1, $3);
    }
;

argument_list: { $$ = Minimal_SyntaxTree_createBlank(); }
              | argument_list expr { if($1->syntax_tree->type == ST_BLANK) {
                                        $$ = Minimal_SyntaxTree_createArgumentList1($2);
                                        Minimal_delReference($1);
                                       } else {
                                        $$ = Minimal_SyntaxTree_createArgumentList2($1, $2);
                                       }
                                     }
;

tuple: YY_OPENB YY_CLOSEB { $$ = Minimal_SyntaxTree_createTuple(0); }
     | expr YY_COMMA tuple2 { int i;
                              $$ = Minimal_SyntaxTree_createTuple(1);
                              Minimal_SyntaxTree_setTuple($$, 0, $1);
                              for(i=0; i<$3->syntax_tree->size; i++) {
                                  Minimal_addReference($3->syntax_tree->tuple[i]);
                                  Minimal_SyntaxTree_addToTuple($$, $3->syntax_tree->tuple[i]);
                              }
                              Minimal_delReference($3);
                            }
;

tuple2: { $$ = Minimal_SyntaxTree_createBlank(); }
        expr { $$ = Minimal_SyntaxTree_createTuple(1);
               Minimal_SyntaxTree_setTuple($$, 0, $1);
             }
      | tuple2 YY_COMMA expr { Minimal_SyntaxTree_addToTuple($1, $3);
                            $$ = $1;
                            Minimal_delReference($2);
                          }
;

if_expr: YY_IF expr YY_THEN expr YY_ELSE expr YY_ENDIF {
        $$ = Minimal_SyntaxTree_createIfExpr($2, $4, $6);
        Minimal_delReference($1); Minimal_delReference($3); Minimal_delReference($5); Minimal_delReference($7);
        }
;

test_expr: expr YY_TESTOP expr { $$ = Minimal_SyntaxTree_createOperator($2, $1, $3); }
;


%%
