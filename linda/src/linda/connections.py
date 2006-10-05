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

import struct
import thread
import threading
import socket
import select
import sys

import _linda_server
from options import getOptions

from messages import *
import thread_pool

neighbours = {}

connections = {}

thread_lock = threading.Lock()

return_locks = {}
message_store = {}
ms_lock = threading.Lock()

socket_lock = threading.Lock()
sockets = []
close = False
def socket_watcher():
    _linda_server.serve(getOptions().use_domain, getOptions().port)
    for s in _linda_server.serverSockets():
        if s > 0:
            sockets.append(Connection(s))
            sockets[-1].type = "bound"

    Handler = server.LindaConnection()
    while True:
        iwtd, owtd, ewtd = select.select(sockets, [], sockets, 1)

        if close:
            break
        elif len(iwtd) == 0 and len(owtd) == 0 and len(ewtd) == 0:
            continue

        for s in iwtd:
            if s.type == "bound":
                 ns = _linda_server.accept(s)
                 if ns == -1:
                     del sockets[sockets.index(s)]
                     sys.stderr.write("Error accepting connection on %s\n" % (s, ))
                 else:
                    ns = Connection(ns)
                    ns.type = "CLIENT"
                    sockets.append(ns)
                 continue
            m = _linda_server.recv(s.fileno())
            if m is None:
                 if s.type == "MONITOR":
                     print "Server shutting down."
                     server.cleanShutdown()
                     return # Don't do anything else, just quit.
                 elif s.type == "SERVER":
                    connect_lock.acquire()
                    try:
                        del neighbours[s.name]
                    finally:
                        connect_lock.release()
                 socket_lock.acquire()
                 try:
                     del sockets[sockets.index(s)]
                     continue
                 finally:
                     socket_lock.release()
            else:
                msgid, msg = m[0], m[1:]
                if msgid is None:
                    thread_pool.giveJob(target=Handler.handle, args=(s, None, msg))
                elif msgid[0] != server.node_id and msgid[1] != server.node_id:
                    sendMessageToNode(msgid[0], msgid, *msg)
                elif msgid[0] == server.node_id:
                    thread_pool.giveJob(target=Handler.handle, args=(s, msgid, msg))
                elif msgid[1] == server.node_id:
                    ms_lock.acquire()
                    try:
                        message_store[msgid] = (msgid, msg)
                        return_locks[msgid].release()
                    finally:
                        ms_lock.release()
                else:
                    raise SystemError

class Connection:
    def __init__(self, sd):
        assert isinstance(sd, int)
        self.sd = sd
        self.lock = threading.Lock()
        self.buf = ""
        self.name = None
    def setblocking(self, value):
        _linda_server.setblocking(self.sd, value)
    def getsockname(self):
        return _linda_server.getsockname(self.sd)
    def getpeername(self):
        return _linda_server.getpeername(self.sd)
    def fileno(self):
        return self.sd
    def __int__(self):
        return self.sd
    def send(self, msgid, msg):
        assert isinstance(msg, tuple), type(msg)
        msg = utils.makeMessageXMLSafe(msg)
        if msgid:
            _linda_server.send(self.sd, (msgid, ) + msg)
        else:
            _linda_server.send(self.sd, msg)
        return msgid
    def recv(self, msgid=None):
        if msgid is None:
            return self.realrecv()
        else:
            ms_lock.acquire()
            try:
                s = threading.Semaphore(0)
                return_locks[msgid] = s
            finally:
                ms_lock.release()
            s.acquire()
            ms_lock.acquire()
            try:
                del return_locks[msgid]
                m = message_store[msgid]
                del message_store[msgid]
            finally:
                ms_lock.release()
            return m[1]
    def realrecv(self):
        r = _linda_server.recv(self.sd)
        return r
    def sendrecv(self, msgid, msg):
        return self.recv(self.send(msgid, msg))

    def shutdown(self, i):
        _linda_server.socket_shutdown(self.sd, i)
    def close(self):
        _linda_server.socket_close(self.sd)
    def __repr__(self):
        return "<Connection %i>" % (self.fileno(), )

def getNeighbourDetails(node):
    assert isinstance(node, str), node
    if not neighbours.has_key(node):
        if connectTo(node):
            return getNeighbourDetails(node)
        else:
            return None
    elif type(neighbours[node]) == str:
        return getNeighbourDetails(neighbours[node])
    else:
        return neighbours[node]

def sendMessageToNode(node, msgid, *args):
    """\internal
    Helper function to ensure that we have connected to the server we want to talk to, and to send the message.
    """
    assert node is not None
    assert node != server.node_id, "Cannot send msg %s to self" % (str(args), )

    if msgid is None:
        msgid = (node, server.node_id, getMsgId())

    assert isinstance(node, str), node
    assert isinstance(getNeighbourDetails(node), Connection)
    try:
        return getNeighbourDetails(node).sendrecv(msgid, args)
    except socket.error, e:
        if e[0] == 32:
            connect_lock.acquire()
            try:
                del neighbours[node]
            finally:
                connect_lock.release()
        else:
            raise

connect_lock = threading.Semaphore()
def connectTo(node):
    connect_lock.acquire()
    try:
        if not neighbours.has_key(node): # check that we still don't know how to connect
            connectToMain(node)
    finally:
        connect_lock.release()

def connectToMain(node):
    """\internal
    \brief Find the address of, and connect to a new server.
    """
    details = broadcast_firstreplyonly(get_connect_details, node)
    if details == "DONT_KNOW":
        sys.stderr.write("Failed to get connection details for %s.\n" % (node, ))
        return False
    neighbours[node] = details[1]
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        s.connect(details[0])
    except socket.error, e:
        print "Failed to connect to", details
        neighbours[node] = details[1]
    else:
        neighbours[node] = s

        utils.send(s, None, None, (get_node_id, ))
        cnode = utils.recv(s)[1]

        if cnode != node:
            print "Failed to connect to", details
            neighbours[node] = None
            s.shutdown(1)
            s.close()
            return False

        msgid = sendMessageToNode(node, None, my_name_is, server.node_id)

        s = Connection(s)
        s.name = node
        neighbours[node] = s
        addr = (socket.gethostbyname(details[0][0]), details[0][1])
        server.server.process_request(s, addr)
        return True

def broadcast_message(*args):
    memo = [server.node_id]
    r = []
    todo = neighbours.keys()

    while len(todo) > 0:
        node, todo = todo[0], todo[1:]
        if node in memo:
            continue
        else:
            memo.append(node)

        m = sendMessageToNode(node, None, *args)
        if m is not None and m[0] != dont_know:
            r.append(m)

        n = sendMessageToNode(node, None, get_neighbours)
        if n is None:
            print "Broken connection to %s" % (node, )
        elif n[0] != dont_know:
            todo.extend(n[1])
    return r

def broadcast_firstreplyonly(*args):
    memo = [server.node_id]
    todo = neighbours.keys()
    while len(todo) > 0:
        node, todo = todo[0], todo[1:]
        if node in memo:
            continue
        else:
            memo.append(node)
        assert utils.isNodeId(node)
        m = sendMessageToNode(node, None, *args)

        if m is not None and m[0] != dont_know:
            return m

        n = sendMessageToNode(node, None, get_neighbours)
        if n is not None and n[0] != dont_know:
            todo.extend(n[1])

    return dont_know

def broadcast_tonodes(nodes, firstreplyonly, *args):
    todo = nodes[:]
    r = []
    for n in todo:
        assert utils.isNodeId(n)
        m = sendMessageToNode(n, None, *args)

        print m
        if m is None or m[0] == dont_know:
            pass
        elif firstreplyonly:
            return m
        else:
            r.append(m)
    if firstreplyonly:
        return dont_know
    else:
        return r

import utils
getMsgId = utils.Counter()

import server
