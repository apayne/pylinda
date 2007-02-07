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

#include "minimal.h"

int main(int argc, char* argv[]) {
    char* buf;
	MinimalValue rectype;
	MinimalValue recvalue;

    Minimal_init();

    rectype = Minimal_type("rectype :: int * ptr(rectype);");

    recvalue = Minimal_tuple(2);
    Minimal_setType(recvalue, rectype);
    Minimal_tupleSet(recvalue, 0, Minimal_int(1));
    Minimal_tupleSet(recvalue, 1, Minimal_ptr(recvalue));
    buf = Minimal_serialise(recvalue, 1);
    printf("%s\n", buf);
    free(buf);
    Minimal_delReference(recvalue);
    Minimal_delReference(rectype);

    Minimal_finalise();

    return 0;
}
