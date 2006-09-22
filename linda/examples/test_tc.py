#!/usr/bin/python

import linda._tuplecontainer as tc

t = tc.TupleContainer()

t.add((1,))
t.add((1,))
print t.isEmpty()
t.delete((1,))
print t.isEmpty()
t.delete((1,))
print t.isEmpty()
