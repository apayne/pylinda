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
import math

import sys

linda.connect()

LIMIT = 10000
GRAIN = 250

def is_prime():
        primes = {}
        while 1:
            t = ts._in(("next task", int))
            start = t[1]
            if start >= LIMIT:
	        ts._out(("next task", start))
	        return 0
            print start
	    ts._out(("next task", start + GRAIN))

	    for me in range(start, start + GRAIN):
	        i = 2
		while i <= math.sqrt(me):
	            if not primes.has_key(i):
                        ok = ts._rd(("prime", i, bool))[2]
                        primes[i] = ok
                    else:
                        ok = primes[i]
		    if ok and (me % i == 0):
		        ts._out(("prime", me, False))
			break
	            i = i + 1
                if i > math.sqrt(me):
		    ts._out(("prime", me, True))


import sys
def main():
    ts._out(("next task", 1))

    ts._rd(("next task", LIMIT+1 ))

    linda.universe._in(("ts", linda.TupleSpace))
    print "finished"

    linda.kernel.message_noreturn("kill_server")

if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "master":
        ts = linda.TupleSpace()
        linda.universe._out(("ts", ts))
    else:
        ts = linda.universe._rd(("ts", linda.TupleSpace))[1]

    print ts

    if len(sys.argv) > 1 and sys.argv[1] == "master":
        main()
    else:
        is_prime()
