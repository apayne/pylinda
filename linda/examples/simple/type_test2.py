#!/usr/bin/python

import linda
linda.connect()

class IntPair:
    def __init__(self, a, b):
       self.a = a
       self.b = b

def IntPairToValue(pair, memo):
    return linda.Value((pair.a, pair.b))

def ValueToIntPair(v, memo):
    pair = IntPair(int(v[0]), int(v[1]))
    memo.register(v, pair)
    return pair

t = linda.Type("intpair :: (long * long);", IntPair, IntPairToValue, 
ValueToIntPair)

linda.uts._out((IntPair(1, 2), ))

print linda.uts._in((IntPair, ))
