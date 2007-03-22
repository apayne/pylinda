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

## \namespace utils
## \brief This module contains all the various functions that are shared amongst the linda modules
##
## \internal

import thread
import threading
import struct
import socket
import htmlentitydefs

import messages

import _linda_server

def makeMessageXMLSafe(msg):
    r = ()
    for m in msg:
        if isinstance(m, str) or isinstance(m, unicode):
            m = m.replace("&", "&amp;")
            m = m.replace("<", "&lt;")
            m = m.replace(">", "&gt;")
        elif isinstance(m, tuple):
            m = makeMessageXMLSafe(m)
        r = r + (m, )
    return r

def addReference(tup, new, node=None):
    def do_replace(e):
        if isinstance(e, _linda_server.Value) and e.isTupleSpace() and e.tsid != "UTS":
            if node is None:
                server.local_ts.addReference(e.tsid, new)
            else:
                connections.sendMessageToNode(node, None, messages.add_reference, e.tsid, new)
        return e
    [do_replace(x) for x in tup]

def delReference(tup, cur):
    def do_replace(e):
        if isinstance(e, _linda_server.Value) and e.isTupleSpace() and e.tsid != "UTS":
            server.local_ts.deleteReference(e.tsid, cur)
        return e
    [do_replace(x) for x in tup]

def changeOwner(tup, cur, new, node=None):
    def do_replace(e):
        if isinstance(e, _linda_server.Value) and e.isTupleSpace() and e.tsid != "UTS":
            if node is None:
                server.local_ts.addReference(e.tsid, new)
            else:
                connections.sendMessageToNode(node, None, messages.add_reference, e.tsid, new)
            server.local_ts.deleteReference(e.tsid, cur)
        return e
    [do_replace(x) for x in tup]

def isTupleSpaceId(id):
    """\internal
    \brief Checks if the given id is a valid tuplespace id

    This function does not check that a tuplespace exists with this id, only the id is of the correct format
    \param id The id to be checked
    """
    if isinstance(id, _linda_server.Value) and id.isTupleSpace():
        id = id.tsid
    return isinstance(id, str) and (id == "UTS" or (len(id) == 41 and id[0] == "S"))

def isNodeId(id):
    """\internal
    \brief Checks if the given id is a valid node id

    This function does not check that a node exists with this id, only the id is of the correct format
    \param id The id to be checked
    """
    return (isinstance(id, str) or (isinstance(id, _linda_server.Value) and id.isString())) and len(id) == 41 and id[0] == "N"

def isProcessId(id):
    """\internal
    \brief Checks if the given id is a valid process id

    This function does not check that a process exists with this id, only the id is of the correct format
    \param id The id to be checked
    """
    return (isinstance(id, str) or (isinstance(id, _linda_server.Value) and id.isString())) and len(id) == 41 and id[0] == "P"

def isThreadId(id):
    """\internal
    \brief Checks if the given id is a valid thread id

    This function does not check that a thread exists with this id, only the id is of the correct format
    \param id The id to be checked
    """
    try:
        if not (isinstance(id, str) or (isinstance(id, _linda_server.Value) and id.isString())):
            return False
        pid, thread = id.split("!")
        if not isProcessId(pid): raise ValueError
        int(thread)
    except ValueError:
        return False
    else:
        return True

def getProcessIdFromThreadId(tid):
    assert isThreadId(tid), "Not a thread id %s" % (tid, )

    return tid.split("!")[0]

class Counter:
    def __init__(self, start=0):
        self.counter = start
        self.lock = thread.allocate_lock()
        self.limit = None

    def __call__(self):
        self.lock.acquire()
        try:
            self.counter = self.counter + 1
            if self.limit and self.counter > self.limit:
                raise ValueError, "Counter Exceeded Maximum Allowed Value"
            return self.counter
        finally:
            self.lock.release()

    def next(self):
        return self()

    def setLimit(self, limit):
        self.limit = limit

class RLock(threading._RLock):
    def acquire(self, msg=""):
        threading._RLock.acquire(self)
    def release(self, msg=""):
        threading._RLock.release(self)
    def setOwner(self, o):
        self.__owner = o

def mask(bits):
    assert 0 <= bits <= 32

    return ~((1 << (32 - bits)) - 1)

import connections
import server
