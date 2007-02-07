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

#define MINIMAL_INTERNAL
#include "minimal.h"
#define FORCE_C
#include "minimal.h"

namespace Minimal {

Value::Value() {
    this->value = NULL;
}

Value::Value(int i) {
    this->value = Minimal_int(i);
}

Value::Value(bool b) {
    this->value = Minimal_bool(b);
}

void Value::setType(Value& type) {
    Minimal_setType(this->value, type.value);
}

void Value::setSumPos(int sumpos) {
    Minimal_setSumPos(this->value, sumpos);
}

void Value::set(int pos, Value obj) {
    obj.addReference();
    Minimal_tupleSet(this->value, pos, obj.value);
}

std::string Value::serialise(bool include_type) {
    char* cs = Minimal_serialise(this->value, include_type);
    std::string s = cs;
    free(cs);
    return s;
}

void Value::addReference() {
    Minimal_addReference(this->value);
}

void Value::delReference() {
    Minimal_delReference(this->value);
}

Value Value::operator[](int i) {
    return Minimal_tupleGet(this->value, i);
}

Value& Value::operator=(Value& other) {
    if(this->value != NULL) { this->delReference(); }
    this->value = other.value;
    this->addReference();
    return *this;
}

Value& Value::operator=(MinimalValue other) {
    if(this->value != NULL) { this->delReference(); }
    this->value = other;
    this->addReference();
    return *this;
}

Value& Value::operator=(int i) {
    if(this->value != NULL) { this->delReference(); }
    this->value = Minimal_int(i);
    return *this;
}

Value Type(std::string spec) {
    Value v;
    MinimalValue value = Minimal_type(spec.c_str());
    v = value;
    Minimal_delReference(value);
    return v;
}


Value Tuple(int size) {
    Value v;
    MinimalValue value = Minimal_tuple(size);
    v = value;
    Minimal_delReference(value);
    return v;
}

}
