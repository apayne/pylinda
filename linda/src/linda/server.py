#!/usr/bin/python

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

import socket
import struct
import sys
import threading
import gc

node_id = None

from messages import *

from options import getOptions
from tscontainer import TupleSpaceContainer
from tuplespace import TupleSpace
import connections
from connections import neighbours, sendMessageToNode, broadcast_message, broadcast_firstreplyonly, broadcast_tonodes, Connection, socket_watcher
import stats
import guid

import _linda_server

import utils as utils

if _linda_server.use_types and _linda_server.register_types:
    from type_cache import registerType, updateType, emptyTypeCache, unregisterTypesFromProcess
    from iso_cache import emptyIsoCache
    import interserver_types

process_id = utils.Counter()
ts_ids = utils.Counter()

processes = {}
threads = {}

pthreads = {}
pthread_count = {}

local_ts = TupleSpaceContainer()
blocked_threads = {}

class LindaConnection:
    def __init__(self):
        self.messages = {
            register_process: self.register_process,
            register_thread: self.register_thread,
            register_type: self.register_type,
            update_type: self.update_type,
            my_name_is: self.my_name_is,
            create_tuplespace: self.create_tuplespace,
            get_connect_details: self.get_connect_details,
            get_node_id: self.get_node_id,
            rd_tuple: self.read_tuple,
            in_tuple: self.in_tuple,
            rdp_tuple: self.read_tuple,
            inp_tuple: self.in_tuple,
            out_tuple: self.out_tuple,
            return_tuple: self.return_tuple,
            collect: self.collect,
            copy_collect: self.copy_collect,
            multiple_in: self.multiple_in,
            add_reference: self.add_reference,
            delete_reference: self.delete_reference,
            has_tuplespace: self.has_tuplespace,
            tuple_request: self.tuple_request,
            cancel_request: self.cancel_request,
            is_deadlocked: self.is_deadlocked,
            get_requests: self.get_requests,
            get_neighbours: self.get_neighbours,
            kill_server: self.kill_server,
            monitor: self.monitor,
            list_ts: self.list_ts,
            inspect_ts: self.inspect_ts,
            get_routes: self.get_routes,
            get_partitions: self.get_partitions,
            register_partition: self.register_partition,
            deleted_partition: self.deleted_partition,
            }

    def handle(self, req, msgid, m):
        assert isinstance(m, tuple), m

        self.handle_msg(req, msgid, m[0], m[1:])
        return True

    def handle_msg(self, req, msgid, message, data):
        try:
            func = self.messages[message]
        except KeyError:
            print "Unknown Message: %s (%s)" % (repr(message), str(data))
            req.send(msgid, (done, ))
        else:
            func(req, msgid, message, data)

    def get_node_id(self, req, msgid, message, data):
        req.send(msgid, ("RESULT_STRING", str(node_id)))

    def my_name_is(self, req, msgid, message, data):
        # when someone connects who already has an id they need to let us know who they are...
        name = str(data[0])
        assert utils.isNodeId(name)

        req.name = str(name)
        req.send(msgid, (done, ))
        if neighbours.has_key(name):
            return
        neighbours[name] = req
        req.type = "SERVER"
        sendMessageToNode(name, None, my_name_is, node_id)

        stats.inc_stat("server_con_current")
        stats.inc_stat("server_con_total")

    def register_process(self, req, msgid, message, data):
        # When a new process connects they need to acquire new process id
        p_id = "P%s" % (guid.generate(), )
        processes[p_id] = req
        req.type = "CLIENT"
        req.name = p_id

        pthreads[p_id] = []
        pthread_count[p_id] = utils.Counter()

        req.send(msgid, ("RESULT_STRING", p_id))

    def register_thread(self, req, msgid, message, data):
        # When a new thread connects they need to acquire new thread id
        p_id = str(data[0])
        t_id = "%s!%i" % (p_id, pthread_count[p_id].next())
        pthreads[p_id].append(t_id)
        threads[t_id] = req
        req.type = "CLIENT"
        req.name = t_id

        req.send(msgid, ("RESULT_STRING", t_id))

        stats.inc_stat("process_con_current")
        stats.inc_stat("process_con_total")

    def register_type(self, req, msgid, message, data):
        type, pid = data[0], data[1]

        typeid = registerType(type, pid)

        req.send(msgid, ("RESULT_STRING", typeid))

    def update_type(self, req, msgid, message, data):
        type_id, type, pid = data[0], data[1], data[2]

        type.type_id = type_id
        updateType(type_id, type)

        req.send(msgid, ("DONE", ))

    def unregister_thread(self, req, msgid, message, data):
        # if a process is about to disconnect they can let us know first
        # removeProcess removes any references held by the process

        pid, tid = str(data[0]), str(data[1])

        pthreads[pid].remove(tid)
        del threads[tid]

        req.send(msgid, (done, ))

    def unregister_server(self, req, msgid, message, data):
        nid, local = str(datap[0]), data[1]

        req.send(msgid, ("DONE", ))

        removeServer(nid, local)

    def get_connect_details(self, req, msgid, message, data):
        # try to find out how to connect to a server
        if str(data[0]) == node_id:
            # they're looking for us! Return our details
            req.send(msgid, ("RESULT_TUPLE", ((req.getsockname()[0], getOptions().port), None)))
        elif str(data[0]) in neighbours.keys():
            # they're looking for our neighbour, ask them how to connect to them
            r = sendMessageToNode(str(data[0]), None, get_connect_details, str(data[0]))
            if isinstance(neighbours[str(data[0])], str): # we don't have a direct connection
                req.send(msgid, ("RESULT_TUPLE", (r[1], neighbours[str(data[0])])))
            else: # we have a direct connection, tell people to go through us
                req.send(msgid, ("RESULT_TUPLE", (r[1], node_id)))
        else:
            # we don't know the node they're looking for
            req.send(msgid, (dont_know, ))

    def create_tuplespace(self, req, msgid, message, data):
        # return a new tuplespace id
        ts = "S%s" % (guid.generate(), )

        local_ts.newTupleSpace(ts)
        local_ts.addReference(ts, utils.getProcessIdFromThreadId(str(data[0])))
        req.send(msgid, ("RESULT_STRING", ts))

    def out_tuple(self, req, msgid, message, data):
        # output a tuple into a tuplespace
        ts, tup = str(data[0]), data[1]

        assert utils.isTupleSpaceId(ts), "%s is not a tuplespace id" % (ts, )
        assert local_ts.has_key(ts)

        local_ts[ts]._out(tup)
        stats.inc_stat("message_out_total")

        req.send(msgid, (done, ))

    def read_tuple(self, req, msgid, message, data):
        ts, template, tid = str(data[0]), data[1], str(data[2])
        unblockable = message == inp_tuple

        blocked_threads[tid] = (req, ts)

        assert utils.isTupleSpaceId(ts), "%s is not a tuplespace id" % (ts, )
        assert local_ts.has_key(ts)

        r = local_ts[ts]._rd(tid, template, unblockable)
        stats.inc_stat("message_rd_total")

        if r is not None:
            del blocked_threads[tid]

            # we found a tuple so update the references and return it
            utils.addReference(r, utils.getProcessIdFromThreadId(tid))

            req.send(msgid, ("RESULT_TUPLE", r))
        else:
            pass # this thread is now blocked

    def in_tuple(self, req, msgid, message, data):
        ts, template, tid = str(data[0]), data[1], str(data[2])
        unblockable = message == inp_tuple

        blocked_threads[tid] = (req, ts)

        assert utils.isTupleSpaceId(ts), "%s is not a tuplespace id" % (ts, )
        assert local_ts.has_key(ts)

        r = local_ts[ts]._in(tid, template, unblockable)
        stats.inc_stat("message_in_total")

        if r is not None:
            del blocked_threads[tid]
            utils.changeOwner(r, ts, utils.getProcessIdFromThreadId(tid))
            req.send(msgid, ("RESULT_TUPLE", r))
        else:
            pass # this thread is now blocked

    def return_tuple(self, req, msgid, message, data):
        tid, tup = str(data[0]), data[1]

        assert tid in blocked_threads.keys()

        s, ts = blocked_threads[tid]
        del blocked_threads[tid]
        s.lock.acquire()
        s.send(tup)
        s.lock.release()
        req.send(msgid, done)

    def collect(self, req, msgid, message, data):
        ts, dest_ts, template = str(data[0]), str(data[1]), data[2]

        try:
            ts = local_ts[ts]
        except KeyError:
            req.send(msgid, 0)
            return

        try:
            dest_ts = local_ts[dest_ts]
        except KeyError:
            local_ts.newTupleSpace(dest_ts)
            dest_ts = local_ts[dest_ts]
            new = True
        else:
            new = False

        tups = ts.collect(template)
        if tups != []:
            for t in tups:
                utils.changeOwner(t, ts, dest_ts._id)
                local_ts[dest_ts._id]._out(t)

        if req.type != "SERVER":
            r = broadcast_tonodes(ts.partitions, False, collect, ts._id, dest_ts._id, template)
            total = sum([x[1] for x in r]) + len(tups)
        else:
            total = len(tups)

        if new:
            local_ts.garbage(dest_ts)

        req.send(msgid, ("RESULT_INT", total))

    def copy_collect(self, req, msgid, message, data):
        ts, dest_ts, template = str(data[0]), str(data[1]), data[2]

        try:
            ts = local_ts[ts]
        except KeyError:
            req.send(msgid, 0)
            return

        try:
            dest_ts = local_ts[dest_ts]
        except KeyError:
            local_ts.newTupleSpace(dest_ts)
            dest_ts = local_ts[dest_ts]
            new = True
        else:
            new = False

        tups = ts.copy_collect(template)
        if tups != []:
            for t in tups:
                utils.addReference(t, ts, dest_ts._id)
                local_ts[dest_ts._id]._out(t)

        if req.type != "SERVER":
            r = broadcast_tonodes(ts.partitions, False, copy_collect, ts._id, dest_ts._id, template)
            total = sum([x[1] for x in r]) + len(tups)
        else:
            total = len(tups)

        if new:
            local_ts.garbage(dest_ts)

        req.send(msgid, ("RESULT_INT", total))

    def tuple_request(self, req, msgid, message, data):
        ts, template = str(data[0]), data[1]

        if local_ts.has_key(ts):
            r = local_ts[ts].tuple_request(msgid[1], interserver_types.getTypesFromServer(msgid[1], template))
        else:
            r = []
        # we ignore the original representation and only pass on the converted format.
        req.send(msgid, ("RESULT_TUPLE", tuple(r)))

    def cancel_request(self, req, msgid, message, data):
        ts, template = str(data[0]), data[1]

        if local_ts.has_key(ts):
            local_ts[ts].cancel_request(msgid[1], template)

        req.send(msgid, (done, ))

    def get_requests(self, req, msgid, message, data):
        ts = str(data[0])

        if local_ts.has_key(ts):
            req.send(msgid, ("RESULT_TUPLE", tuple(local_ts[ts].get_requests())))
        else:
            req.send(msgid, (dont_know,))

    def is_deadlocked(self, req, msgid, message, data):
        try:
            ts = local_ts[str(data[0])]
        except KeyError:
            req.send(msgid, True)
        else:
            req.send(msgid, ts.isLocallyDeadlocked())

    def multiple_in(self, req, msgid, message, data):
        ts, tups = str(data[0]), data[1]

        assert local_ts.has_key(ts)

        [local_ts[ts]._out(t) for t in tups]

        req.send(msgid, (done, ))

    def add_reference(self, req, msgid, message, data):
        ts, ref = str(data[0]), str(data[1])

        if utils.isThreadId(ref):
            ref = utils.getProcessIdFromThreadId(ref)

        if not local_ts.has_key(ts):
            local_ts.newTupleSpace(ts)
        local_ts.addReference(ts, ref)

        req.send(msgid, (done, ))

    def delete_reference(self, req, msgid, message, data):
        ts, ref = str(data[0]), str(data[1])

        if ts == "UTS":
            req.send(msgid, (done, ))
            return

        if utils.isThreadId(ref):
            ref = utils.getProcessIdFromThreadId(ref)

        # see the note in the TupleSpace.removeallreference for an explanation of the killlock
        try:
            local_ts[ts].killlock.acquire()
        except KeyError:
            pass
        else:
            threading.Thread(target=local_ts.deleteReference, args=(ts, ref)).start()

        req.send(msgid, (done, ))

    def has_tuplespace(self, req, msgid, message, data):
        ts = str(data[0])
        if local_ts.has_key(ts):
            req.send(msgid, node_id)
        else:
            req.send(msgid, dont_know)

    def get_threads(self, req, msgid, message, data):
        pid = str(data[0])

        assert pthreads.has_key(pid)

        req.send(msgid, utils.encode(pthreads[pid]))

    def get_neighbours(self, req, msgid, message, data):
        req.send(msgid, ("RESULT_TUPLE", tuple(neighbours.keys())))

    def kill_server(self, req, msgid, message, data):
        cleanShutdown()
        req.send(msgid, done)

    def monitor(self, req, msgid, message, data):
        req.type = "MONITOR"
        req.send(msgid, ("DONE", ))

    def list_ts(self, req, msgid, message, data):
        req.send(msgid, ("RESULT_STRING", str(local_ts.keys())))

    def inspect_ts(self, req, msgid, message, data):
        ts = str(data[0])
        try:
            ts = local_ts[ts]
        except KeyError:
            req.send(msgid, (dont_know, ))
        else:
            ts.lock.acquire()
            try:
                req.send(msgid, ("RESULT_TUPLE", (str(ts.refs), str(ts.blocked_list.keys()), str(list(ts.ts.matchAllTuples())), str(ts.partitions), str([x[0] for x in ts.requests]))))
            finally:
                ts.lock.release()

    def get_routes(self, req, msgid, message, data):
        routes = []
        for n in neighbours.keys():
            if n == node_id:
                routes.append((n, "Loopback"))
            else:
                c = neighbours[n]
                if isinstance(c, str):
                    routes.append((n, c))
                else:
                    routes.append((n, c.getpeername()))
        req.send(msgid, ("RESULT_TUPLE", tuple(routes)))

    def get_partitions(self, req, msgid, message, data):
        tsid = str(data[0])
        try:
            ts = local_ts[tsid]
        except KeyError:
            req.send(msgid, (dont_know, ))
        else:
            req.send(msgid, ("RESULT_TUPLE", tuple(ts.partitions + [node_id])))

    def register_partition(self, req, msgid, message, data):
        tsid, nid = str(data[0]), str(data[1])
        try:
            ts = local_ts[tsid]
        except KeyError:
            req.send(msgid, (dont_know, ))
        else:
            ts.partitions.append(nid)
            req.send(msgid, (done, ))

    def deleted_partition(self, req, msgid, message, data):
        tsid, nid = str(data[0]), str(data[1])
        try:
            ts = local_ts[tsid]
        except KeyError:
            req.send(msgid, (dont_know, ))
        else:
            del ts.partitions[ts.partitions.index(nid)]
            req.send(msgid, (done, ))

def removeProcess(pid):
    """\internal
    \brief Called if a process leaves the system to ensure that no references that the process had remain in the system.

    If a process exits normally then all this should be unnessicary, however we can't trust user processes.
    """
    # check it actually is a process and not another node
    if utils.isThreadId(pid):
        tid = pid
        pid = utils.getProcessIdFromThreadId(pid)
        del threads[tid]
        pthreads[pid].remove(tid)
        if len(pthreads[pid]) > 0:
            for ts in local_ts:
                if local_ts[ts].isDeadLocked():
                    local_ts[ts].unblockRandom()
            return
    if not utils.isProcessId(pid):
        return

    # check that it wasn't blocked when the connection was lost
    for tid in blocked_threads.keys():
        if utils.getProcessIdFromThreadId(tid) == pid:
            del blocked_threads[tid]

    del pthreads[pid]

    if _linda_server.use_types and _linda_server.register_types:
        unregisterTypesFromProcess(pid)

    # remove any references the process may have had to our tuplespaces
    for ts in local_ts:
        local_ts.deleteAllReferences(ts, pid)

def removeServer(nid):
    print "A server (%s) has left. Probably should do something about this..." % (nid, )

def cleanShutdown():
    # Stop accepting new connections
    connections.socket_lock.acquire()
    try:
        _linda_server.server_disconnect()
        connections.sockets = [s for s in connections.sockets if s.type != "bound"]
    finally:
        connections.socket_lock.release()
    # Disappear from zeroconf network
    if options.mdns and not options.disable_mdns:
        import mdns
        mdns.disconnect()
    # Close client connections
    connections.socket_lock.acquire()
    try:
        for t in threads.values():
            t.close()
        for p in processes.values():
            p.close()
    finally:
        connections.socket_lock.release()
    # garbage collect all tuplespaces
    local_ts.forceGarbage()
    connections.close = True
    connections.socket_lock.acquire()
    try:
        for s in connections.sockets:
            s.close()
    finally:
        connections.socket_lock.release()

    if _linda_server.use_types and _linda_server.register_types:
        emptyTypeCache()
        emptyIsoCache()

    gc.collect() # Run garbage collection so libminimal doesn't produce spurious warnings.

    sys.exit()

def unblock_thread(tid):
    assert tid in blocked_threads.keys(), (tid, blocked_threads.keys())

    s, ts = blocked_threads[tid]
    s.lock.acquire()
    del blocked_threads[tid]
    s.send(None, (unblock, ))
    s.lock.release()

server = None
domain_server = None
def main():
    """\internal
    \brief Parse command line options and start the server.
    """
    global server, domain_server, node_id, neighbours, local_ts, options

    options = getOptions()

    if not options.daemon:
        import subprocess
        subprocess.Popen("linda_monitor", shell=True)
        #pid = os.fork()
        #if pid != 0:
        #    from monitor import monitor
        #    return monitor.Thread(options.port).run()

    if options.peer:
        options.peer.append("127.0.0.1") # always allow local connections.

    def lookupname(addr):
        try:
            addr,r = addr.split("/")
        except ValueError:
            r = "32"

        addr = socket.gethostbyname(addr)
        if addr.count(".") != 3:
            print "%s is not in n.n.n.n[/r] format" % (addr+"/"+r)
            sys.exit(0)
        return addr+"/"+r

    node_id = "N"+guid.generate()
    _linda_server.setnodeid(node_id)

    if options.mdns and not options.disable_mdns:
        import mdns
        s = mdns.connect()
        if s:
            options.connect = s[0]
            options.connectport = s[1]

    if options.connect is not None and len(options.connect) > 0:
        if options.connectport:
            con = zip(options.connect, options.connectport) + [(x, 2102) for x in options.connect[len(options.connectport):]]
        else:
            con = zip(options.connect, [2102 for _ in options.connect])
        assert len(con) == len(options.connect)
        i = 0
        while True:
            svr, port = con[i]
            s = _linda_server.connect("%s:%s" % (svr, port));
            if s is None:
                if i < len(con)-1:
                    i += 1
                    continue
                else:
                    print "Unable to connect to server %s:%i." % (svr, port)
                    sys.exit(-1)

            s = Connection(s)
            s.type = None

            s.send(None, (get_node_id, ))
            node = s.recv()[2]
            assert utils.isNodeId(node), repr(node)

            s.send(None, (my_name_is, node_id))
            s.recv()
            s.type = "SERVER"

            s.name = node
            neighbours[node] = s
            connections.sockets.append(s)
            break

    local_ts.newTupleSpace("UTS")

    while True:
        try:
            socket_watcher()
        except KeyboardInterrupt:
            cleanShutdown()
            raise
        break
