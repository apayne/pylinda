#!/usr/bin/python

import linda

linda.connect()

t1 = linda.Type("t1 :: int * int * int;")
t2 = linda.Type("t2 :: int * (int * int);")

print len(t1), len(t2)

v = linda.Value((1, 2, 3), t1)

print v
linda.uts._out((v, ))

print linda.uts._in((t2, ))[0]
