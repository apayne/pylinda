#!/usr/bin/python

import linda
linda.connect()

t = linda.Type("a :: (long * long);")
v = linda.Value((1, 2), t)

linda.uts._out((v, ))

print linda.uts._in((t, ))
