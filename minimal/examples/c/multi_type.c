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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "minimal.h"

int main(int argc, char* argv[]) {
    char* buf;

    Minimal_init();

    MinimalValue atype = Minimal_type("a :: int * ptr(b);");
    MinimalValue btype = Minimal_type("b :: float;");

    MinimalValue val2 = Minimal_float(1.0);
    Minimal_setType(val2, btype);

    MinimalValue val = Minimal_tuple(2);
    Minimal_tupleSet(val, 0, Minimal_int(1));
    Minimal_tupleSet(val, 1, Minimal_ptr(val2));
    Minimal_setType(val, atype);

    buf = Minimal_serialise(val, 1);
    printf("%s\n", buf);
    free(buf);

    Minimal_delReference(val);
    Minimal_delReference(val2);
    Minimal_delReference(atype);
    Minimal_delReference(btype);

    Minimal_finalise();

    return 0;
}
