#!/usr/bin/python

import linda
linda.connect()

class Test:
    def __init__(self):
        self.other = None

    def __str__(self):
        return "<Test %s>" % (str(self.other is self))

def convertTo(val, memo):
    v = linda.Ptr(None, type)
    memo.register(val, v)
    v.ptr = memo.getValue(val.other)
    return v

def convertFrom(val, memo):
    t = Test()
    memo.register(val, t)
    t.other = val.ptr
    return t

type = linda.Type("Test :: ptr(Test);", Test, convertTo, convertFrom)

v = Test()
v.other = v
linda.uts._out((v, ))

tup = linda.uts._in((Test, ))
print tup
print id(tup[0])
print id(tup[0].other)
