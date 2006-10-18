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

#include "minimal.h"

#ifndef MINIMAL_INTERNAL_H

void Minimal_Layer_init();
void Minimal_Layer_finalise();

Minimal_SyntaxTree Minimal_SyntaxTree_createID(char* id);
Minimal_SyntaxTree Minimal_SyntaxTree_createInteger(int i);

Minimal_SyntaxTree* Minimal_SyntaxTree_copy(Minimal_SyntaxTree* tree);
void Minimal_SyntaxTree_free(Minimal_SyntaxTree* tree);
void Minimal_SyntaxTree_clear(Minimal_SyntaxTree* tree);

MinimalFunction Minimal_createFunction(Minimal_SyntaxTree* tree);

void Minimal_addName(Minimal_NameSyntaxMap* map, char* name, Minimal_SyntaxTree* tree);
Minimal_SyntaxTree* Minimal_getName(Minimal_NameSyntaxMap* map, char* name);
void Minimal_delName(Minimal_NameSyntaxMap* map, char* name);
void Minimal_SyntaxMap_empty(Minimal_NameSyntaxMap* map);

void Minimal_Layer_addTree(MinimalLayer layer, Minimal_SyntaxTree* tree);

void Minimal_deleteLayer(MinimalLayer layer);

void Minimal_interpreter();

#define Minimal_SyntaxMap_init(map) \
    (map)->name = NULL; \
    (map)->left = NULL; \
    (map)->right = NULL

#endif
