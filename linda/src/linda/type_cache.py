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

from threading import RLock

from connections import sendMessageToNode
from guid import generate
from utils import isNodeId

cache_lock = RLock()

__cache = {}

def registerType(type, pid):
    assert type.isType()

    cache_lock.acquire()
    try:
        id = "T"+generate()
        if isNodeId(pid):
            __cache[id] = [type, None, __getTypeReferences(type)]
        else:
            __cache[id] = [type, pid, __getTypeReferences(type)]

        return id
    finally:
        cache_lock.release()

def updateType(type_id, type):
    assert type.isType()

    cache_lock.acquire()
    try:
        __cache[type_id][0] = type
        __cache[type_id][2] = __getTypeReferences(type)
    finally:
        cache_lock.release()

def emptyTypeCache():
    __cache = None

def lookupType(id):
    try:
        return __cache[id][0]
    except KeyError:
        raise KeyError, "Type %s not found (%s)" % (id, __cache.keys())

def getTypeReferences(id):
    return __cache[id][2]

def __getTypeReferences(type, refs=None):
    if refs is None:
        refs = [type.type_id]
    if type.isNil():
        return refs
    elif type.isId():
        if type.id in builtin:
            return refs
        elif type.id_type_id == 0:
            return refs
        elif type.id_type_id in refs:
            return refs
        else:
            refs.append(type.id_type_id)
            return refs
    elif type.isProductType():
        for i in range(len(type)):
            refs = getTypeReferences(type[i], refs)
        return refs
    elif type.isSumType():
        for i in range(len(type)):
            refs = getTypeReferences(type[i], refs)
        return refs
    elif type.isPtrType():
        return getTypeReferences(type.ptrtype, refs)
    elif type.isFunctionType():
        return getTypeReferences(type.arg, getTypeReferences(type.result, refs))
    else:
        raise SystemError

def lookupForeignType(server, tid):
    try:
        return lookupType(tid)
    except KeyError:
        t = sendMessageToNode(server, None, "REQUEST_TYPE", tid)
        registerType(t, server)
        getTypesFromServer(server, (t, ))
        return t

def unregisterTypesFromProcess(pid):
    #FIXME: We can't do this yet, as we don't track what types are in use by values.
    pass
    #garbage = []
    #cache_lock.acquire()
    #try:
        #for tid in __cache.keys():
            #if pid == __cache[tid][1]:
                #clearIsos(__cache[tid][0])
                #del __cache[tid]
    #finally:
        #cache_lock.release()

    #for tid in garbage:
        #doTypeGarbageCollection(tid)

#def doTypeGarbageCollection(tid):
    #cache_lock.acquire()
    #try:
        #if len(__cache[tid][1]) != 0:
            #return

        #memo = [tid]
        #pending = typeReferencedBy(tid)

    #finally:
        #cache_lock.release()

#def typeReferencedBy(tid):
    #l = []
    #for t in __cache.keys():
        #if tid in __cache[t][2]:
            #l.append(t)
    #return l

from iso_cache import clearIsos
from match import builtin
