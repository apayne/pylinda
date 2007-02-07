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
}
%}

%token YY_ID YY_INTEGER YY_TYPESPEC YY_FUNCTION YY_OPERATOR YY_SEMICOLON YY_COMMA YY_EQ YY_OPENB YY_CLOSEB

%% /* Grammar rules and actions follow */

input: { $$.type = ST_BLANK; yy_result = $$; }
     | input typespec_def YY_SEMICOLON {
                        if($1.type == ST_BLANK) {
                            $$ = $2;
                        } else if($2.type == ST_BLANK) {
                            $$ = $1;
                        } else {
                            if($1.type != ST_SEQENTIAL_DEFS) {
                                $$.type = ST_SEQENTIAL_DEFS;
                                $$.length = 2;
                                $$.branches = (struct Minimal_SyntaxTree_t**)malloc(sizeof(void*) * 2);
                                $$.branches[0] = Minimal_SyntaxTree_copy(&$1); Minimal_SyntaxTree_clear(&$1);
                                $$.branches[1] = Minimal_SyntaxTree_copy(&$2); Minimal_SyntaxTree_clear(&$2);
                            } else {
								struct Minimal_SyntaxTree_t** b;
                                $$ = $1;
                                b = malloc(sizeof(void*) * $$.length + 1);
                                memcpy(b, $$.branches, sizeof(void*) * $$.length);
                                b[$$.length] = Minimal_SyntaxTree_copy(&$2); Minimal_SyntaxTree_clear(&$2);
                                free($$.branches); $$.branches = b;
                                $$.length++;
                            }
                        }
                        yy_result = $$;
                        }
     | input definition YY_SEMICOLON {
                        if($1.type == ST_BLANK) {
                            $$ = $2;
                        } else if($2.type == ST_BLANK) {
                            $$ = $1;
                        } else {
                            if($1.type != ST_SEQENTIAL_DEFS) {
                                $$.type = ST_SEQENTIAL_DEFS;
                                $$.length = 2;
                                $$.branches = (struct Minimal_SyntaxTree_t**)malloc(sizeof(void*) * 2);
                                $$.branches[0] = Minimal_SyntaxTree_copy(&$1); Minimal_SyntaxTree_clear(&$1);
                                $$.branches[1] = Minimal_SyntaxTree_copy(&$2); Minimal_SyntaxTree_clear(&$2);
                            } else {
                                struct Minimal_SyntaxTree_t** b;
								$$ = $1;
                                b = malloc(sizeof(void*) * $$.length + 1);
                                memcpy(b, $$.branches, sizeof(void*) * $$.length);
                                b[$$.length] = Minimal_SyntaxTree_copy(&$2); Minimal_SyntaxTree_clear(&$2);
                                free($$.branches); $$.branches = b;
                                $$.length++;
                            }
                        }
                        yy_result = $$;
                        }
     | input expr YY_SEMICOLON {
                        if($1.type == ST_BLANK) {
                            $$ = $2;
                        } else if($2.type == ST_BLANK) {
                            $$ = $1;
                        } else {
                            if($1.type != ST_SEQENTIAL_DEFS) {
                                $$.type = ST_SEQENTIAL_DEFS;
                                $$.length = 2;
                                $$.branches = (struct Minimal_SyntaxTree_t**)malloc(sizeof(void*) * 2);
                                $$.branches[0] = Minimal_SyntaxTree_copy(&$1); Minimal_SyntaxTree_clear(&$1);
                                $$.branches[1] = Minimal_SyntaxTree_copy(&$2); Minimal_SyntaxTree_clear(&$2);
                            } else {
                                struct Minimal_SyntaxTree_t** b;
								$$ = $1;
                                b = malloc(sizeof(void*) * $$.length + 1);
                                memcpy(b, $$.branches, sizeof(void*) * $$.length);
                                b[$$.length] = Minimal_SyntaxTree_copy(&$2); Minimal_SyntaxTree_clear(&$2);
                                free($$.branches); $$.branches = b;
                                $$.length++;
                            }
                        }
                        yy_result = $$;
                        }
;

typespec_def: YY_ID YY_TYPESPEC typespec {
    $$.type_name = (char*)malloc(strlen($1.string)+1); strcpy($$.type_name, $1.string); Minimal_SyntaxTree_clear(&$1);
    $$.type_def  = Minimal_SyntaxTree_copy(&$3); Minimal_SyntaxTree_clear(&$3);
    $$.type = ST_TYPE_SPEC;
    }
;

typespec: YY_ID { if(strcmp($1.string, "Nil") == 0) {
                    Minimal_SyntaxTree_clear(&$1);
                    $$.type = ST_NIL;
                  } else {
                    $$ = $1;
                  }
                }
        | typespec YY_FUNCTION typespec {
                        $$.type = ST_TYPE_FUNCTION;
                        $$.length = 2;
                        $$.branches = (struct Minimal_SyntaxTree_t**)malloc(sizeof(void*) * 2);
                        $$.branches[0] = Minimal_SyntaxTree_copy(&$1); Minimal_SyntaxTree_clear(&$1);
                        $$.branches[1] = Minimal_SyntaxTree_copy(&$3); Minimal_SyntaxTree_clear(&$3);
                        }
        | typespec YY_OPERATOR typespec {
                        if(strcmp($2.string, "*") == 0) {
                            $$.type = ST_PRODUCT_TYPE;
                            $$.length = 2;
                            $$.branches = (struct Minimal_SyntaxTree_t**)malloc(sizeof(void*) * 2);
                            $$.branches[0] = Minimal_SyntaxTree_copy(&$1); Minimal_SyntaxTree_clear(&$1);
                            $$.branches[1] = Minimal_SyntaxTree_copy(&$3); Minimal_SyntaxTree_clear(&$3);
                        } else if(strcmp($2.string, "+") == 0) {
                            $$.type = ST_SUM_TYPE;
                            $$.length = 2;
                            $$.branches = (struct Minimal_SyntaxTree_t**)malloc(sizeof(void*) * 2);
                            $$.branches[0] = Minimal_SyntaxTree_copy(&$1); Minimal_SyntaxTree_clear(&$1);
                            $$.branches[1] = Minimal_SyntaxTree_copy(&$3); Minimal_SyntaxTree_clear(&$3);
                        } else {
                            fprintf(stderr, "Error: Type operator '%s' is not defined.\n", $2.string);
                            $$.type = ST_BLANK;
                        }
                        Minimal_SyntaxTree_clear(&$2);
                        }
        | YY_OPENB typespec YY_CLOSEB {
                        $$ = $2;
                        }
        | YY_ID YY_OPENB YY_ID YY_CLOSEB {
                        if(strcmp($1.string, "ptr") == 0) {
                            $$.type = ST_POINTER;
                            $$.ptr = (char*)malloc(strlen($3.string)+1); strcpy($$.type_name, $3.string); Minimal_SyntaxTree_clear(&$3);
                            Minimal_SyntaxTree_clear(&$1);
                        } else {
                            fprintf(stderr, "Error: %s is an invalid type function.\n", $1.string);
                            $$.type = ST_BLANK;
                            Minimal_SyntaxTree_clear(&$1);
                        }
                        }
;

definition: YY_ID parameter_list YY_EQ expr { $$.type = ST_FUNCTION_DEF;
                                              $$.func_name = (char*)malloc(strlen($1.string)+1); strcpy($$.func_name, $1.string);
                                              Minimal_SyntaxTree_clear(&$1);
                                              $$.type_spec = NULL;
                                              $$.parameter_list = Minimal_SyntaxTree_copy(&$2); Minimal_SyntaxTree_clear(&$2);
                                              $$.body = Minimal_SyntaxTree_copy(&$4); Minimal_SyntaxTree_clear(&$4);
                                            }
;


expr: value { $$ = $1; }
    | YY_OPENB expr YY_CLOSEB { $$ = $2; }
    | expr YY_OPERATOR expr { $$.type = ST_OPERATOR;
                              $$._operator = (char*)malloc(strlen($2.string)+1); strcpy($$.func_name, $2.string);
                              Minimal_SyntaxTree_clear(&$2);
                              $$.op1 = Minimal_SyntaxTree_copy(&$1); Minimal_SyntaxTree_clear(&$1);
                              $$.op2 = Minimal_SyntaxTree_copy(&$3); Minimal_SyntaxTree_clear(&$3);
                            }
    | function_call { $$ = $1; }
    | tuple { $$ = $1; }
;

value: YY_ID { if(strcmp($1.string, "Nil") == 0) {
                    Minimal_SyntaxTree_clear(&$1);
                    $$.type = ST_NIL;
                  } else {
                    $$ = $1;
                  }
             }
     | YY_INTEGER { $$ = $1; }
;

parameter_list: { $$.type = ST_BLANK; }
              | parameter_list YY_ID { if($1.type == ST_BLANK) {
                                        $$.type = ST_PARAMETER_LIST;
                                        $$.var_name = (char*)malloc(strlen($2.string)+1); strcpy($$.var_name, $2.string);
                                        $$.next_var = NULL; Minimal_SyntaxTree_clear(&$2);
                                       } else {
                                        $$ = $1;
                                        $$.next_var = (Minimal_SyntaxTree*)malloc(sizeof(Minimal_SyntaxTree));
                                        $$.next_var->var_name = (char*)malloc(strlen($2.string)+1); strcpy($$.next_var->var_name, $2.string);
                                        $$.next_var->var_name = NULL; Minimal_SyntaxTree_clear(&$2);
                                       }
                                     }
;

function_call: expr YY_OPENB argument_list YY_CLOSEB {
        $$.type = ST_FUNCTION_CALL;
        $$.function = Minimal_SyntaxTree_copy(&$1); Minimal_SyntaxTree_clear(&$1);
        $$.arguments = Minimal_SyntaxTree_copy(&$3); Minimal_SyntaxTree_clear(&$3);
    }
;

argument_list: { $$.type = ST_BLANK; }
              | argument_list expr { if($1.type == ST_BLANK) {
                                        $$.type = ST_ARGUMENT_LIST;
                                        $$.argument = Minimal_SyntaxTree_copy(&$2); Minimal_SyntaxTree_clear(&$2);
                                        $$.next_var = NULL;
                                       } else {
                                        $$ = $1;
                                        $$.next_arg = (Minimal_SyntaxTree*)malloc(sizeof(Minimal_SyntaxTree));
                                        $$.next_arg->argument = Minimal_SyntaxTree_copy(&$2); Minimal_SyntaxTree_clear(&$2);
                                        $$.next_arg->next_arg = NULL;
                                       }
                                     }
;

tuple: YY_OPENB YY_CLOSEB { $$ = Minimal_SyntaxTree_createTuple(0); }
     | expr YY_COMMA tuple2 { int i;
							  $$ = Minimal_SyntaxTree_createTuple(1);
                              $$.tuple[0] = Minimal_SyntaxTree_copy(&$1); Minimal_SyntaxTree_clear(&$1);
                              for(i=0; i<$3.size; i++) {
                                  Minimal_SyntaxTree_addToTuple(&$$, $3.tuple[i]);
                              }
                              Minimal_SyntaxTree_clear(&$3);
                            }
;

tuple2: { $$.type = ST_BLANK; }
        expr { $$ = Minimal_SyntaxTree_createTuple(1);
               $$.tuple[0] = Minimal_SyntaxTree_copy(&$1); Minimal_SyntaxTree_clear(&$1);
             }
      | tuple2 YY_COMMA expr { Minimal_SyntaxTree_addToTuple($1.tuple[0], Minimal_SyntaxTree_copy(&$3)); Minimal_SyntaxTree_clear(&$3);
                            $$ = $1;
                          }
;

%%
