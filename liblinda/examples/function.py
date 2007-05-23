#!/usr/bin/python

import linda

linda.connect()

f = linda.Function("""
double :: int -> int;
double x = 2 * x;
""")

print f(2)

linda.uts._out((f, ))

(nf, ) = linda.uts._rd((f.type, ))

print nf(4)

(nf, ) = linda.uts._in((f, ))

print "func2"
f2 = linda.Function("""
double2 :: int * 1 -> int * 1;
double2 x = 2 * x[0];
""")

print f2((6, ))

linda.uts._out((f2, ))

(nf2, ) = linda.uts._rd((f2.type, ))
print nf2((8, ))

t = linda.Type("f2 :: int * (int * 0) -> int * (int * 0);")
nf2 = linda.uts._in((t, ))
print nf2((10, ()))
