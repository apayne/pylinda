#!/usr/bin/python

#    Copyright 2004 Andrew Wilkinson <aw@cs.york.ac.uk>.
#
#    This file is part of PyLinda (http://www-users.cs.york.ac.uk/~aw/pylinda)
#
#    PyLinda is free software; you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation; either version 2.1 of the License, or
#    (at your option) any later version.
#
#    PyLinda is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with PyLinda; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

import linda.tuplecontainer as pytc
import linda._tuplecontainer as ctc

import random
import time
from linda.profile import P

def addsamelength(code, mod, count):
    t = mod.TupleContainer()
    for i in range(count):
        tup = tuple([random.randint(0,100) for x in range(1,5)])
        P.enter(code)
        t.add(tup)
        P._exit()

def addrandomlength(code, mod, count):
    t = mod.TupleContainer()
    for i in range(count):
        tup = tuple([random.randint(0,100) for x in range(1,random.randint(2,10))])
        P.enter(code)
        t.add(tup)
        P._exit()

def adddeletesamelength(code, mod, count):
    t = mod.TupleContainer()
    for i in range(count):
        tup = tuple([random.randint(0,100) for x in range(1,5)])
        P.enter(code)
        t.add(tup)
        t.delete(tup)
        P._exit()

def adddeleterandomlength(code, mod, count):
    t = mod.TupleContainer()
    for i in range(count):
        tup = tuple([random.randint(0,100) for x in range(1,random.randint(2,10))])
        P.enter(code)
        t.add(tup)
        t.delete(tup)
        P._exit()

print "Doing Python test 1..."
addsamelength("addsamelength_py", pytc, 10000)

print "Doing C test 1..."
addsamelength("addsamelength_c", ctc, 10000)

print "Doing Python test 2..."
addrandomlength("addrandomlength_py", pytc, 10000)

print "Doing C test 2..."
addrandomlength("addrandomlength_c", ctc, 10000)

print "Doing Python test 3..."
adddeletesamelength("adddeletesamelength_py", pytc, 10000)

print "Doing C test 3..."
adddeletesamelength("adddeletesamelength_c", ctc, 10000)

print "Doing Python test 4..."
adddeleterandomlength("adddeleterandomlength_py", pytc, 10000)

print "Doing C test 4..."
adddeleterandomlength("adddeleterandomlength_c", ctc, 10000)
