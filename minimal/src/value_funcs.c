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

MinimalValue Minimal_Value_add(MinimalValue op1, MinimalValue op2) {
    if(Minimal_isInt(op1) && Minimal_isInt(op2)) {
        return Minimal_int(Minimal_getInt(op1) + Minimal_getInt(op2));
    } else if(Minimal_isFloat(op1) && Minimal_isFloat(op2)) {
        return Minimal_float(Minimal_getFloat(op1) + Minimal_getFloat(op2));
    } else if(Minimal_isString(op1) && Minimal_isString(op2)) {
        char* r = (char*)malloc(Minimal_getStringLen(op1) + Minimal_getStringLen(op2) + 1);
        memcpy(r, Minimal_getString(op1), Minimal_getStringLen(op1));
        memcpy(&(r[Minimal_getStringLen(op1)+1]), Minimal_getString(op2), Minimal_getStringLen(op2));
        r[Minimal_getStringLen(op1) + Minimal_getStringLen(op2)] = '\0';
        return Minimal_string2(r, Minimal_getStringLen(op1) + Minimal_getStringLen(op2));
    } else {
        fprintf(stderr, "Error: Incompatible types for add %i and %i.\n", op1->type, op2->type);
        return NULL;
    }
}

MinimalValue Minimal_Value_sub(MinimalValue op1, MinimalValue op2) {
    if(Minimal_isInt(op1) && Minimal_isInt(op2)) {
        return Minimal_int(Minimal_getInt(op1) - Minimal_getInt(op2));
    } else if(Minimal_isFloat(op1) && Minimal_isFloat(op2)) {
        return Minimal_float(Minimal_getFloat(op1) - Minimal_getFloat(op2));
    } else {
        fprintf(stderr, "Error: Incompatible types for add %i and %i.\n", op1->type, op2->type);
        return NULL;
    }
}

MinimalValue Minimal_Value_mul(MinimalValue op1, MinimalValue op2) {
    if(Minimal_isInt(op1) && Minimal_isInt(op2)) {
        return Minimal_int(Minimal_getInt(op1) * Minimal_getInt(op2));
    } else if(Minimal_isFloat(op1) && Minimal_isFloat(op2)) {
        return Minimal_float(Minimal_getFloat(op1) * Minimal_getFloat(op2));
    } else {
        fprintf(stderr, "Error: Incompatible types for add %i and %i.\n", op1->type, op2->type);
        return NULL;
    }
}

MinimalValue Minimal_Value_div(MinimalValue op1, MinimalValue op2) {
    if(Minimal_isInt(op1) && Minimal_isInt(op2)) {
        return Minimal_int(Minimal_getInt(op1) / Minimal_getInt(op2));
    } else if(Minimal_isFloat(op1) && Minimal_isFloat(op2)) {
        return Minimal_float(Minimal_getFloat(op1) / Minimal_getFloat(op2));
    } else {
        fprintf(stderr, "Error: Incompatible types for add %i and %i.\n", op1->type, op2->type);
        return NULL;
    }
}
