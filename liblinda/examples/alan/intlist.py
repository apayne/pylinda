#!/usr/bin/python

import linda

linda.connect()

type = linda.Type("intlist :: Nil + (int * intlist);")
print type

value = linda.Value(None, type)
value.setSumPos(0)

linda.uts._out((value, ))

value = linda.Value(None)
value.setSumPos(0)
value = linda.Value((3, value))
value.setSumPos(1)
value = linda.Value((2, value))
value.setSumPos(1)
value = linda.Value((1, value), type)
value.setSumPos(1)
# end result is (1, (2, (3, None)))

linda.uts._out((value, ))

print linda.uts._in((type, ))

print linda.uts._in((type, ))
