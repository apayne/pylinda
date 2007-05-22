#!/usr/bin/python

import linda

linda.connect()

t1 = linda.Type("t1 :: int + string;")
t2 = linda.Type("t2 :: string + int;")

v = linda.Sum(1, 0, t1)

linda.uts._out((v, ))

r = linda.uts._rd((t1, ))

print r, r[0].sum_pos

r = linda.uts._rd((t2, ))

print r, rp[0].sum_pos
