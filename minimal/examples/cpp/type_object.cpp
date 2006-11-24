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

#include <iostream>
#include <string>

#include "minimal.h"

int main(int argc, char* argv[]) {
    std::string buf;

    Minimal::init();

    Minimal::Value inttype = Minimal::Type("inttype :: int;");
    Minimal::Value pairtype = Minimal::Type("pairtype :: int * int;");
    Minimal::Value sumtype = Minimal::Type("sumtype :: int + float;");
    Minimal::Value intlisttype = Minimal::Type("intlist :: Nil + (int * intlist);");

    Minimal::Value intvalue = 1;
    intvalue.setType(inttype);
    buf = intvalue.serialise(true);
    std::cout << buf << std::endl << "---" << std::endl;

    Minimal::Value pairvalue = Minimal::Tuple(2);
    pairvalue.set(0, 1);
    pairvalue.set(1, 2);
    pairvalue.setType(pairtype);
    buf = pairvalue.serialise(true);
    std::cout << buf << std::endl << "---" << std::endl;

    Minimal::Value sumvalue = 1;
    sumvalue.setType(sumtype);
    sumvalue.setSumPos(0);
    buf = sumvalue.serialise(true);
    std::cout << buf << std::endl << "---" << std::endl;

    Minimal::Value intlistvalue = Minimal::Tuple(2);
    intlistvalue.setSumPos(1);
    intlistvalue.set(0, 1);
    Minimal::Nil.addReference();
    intlistvalue.set(1, Minimal::Nil);
    intlistvalue[1].setSumPos(0);
    intlistvalue.setType(intlisttype);
    buf = intlistvalue.serialise(true);
    std::cout << buf << std::endl << "---" << std::endl;

    Minimal::finalise();

    return 0;
}
