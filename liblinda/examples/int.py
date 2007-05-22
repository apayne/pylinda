#!/usr/bin/python

import linda

linda.connect()

linda.uts._out((1,))

print linda.uts._rd((int, ))

t = linda.Type("t :: long;")

print linda.uts._in((t, ))
