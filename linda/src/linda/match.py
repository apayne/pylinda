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
from wrap_function import wrap_function

builtin = ["type", "bool", "int8", "int16", "int32", "int64", "uint8", "uint16", "uint32", "uint64", "ieeesingle", "ieeedouble", "string"]

identity = lambda get, value: value

def all_perms(str):
    if len(str) <=1:
        yield str
    else:
        for perm in all_perms(str[1:]):
            for i in range(len(perm)+1):
                yield perm[:i] + str[0:1] + perm[i:]

def compare_notypes(t1, t2):
    assert isinstance(t1, _linda_server.Value) and t1.isType(), str(t1)
    assert isinstance(t2, _linda_server.Value) and t2.isType(), str(t2)

    assert t1.isId(), str(t1)
    assert t2.isId(), str(t2)

    return t1.id == t2.id

def compare_types(t1, t2, checked=None):
    assert t1.isType()
    assert t2.isType()

    if checked is None:
        checked = {(t1, t2): []}
    elif (t1, t2) in checked:
        return lambda x: convertValue(checked(t1, t2), x)
    else:
        checked[(t1, t2)] = []

    code = checked[(t1, t2)]

    print t1, t2, t1.isFunctionType(), t2.isFunctionType()

    if t1.isNil() and t2.isNil():
        code.append(("SETTYPEID", t2.type_id))
    elif t1.isId() and t2.isId():
        if t1.id in builtin and t2.id in builtin:
            if t1.id == t2.id:
                code.append(("SETTYPEID", t2.type_id))
                return lambda x: convertValue(checked[(t1, t2)], x)
            else:
                return None
        else:
            print t1.id, t2.id
            if t1.id not in builtin:
                nt = lookupType(t1.id_type_id)
                func = compare_types(nt, t2, checked)
                if func:
                    code.append(("EVAL", checked[(nt, t2)]))
                    return lambda x: convertValue(checked[(t1, t2)], x)
            if t2.id not in builtin:
                nt = lookupType(t2.id_type_id)
                func = compare_types(t1, nt, checked)
                if func:
                    code.append(("EVAL", checked[(t1, nt)]))
                    return lambda x: convertValue(checked[(t1, t2)], x)
            if t1.id not in builtin and t2.id not in builtin:
                nt1 = lookupType(t1.id_type_id)
                nt2 = lookupType(t2.id_type_id)
                func = compare_types(nt1, nt2, checked)
                if func:
                    code.append(("EVAL", checked[(nt1, nt2)]))
                    return lambda x: convertValue(checked[(t1, t2)], x)
                else:
                    return None
    elif t1.isProductType() and t2.isProductType():
        t1e = flattenProductType(t1)
        t2e = flattenProductType(t2)
        if len(t1e) != len(t2e):
            return None
        funcs = []
        for e1, e2 in zip(t1e, t2e):
            f = compare_types(e1, e2, checked)
            if f is None:
                return
            else:
                funcs.append(checked[(e1, e2)])
        code.append(("EXPLODE", t1))
        for i in range(len(funcs)):
            code.append(("EVAL", funcs[i]))
            code.append(("ROTATE", ))
        code.append(("CONSTRUCT", t2))
        code.append(("SETTYPEID", t2.type_id))
        return lambda x: convertValue(checked[(t1, t2)], x)
    elif t1.isSumType() and t2.isSumType():
        t1e = flattenSumType(t1)
        t2e = flattenSumType(t2)
        if len(t1e) != len(t2e):
            return None
        for perm in all_perms(range(len(t2))):
            funcs = []
            for i in range(len(perm)):
                f = compare_types(t1e[i][0], t2e[perm[i]][0], checked)
                if f is None:
                    break
                else:
                    funcs.append(checked[(t1e[i][0], t2e[perm[i]][0])])
                    for pos in t2e[perm[i]][1][::-1]:
                        funcs[-1].append(("CREATESUM", perm[i]))
            if len(funcs) == len(perm):
                code.append(("SUMCASE", funcs))
        code.append(("SETTYPEID", t2.type_id))
        return lambda x: convertValue(checked[(t1, t2)], x)
    elif t1.isPtrType() and t2.isPtrType():
        f = compare_types(t1.ptrtype, t2.ptrtype, checked)
        if f is None:
            return
        code.append(("PTRCONV", checked[(t1.ptrtype, t2.ptrtype)]))
        return lambda x: convertValue(checked[(t1, t2)], x)
    elif t1.isFunctionType() and t2.isFunctionType():
        print "got functions", t1, t2
        arg = compare_types(t1.arg, t2.arg, checked)
        if arg is None:
            return None
        result = compare_types(t2.result, t1.result, checked)
        if result is None:
            return None
        return lambda x: wrap_function(t2.arg, checked[(t1.arg, t2.arg)], t2.result, checked[(t2.result, t1.result)], x)

def flattenProductType(p, memo = None):
    if memo is None:
        memo = {}
    if p.type_id:
        memo[p.type_id] = True
    l = []
    for i in range(len(p)):
        e = p[i]
        if e.isProductType():
            l.extend(flattenProductType(e, memo))
        elif e.isId() and not e.id in builtin:
            t = lookupType(e.id_type_id)
            if t.id_type_id not in memo and t.isProductType():
                l.extend(flattenProductType(t, memo))
            else:
                l.append(t)
        else:
            l.append(t)
    return l

def flattenSumType(p, memo = None, history=None):
    if memo is None:
        memo = {}
    if history is None:
        history = []
    if p.type_id:
        memo[p.type_id] = True
    l = []
    for i in range(len(p)):
        e = p[i]
        if e.isSumType():
            history.append(i)
            l.extend(flattenSumType(e, memo, history))
            history.pop()
        elif e.isId() and not e.id in builtin:
            t = lookupType(e.id_type_id)
            if t.id_type_id not in memo and t.isSumType():
                history.append(i)
                l.extend(flattenSumType(t, memo))
                history.pop()
            else:
                l.append((t, history[:] + [i]))
        else:
            l.append((e, history[:] + [i]))
    return l

#def flattenProductValue(v, p):
    #nv = []
    #for i in range(len(p)):
        #e = p[i]
        #if e.isProductType():
            #nv.extend(flattenProductValue(v[i], e))
        #elif e.isId() and not e.id in builtin:
            #t = lookupType(e.id_type_id)
            #if t.isProductType():
                #nv.extend(flattenProductValue(v[i], t))
            #else:
                #nv.append(v[i])
        #else:
            #nv.append(v[i])
    #return nv

#def raiseProduct(v, p):
    #nv = []
    #for i in range(len(p)):
        #e = p[i]
        #if e.isProductType():
            #nv.append(raiseProduct(v, e))
        #elif e.isId() and not e.id in builtin:
            #t = lookupType(e.id_type_id)
            #if t.isProductType():
                #nv.append(raiseProduct(v, t))
            #else:
                #nv.append(v[0])
                #del v[0]
        #else:
            #nv.append(v[0])
            #del v[0]
    #return _linda_server.Value(tuple(nv))

if not _linda_server.use_types:
    compare = compare_notypes
else:
    compare = compare_types

from convertvalue import convertValue

if _linda_server.use_types:
    from type_cache import lookupType
