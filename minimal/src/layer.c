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

MinimalLayer Minimal_defaultLayer;
static MinimalLayer Minimal_currentLayer;

void Minimal_Layer_init() {
    Minimal_defaultLayer = Minimal_createLayer();
    Minimal_defaultLayer->name = (char*)malloc(strlen("__default__")+1);
    strcpy(Minimal_defaultLayer->name, "__default__");
    Minimal_addReference(Minimal_defaultLayer);
    Minimal_currentLayer = Minimal_defaultLayer;
}

void Minimal_Layer_finalise() {
    Minimal_delReference(Minimal_defaultLayer); Minimal_defaultLayer = NULL;
    Minimal_delReference(Minimal_currentLayer); Minimal_currentLayer = NULL;
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
    case ST_BLANK:
        break;
    case ST_SEQENTIAL_DEFS:
        Minimal_Layer_addTree(layer, tree->branch1);
        Minimal_Layer_addTree(layer, tree->branch2);
        break;
    case ST_TYPE_SPEC:
        {
        MinimalValue t = Minimal_typeSpec(tree->type_name, tree->type_def);
        Minimal_addName(&(layer->map), tree->type_name, t);
        }
        break;
    case ST_FUNCTION_DEF:
        {
        MinimalValue f;
        MinimalValue typespec = Minimal_getName(layer, tree->func_name);
        if(typespec != NULL && Minimal_isType(typespec)) {
            tree->type_def = NULL;
        }
        if(tree->parameter_list == NULL) { fprintf(stderr, "Error: Parameter list is NULL from tree.\n"); *((int*)NULL) = 1; }
        f = Minimal_function2(tree->func_name, tree->type_def, tree->parameter_list, tree->body);
        Minimal_delReference(f->layer);
        Minimal_addReference(layer);
        f->layer = layer;
        if(f->parameter_list == NULL) { fprintf(stderr, "Error: Parameter list is NULL when creating function.\n"); *((int*)NULL) = 1; }
        if(typespec != NULL && Minimal_isType(typespec)) {
            Minimal_setType(f, typespec);
            Minimal_delReference(typespec);
        }
        Minimal_addName(&(layer->map), tree->func_name, f);
        }
        break;
    default:
        fprintf(stderr, "Unknown tree node type in Minimal_Layer_addTree (%i)\n", tree->type);
        return;
    }
}

MinimalLayer Minimal_createLayer() {
    MinimalLayer layer = Minimal_newReference(MINIMAL_LAYER, MinimalLayer, struct MinimalLayer_t);
    layer->name = NULL;
    layer->parent = NULL;
    Minimal_SyntaxMap_init(&(layer->map));
    return layer;
}

MinimalLayer Minimal_createLayer2(MinimalLayer parent) {
    MinimalLayer layer = Minimal_newReference(MINIMAL_LAYER, MinimalLayer, struct MinimalLayer_t);
    layer->name = NULL;
    Minimal_addReference(parent);
    layer->parent = parent;
    Minimal_SyntaxMap_init(&(layer->map));
    return layer;
}

void Minimal_Layer_free(MinimalLayer layer) {
    free(layer->name);
    if(layer->parent != NULL) { Minimal_delReference(layer->parent); }
    Minimal_SyntaxMap_empty(&(layer->map));
    free(layer);
}

MinimalObject* Minimal_Layer_getReferences(MinimalLayer layer) {
    MinimalObject* list;
    int i = 0;
    MinimalObject* tree = Minimal_SyntaxMap_getReferences(&(layer->map));

    if(layer->parent == NULL) { return tree; }

    while(tree[i] != NULL) { i++; }
    list = malloc(sizeof(void*) * (i+1));
    memcpy(list, tree, sizeof(void*) * i);
    list[i] = layer->parent;
    list[i+1] = NULL;
    free(tree);
    return list;
}
