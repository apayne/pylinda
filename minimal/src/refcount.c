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

struct MinimalRefCount {
    MinimalTypeId type_id;
    void* ptr;
    unsigned long count;
    struct MinimalRefCount* left;
    struct MinimalRefCount* right;
    char* file;
    int line;
};

struct MinimalRefCount* Minimal_refCountTree = NULL;

void Minimal_delObject(MinimalTypeId type_id, void* ptr);

void* Minimal_newReference2(MinimalTypeId type_id, void* ptr, char* file, int line) {
    if(Minimal_refCountTree == NULL) {
        Minimal_refCountTree = (struct MinimalRefCount*)malloc(sizeof(struct MinimalRefCount));
        Minimal_refCountTree->type_id = type_id;
        Minimal_refCountTree->ptr = ptr;
        Minimal_refCountTree->count = 1;
        Minimal_refCountTree->left = NULL;
        Minimal_refCountTree->right = NULL;
        Minimal_refCountTree->file = file;
        Minimal_refCountTree->line = line;
        return ptr;
    } else {
        struct MinimalRefCount* tree = Minimal_refCountTree;
        while(1) {
            if(((unsigned long)ptr) < ((unsigned long)tree->ptr)) {
                if(tree->left == NULL) {
                    tree->left = (struct MinimalRefCount*)malloc(sizeof(struct MinimalRefCount));
                    tree->left->type_id = type_id;
                    tree->left->ptr = ptr;
                    tree->left->count = 1;
                    tree->left->left = NULL;
                    tree->left->right = NULL;
                    tree->left->file = file;
                    tree->left->line = line;
                    return ptr;
                } else {
                    tree = tree->left;
                }
            } else if(((unsigned long)ptr) > ((unsigned long)tree->ptr)) {
                if(tree->right == NULL) {
                    tree->right = (struct MinimalRefCount*)malloc(sizeof(struct MinimalRefCount));
                    tree->right->type_id = type_id;
                    tree->right->ptr = ptr;
                    tree->right->count = 1;
                    tree->right->left = NULL;
                    tree->right->right = NULL;
                    tree->right->file = file;
                    tree->right->line = line;
                    return ptr;
                } else {
                    tree = tree->right;
                }
            } else if(tree->count == 0) {
                tree->type_id = type_id;
                tree->ptr = ptr;
                tree->count = 1;
                tree->file = file;
                tree->line = line;
                return ptr;
            } else {
                fprintf(stderr, "Error: New reference in the same location as an unfreed reference.\n");
                exit(1);
            }
        }
    }
}

void Minimal_addReference2(MinimalObject ptr, char* file, int line) {
    struct MinimalRefCount* tree = Minimal_refCountTree;
    while(tree != NULL) {
        if(tree->ptr == ptr) {
            tree->count++;
            return;
        } else if(((unsigned long)ptr) < ((unsigned long)tree->ptr)) {
            tree = tree->left;
        } else if(((unsigned long)ptr) > ((unsigned long)tree->ptr)) {
            tree = tree->right;
        }
    }
    fprintf(stderr, "Error: addReference to pointer (%p) not allocated with Minimal_newReference (%s:%i).\n", ptr, file, line);
    int i = *((int*)ptr);
    i = i + 1;
}

int Minimal_getReferenceCount(MinimalObject ptr) {
    struct MinimalRefCount* tree = Minimal_refCountTree;
    while(tree != NULL) {
        if(tree->ptr == ptr) {
            return tree->count;
        } else if(((unsigned long)ptr) < ((unsigned long)tree->ptr)) {
            tree = tree->left;
        } else if(((unsigned long)ptr) > ((unsigned long)tree->ptr)) {
            tree = tree->right;
        }
    }
    return -1;
}

void Minimal_delReference2(MinimalObject ptr, char* file, int line) {
    struct MinimalRefCount* parent = NULL;
    struct MinimalRefCount* tree = Minimal_refCountTree;
    while(tree != NULL) {
        if(tree->ptr == ptr) {
            if(tree->count <= 0) {
                return;
            }
            tree->count--;
            if(tree->count == 0) {
                Minimal_delObject(tree->type_id, ptr);
                if(tree->left == NULL && tree->right == NULL) {
                    if(parent->left == tree) {
                        parent->left = NULL;
                    } else {
                        parent->right = NULL;
                    }
                    free(tree);
                } else {
                    ptr = NULL;
                }
            }
            return;
        } else if(((unsigned long)ptr) < ((unsigned long)tree->ptr)) {
            parent = tree;
            tree = tree->left;
        } else if(((unsigned long)ptr) > ((unsigned long)tree->ptr)) {
            parent = tree;
            tree = tree->right;
        }
    }
    fprintf(stderr, "Error: delReference to pointer (%p) not allocated with Minimal_newReference (%s:%i).\n", ptr, file, line);
}

void Minimal_delObject(MinimalTypeId type_id, void* ptr) {
    switch(type_id) {
    case MINIMAL_VALUE:
        Minimal_Value_free((MinimalValue)ptr);
        break;
    case MINIMAL_LAYER:
        Minimal_Layer_free((MinimalLayer)ptr);
        break;
    case MINIMAL_MAP:
        Minimal_SyntaxMap_empty((Minimal_NameValueMap*)ptr);
        break;
    default:
        fprintf(stderr, "Error: Deleting object with unrecognised type_id (%i).\n", type_id);
    }
}

int Minimal_countAllReferences(struct MinimalRefCount* tree) {
    if(tree == NULL) {
        return 0;
    } else {
        return tree->count + Minimal_countAllReferences(tree->left) + Minimal_countAllReferences(tree->right);
    }
}

static void Minimal_refCountEmpty(struct MinimalRefCount* tree) {
    if(tree->left != NULL) {
        Minimal_refCountEmpty(tree->left);
        free(tree->left);
    }
    if(tree->right != NULL) {
        Minimal_refCountEmpty(tree->right);
        free(tree->right);
    }
    if(tree->count > 0) {
        fprintf(stderr, "%p (%s:%i) still has %li references.\n", tree->ptr, tree->file, tree->line, tree->count);
    }
}

void Minimal_refCountFinalise() {
    int i = Minimal_countAllReferences(Minimal_refCountTree);
    if(i > 0) {
        fprintf(stderr, "Warning, %i references unfreed.\n", i);
    }
    if(Minimal_refCountTree != NULL) {
        Minimal_refCountEmpty(Minimal_refCountTree);
        free(Minimal_refCountTree);
    }
}
