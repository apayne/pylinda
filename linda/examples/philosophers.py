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

import linda
linda.connect()

import random
import time

number = 5

def Philosopher(i):
    while 1:
        think(i)
        if random.random() < 0.3: # have they eaten enough?
            break
        ts._in(("room ticket", ))
        ts._in(("chopstick", i))
        ts._in(("chopstick", (i+1)%number))
        ts._out(("room ticket", ))
        eat(i)
        ts._out(("chopstick", i))
        ts._out(("chopstick", (i+1)%number))
    print "Philosopher %i leaving" % (i, )

def think(i):
    print "Philosopher %i thinking" % (i, )
    while random.random() < 0.6:
        time.sleep(1)

def eat(i):
    print "Philosopher %i eating" % (i, )
    while random.random() < 0.6:
        time.sleep(1)

# create the philosophers
import sys

if len(sys.argv) == 1:
    ts = linda.TupleSpace()

    import os
    for i in range(number):
        if os.fork() == 0:
            os.execlp("./philosophers.py", "./philosophers.py", str(i))
        linda.uts._out((ts, ))
        ts._out(("chopstick", i))

    ts._out(("room ticket", ))

else:
    ts = linda.uts._in((linda.TupleSpace, ))[0]

    Philosopher(int(sys.argv[1]))

