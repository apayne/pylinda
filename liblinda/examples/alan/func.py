#!/usr/bin/python

import linda

linda.connect()

func = linda.Function("""
double :: int -> int;
double x = 2 * x;
""")
print func

linda.uts._out((func, ))
