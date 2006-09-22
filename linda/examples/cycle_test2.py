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

from linda import *
connect()

import sys
id = int(sys.argv[1])

if id == 1:
    ts1 = TupleSpace()
    uts._out((1,ts1))
    uts._out((1,ts1))
elif id == 2:
    ts2 = TupleSpace()
    uts._out((2,ts2))
    uts._out((2,ts2))
elif id == 3:
    ts3 = TupleSpace()
    uts._out((3,ts3))
    uts._out((3,ts3))

#print "output"
#raw_input()

if id != 1:
    ts1 = uts._in((1, TupleSpace))

if id != 2:
    ts2 = uts._in((2, TupleSpace))

if id != 3:
    ts3 = uts._in((3, TupleSpace))

#print "input"
#raw_input()

if id == 1:
    ts1._out((ts2, ))
elif id == 2:
    ts2._out((ts3, ))
elif id == 3:
    ts3._out((ts1, ))
