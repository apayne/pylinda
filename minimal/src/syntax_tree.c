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
#include <string.h>

#include "minimal_internal.h"

Minimal_SyntaxTree Minimal_SyntaxTree_createID(char* id) {
    Minimal_SyntaxTree tree;
    tree.type = IDENTIFIER;
    tree.string = id;
    return tree;
}

Minimal_SyntaxTree Minimal_SyntaxTree_createInteger(int i) {
    Minimal_SyntaxTree tree;
    tree.type = INTEGER;
    tree.integer = i;
    return tree;
}

Minimal_SyntaxTree* Minimal_SyntaxTree_copy(Minimal_SyntaxTree* tree) {
    Minimal_SyntaxTree* ntree;
    if(tree == NULL) { return NULL; }
    ntree = (Minimal_SyntaxTree*)malloc(sizeof(struct Minimal_SyntaxTree_t));
    switch(tree->type) {
    case IDENTIFIER:
        ntree->type = IDENTIFIER;
        ntree->string = (char*)malloc(strlen(tree->string)+1);
        strcpy(ntree->string, tree->string);
        return ntree;
    case INTEGER:
        ntree->type = INTEGER;
        ntree->integer = tree->integer;
        return ntree;
    case SEQENTIAL_DEFS:
    case TYPE_SPEC:
    case TYPE_FUNCTION:
    default:
        fprintf(stderr, "Unknown tree node type in Minimal_SyntaxTree_copy (%i)\n", tree->type);
        free(ntree);
        return NULL;
    }
}

void Minimal_SyntaxTree_free(Minimal_SyntaxTree* tree) {
    if(tree == NULL) { return; }

    switch(tree->type) {
    case IDENTIFIER:
        free(tree->string);
        break;
    case INTEGER:
        break;
    case SEQENTIAL_DEFS:
    case TYPE_SPEC:
    case TYPE_FUNCTION:
    default:
        fprintf(stderr, "Unknown tree node type in Minimal_SyntaxTree_free (%i)\n", tree->type);
        break;
    }
    free(tree);
}
