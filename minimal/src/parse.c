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

#include "minimal_internal.h"

#include "yy.lex.h"

int yyparse();
extern Minimal_SyntaxTree yylval;

MinimalFunction Minimal_parseTypeSpec(char* code) {
    return NULL;
}

MinimalFunction Minimal_parseCode(char* code) {
    int r;
    YY_BUFFER_STATE buf = yy_scan_string(code);
    r = yyparse(code);
    yy_delete_buffer(buf);
    if(r == 0) {
        return NULL;
    } else {
        MinimalFunction f;
        MinimalLayer layer = Minimal_setCurrentLayer(Minimal_defaultLayer);
        f = Minimal_createFunction(&yylval);
        Minimal_setCurrentLayer(layer);
        Minimal_delReference(layer);
        Minimal_SyntaxTree_clear(&yylval);
        return f;
    }
}
