#!/usr/bin/python

import sys
import linda

linda.connect()

prime_ts = linda.TupleSpace()
linda.uts._out(("prime_ts", prime_ts))

work_ts = linda.TupleSpace()
for i in range(int(sys.argv[1])):
    linda.uts._out(("work_ts", work_ts))

work_ts._out((2, 0, 0))

for i in range(3, 1000):
    work_ts._out((i, 2, i-3))

while True:
    tup = prime_ts._inp((int, ))
    if tup is None:
        break
    print tup[0]
