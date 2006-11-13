/*
*    Copyright 2006 Andrew Wilkinson <aw@cs.york.ac.uk>.
*
*    This file is part of LibLinda (http://www-users.cs.york.ac.uk/~aw/pylinda)
*
*    LibLinda is free software; you can redistribute it and/or modify
*    it under the terms of the GNU Lesser General Public License as published by
*    the Free Software Foundation; either version 2.1 of the License, or
*    (at your option) any later version.
*
*    LibLinda is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with LibLinda; if not, write to the Free Software
*    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "linda.h"

#define FORCE_C
#include "linda.h"
#include "../src/linda_internal.h"

namespace Linda {

Value::Value(bool b) {
    this->value = Linda_bool(b);
}

Value::Value(int i) {
    this->value = Linda_int(i);
}

Value::Value(char* s) {
    this->value = Linda_string(s);
}

Value::Value(const LindaValue v) {
    this->value = Linda_copy(v);
}

Value::~Value() {
    Linda_delReference(this->value);
}

template<> int Value::get() {
    return Linda_getInt(this->value);
}

template<> double Value::get() {
    return (double)Linda_getFloat(this->value);
}
template<> TupleSpace Value::get() {
    return TupleSpace(Linda_copy(this->value));
}

Value boolType(Linda_boolType);
Value intType(Linda_intType);
Value floatType(Linda_floatType);
Value stringType(Linda_stringType);
Value tuplespaceType(Linda_tupleSpaceType);

}

