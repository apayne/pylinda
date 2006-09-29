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

Value::Value(int i) {
    this->value = malloc(sizeof(struct Value_t));
    *((struct Value_t*)this->value) = Value_int(i);
}

Value::Value(const Value& v) {
    this->value = Value_copyptr((struct Value_t*)v.value);
}

Value::~Value() {
    Value_free((struct Value_t*)this->value);
}

}

