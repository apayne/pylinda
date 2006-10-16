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

#ifndef MINIMAL_H

void Minimal_init();
void Minimal_finalise();

typedef void* MinimalObject;

struct Minimal_SyntaxTree_t {
    enum {
        BLANK,
        IDENTIFIER,
        INTEGER,
        SEQENTIAL_DEFS,
        TYPE_SPEC,
        TYPE_FUNCTION
    } type;
    union {
        int integer;
        double floating;
        char* string;
        struct {
            struct Minimal_SyntaxTree_t* branch1;
            struct Minimal_SyntaxTree_t* branch2;
        };
        struct {
            char* type_name;
            struct Minimal_SyntaxTree_t* type_def;
        };
    };
};
typedef struct Minimal_SyntaxTree_t Minimal_SyntaxTree;

struct Minimal_NameSyntaxMap_t {
    char* name;
    Minimal_SyntaxTree* tree;
    struct Minimal_NameSyntaxMap_t* left;
    struct Minimal_NameSyntaxMap_t* right;
};
typedef struct Minimal_NameSyntaxMap_t Minimal_NameSyntaxMap;

struct MinimalLayer_t {
    char* name;
    Minimal_NameSyntaxMap map;
};
typedef struct MinimalLayer_t* MinimalLayer;

struct MinimalFunction_t {
    char* name;
    Minimal_SyntaxTree* type;
    Minimal_SyntaxTree* code;
};
typedef struct MinimalFunction_t* MinimalFunction;

MinimalLayer Minimal_createLayer();

MinimalFunction Minimal_parseTypeSpec(char* code);
MinimalFunction Minimal_parseCode(char* code);

MinimalFunction Minimal_getFunction(char* funcname);

void Minimal_addReference(MinimalObject obj);
void Minimal_delReference(MinimalObject obj);

MinimalLayer Minimal_getCurrentLayer();
MinimalLayer Minimal_setCurrentLayer(MinimalLayer layer);
extern MinimalLayer Minimal_defaultLayer;

#endif
