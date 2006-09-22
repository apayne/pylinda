#!/usr/bin/python

#    Copyright 2004 Andrew Wilkinson <aw@cs.york.ac.uk>.
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

node_id = None

from messages import *

from options import getOptions
from tscontainer import TupleSpaceContainer
from tuplespace import TupleSpace
import connections
from connections import neighbours, sendMessageToNode, connectTo, broadcast_message, broadcast_firstreplyonly, Connection, getMsgId, socket_watcher
import stats
import guid

import _linda_server

import utils as utils

process_id = utils.Counter()
ts_ids = utils.Counter()

processes = {}
threads = {}

pthreads = {}
pthread_count = {}

local_ts = TupleSpaceContainer()
blocked_processes = {}

class LindaConnection:
    def __init__(self):
        self.messages = {
            register_process: self.register_process,
            register_thread: self.register_thread,
            unregister_thread: self.unregister_thread,
            my_name_is: self.my_name_is,
            create_tuplespace: self.create_tuplespace,
            get_connect_details: self.get_connect_details,
            get_node_id: self.get_node_id,
            rd_tuple: self.read_tuple,
            in_tuple: self.in_tuple,
            rdp_tuple: self.read_tuple,
            inp_tuple: self.in_tuple,
            out_tuple: self.out_tuple,
            unblock: self.unblock,
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

        print msgid, m[0], m[1:]
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
        assert utils.isNodeId(data[0])

        req.name = data[0]
        req.send(msgid, (done, ))
        if neighbours.has_key(data[0]):
            return
        neighbours[data[0]] = req
        req.type = "SERVER"
        sendMessageToNode(data[0], None, my_name_is, node_id)

        stats.inc_stat("server_con_current")
        stats.inc_stat("server_con_total")

    def register_process(self, req, msgid, message, data):
        # When a new process connects they need to acquire new process id
        p_id = "P%s" % (guid.generate(), )
        processes[p_id] = req
        req.type = "CLIENT"

        pthreads[p_id] = []
        pthread_count[p_id] = utils.Counter()

        req.send(msgid, ("RESULT_STRING", p_id))

    def register_thread(self, req, msgid, message, data):
        # When a new thread connects they need to acquire new thread id
        p_id = data[0]
        t_id = "%s!%i" % (p_id, pthread_count[p_id].next())
        pthreads[p_id].append(t_id)
        threads[t_id] = req
        req.type = "CLIENT"

        req.send(msgid, ("RESULT_STRING", t_id))

        stats.inc_stat("process_con_current")
        stats.inc_stat("process_con_total")

    def unregister_thread(self, req, msgid, message, data):
        # if a process is about to disconnect they can let us know first
        # removeProcess removes any references held by the process

        pid, tid = data

        p_id = utils.getProcessIdFromThreadId(tid)
        del pthreads[pid][pthreads[pid].index(tid)]
        del threads[tid]

        req.send(msgid, done)

    def unregister_server(self, req, msgid, message, data):
        nid, local = data

        req.send(msgid, done)

        removeServer(nid, local)

    def get_connect_details(self, req, msgid, message, data):
        # try to find out how to connect to a server
        if data[0] == node_id:
            # they're looking for us! Return our details
            req.send(msgid, ((self.request.getsockname()[0], getOptions().port), None))
        elif data[0] in neighbours.keys():
            # they're looking for our neighbour, ask them how to connect to them
            r = sendMessageToNode(data[0], None, get_connect_details, data[0])
            if isinstance(neighbours[data[0]], str): # we don't have a direct connection
                req.send(msgid, (r[0], neighbours[data[0]]))
            else: # we have a direct connection, tell people to go through us
                req.send(msgid, (r[0], node_id))
        else:
            # we don't know the node they're looking for
            req.send(msgid, (dont_know, ))

    def create_tuplespace(self, req, msgid, message, data):
        # return a new tuplespace id
        ts = "S%s" % (guid.generate(), )

        local_ts.newTupleSpace(ts)
        local_ts.addReference(ts, utils.getProcessIdFromThreadId(data[0]))
        req.send(msgid, ("RESULT_STRING", ts))

    def out_tuple(self, req, msgid, message, data):
        # output a tuple into a tuplespace
        ts, tup = data

        assert utils.isTupleSpaceId(ts)
        assert local_ts.has_key(ts)

        local_ts[ts]._out(tup)
        stats.inc_stat("message_out_total")

        req.send(msgid, (done, ))

    def read_tuple(self, req, msgid, message, data):
        ts, template, tid = data
        unblockable = message == inp_tuple

        blocked_processes[tid] = (req, ts)

        assert utils.isTupleSpaceId(ts)
        assert local_ts.has_key(ts)

        r = local_ts[ts]._rd(tid, template, unblockable)
        stats.inc_stat("message_rd_total")

        if r is not None:
            del blocked_processes[tid]
            req.send(msgid, ("RESULT_TUPLE", r))
        else:
            pass # this thread is now blocked

    def in_tuple(self, req, msgid, message, data):
        ts, template, tid = data
        unblockable = message == inp_tuple

        blocked_processes[tid] = (req, ts)

        assert utils.isTupleSpaceId(ts)
        assert local_ts.has_key(ts)

        r = local_ts[ts]._in(tid, template, unblockable)
        stats.inc_stat("message_in_total")

        if r is not None:
            del blocked_processes[tid]
            req.send(msgid, ("RESULT_TUPLE", r))
        else:
            pass # this thread is now blocked

    def return_tuple(self, req, msgid, message, data):
        tid, tup = data

        assert tid in blocked_processes.keys()

        s, ts = blocked_processes[tid]
        del blocked_processes[tid]
        s.lock.acquire()
        s.send(tup)
        s.lock.release()
        req.send(msgid, done)

    def unblock(self, req, msgid, message, data):
        tid = data[0]

        assert tid in blocked_processes.keys()

        s, semaphore, ts = blocked_processes[tid]
        del blocked_processes[tid]
        semaphore.acquire()
        s.send(unblock)
        semaphore.release()
        req.send(msgid, done)

    def collect(self, req, msgid, message, data):
        ts, dest_ts, template = data

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

        if req.type == "CLIENT":
            r = broadcast_tonodes(collect, False, ts._id, dest_ts._id, template)
            total = sum(r) + len(tups)
        else:
            total = len(tups)

        if new:
            local_ts.garbage(dest_ts._id)

        req.send(msgid, total)

    def copy_collect(self, req, msgid, message, data):
        ts, dest_ts, template = data

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

        if req.type == "CLIENT":
            r = broadcast_tonodes(ts.partitions, False, copy_collect, ts._id, dest_ts._id, template)
            total = sum(r) + len(tups)
        else:
            total = len(tups)

        if new:
            local_ts.garbage(dest_ts._id)

        req.send(msgid, total)

    def tuple_request(self, req, msgid, message, data):
        ts, template = data

        if local_ts.has_key(ts):
            r = local_ts[ts].tuple_request(msgid[0], template)
        else:
            r = []
        req.send(msgid, ("RESULT_TUPLE", tuple(r)))

    def cancel_request(self, req, msgid, message, data):
        ts, template = data

        if local_ts.has_key(ts):
            local_ts[ts].cancel_request(self.other_nid, template)

        req.send(msgid, done)

    def get_requests(self, req, msgid, message, data):
        ts = data[0]

        if local_ts.has_key(ts):
            print "message", data, local_ts[ts].get_requests()
            req.send(msgid, ("RESULT_TUPLE", tuple(local_ts[ts].get_requests())))
        else:
            print "message", data, dont_know
            req.send(msgid, (dont_know,))

    def is_deadlocked(self, req, msgid, message, data):
        try:
            ts = local_ts[data[0]]
        except KeyError:
            req.send(msgid, True)
        else:
            req.send(msgid, ts.isLocallyDeadlocked())

    def multiple_in(self, req, msgid, message, data):
        ts, tups = data

        assert local_ts.has_key(ts)

        [local_ts[ts]._out(t) for t in tups]

        req.send(msgid, done)

    def add_reference(self, req, msgid, message, data):
        ts, ref = data

        if utils.isThreadId(ref):
            ref = utils.getProcessIdFromThreadId(ref)

        if not local_ts.has_key(ts):
            local_ts.newTupleSpace(ts)
        local_ts.addReference(ts, ref)

        req.send(msgid, (done, ))

    def delete_reference(self, req, msgid, message, data):
        ts, ref = data

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

        threading.Thread(target=local_ts.deleteReference,args=(ts, ref)).start()

        req.send(msgid, (done, ))

    def has_tuplespace(self, req, msgid, message, data):
        ts = data[0]
        if local_ts.has_key(ts):
            req.send(msgid, node_id)
        else:
            req.send(msgid, dont_know)

    def get_threads(self, req, msgid, message, data):
        pid = data[0]

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
        ts = data[0]
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
        routes = {}
        for n in neighbours.keys():
            if n == node_id:
                routes[n] = "Loopback"
            else:
                c = neighbours[n]
                if isinstance(c, str):
                    routes[n] = c
                else:
                    routes[n] = c.getpeername()
        req.send(msgid, routes)

    def get_partitions(self, req, msgid, message, data):
        tsid = data[0]
        try:
            ts = local_ts[tsid]
        except KeyError:
            req.send(msgid, (dont_know, ))
        else:
            req.send(msgid, ("RESULT_TUPLE", tuple(ts.partitions + [node_id])))

    def register_partition(self, req, msgid, message, data):
        tsid = data[0]
        try:
            ts = local_ts[tsid]
        except KeyError:
            req.send(msgid, dont_know)
        else:
            ts.partitions.append(msgid[0])
            req.send(msgid, done)

    def deleted_partition(self, req, msgid, message, data):
        tsid = data[0]
        try:
            ts = local_ts[tsid]
        except KeyError:
            req.send(msgid, dont_know)
        else:
            del ts.partitions[ts.partitions.index(msgid[0])]
            req.send(msgid, done)

def removeProcess(pid, local=True):
    """\internal
    \brief Called if a process leaves the system to ensure that no references that the process had remain in the system.

    If a process exits normally then all this should be unnessicary, however we can't trust user processes.
    """
    # check it actually is a process and not another node
    if not utils.isProcessId(pid):
        return

    # check that it wasn't blocked when the connection was lost
    for tid in blocked_processes.keys():
        if utils.getProcessIdFromThreadId(tid) == pid:
            del blocked_processes[tid]

    # remove any references the process may have had to our processes
    for ts in local_ts:
        local_ts.deleteAllReferences(ts, pid)

    # if the process was connected to us then broadcast the fact that it has left the system
    if local:
        broadcast_message(unregister_process, pid, False)

def cleanShutdown():
    # Stop accepting new connections
    _linda_server.server_disconnect();
    # Disappear from zeroconf network
    if options.mdns and not options.disable_mdns:
        import mdns
        mdns.disconnect()
    # Close client connections
    for t in threads.values():
        t.close()
    for p in processes.values():
        p.close()
    # garbage collect all tuplespaces
    local_ts.forceGarbage()
    connections.close = True
    for s in connections.sockets:
        s.close()

    sys.exit()

server = None
domain_server = None
def main():
    """\internal
    \brief Parse command line options and start the server.
    """
    global server, domain_server, node_id, neighbours, local_ts, options

    options = getOptions()

    if not options.daemon:
        import os
        pid = os.fork()
        if pid != 0:
            from monitor import monitor
            return monitor.Thread().run()

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
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                s.connect((svr, port))
            except socket.error, e:
                if i < len(con)-1:
                    i += 1
                    continue
                else:
                    print "Unable to connect to server %s:%i. Reason: %s" % (svr, port, e)
                    sys.exit(-1)

            s = Connection(s)
            s.type = None

            s.send(None, (get_node_id, ))
            node = s.recv()[2]
            assert utils.isNodeId(node), repr(node)

            s.send(None, (my_name_is, node_id))
            s.recv()
            s.type = "SERVER"

            print "Connected to %s:%i (%s)" % (svr, port, node)

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
