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

import copy

import _linda_server

def convertFrom_none(x): return x
def convertTo_none(x): return x

def typeEq(type1, type2, memo = None):
    if memo is None:
        memo = {}
    else:
        if (type1, type2) in memo:
            return True
    memo[(type1, type2)] = True

    if type1.type_name != type2.type_name:
        return False
    elif type1.isNil():
        if not type2.isNil():
            return False
        return True
    elif type1.isId():
        if not type2.isId():
            return False
        if type1.id in match.builtin:
            return type1.id == type2.id
        t = findType(type1.typemap[type1.id], memo)
        if t.type_id == type2.id_type_id:
            type1.id_type_id = t.type_id
            return True
        else:
            return False
    elif type1.isProductType():
        if not type2.isProductType():
            return False
        if len(type1) != len(type2):
            return False
        else:
            for i in range(len(type1)):
                if not typeEq(type1[i], type2[i], memo):
                    return False
            return True
    elif type1.isSumType():
        if not type2.isSumType():
            return False
        if len(type1) != len(type2):
            return False
        else:
            for i in range(len(type1)):
                if not typeEq(type1[i], type2[i], memo):
                    return False
            return True
    elif type1.isPtrType():
        if not type2.isPtrType():
            return False
        return False
    elif type1.isFunctionType():
        if not type2.isFunctionType():
            return False
        return False
    else:
        raise SystemError, (type1, type2)

def registerAllTypes(type, memo = None):
    if memo is None:
        memo = {}
    else:
        if type in memo:
            return memo[type]
    memo[type] = type_cache.registerType(type, "")

    if type.isNil():
        pass
    elif type.isId():
        if type.id in match.builtin:
            pass
        else:
            t = registerAllTypes(type.typemap[type.id], memo)
            type.id_type_id = t
    elif type.isProductType():
        for i in range(len(type)):
            registerAllTypes(type[i], memo)
    elif type.isSumType():
        for i in range(len(type)):
            registerAllTypes(type[i], memo)
    elif type.isPtrType():
        raise SystemError, type
    elif type.isFunctionType():
        raise SystemError, type
    else:
        raise SystemError, type

    type.type_id = memo[type]
    type_cache.updateType(type.type_id , type)
    return type.type_id

def findType(type, memo = None):
    type_cache.cache_lock.acquire()
    try:
        for t in type_cache.__cache.keys():
            if typeEq(type, type_cache.lookupType(t), memo):
                return _linda_server.TypeFromId(t)
    finally:
        type_cache.cache_lock.release()
    return _linda_server.TypeFromId(registerAllTypes(type))

def convertFrom_unregistered(value):
    if isinstance(value, tuple):
        return tuple([convertFrom_unregistered(v) for v in value])
    elif isinstance(value, _linda_server.Value):
        if value.type:
            value.type = findType(value.type)
        if value.isNil():
            return value
        elif value.isLong():
            return value
        elif value.isString():
            return value
        elif value.isTuple():
            return value
        elif value.isTupleSpace():
            return value
        elif value.isFunction():
            return value
        elif value.isType():
            return findType(value)
        else:
            raise SystemError, (value, type(value))
    else:
        return value

def convertTo_unregistered(value):
    if isinstance(value, tuple):
        value = copy.deepcopy(value)
        return tuple([convertTo_unregistered(v) for v in value])
    elif isinstance(value, _linda_server.Value):
        if value.type.type_id:
            value.type = type_cache.lookupType(value.type.type_id)
        if value.isNil():
            return value
        elif value.isLong():
            return value
        elif value.isString():
            return value
        elif value.isTuple():
            return value
        elif value.isTupleSpace():
            return value
        elif value.isFunction():
            return value
        elif value.isType():
            return type_cache.lookupType(value.type.type_id)
        else:
            raise SystemError, (value, type(value))
    else:
        return value

if not _linda_server.use_types:
    convertFrom = convertFrom_none
    convertTo = convertTo_none
elif _linda_server.register_types:
    convertFrom = convertFrom_none
    convertTo = convertTo_none
else:
    convertFrom = convertFrom_unregistered
    convertTo = convertTo_unregistered

from connections import Connection

import type_cache
import match
