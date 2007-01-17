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

## \namespace tuplespace
## \brief This module contains the class the actually implements a tuplespace.
## \internal
##
## \attention If you're not developing the PyLinda server then you actually want the kernel::TupleSpace class.
## \author Andrew Wilkinson <aw@cs.york.ac.uk>
##

import sys
import threading

from connections import broadcast_message, broadcast_firstreplyonly, broadcast_tonodes, sendMessageToNode
from tuplecontainer import TupleContainer, doesMatch, NoTupleMatch
from messages import *

class MatchedTuple(Exception):
    pass

## \class TupleSpace
## \internal
## \brief This class is the actual tuplespace stored on the server. The class kernel::TupleSpace is a reference to one instance of this class.
##
## \author Andrew Wilkinson <aw@cs.york.ac.uk>
##
class TupleSpace:
    def __init__(self, _id):
        self._id = _id
        self.lock = utils.RLock()

        self.ts = TupleContainer()

        self.killlock = threading.Semaphore()
        self.refs = []
        self.blocked_list = {}
        self.requests = []
        self.partitions = []

        self.lock.acquire(msg=("start", self._id))
        t = threading.Thread(target=self.register)
        self.lock.setOwner(t)
        t.start()

    def register(self):
        try:
            broadcast_message(register_partition, self._id, server.node_id)

            ps = broadcast_firstreplyonly(get_partitions, self._id)
            if ps != dont_know:
                self.partitions.extend([str(p) for p in ps[1] if str(p) != server.node_id])

            r = broadcast_tonodes(self.partitions, True, get_requests, self._id)
            if r != dont_know:
                for (nid, template) in r:
                    self.requests.append((str(nid), template))
        finally:
            self.lock.release(msg=("end start", self._id))

    def __del__(self):
        broadcast_tonodes(self.partitions, False, deleted_partition, self._id, server.node_id)
        if self.ts.count() > 0 and len(self.partitions) > 0:
            node = self.partitions[0]
            tups = list(self.ts.matchAllTuples())
            map(lambda x: utils.changeOwner(x, self._id, self._id, node), tups)
            sendMessageToNode(node, None, multiple_in, self._id, tuple(tups))
        else:
            tups = list(self.ts.matchAllTuples())
            map(lambda x: utils.delReference(x, self._id), tups)

    ## \brief This function is called to put a tuple into the tuplespace
    def _out(self, tup):
        assert tup is not None

        self.lock.acquire(msg=("out", self._id, tup))
        try:
            # Before we add the tuple to the tuplespace we need to check if any processes are waiting on a template
            # that this tuple matches - first we get a list of the blocked processes
            blist = self.blocked_list.keys()

            def check_blocked(tid):
                # for each blocked process...
                try:
                    matched_tup = doesMatch(self.blocked_list[tid][0], tup)
                except NoTupleMatch:
                    pass
                else:
                    # ... if the tuple matches their template wake them up.
                    pattern = self.blocked_list[tid][0]
                    destructive = self.blocked_list[tid][2]
                    del self.blocked_list[tid]

                    def do_return_tuple():
                        utils.changeOwner(matched_tup, self._id, utils.getProcessIdFromThreadId(tid))
                        req, ts = server.blocked_threads[tid]
                        del server.blocked_threads[tid]
                        req.lock.acquire()
                        req.send(None, ("RESULT_TUPLE", matched_tup))
                        req.lock.release()

                        broadcast_tonodes(self.partitions, False, cancel_request, self._id, pattern)

                    threading.Thread(target=do_return_tuple).start()

                    if destructive == True: # if they were doing an in then stop here, otherwise we continue
                        raise MatchedTuple

            try:
                map(check_blocked, blist)
            except MatchedTuple:
                return

            def check_requests((node, pattern)):
                try:
                    matched = doesMatch(pattern, tup)
                except NoTupleMatch:
                    return
                else:
                    def do_return_tuple():
                        utils.changeOwner(matched, self._id, self._id, node)
                        sendMessageToNode(node, None, multiple_in, self._id, (matched, )) # return the tuple to the server

                    threading.Thread(target=do_return_tuple).start()

                    raise MatchedTuple

            try:
                map(check_requests, self.requests)
            except MatchedTuple:
                return

            self.ts.add(tup) # add the tuple to the tuplespace
        finally:
            self.lock.release(msg=("out", self._id, tup))

    ## \brief This function is called when a process reads from the tuplespace
    ##
    ## If a matching tuple is immediatly found then it is returned, otherwise <b>None</b> is returned and
    ## the process is added to the list of blocked processes
    def _rd(self, tid, pattern, unblockable):
        self.lock.acquire()
        try:
            try:
                # try to match a tuple
                real, matched = self.ts.matchOneTuple(pattern)
            except NoTupleMatched:
                def add_tuples():
                    for server in [x[1] for x in broadcast_tonodes(self.partitions, False, tuple_request, self._id, pattern)]:
                        for t in server:
                            self._out(tuple(t))
                    # check that we have created a deadlock
                    if self.isDeadLocked():
                        # if we have then unblock a random process
                        self.unblockRandom()
                threading.Thread(target=add_tuples).start()

                # if we didn't find a tuple then we block
                self.blocked_list[tid] = (pattern, unblockable, False)
            else:
                # we found a tuple so update the references and return it
                utils.addReference(matched, utils.getProcessIdFromThreadId(tid))
                return matched
        finally:
            self.lock.release()

    ## \brief This function is called when a process ins from the tuplespace
    ##
    ## If a matching tuple is immediatly found then it is returned, otherwise <b>None</b> is returned and
    ## the process is added to the list of blocked processes
    def _in(self, tid, pattern, unblockable):
        self.lock.acquire()
        try:
            try:
                # try to match a tuple
                real, matched = self.ts.matchOneTuple(pattern)
            except StopIteration:
                def add_tuples():
                    for server in [x[1] for x in broadcast_tonodes(self.partitions, False, tuple_request, self._id, pattern)]:
                        for t in server:
                            self._out(tuple(t))
                    # check if we have created a deadlock
                    if self.isDeadLocked():
                        # if we have then unblock a random process
                        self.unblockRandom()

                threading.Thread(target=add_tuples).start()

                # if we didn't find a tuple then we block
                self.blocked_list[tid] = (pattern, unblockable, True)
            except:
                raise
            else:
                # we found a tuple so update the references and return it
                self.ts.delete(real) # since this is destructive delete the tuple from the tuplespace
                utils.changeOwner(matched, self._id, utils.getProcessIdFromThreadId(tid))
                return matched
        finally:
            self.lock.release()

    ## \brief If we encounter a deadlock this function is called to unblock a process
    def unblockRandom(self):
        self.lock.acquire()
        try:
            # Get a list of the blocked processes
            l = self.blocked_list.keys()
            if len(l) == 0: # if the list is empty we can't do anything...
                return

            # Check each process until we find one that is unblockable
            p, l = l[0], l[1:]
            while not self.blocked_list[p][1]: # this holds a boolean that is true if the process is unblockable
                if len(l) == 0:
                    return # we have no unblockable processes so just bail out
                p, l = l[0], l[1:]

            # Delete the process we're unblocking from the blocked list and send it an unblock message
            del self.blocked_list[p]
            server.unblock_thread(p)
        finally:
            self.lock.release()

    ## \brief This is called when a process does a collect operation.
    ##
    ## Any matched tuples are removed from the tuplespace
    ## \return A list of tuples matching the pattern
    ## \param pattern The pattern to match the tuples against
    def collect(self, pattern):
        self.lock.acquire()
        try:
            tups = []
            try:
                m = self.ts.matchTuples(pattern) # Create the iterator
                while True: # Keep iterating and adding tuples to the list
                    tups.append(m.next())
            except StopIteration: # Stop when we get a StopIteration exception
                for t in tups: # Delete the tuples we've found
                    self.ts.delete(t)
                return tups # return the list of tuples
        finally:
            self.lock.release()

    ## \brief This is called when a process does a copy_collect operation.
    ## \return A list of tuples matching the pattern
    ## \param pattern The pattern to match the tuples against
    def copy_collect(self, pattern):
        self.lock.acquire()
        try:
            tups = []
            try:
                m = self.ts.matchTuples(pattern) # Create the iterator
                while True: # Keep iterating and adding tuples to the list
                    tups.append(m.next())
            except StopIteration: # Stop when we get a StopIteration exception
                return tups # return the list of tuples
        finally:
            self.lock.release()

    ## \brief Add a new reference to the tuplespace
    ## \param ref The object that will own the reference
    def addreference(self, ref):
        if self._id == "UTS": # Check we're not the universal tuplespace, which is excluded from garbage collection
            return
        assert not utils.isThreadId(ref), ref
        self.lock.acquire()
        try:
            self.refs.append(ref)
        finally:
            self.lock.release()

    ## \brief Remove a reference from the given object to this tuplespace
    ## \param ref The object with the reference
    def removereference(self, ref):
        try:
            if self._id == "UTS": # Check we're not the universal tuplespace, which is excluded from garbage collection
                return
            assert not utils.isThreadId(ref), ref

            self.lock.acquire()
            try:
                try:
                    self.refs.remove(ref) # Remove the reference from the list
                except ValueError: # if the reference doesn't exist then ValueError is raise - and something has gone badly wrong
                    print "!!!%s not in %s for %s" % (ref, str(self.refs), self._id)
                    raise SystemError, "Internal reference counting error"
            finally:
                self.lock.release()
        finally:
            self.killlock.release()

        # if a reference is removed this may mean the remaining processes are deadlocked - check if that is the case
        if self.isDeadLocked():
            self.unblockRandom()
        # check to see if we're now garbage
        self.doGarbageCollection()

        return len(self.refs) # return the number of remaining references

    ## \brief Remove all references from the given object to this tuplespace
    ## \param ref The object with the references
    def removeanyreferences(self, ref):
        if self._id == "UTS": # Check we're not the universal tuplespace, which is excluded from garbage collection
            return

        # NOTE:
        # For a long time this system suffered a bug where when a process died it would correctly delete a reference,
        # and then when it actually died the system would clean up any remaining reference with a call to
        # removeanyreferences. In order to allow a proccess to continue the garbage collection for a normal delete reference
        # is done in a new thread, when a process has died this is not necessary and the garbage collection is done in the
        # main thread. This lead to the situation where a removereference would be called, but then because of the way the
        # thread system worked the removeanyreference call, that actually came later, would complete first - and obviosuly
        # this caused an exception.
        # The solution was to have a killlock that is locked before the new thread is created and this causes the
        # removeallreference to block until the removereference has completed.
        #
        self.killlock.acquire()
        try:
            assert utils.isProcessId(ref)

            # This is just a sanity check to make sure something hasn't gone horribly wrong...
            self.lock.acquire(msg=("remove all", self._id))
            try:
                if ref in self.blocked_list.keys():
                    print "ERROR : Deleting references for a blocked process %s" % (str(self.blocked_list.keys()), )

                try:
                    while True: # Remove all references...
                        self.refs.remove(ref)
                except ValueError: # ... until there are none left and a ValueError is raised.
                    pass
            finally:
                self.lock.release(msg=("done remove all", self._id))
        finally:
            self.killlock.release()

        # if a reference is removed this may mean the remaining processes are deadlocked - check if that is the caseoved this may mean the remaining processes are deadlocked - check if that is the case
        if self.isDeadLocked():
            self.unblockRandom()
        # check to see if we're now garbage
        self.doGarbageCollection()

        return len(self.refs) # return the number of remaining references

    def doGarbageCollection(self):
        memo = [self._id] # what tuplespaces have we already checked?
        to_check = self.refs[:] # what tuplespaces are left to check?
        while len(to_check) > 0:
            i, to_check = to_check[0], to_check[1:]
            if i in memo: # if we've already checked it then skip it
                continue

            if utils.isProcessId(i) or i == "UTS": # this is either a process or the universal tuplespace so we're still active
                return

            memo.append(i)

            try:
                ts = server.local_ts[i]
            except KeyError: # Tuplespace has been deleted while we're working
                continue
            else:
                to_check.extend(ts.refs)
        # if we reach here then we're (locally) garbage. Set references to empty so we are garbage collected.
        self.refs = []

    def isLocallyDeadlocked(self):
        if self._id == "UTS":
            return False
        refs = self.refs[:]
        threads = {}
        while True:
            if len(refs) == 0:
                return True
            r, refs = refs[0], refs[1:]
            if utils.isTupleSpaceId(r):
                if r == "UTS":
                    return False
                refs.extend(server.local_ts[r].refs)
            elif utils.isProcessId(r):
                refs.extend(server.pthreads[r])
            elif utils.isThreadId(r):
                if not threads.has_key(r):
                    if server.blocked_threads.has_key(r):
                        threads[r] = True
                    else:
                        return False
            else:
                assert False, r

    ## \brief Checks to see if there is a deadlock in the system
    def isDeadLocked(self):
        if not self.isLocallyDeadlocked():
            return False

        d = broadcast_tonodes(self.partitions, False, is_deadlocked, self._id)
        d = [x for x in d if d is not True]
        return len(d) == 0

    def tuple_request(self, node, pattern):
        assert node is not None
        assert isinstance(pattern, tuple)

        self.lock.acquire()
        self.requests.append((str(node), pattern))
        try:
            try:
                t = self.ts.matchOneTuple(pattern) # Create the iterator
            except StopIteration: # Stop when we get a StopIteration exception
                return []
            else:
                self.ts.delete(t[0])
                self.lock.release()
                utils.changeOwner(t[1], self._id, self._id, str(node))
                self.lock.acquire()
                return [t] # return the list of tuples
        finally:
            try:
                self.lock.release()
            except AssertionError:
                pass

    def cancel_request(self, node, pattern):
        assert node is not None
        assert isinstance(pattern, tuple)

        self.lock.acquire()
        try:
            try:
                del self.requests[self.requests.index((str(node), pattern))]
            except ValueError:
                sys.stderr.write("Cancel Request: %s not found in %s.\n" % (str((node, pattern)), str(self.requests)))
        finally:
            self.lock.release()

    def get_requests(self):
        self.lock.acquire()
        try:
            return self.requests + [(server.node_id, b[0]) for b in self.blocked_list.values()]
        finally:
            self.lock.release()

    def __repr__(self):
        return "<Tuplespace Partition %s>" % (self._id, )

import utils
import server
