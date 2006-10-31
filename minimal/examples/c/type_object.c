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
    Minimal_init();

    MinimalValue inttype = Minimal_type("inttype :: int;");
    MinimalValue pairtype = Minimal_type("pairtype :: int * int;");
    MinimalValue sumtype = Minimal_type("sumtype :: int + float;");
    MinimalValue intlisttype = Minimal_type("intlist :: Nil + (int * intlist);");

    MinimalValue intvalue = Minimal_int(1);
    Minimal_setType(intvalue, inttype);
    printf("%s\n---\n", Minimal_serialize(intvalue));

    MinimalValue pairvalue = Minimal_tuple(2);
    Minimal_tupleSet(pairvalue, 0, Minimal_int(1));
    Minimal_tupleSet(pairvalue, 1, Minimal_int(2));
    Minimal_setType(pairvalue, pairtype);
    printf("%s\n---\n", Minimal_serialize(pairvalue));

    MinimalValue sumvalue = Minimal_int(1);
    Minimal_setType(sumvalue, sumtype);
    printf("%s\n---\n", Minimal_serialize(sumvalue));

    MinimalValue intlistvalue = Minimal_tuple(2);
    Minimal_tupleSet(intlistvalue, 0, Minimal_int(1));
    Minimal_addReference(Minimal_Nil);
    Minimal_tupleSet(intlistvalue, 1, Minimal_Nil);
    Minimal_setType(intlistvalue, intlisttype);
    printf("%s\n---\n", Minimal_serialize(intlistvalue));

    return 0;
}
