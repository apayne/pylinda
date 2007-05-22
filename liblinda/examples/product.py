#!/usr/bin/python

import linda

linda.connect()

t1 = linda.Type("t1 :: int * (int * int);")
t2 = linda.Type("t2 :: (int * int) * int;")
t3 = linda.Type("t3 :: int * int * int;")

v = linda.Value((1, (2, 3)), t1)

linda.uts._out((v, ))

print linda.uts._rd((t1, ))

print linda.uts._rd((t2, ))

print linda.uts._in((t3, ))
