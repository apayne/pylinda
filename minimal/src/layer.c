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

void Minimal_Layer_refCheck(Minimal_NameValueMap* map) {
    if(map->name != NULL) {
        printf("%s in layer with %li references\n", map->name, map->value->ref_count);
    }
    if(map->left != NULL) { Minimal_Layer_refCheck(map->left); }
    if(map->right != NULL) { Minimal_Layer_refCheck(map->right); }
}

void Minimal_Layer_finalise() {
    Minimal_delReference(Minimal_defaultLayer); Minimal_defaultLayer = NULL;
    Minimal_delReference(Minimal_currentLayer); Minimal_currentLayer = NULL;
}

MinimalLayer Minimal_setCurrentLayer(MinimalLayer layer) {
    if(layer == NULL) {
        fprintf(stderr, "Error: Setting current layer to NULL.\n");
        *((int*)NULL) = 0;
    }
    Minimal_addReference(layer);
    Minimal_delReference(Minimal_currentLayer);

    Minimal_currentLayer = layer;
    return layer;
}

MinimalLayer Minimal_getCurrentLayer() {
    Minimal_addReference(Minimal_currentLayer);

    return Minimal_currentLayer;
}

void Minimal_Layer_addTree(MinimalLayer layer, Minimal_SyntaxTree tree) {
    int i;
    switch(tree->type) {
    case ST_BLANK:
        break;
    case ST_SEQENTIAL_DEFS:
        for(i = 0; i < tree->length; i++) {
            Minimal_Layer_addTree(layer, tree->branches[i]);
        }
        break;
    case ST_TYPE_SPEC:
        {
        MinimalValue v;
        Minimal_addReference(tree);
        v = Minimal_typeSpec(tree->type_name, tree);
        Minimal_addName(layer->map, tree->type_name, v);
        }
        break;
    case ST_FUNCTION_DEF:
        {
        MinimalValue f;
        MinimalValue typespec = Minimal_getName(layer, tree->func_name);
        if(typespec != NULL && Minimal_isType(typespec)) {
            Minimal_addReference(typespec->type_spec);
            tree->type_spec = typespec->type_spec;
        } else {
            tree->type_spec = NULL;
        }
        if(tree->parameter_list == NULL) { fprintf(stderr, "Error: Parameter list is NULL from tree.\n"); *((int*)NULL) = 1; }
        Minimal_addReference(tree->parameter_list);
        Minimal_addReference(tree->body);
        f = Minimal_function2(tree->func_name, tree->type_spec, tree->parameter_list, tree->body);
        Minimal_delReference(f->layer);
        Minimal_addReference(layer);
        f->layer = layer;
        if(f->parameter_list == NULL) { fprintf(stderr, "Error: Parameter list is NULL when creating function.\n"); *((int*)NULL) = 1; }
        if(typespec != NULL && Minimal_isType(typespec)) {
            Minimal_delReference(typespec);
        }
        Minimal_addName(layer->map, tree->func_name, f);
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
    layer->map = Minimal_newReference(MINIMAL_MAP, Minimal_NameValueMap*, struct Minimal_NameValueMap_t);
    Minimal_SyntaxMap_init(layer->map);
    return layer;
}

MinimalLayer Minimal_createLayer2(MinimalLayer parent) {
    MinimalLayer layer = Minimal_newReference(MINIMAL_LAYER, MinimalLayer, struct MinimalLayer_t);
    layer->name = NULL;
    Minimal_addReference(parent);
    layer->parent = parent;
    layer->map = Minimal_newReference(MINIMAL_MAP, Minimal_NameValueMap*, struct Minimal_NameValueMap_t);
    Minimal_SyntaxMap_init(layer->map);
    return layer;
}

void Minimal_Layer_free(MinimalLayer layer) {
    free(layer->name);
    if(layer->parent != NULL) { Minimal_delReference(layer->parent); }
    Minimal_delReference(layer->map);
}

void Minimal_Layer_getReferences(struct CyclicGarbageList* list, MinimalLayer layer) {
    Minimal_addToCyclicGarbageList(list, layer->map);

    if(layer->parent == NULL) { return; }

    Minimal_addToCyclicGarbageList(list, (MinimalValue)layer->parent);
}
