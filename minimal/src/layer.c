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

MinimalLayer Minimal_defaultLayer;
static MinimalLayer Minimal_currentLayer;

void Minimal_Layer_init() {
}

MinimalLayer Minimal_setCurrentLayer(MinimalLayer layer) {
    Minimal_addReference(layer);
    Minimal_delReference(Minimal_currentLayer);

    Minimal_currentLayer = layer;
    return layer;
}

MinimalLayer Minimal_getCurrentLayer() {
    Minimal_addReference(Minimal_currentLayer);

    return Minimal_currentLayer;
}

void Minimal_Layer_addTree(MinimalLayer layer, Minimal_SyntaxTree* tree) {
    switch(tree->type) {
    case IDENTIFIER:
    case INTEGER:
    case TYPE_FUNCTION:
        break;
    case SEQENTIAL_DEFS:
        Minimal_Layer_addTree(layer, tree->branch1);
        Minimal_Layer_addTree(layer, tree->branch2);
        break;
    case TYPE_SPEC:
        Minimal_addName(&(layer->map), tree->type_name, tree->type_def);
        break;
    default:
        fprintf(stderr, "Unknown tree node type in Minimal_Layer_addTree (%i)\n", tree->type);
        return;
    }
}
