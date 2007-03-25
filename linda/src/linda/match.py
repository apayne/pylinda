#    Copyright 2004-2006 Andrew Wilkinson <aw@cs.york.ac.uk>
#
#    This file is part of PyLinda (http://www-users.cs.york.ac.uk/~aw/pylinda)
#
#    PyLinda is free software; you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation; either version 2.1 of the License, or
#    (at your option) any later version.
#
#    PyLinda is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with PyLinda; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

import traceback

import _linda_server

builtin = ["type", "bool", "int8", "int16", "int32", "int64", "uint8", "uint16", "uint32", "uint64", "ieeesingle", "ieeedouble", "string"]

identity = lambda value: value

def compare_notypes(t1, t2):
    assert isinstance(t1, _linda_server.Value) and t1.isType(), str(t1)
    assert isinstance(t2, _linda_server.Value) and t2.isType(), str(t2)

    assert t1.isId()
    assert t2.isId()

    return t1.id == t2.id

def compare_types(t1, t2, checked=None):
    assert t1.isType()
    assert t2.isType()

    func = None

    if checked is None:
        checked = {(t1, t2): func}
    elif (t1, t2) in checked:
        return lambda x: checked[(t1, t2)](x)
    else:
        checked[(t1, t2)] = func

    try:
        if t1.isNil() and t2.isNil():
            func = True
        elif t1.isId() and t2.isId():
            if t1.id in builtin or t2.id in builtin:
                if t1.id == t2.id:
                    def func(x):
                        x.type_id = t1.type_id
                        return x
                else:
                    func = None
            else:
                convfunc = compare(lookupType(t1.id_type_id), lookupType(t2.id_type_id), checked)
                def func(value):
                    v = convfunc(value)
                    v.type_id = t1.type_id
                    return v
        elif t1.isProductType() and t2.isProductType():
            if len(t1) != len(t2):
                func = None
            else:
                l = []
                for i in range(len(t1)):
                    e1, e2 = t1[i], t2[i]
                    x = compare(e1, e2, checked)
                    if x is None:
                        func = None
                        break
                    l.append(x)
                if len(l) == len(t1):
                    def func(x):
                        v = _linda_server.Value(tuple([l[i](x[i]) for i in range(len(t1))]))
                        v.type_id = t1.type_id
                        return v
        elif t1.isSumType() and t2.isSumType():
            raise NotImplementedError
        elif t1.isPtrType() and t2.isPtrType():
            func = compare(t1.ptrtype, t2.ptrtype, checked)
        elif t1.isFunctionType() and t2.isFunctionType():
            func = compare(t1.arg, t2.arg, checked) and compare(t1.result, t2.result, checked)
        else:
            func = None
    except:
        print traceback.print_exc()
    else:
        checked[(t1, t2)] = func
        return func

if not _linda_server.use_types:
    compare = compare_notypes
else:
    compare = compare_types

if _linda_server.use_types:
    from type_cache import lookupType
