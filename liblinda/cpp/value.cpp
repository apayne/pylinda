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
    this->value = new struct Value_t;
    *(this->value) = Value_bool(b);
}

Value::Value(int i) {
    this->value = new struct Value_t;
    *(this->value) = Value_int(i);
}

Value::Value(char* s) {
    this->value = new struct Value_t;
    *(this->value) = Value_string(s);
}

Value::Value(const Value& v) {
    this->value = Value_copyptr((struct Value_t*)v.value);
}

Value::Value(const struct Value_t& v) {
    this->value = Value_copyptr(&v);
}

Value::~Value() {
    Value_free((struct Value_t*)this->value);
}

template<> int Value::get() {
    return Value_get_int(this->value);
}
template<> double Value::get() {
    return (double)Value_get_float(this->value);
}
template<> TupleSpace Value::get() {
    return TupleSpace(Value_get_tsref(this->value));
}

Value boolType(Value_boolType);
Value intType(Value_intType);
Value floatType(Value_floatType);
Value stringType(Value_stringType);
Value tuplespaceType(Value_tuplespaceType);

}

