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
    for t in tup:
        try:
            t.type_id = getServerIds(t.type_id, server)
        except KeyError:
            types = getTypeReferences(t.type_id)
            for type in types:
                type = lookupType(type)
                tid = type.type_id
                type.type_id = 0
                print "register", type
                new_tid = sendMessageToNode(server, None, "REGISTER_TYPE", type)[1]
                print "got", new_tid
                type.type_id = tid
                setServerIds(tid, server, new_tid)
            for type in types:
                type = lookupType(type)
                tid = type.type_id
                type.type_id = 0
                print getServerIds(tid, server)
                new_tid = sendMessageToNode(server, None, "UPDATE_TYPE", getServerIds(tid, server), type, tid)
                type.type_id = tid
            t.type_id = getServerIds(t.type_id, server)
    return tup

def convertTupleForServer_none(server, tup):
    return tup

if not _linda_server.use_types:
    convertTupleForServer = convertTupleForServer_none
elif _linda_server.register_types:
    convertTupleForServer = convertTupleForServer_registered
    from type_cache import getServerIds, getTypeReferences, setServerIds, lookupType
    from connections import sendMessageToNode
else:
    convertTupleForServer = convertTupleForServer_none
