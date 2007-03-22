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

def getTypesFromServer_registered(server, tup):
    l = []
    for v in tup:
        l += getTypesFromValue(v)

    for t in l:
        lookupForeignType(server, t)

    return tup

def getTypesFromValue(value):
    l = []
    if value.type.type_id != 0:
        l.append(value.type.type_id)

    if value.isType():
        l += getTypesFromType(value)
    #elif value.isNil():
    #    pass
    #elif value.isInt():
    #    pass
    elif value.isLong():
        pass
    elif value.isTuple():
        for i in range(len(value)):
            l += convertValueTypes(value[i])
    elif value.isPtr():
        l += getTypesFromValue(value.ptr)
    else:
        raise SystemError
    return l

def getTypesFromType(type):
    l = []
    if type.type_id != 0:
        l.append(type.type_id)

    if type.isNil():
        pass
    elif type.isId():
        if type.id in builtin:
            pass
        elif type.id_type_id == 0:
            pass
        else:
            l.append(type.id_type_id)
    elif type.isProductType():
        for i in range(len(type)):
            l += getTypesFromType(type[i])
    elif type.isSumType():
        for i in range(len(type)):
            l += getTypesFromType(type[i])
    elif type.isPtrType():
        l += getTypesFromType(type.ptrtype)
    elif type.isFunctionType():
        l += getTypesFromType(type.arg)
        l += getTypesFromType(type.result)

    return l

def getTypesFromServer_none(server, tup):
    return tup

if not _linda_server.use_types:
    getTypesFromServer = getTypesFromServer_none
elif _linda_server.register_types:
    getTypesFromServer = getTypesFromServer_registered
    from type_cache import getTypeReferences, lookupType, lookupForeignType
    from match import builtin
else:
    getTypesFromServer = getTypesFromServer_none
