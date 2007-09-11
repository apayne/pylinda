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

#include "minimal_internal.h"

MinimalValue Minimal_builtIn_settypeid(MinimalValue val, MinimalValue typeid) {
    if(val->typeobj != NULL) {
        Minimal_delReference(val->typeobj);
    }
    printf("%p %i\n", typeid, typeid->type);
    printf("%s\n", typeid->string);
    val->typeobj = Minimal_typeFromId(typeid->string);

    return val;
}

void Minimal_initBuiltInFuncs() {
    MinimalLayer layer = Minimal_defaultLayer;

    MinimalValue settypeid = Minimal_newReference(MINIMAL_VALUE, MinimalValue, struct MinimalValue_t);
    settypeid->type = M_BUILT_IN_FUNC;
    settypeid->typeobj = NULL;
    settypeid->built_in_name = "settypeid";
    settypeid->arg_count = 2;
    settypeid->func_ptr = (void*)&Minimal_builtIn_settypeid;

    Minimal_addName(layer->map, "settypeid", settypeid);
}
