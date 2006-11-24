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

    MinimalValue inttype = Minimal_type("inttype :: int;");
    MinimalValue pairtype = Minimal_type("pairtype :: int * int;");
    MinimalValue sumtype = Minimal_type("sumtype :: int + float;");
    MinimalValue intlisttype = Minimal_type("intlist :: Nil + (int * intlist);");

    if(strcmp(argv[1], "1") == 0) {
        MinimalValue intvalue = Minimal_int(1);
        Minimal_setType(intvalue, inttype);
        buf = Minimal_serialise(intvalue, 1);
        printf("%s\n", buf);
        free(buf);
        Minimal_delReference(intvalue);
    }

    if(strcmp(argv[1], "2") == 0) {
        MinimalValue pairvalue = Minimal_tuple(2);
        Minimal_tupleSet(pairvalue, 0, Minimal_int(1));
        Minimal_tupleSet(pairvalue, 1, Minimal_int(2));
        Minimal_setType(pairvalue, pairtype);
        buf = Minimal_serialise(pairvalue, 1);
        printf("%s\n", buf);
        free(buf);
        Minimal_delReference(pairvalue);
    }

    if(strcmp(argv[1], "3") == 0) {
        MinimalValue sumvalue = Minimal_int(1);
        Minimal_setType(sumvalue, sumtype);
        Minimal_setSumPos(sumvalue, 0);
        buf = Minimal_serialise(sumvalue, 1);
        printf("%s\n", buf);
        free(buf);
        Minimal_delReference(sumvalue);
    }

    if(strcmp(argv[1], "4") == 0) {
        MinimalValue intlistvalue = Minimal_tuple(2);
        Minimal_setSumPos(intlistvalue, 1);
        Minimal_tupleSet(intlistvalue, 0, Minimal_int(1));
        Minimal_addReference(Minimal_Nil);
        Minimal_tupleSet(intlistvalue, 1, Minimal_Nil);
        Minimal_setSumPos(Minimal_tupleGet(intlistvalue, 1), 0);
        Minimal_setType(intlistvalue, intlisttype);
        buf = Minimal_serialise(intlistvalue, 1);
        printf("%s\n", buf);
        free(buf);
        Minimal_delReference(intlistvalue);
    }

    Minimal_delReference(inttype);
    Minimal_delReference(pairtype);
    Minimal_delReference(sumtype);
    Minimal_delReference(intlisttype);

    Minimal_finalise();

    return 0;
}
