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

#ifdef WIN32
#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)
#else
#define EXPORT
#define IMPORT
#endif

struct MinimalValue_t;
typedef struct MinimalValue_t* MinimalValue;

namespace Minimal {

EXPORT void init();
EXPORT void finalise();

class Value {
public:
    EXPORT Value();
    EXPORT Value(int i);
    EXPORT Value(bool b);

    EXPORT std::string serialise(bool include_type, bool include_type_spec);

    EXPORT void setType(Value& type);
    EXPORT void setSumPos(int pos);

    EXPORT void set(int pos, Value obj);

    EXPORT Value& operator=(Value& other);
    EXPORT Value& operator=(MinimalValue other);

    EXPORT Value& operator=(int i);

    EXPORT Value operator[](int index);

    EXPORT void addReference();
    EXPORT void delReference();

private:
    MinimalValue value;
};

EXPORT Value Type(std::string spec);
EXPORT Value Tuple(int size);

#ifdef MINIMAL_INTERNAL
EXPORT 
#else
IMPORT
#endif
extern Value Nil;

}

#endif
