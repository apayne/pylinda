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

#include "libxml/parser.h"

#include "minimal_internal.h"

MinimalValue Minimal_typeType;
MinimalValue Minimal_nilType;
MinimalValue Minimal_boolType;
MinimalValue Minimal_byteType;
MinimalValue Minimal_shortType;
MinimalValue Minimal_intType;
MinimalValue Minimal_longType;
MinimalValue Minimal_ubyteType;
MinimalValue Minimal_ushortType;
MinimalValue Minimal_uintType;
MinimalValue Minimal_ulongType;
MinimalValue Minimal_floatType;
MinimalValue Minimal_doubleType;
MinimalValue Minimal_stringType;
MinimalValue Minimal_tupleSpaceType;

unsigned char Minimal_use_types = 1;

#define createIntType(var, type, size8, size16, size32, size64) \
    if(sizeof(type) == 1) { \
        var = Minimal_type(size8); \
    } else if(sizeof(type) == 2) { \
        var = Minimal_type(size16); \
    } else if(sizeof(type) == 4) { \
        var = Minimal_type(size32); \
    } else if(sizeof(type) == 8) { \
        var = Minimal_type(size64); \
    } else { \
        fprintf(stderr, "Error, this machine has an unrecognised byte size (%s = %i).\n", size8, (int)sizeof(type)); \
    }

void Minimal_init() {
    Minimal_Layer_init();

    Minimal_typeType = Minimal_type("typetype :: type;");
    Minimal_addReference(Minimal_typeType);
    Minimal_typeType->typeobj = Minimal_typeType;
    Minimal_nilType = Minimal_type("niltype :: Nil;");
    Minimal_boolType = Minimal_type("booltype :: bool;");
    createIntType(Minimal_byteType, char, "byte :: int8;", "byte :: int16;", "byte :: int32;", "byte :: int64;")
    createIntType(Minimal_shortType, short, "short :: int8;", "short :: int16;", "short :: int32;", "short :: int64;")
    createIntType(Minimal_intType, int, "int :: int8;", "int :: int16;", "int :: int32;", "int :: int64;")
    createIntType(Minimal_longType, long, "long :: int8;", "long :: int16;", "long :: int32;", "long :: int64;")
    createIntType(Minimal_ubyteType, unsigned char, "ubyte :: uint8;", "ubyte :: uint16;", "ubyte :: uint32;", "ubyte :: uint64;")
    createIntType(Minimal_ushortType, unsigned short, "ushort :: uint8;", "ushort :: uint16;", "ushort :: uint32;", "ushort :: uint64;")
    createIntType(Minimal_uintType, unsigned int, "uint :: uint8;", "uint :: uint16;", "uint :: uint32;", "uint :: uint64;")
    createIntType(Minimal_ulongType, unsigned long, "ulong :: uint8;", "ulong :: uint16;", "ulong :: uint32;", "ulong :: uint64;")
    Minimal_floatType = Minimal_type("float :: ieeesingle;");
    Minimal_doubleType = Minimal_type("double :: ieeedouble;");
    Minimal_stringType = Minimal_type("stringtype :: string;");
    Minimal_tupleSpaceType = Minimal_type("tupleSpacetype :: tuplespace;");

    Minimal_Nil = Minimal_nil();

    Minimal_initBuiltInFuncs();
}

void Minimal_finalise() {
    Minimal_delReference(Minimal_Nil); Minimal_Nil = NULL;

    Minimal_delReference(Minimal_typeType); Minimal_typeType = NULL;
    Minimal_delReference(Minimal_nilType); Minimal_nilType = NULL;
    Minimal_delReference(Minimal_boolType); Minimal_boolType = NULL;
    Minimal_delReference(Minimal_byteType); Minimal_byteType = NULL;
    Minimal_delReference(Minimal_shortType); Minimal_shortType = NULL;
    Minimal_delReference(Minimal_intType); Minimal_intType = NULL;
    Minimal_delReference(Minimal_longType); Minimal_longType = NULL;
    Minimal_delReference(Minimal_ubyteType); Minimal_ubyteType = NULL;
    Minimal_delReference(Minimal_ushortType); Minimal_ushortType = NULL;
    Minimal_delReference(Minimal_uintType); Minimal_uintType = NULL;
    Minimal_delReference(Minimal_ulongType); Minimal_ulongType = NULL;
    Minimal_delReference(Minimal_floatType); Minimal_floatType = NULL;
    Minimal_delReference(Minimal_doubleType); Minimal_doubleType = NULL;
    Minimal_delReference(Minimal_stringType); Minimal_stringType = NULL;
    Minimal_delReference(Minimal_tupleSpaceType); Minimal_tupleSpaceType = NULL;

    Minimal_Layer_finalise();

    xmlCleanupParser();
}
