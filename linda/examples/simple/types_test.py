#!/usr/bin/python

import linda

linda.connect()

t = linda.Type("a :: int;")
v = linda.Value(1, t)

linda.uts._out((v, ))

print linda.uts._in((t,))
