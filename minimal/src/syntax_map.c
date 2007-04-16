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
#include <string.h>

#include "minimal_internal.h"

void Minimal_addName(Minimal_NameValueMap* map, char* name, MinimalValue tree) {
    if(strlen(name) == 0) {
        fprintf(stderr, "Minimal Error: empty name provided.\n"); (*((int*)NULL))++;
        return;
    } else if(map == NULL) {
        fprintf(stderr, "Minimal Error: addName called with NULL map parameter.\n");
        return;
    } else if(name == NULL) {
        fprintf(stderr, "Minimal Error: addName called with NULL name parameter.\n");
        return;
    } else if(tree == NULL) {
        fprintf(stderr, "Minimal Error: addName called with NULL tree parameter.\n");
        return;
    } else if(map->name == NULL && map->left && strcmp(map->left->name, name) < 0) {
        Minimal_addName(map->left, name, tree);
    } else if(map->name == NULL && map->right && strcmp(map->left->name, name) > 0) {
        Minimal_addName(map->right, name, tree);
    } else if(map->name == NULL) {
        map->name = (char*)malloc(strlen(name)+1);
        strcpy(map->name, name);
        map->value = tree;
     } else if(strcmp(map->name, name) == 0) {
        MinimalValue old = map->value;
        map->value = tree;
        Minimal_delReference(old);
    } else if(strcmp(map->name, name) == -1) {
        if(map->left == NULL) {
            map->left = (Minimal_NameValueMap*)malloc(sizeof(struct Minimal_NameValueMap_t));
            Minimal_SyntaxMap_init(map->left);
        }
        Minimal_addName(map->left, name, tree);
    } else if(strcmp(map->name, name) == 1) {
        if(map->right == NULL) {
            map->right = (Minimal_NameValueMap*)malloc(sizeof(struct Minimal_NameValueMap_t));
            Minimal_SyntaxMap_init(map->right);
        }
        Minimal_addName(map->right, name, tree);
    } else {
        fprintf(stderr, "Minimal Error: Failed to add name. This should never happen.\n");
    }
}

MinimalValue Minimal_getName(MinimalLayer layer, char* name) {
    return Minimal_getName2(&(layer->map), name);
}

MinimalValue Minimal_getName2(Minimal_NameValueMap* map, char* name) {
    if(map == NULL) {
        return NULL;
    } else if(map->name == NULL) {
        MinimalValue v = Minimal_getName2(map->left, name);
        if(v == NULL) {
            v = Minimal_getName2(map->right, name);
        }
        return v;
    }
    if(strcmp(map->name, name) == 0) {
        Minimal_addReference(map->value);
        return map->value;
    } else if(strcmp(map->name, name) < 0) {
        return Minimal_getName2(map->left, name);
    } else if(strcmp(map->name, name) > 0) {
        return Minimal_getName2(map->right, name);
    } else {
        return NULL; /* should never get here */
    }
}

void Minimal_delName(Minimal_NameValueMap* map, char* name) {
    if(map == NULL || map->name == NULL) {
        return;
    } else if(strcmp(map->name, name) == 0) {
        free(map->name);
        map->name = NULL;
        Minimal_delReference(map->value); map->value = NULL;
    } else if(strcmp(map->name, name) < 0) {
        Minimal_delName(map->left, name);
    } else if(strcmp(map->name, name) > 0) {
        Minimal_delName(map->left, name);
    }
}

int Minimal_SyntaxMap_size(Minimal_NameValueMap* map) {
    int count = 0;
    if(map == NULL) { return 0; }
    if(map->left != NULL) {
        count += Minimal_SyntaxMap_size(map->left);
    }
    if(map->right != NULL) {
        count += Minimal_SyntaxMap_size(map->right);
    }
    if(map->name != NULL) {
        count += 1;
    }
    return count;
}

void Minimal_SyntaxMap_empty(Minimal_NameValueMap* map) {
    if(map->left != NULL) {
        Minimal_SyntaxMap_empty(map->left); free(map->left);
        map->left = NULL;
    }
    if(map->right != NULL) {
        Minimal_SyntaxMap_empty(map->right); free(map->right);
        map->right = NULL;
    }
    if(map->name != NULL) {
        Minimal_delName(map, map->name);
    }
}

void Minimal_SyntaxMap_getReferences(struct CyclicGarbageList* list, Minimal_NameValueMap* map) {
    if(map->left != NULL) {
        Minimal_SyntaxMap_getReferences(list, map->left);
    }
    if(map->right != NULL) {
        Minimal_SyntaxMap_getReferences(list, map->right);
    }
    if(map->name != NULL) {
        Minimal_addToCyclicGarbageList(list, map->value);
    }
}
