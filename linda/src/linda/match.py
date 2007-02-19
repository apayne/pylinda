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

builtin = ["bool", "int8", "int16", "int32", "int64", "uint8", "uint16", "uint32", "uint64", "ieeesingle", "ieeedouble", "string"]

if _linda_server.use_types and _linda_server.register_types:
    from type_cache import lookupType

def identity(value):
    return value

def compare_notypes(t1, t2):
    assert isinstance(t1, _linda_server.Value) and t1.isType()
    assert isinstance(t1, _linda_server.Value) and t2.isType()

    assert t1.isId()
    assert t2.isId()

    return t1.id == t2.id

def compare_unregistered(t1, t2, checked=None):
    try:
        assert t1.isType()
        assert t2.isType()
    except:
        print "Error matching types", t1, t2
        raise

    func = None

    if checked is None:
        checked = {(t1, t2): func}
    elif (t1, t2) in checked:
        return lambda value: checked[(t1,t2)](value)
    else:
        checked[(t1, t2)] = func

    try:
        try:
            if t1.isNil() and t2.isNil():
                func = identity
            elif t1.isId() and t2.isId():
                if t1.id in builtin or t2.id in builtin:
                    if t1.id == t2.id:
                        func = identity
                    else:
                        func = None
                else:
                    try:
                        func = compare(t1.typemap[t1.id], t2.typemap[t2.id], checked)
                    except IndexError:
                        raise IndexError, "%s in %s, or %s in %s" % (t1.id, t1.typemap.keys(), t2.id, t2.typemap.keys())
            elif t1.isProductType() and t2.isProductType():
                if len(t1) != len(t2):
                    func = None
                else:
                    func = True
                    for i in range(len(t1)):
                        e1, e2 = t1[i], t2[i]
                        if not compare(e1, e2, checked):
                            func = None
                            break
                    if func:
                        def func(value):
                            l = []
                            for i in range(len(t1)):
                                l.append(checked[(t1[i],t2[i])](value[i]))
                            return tuple(l)
            elif t1.isSumType() and t2.isSumType():
                if len(t1) != len(t2):
                    func = None
                else:
                    map = [None for _ in range(len(t1))]
                    for i in range(len(t1)):
                        for j in range(len(t2)):
                            print "sum_type", map, i, j, len(t1), len(t2)
                            if j in map:
                                print "continue"
                                continue
                            print "compare"
                            f = compare(t1[i], t2[j], checked)
                            if f is not None:
                                print "got func"
                                map[i] = j
                        print "done j"
                        if map[i] is None:
                            func = None
                            break
                    print "done i"
                    if None not in map:
                        def func(value):
                            e1 = t1[value.sum_pos]
                            np = map[value.sum_pos]
                            e2 = t2[np]
                            v = checked[(e1, e2)](value)
                            v.sum_pos = np
                            return v
            elif t1.isPtrType() and t2.isPtrType():
                f = compare(t1.ptrtype, t2.ptrtype, checked)
                if f is None:
                    func = None
                else:
                    func = identity
            elif t1.isFunctionType() and t2.isFunctionType():
                arg_func = compare(t1.arg, t2.arg, checked)
                res_func = compare(t1.result, t2.result, checked)
                raise SystemError
            else:
                func = None
        except:
            print traceback.print_exc()
    else:
        checked[(t1, t2)] = func
        return func

def compare_registered(t1, t2, checked=None):
    assert t1.isType()
    assert t2.isType()

    func = None

    if checked is None:
        checked = {(t1, t2): func}
    elif (t1, t2) in checked:
        return lambda x: x
    else:
        checked[(t1, t2)] = func

    try:
        try:
            if t1.isNil() and t2.isNil():
                func = True
            elif t1.isId() and t2.isId():
                if t1.id in builtin or t2.id in builtin:
                    if t1.id == t2.id:
                        func = identity
                    else:
                        func = None
                else:
                    func = compare(lookupType(t1.id_type_id), lookupType(t2.id_type_id), checked)
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
                            v = []
                            for i in range(len(t1)):
                                v.append(l[i](x[i]))
                            return tuple(v)
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
    finally:
        checked[(t1, t2)] = func
        return func

if not _linda_server.use_types:
    compare = compare_notypes
elif _linda_server.register_types:
    compare = compare_registered
else:
    compare = compare_unregistered

from type_cache import lookupType
