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

#ifndef __LINDA_CPP_H__

#define FORCE_C
#include "linda.h"
#undef FORCE_C

#include <string>
#include <vector>

namespace Linda {

class Tuple;
class TupleSpace;

bool connect(int port=Linda_port);
void disconnect();

extern bool connected;

class Value {
    friend class Tuple;
    friend class TupleSpace;
public:
    Value(bool b);
    Value(const int i);
    Value(float f);
    Value(std::string s);
    Value(char* s);
    Value(char* s, int len);
    Value(const Value& v);
    Value(const struct Value_t& t);

    ~Value();

    void makeStringIntoType();

    template<typename T> T get();
protected:
    struct Value_t* value;
};

class Tuple {
    friend class TupleSpace;
public:
    Tuple(int size=0);
    Tuple(const Tuple& t);
    Tuple(struct Tuple_t* t);
    ~Tuple();

    void add(Value& v);
    void set(int i, const Value& v);
    Value get(int i);
protected:
    struct Tuple_t* values;
};

class TupleSpace {
public:
    TupleSpace();
    TupleSpace(std::string tsid);
    TupleSpace(const TupleSpace& ts);
    ~TupleSpace();

    void out(Tuple& t);
    Tuple in(Tuple& t);
    Tuple rd(Tuple& t);
    Tuple* inp(Tuple& t);
    Tuple* rdp(Tuple& t);

    int collect(TupleSpace& ts, Tuple& t);
    int copy_collect(TupleSpace& ts, Tuple& t);

    TupleSpace& operator=(const TupleSpace& ts);

private:
    std::string tsid;
    bool initialised;
};

extern TupleSpace uts;

extern Value boolType;
extern Value intType;
extern Value floatType;
extern Value stringType;
extern Value tsType;
extern Value tupleType;
extern Value tuplespaceType;

template<> int Value::get();
template<> double Value::get();
template<> TupleSpace Value::get();

}

#endif
