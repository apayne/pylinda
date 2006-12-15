#!/usr/bin/python

import linda

linda.connect()

t = linda.Type("tpy :: int + string;")

v = linda.Value(1, t)
v.sum_pos = 0

linda.uts._out(("py", v))

v = linda.Value("A", t)
v.sum_pos = 1

linda.uts._out(("py", v))

v = linda.Value(2, t)
v.sum_pos = 0

linda.uts._out(("py", v))

v = linda.Value("B", t)
v.sum_pos = 1

linda.uts._out(("py", v))

for i in range(4):
    tup = linda.uts._in(("c", t))
    v = tup[1]

    if v.sum_pos == 0:
        print "int", v
    else:
        print "string", v
