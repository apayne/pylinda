#!/usr/bin/python

import linda

linda.connect()

ptr1 = linda.Type("ptr1 :: int * ptr(ptr2);")
ptr2 = linda.Type("ptr2 :: int;")

value2 = linda.Value(1, ptr2)
value = linda.Value((1, linda.Ptr(value2)), ptr1)

linda.uts._out((value, ))

print linda.uts._in((value, ))
