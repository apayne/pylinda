#!/usr/bin/python

import linda

linda.connect()

linda.uts._out((1, ))

for i in range(2):
    print "in"
    t = linda.uts._in((int, ))

    linda.uts._out((t[0], ))

t = None
