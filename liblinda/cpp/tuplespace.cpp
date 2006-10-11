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

#include <iostream>

#include "linda.h"

#define FORCE_C
#include "linda.h"
#include "../src/linda_internal.h"

namespace Linda {

TupleSpace::TupleSpace() {
    this->tsid = Linda_createTuplespace();
}

TupleSpace::TupleSpace(std::string id) {
    if(id != "UTS") {
        std::cerr << "A TupleSpace ID can only be specified when creating the UTS" << std::endl;
        exit(-1);
    }
    this->tsid = id;
}

TupleSpace::~TupleSpace() {
    if(this->tsid != "UTS") {
        Linda_deleteReference((const Linda_tuplespace)this->tsid.c_str());
    }
    this->tsid.erase();
}

void TupleSpace::out(Tuple& t) {
    Linda_out((char*)this->tsid.c_str(), (struct Tuple_t*)t.values);
}

Tuple TupleSpace::in(Tuple& t) {
    return Linda_in((const Linda_tuplespace)this->tsid.c_str(), (struct Tuple_t*)t.values);
}

Tuple TupleSpace::rd(Tuple& t) {
    return Linda_rd((char*)this->tsid.c_str(), (struct Tuple_t*)t.values);
}

Tuple* TupleSpace::inp(Tuple& t) {
    struct Tuple_t* rt = Linda_inp((char*)this->tsid.c_str(), (struct Tuple_t*)t.values);
    if(rt == NULL) {
        return NULL;
    } else {
        Tuple* tp = new Tuple(rt);
        Tuple_free(rt);
        return tp;
    }
}

Tuple* TupleSpace::rdp(Tuple& t) {
    struct Tuple_t* rt = Linda_rdp((char*)this->tsid.c_str(), (struct Tuple_t*)t.values);
    if(rt == NULL) {
        return NULL;
    } else {
        Tuple* tp = new Tuple(rt);
        Tuple_free(rt);
        return tp;
    }
}

int TupleSpace::collect(TupleSpace& ts, Tuple& t) {
    return Linda_collect((const Linda_tuplespace)ts.tsid.c_str(), (char*)this->tsid.c_str(), (struct Tuple_t*)t.values);
}

int TupleSpace::copy_collect(TupleSpace& ts, Tuple& t) {
    return Linda_copy_collect((const Linda_tuplespace)ts.tsid.c_str(), (char*)this->tsid.c_str(), (struct Tuple_t*)t.values);
}

TupleSpace& TupleSpace::operator=(const TupleSpace& ts) {
    Linda_addReference((const Linda_tuplespace)ts.tsid.c_str());
    Linda_deleteReference((const Linda_tuplespace)this->tsid.c_str());

    this->tsid = ts.tsid;

    return *this;
}

}

