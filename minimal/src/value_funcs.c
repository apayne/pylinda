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

unsigned char Minimal_Value_ge(MinimalValue op1, MinimalValue op2) {
    if(op1->type != op2->type) {
        return op1->type > op2->type;
    } else {
    switch(op1->type) {
    case M_NIL:
        return 0;
    case M_BOOLEAN:
        if(op1->boolean && !op2->boolean) {
            return 1;
        } else {
            return 0;
        }
    case M_BYTE:
    case M_SHORT:
    case M_INTEGER:
    case M_LONG:
        return op1->integer > op2->integer;
    case M_UBYTE:
    case M_USHORT:
    case M_UINTEGER:
    case M_ULONG:
        return op1->uinteger > op2->uinteger;
    case M_FLOAT:
        return op1->singlefloat > op2->singlefloat;
    case M_DOUBLE:
        return op1->doublefloat > op2->doublefloat;
    case M_STRING:
        if(strcmp(op1->string, op2->string) == 1) {
            return 1;
        } else {
            return 0;
        }
    case M_TYPE:
        return (unsigned long)op1 > (unsigned long)op2;
    case M_TSREF:
        if(strcmp(op1->tsid, op2->tsid) == 1) {
            return 1;
        } else {
            return 0;
        }
    case M_TUPLE:
        if(op1->size > op2->size) {
            return 1;
        } else if(op1->size < op2->size) {
            return 0;
        } else {
            int i;
            for(i = 0; i < op1->size; i++) {
                if(Minimal_Value_ge(op1->values[i], op2->values[i])) {
                    return 1;
                } else if(!Minimal_Value_eq(op1->values[i], op2->values[i])) {
                    return 0;
                }
            }
            return 0;
        }
    case M_SUM:
        return Minimal_Value_ge(op1->value, op2->value);
    case M_FUNCTION:
        return (unsigned long)op1 > (unsigned long)op2;
    case M_POINTER:
        return (unsigned long)op1->ptr > (unsigned long)op2->ptr;
    case M_SYNTAX_TREE:
        return (unsigned long)op1->syntax_tree > (unsigned long)op2->syntax_tree;
    }
    }
    return 0;
}

unsigned char Minimal_Value_eq(MinimalValue op1, MinimalValue op2) {
    if(op1->type != op2->type) {
        return 0;
    } else {
    switch(op1->type) {
    case M_NIL:
        return 0;
    case M_BOOLEAN:
        if(op1->boolean == op2->boolean) {
            return 1;
        } else {
            return 0;
        }
    case M_BYTE:
    case M_SHORT:
    case M_INTEGER:
    case M_LONG:
        return op1->integer == op2->integer;
    case M_UBYTE:
    case M_USHORT:
    case M_UINTEGER:
    case M_ULONG:
        return op1->uinteger == op2->uinteger;
    case M_FLOAT:
        return op1->singlefloat == op2->singlefloat;
    case M_DOUBLE:
        return op1->doublefloat ==op2->doublefloat;
    case M_STRING:
        if(strcmp(op1->string, op2->string) == 0) {
            return 1;
        } else {
            return 0;
        }
    case M_TYPE:
        return (unsigned long)op1 == (unsigned long)op2;
    case M_TSREF:
        if(strcmp(op1->tsid, op2->tsid) == 0) {
            return 1;
        } else {
            return 0;
        }
    case M_TUPLE:
        if(op1->size > op2->size) {
            return 0;
        } else if(op1->size < op2->size) {
            return 0;
        } else {
            int i;
            for(i = 0; i < op1->size; i++) {
                if(Minimal_Value_eq(op1->values[i], op2->values[i])) {
                    return 1;
                } else if(!Minimal_Value_eq(op1->values[i], op2->values[i])) {
                    return 0;
                }
            }
            return 0;
        }
    case M_SUM:
        return Minimal_Value_eq(op1->value, op2->value);
    case M_FUNCTION:
        return (unsigned long)op1 == (unsigned long)op2;
    case M_POINTER:
        return (unsigned long)op1->ptr == (unsigned long)op2->ptr;
    case M_SYNTAX_TREE:
        return (unsigned long)op1->syntax_tree == (unsigned long)op2->syntax_tree;
    }
    }
    return 0;
}
