#!/usr/bin/python

import linda

linda.connect()

type1 = linda.Type("intlist :: Nil + (int * floatlist);")
type2 = linda.Type("floatlist :: Nil + (float * intlist);")

value = linda.Value(None, type1)
value.setSumPos(0)

linda.uts._out((value, ))

value = linda.Value(None)
value.setSumPos(0)
value = linda.Value((3, value))
value.setSumPos(1)
value = linda.Value((2.0, value))
value.setSumPos(1)
value = linda.Value((1, value), type1)
value.setSumPos(1)
# end result is (1, (2, (3, None)))

linda.uts._out((value, ))

print linda.uts._in((type1, ))

print linda.uts._in((type1, ))
