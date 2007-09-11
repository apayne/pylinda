#!/usr/bin/python

import linda

linda.connect()

t = linda.Type("a :: ((int * int) * bool);")
v = linda.Value(((1, 1), True), t)

linda.uts._out((v, ))

t2 = linda.Type("b :: int * int;")
t3 = linda.Type("c :: (b * bool);")

print linda.uts._in((t3,))
