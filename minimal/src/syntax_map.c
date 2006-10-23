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
    if(map == NULL) {
        fprintf(stderr, "Minimal Error: addName called with NULL map parameter.\n");
        return;
    } else if(map->name == NULL) {
        map->name = (char*)malloc(strlen(name)+1);
        strcpy(map->name, name);
        map->value = tree;
    } else if(strcmp(map->name, name) == 0) {
        free(map->name);
        map->name = (char*)malloc(strlen(name)+1);
        strcpy(map->name, name);
        Minimal_delReference(map->value);
        map->value = tree;
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
    }
}

MinimalValue Minimal_getName(Minimal_NameValueMap* map, char* name) {
    if(map == NULL || map->name == NULL) {
        return NULL;
    } else if(strcmp(map->name, name) == 0) {
        Minimal_addReference(map->value);
        return map->value;
    } else if(strcmp(map->name, name) < 0) {
        return Minimal_getName(map->left, name);
    } else if(strcmp(map->name, name) > 0) {
        return Minimal_getName(map->right, name);
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
        if(map->left != NULL) {
            map->name = (char*)malloc(strlen(map->left->name)+1);
            strcpy(map->name, map->left->name);
            Minimal_addReference(map->left->value);
            map->value = map->left->value;
            Minimal_delName(map->left, map->left->name);
        } else if(map->right != NULL) {
            map->name = (char*)malloc(strlen(map->right->name)+1);
            strcpy(map->name, map->right->name);
            Minimal_addReference(map->right->value);
            map->value = map->right->value;
            Minimal_delName(map->right, map->right->name);
        }
    } else if(strcmp(map->name, name) < 0) {
        Minimal_delName(map->left, name);
    } else if(strcmp(map->name, name) > 0) {
        Minimal_delName(map->left, name);
    }
}

void Minimal_SyntaxMap_empty(Minimal_NameValueMap* map) {
    while(map->name != NULL) {
        Minimal_delName(map, map->name);
    }
}
