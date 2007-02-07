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

#include "minimal.h"

int main(int argc, char* argv[]) {
    Minimal_init();

    if(argc > 1) {
        Minimal_SyntaxTree* tree;
        int size;
        char* code;

        FILE* fp = fopen(argv[1], "r");
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        code = (char*)malloc(size+1);
        fseek(fp, 0, SEEK_SET);
        fread(code, 1, size, fp);
        code[size] = '\0';
        fclose(fp);

        tree = Minimal_parseCode(code);
        if(tree != NULL) {
            Minimal_Layer_addTree(Minimal_defaultLayer, tree);
            Minimal_SyntaxTree_free(tree);
        }

        free(code);
    }

    Minimal_interpreter();

    Minimal_finalise();

    return 0;
}
