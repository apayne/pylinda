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

#include <stdlib.h>
#include <stdio.h>

#include "minimal_internal.h"

#include "yy.lex.h"

int yyparse();
Minimal_SyntaxTree yy_result;

MinimalFunction Minimal_parseTypeSpec(char* code) {
    return NULL;
}

MinimalFunction Minimal_parseCode(char* code) {
    int r;
    YY_BUFFER_STATE buf = yy_scan_string(code);
    r = yyparse(code);
    if(r != 0) {
        yy_delete_buffer(buf);
        fprintf(stderr, "Syntax error.\n");
        return NULL;
    } else {
        MinimalFunction f;
        MinimalLayer layer = Minimal_setCurrentLayer(Minimal_defaultLayer);
        f = Minimal_createFunction(&yy_result);
        Minimal_SyntaxTree_clear(&yy_result);
        Minimal_setCurrentLayer(layer);
        Minimal_delReference(layer);
        yy_delete_buffer(buf);
        return f;
    }
}
