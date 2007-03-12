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

def convertTupleForServer_registered(server, tup):
    tup = copy.deepcopy(tup)
    return tuple([convertValueTypes(server, v) for v in tup])

def convertValueTypes(nid, value):
    if value.type.type_id != 0:
        value.type.type_id = getServerIds(value.type.type_id, nid)

    if value.isType():
        convertType(nid, value)
    #elif value.isNil():
    #    pass
    #elif value.isInt():
    #    pass
    elif value.isLong():
        pass
    elif value.isTuple():
        for i in range(len(value)):
            convertValueTypes(nid, value[i])
    elif value.isPtr():
        convertValueTypes(nid, value.ptr)
    else:
        raise SystemError
    return value

def convertType(nid, type):
    if type.type_id != 0:
        type.type_id = getServerIds(type.type_id, nid)

    if type.isNil():
        pass
    elif type.isId():
        if type.id in builtin:
            pass
        elif type.id_type_id == 0:
            pass
        else:
            type.id_type_id = getServerIds(type.id_type_id, nid)
    elif type.isProductType():
        for i in range(len(type)):
            convertType(nid, type[i])
    elif type.isSumType():
        for i in range(len(type)):
            convertType(nid, type[i])
    elif type.isPtrType():
        convertType(nid, type.ptrtype)
    elif type.isFunctionType():
        convertType(nid, type.arg)
        convertType(nid, type.result)

    return type

def convertTupleForServer_none(server, tup):
    return tup

if not _linda_server.use_types:
    convertTupleForServer = convertTupleForServer_none
elif _linda_server.register_types:
    convertTupleForServer = convertTupleForServer_registered
    from type_cache import getServerIds, getTypeReferences, setServerIds, lookupType
    from match import builtin
else:
    convertTupleForServer = convertTupleForServer_none
