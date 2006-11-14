#!/usr/bin/python

import linda

linda.connect()

type = linda.Type("inttype :: int;")

value = linda.Value(1, type)

linda.uts._out((value, ))

print linda.uts._rd((value, ))

print linda.uts._rd((type, ))
