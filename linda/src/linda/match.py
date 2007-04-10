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

identity = lambda get, value: value

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
        return lambda get, x: get(t1, t2, x)
    else:
        checked[(t1, t2)] = func

    try:
        if t1.isNil() and t2.isNil():
            func = identity
        elif t1.isId() and t2.isId():
            if t1.id in builtin or t2.id in builtin:
                if t1.id == t2.id:
                    def func(get, x):
                        x.type_id = t1.type_id
                        return x
                else:
                    func = None
            else:
                convfunc = compare(lookupType(t1.id_type_id), lookupType(t2.id_type_id), checked)
                def func(get, value):
                    v = get(lookupType(t1.id_type_id), lookupType(t2.id_type_id), value)
                    if t1.type_id is not None:
                        v.type_id = t1.type_id
                    return v
        #elif t1.isId(): # t2 is something else
        #    compare_types(lookupType(t1.id_type_id), t2)
        #elif t2.isId(): # t1 is something else
        #    compare_types(t1, lookupType(t2.id_type_id))
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
                    l.append((t1[i], t2[i]))
                if len(l) == len(t1):
                    def func(get, x):
                        v = _linda_server.Value(tuple([get(l[i][0], l[i][1], x[i]) for i in range(len(t1))]))
                        v.type_id = t1.type_id
                        return v
        elif t1.isSumType() and t2.isSumType():
            raise NotImplementedError
        elif t1.isPtrType() and t2.isPtrType():
            t1ptr = lookupType(t1.id_type_id)
            t2ptr = lookupType(t2.id_type_id)
            ptrfunc = compare(t1ptr, t2ptr, checked)
            if ptrfunc is not None:
               def func(get, x):
                    if x.isNil():
                        return x
                    else:
                        nv = get(t1ptr, t2ptr, x.ptr)
                        ptr = _linda_server.Ptr(nv)
                        if nv is None:
                            get((t1ptr, t2ptr, ptr), None, x.ptr)
                        return ptr
            else:
                func = None
        elif t1.isFunctionType() and t2.isFunctionType():
            func = compare(t1.arg, t2.arg, checked) and compare(t1.result, t2.result, checked)
        else:
            func = None
    except:
        print traceback.print_exc()
    else:
        checked[(t1, t2)] = func
        def get(t1, t2, value, memo, waiting, first=False):
            if t2 is None:
                waiting[value] = (t1[0], t1[1], [t1[2]])
                return None
            if (t1, t2, value) in memo:
                v = memo[(t1, t2, value)]
            else:
                memo[(t1, t2, value)] = None
                v = checked[(t1, t2)](lambda x, y, z: get(x, y, z, memo, waiting), value)
                if value in waiting:
                    for p in waiting[value][2]:
                        p.ptr = v
                    del waiting[value]
                memo[(t1, t2, value)] = v
            if first:
                while len(waiting) > 0:
                    newvalue = waiting.keys()[0]
                    t1, t2, ptrs = waiting[newvalue]
                    del waiting[newvalue]
                    ptrval = get(t1, t2, newvalue, memo, waiting)
                    for w in ptrs:
                        w.ptr = ptrval
            return v
        return get

if not _linda_server.use_types:
    compare = compare_notypes
else:
    compare = compare_types

if _linda_server.use_types:
    from type_cache import lookupType
