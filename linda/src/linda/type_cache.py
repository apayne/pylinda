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
from utils import Counter, isNodeId

cache_lock = RLock()

__cache = {}
getTypeId = Counter(start=1)

def registerType(type, pid):
    assert type.isType()

    cache_lock.acquire()
    try:
        id = getTypeId()
        if isNodeId(pid):
            __cache[id] = [type, None, __getTypeReferences(type), {pid: 0}]
        else:
            __cache[id] = [type, pid, __getTypeReferences(type), {}]
        return id
    finally:
        cache_lock.release()

def updateType(type_id, type, reverse_id):
    assert type.isType()

    cache_lock.acquire()
    try:
        __cache[type_id][0] = type
        __cache[type_id][2] = __getTypeReferences(type)
        if __cache[type_id][1] is None:
            __cache[type_id][3][__cache[type_id][3].keys()[0]] = reverse_id
    finally:
        cache_lock.release()

def emptyTypeCache():
    __cache = None

def lookupType(id):
    return __cache[id][0]

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

def getServerIds(type_id, nid):
    cache_lock.acquire()
    try:
        server_ids = __cache[type_id][3]
        try:
            return server_ids[nid]
        except KeyError:
            types = getTypeReferences(type_id)
            for type in types:
                type = lookupType(type)
                tid = type.type_id
                if __cache[tid][3].has_key(nid):
                    continue
                type.type_id = 0
                new_tid = sendMessageToNode(nid, None, "REGISTER_TYPE", type)[1]
                type.type_id = tid
                __cache[tid][3][nid] = new_tid
            for type in types:
                type = lookupType(type)
                tid = type.type_id
                type.type_id = 0
                sendMessageToNode(nid, None, "UPDATE_TYPE", __cache[tid][3][nid], convertValueTypes(nid, type), tid)
                type.type_id = tid
            return server_ids[nid]
    finally:
        cache_lock.release()

def setServerIds(type_id, nid, tid):
    __cache[type_id][3][nid] = tid

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
from interserver_types import convertValueTypes
