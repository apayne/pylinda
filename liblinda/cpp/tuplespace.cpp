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

Tuple TupleSpace::inp(Tuple& t) {
    return Linda_inp((char*)this->tsid.c_str(), (struct Tuple_t*)t.values);
}

Tuple TupleSpace::rdp(Tuple& t) {
    return Linda_rdp((char*)this->tsid.c_str(), (struct Tuple_t*)t.values);
}

int TupleSpace::collect(TupleSpace& ts, Tuple& t) {
    return Linda_collect((const Linda_tuplespace)ts.tsid.c_str(), (char*)this->tsid.c_str(), (struct Tuple_t*)t.values);
}

int TupleSpace::copy_collect(TupleSpace& ts, Tuple& t) {
    return Linda_copy_collect((const Linda_tuplespace)ts.tsid.c_str(), (char*)this->tsid.c_str(), (struct Tuple_t*)t.values);
}

}

