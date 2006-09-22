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

ts1 = TupleSpace()
ts2 = TupleSpace()
ts1._out((ts2,))
ts2._out((ts1,))
del ts1
del ts2

raw_input()

ts1 = TupleSpace()
ts2 = TupleSpace()
ts3 = TupleSpace()
ts4 = TupleSpace()
ts1._out((ts2, ))
ts2._out((ts3, ))
ts2._out((ts4, ))
ts3._out((ts1, ))
ts3._out((ts4, ))
ts4._out((ts1, ))

del ts1
del ts2
del ts3
del ts4

raw_input()

tss = []
for i in range(10):
    tss.append(TupleSpace())

for a in tss:
   for b in tss:
        a._out((b,))
