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

#include <string>

#ifndef MINIMAL_CPP_H
#define MINIMAL_CPP_H

struct MinimalValue_t;
typedef struct MinimalValue_t* MinimalValue;

namespace Minimal {

void init();
void finalise();

class Value {
public:
    Value();
    Value(int i);
    Value(bool b);

    std::string serialise();

    void setType(Value& type);
    void setSumPos(int pos);

    void set(int pos, Value obj);

    Value& operator=(Value& other);
    Value& operator=(MinimalValue other);

    Value& operator=(int i);

    Value operator[](int index);

    void addReference();
    void delReference();

private:
    MinimalValue value;
};

Value Type(std::string spec);
Value Tuple(int size);

extern Value Nil;

}

#endif
