#!/usr/bin/python

import linda
linda.connect()

data_in = 100000000*"a"

ts = linda.TupleSpace()
linda.uts._out(("test", ts))

ts._out(("big_string", data_in))

data_out = ts._in(("big_string", str))[1]

print data_out == data_in
