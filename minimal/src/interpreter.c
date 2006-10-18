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

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "minimal_internal.h"

void Minimal_interpreter() {
    Minimal_SyntaxTree* tree;

    while(1) {
        char* line = readline("> ");
        if(line == NULL) { break; }
        if(strlen(line) == 0) { free(line); continue; }

        add_history(line);

        tree = Minimal_parseCode(line);
        if(tree == NULL) { free(line); continue; }

        Minimal_evaluate(tree, Minimal_defaultLayer);

        Minimal_SyntaxTree_free(tree);

        free(line);
    }
}
