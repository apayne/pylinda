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
    if(connected) {
        this->tsid = Linda_createTuplespace();
        this->initialised = true;
    } else {
        this->initialised = false;
    }
}

TupleSpace::TupleSpace(LindaValue id) {
    if(strcmp(Linda_getTupleSpace(id), "UTS") != 0) {
        std::cerr << "A TupleSpace ID can only be specified when creating the UTS" << std::endl;
        exit(-1);
    }
    this->tsid = id;
    this->initialised = true;
}

TupleSpace::~TupleSpace() {
    if(strcmp(Linda_getTupleSpace(this->tsid), "UTS")) {
        Linda_delTSReference(this->tsid);
    }
    Linda_delReference(this->tsid);
}

void TupleSpace::out(Tuple& t) {
    if(!this->initialised) {
        this->tsid = Linda_createTuplespace();
        this->initialised = true;
    }
    Linda_out(this->tsid, t.values);
}

Tuple TupleSpace::in(Tuple& t) {
    if(!this->initialised) {
        this->tsid = Linda_createTuplespace();
        this->initialised = true;
    }
    return Linda_in(this->tsid, t.values);
}

Tuple TupleSpace::rd(Tuple& t) {
    if(!this->initialised) {
        this->tsid = Linda_createTuplespace();
        this->initialised = true;
    }
    return Linda_rd(this->tsid, t.values);
}

Tuple* TupleSpace::inp(Tuple& t) {
    if(!this->initialised) {
        this->tsid = Linda_createTuplespace();
        this->initialised = true;
    }
    LindaValue rt = Linda_inp(this->tsid, t.values);
    if(rt == NULL) {
        return NULL;
    } else {
        Tuple* tp = new Tuple(rt);
        return tp;
    }
}

Tuple* TupleSpace::rdp(Tuple& t) {
    if(!this->initialised) {
        this->tsid = Linda_createTuplespace();
        this->initialised = true;
    }
    LindaValue rt = Linda_rdp(this->tsid, t.values);
    if(rt == NULL) {
        return NULL;
    } else {
        Tuple* tp = new Tuple(rt);
        return tp;
    }
}

int TupleSpace::collect(TupleSpace& ts, Tuple& t) {
    if(!this->initialised) {
        this->tsid = Linda_createTuplespace();
        this->initialised = true;
    }
    return Linda_collect(ts.tsid, this->tsid, t.values);
}

int TupleSpace::copy_collect(TupleSpace& ts, Tuple& t) {
    if(!this->initialised) {
        this->tsid = Linda_createTuplespace();
        this->initialised = true;
    }
    return Linda_copy_collect(ts.tsid, this->tsid, t.values);
}

TupleSpace& TupleSpace::operator=(const TupleSpace& ts) {
    if(ts.initialised) {
        Linda_addReference((const Linda_tuplespace)ts.tsid.c_str());
        if(this->initialised) {
            Linda_deleteReference((const Linda_tuplespace)this->tsid.c_str());
        }

        this->tsid = ts.tsid;
        this->initialised = true;
    } else {
        if(this->initialised) {
            Linda_deleteReference((const Linda_tuplespace)this->tsid.c_str());
        }

        this->tsid = "";
        this->initialised = false;
    }

    return *this;
}

}

