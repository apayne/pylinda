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
#

## \namespace kernel
## \brief This module provides the client code to allow a program to interface with a linda server.
##
## The key elements are the TupleSpace class and the universe global tuple space.
## \attention This module should not be imported directly, it is imported automatically when the linda package is imported.
## \author Andrew Wilkinson <aw@cs.york.ac.uk>
##

raise ImportError

import gc
import socket
socketerror = socket.error # stop attribute error at shutdown time
import time
import thread
import threading

from linda.messages import *
from connections import Connection

run_as_server = False

_connected = False
class NotConnected(Exception):
    """\brief If this exception is raised then you forgot to call the connect method before accessing a linda primitive.
    """
    pass

class TupleSpace:
    """\brief This class represents a tuplespace.
    """
    def __init__(self, tsid=None):
        """\brief Constructor for the tuplespace.

        This function contacts the local server and creates a new tuplespace there.
        \param universe Never set this parameter - it is only used internally
        """
        if tsid:
            self._id = tsid
        else:
            if not _connected:
                raise NotConnected

            self._id = message(create_tuplespace)

    def __del__(self):
        """\brief Tuplespace destructor
        """
        try:
            self._delreference(process_id)
        except TypeError:
            pass

    def _addreference(self, ref):
        """\internal
        \brief Adds a new reference to the tuplespace
        """
        message(increment_ref, self._id, ref)

    def _delreference(self, ref):
        """\internal
        \brief Deletes a reference to the tuplespace
        """
        try:
            message(decrement_ref, self._id, ref)
        except socketerror:
            pass

    def _out(self, tup):
        """\brief Outputs the given tuple to the tuplespace

        \param tup Sends the given tuple to the tuplespace
        """
        if not isinstance(tup, tuple):
            raise TypeError, "out only takes a tuple, not %s" % (type(tup))

        tup = utils.replaceTS(tup, self._id)
        utils.addReference(tup, self._id)
        message(out_tuple, self._id, utils.encode(tup))

    def _rd(self, template):
        """\brief Reads a tuple matching the given template

        \param template The template used to match.
        \return The matching tuple
        """
        if not isinstance(template, tuple):
            raise TypeError, "rd only takes a tuple, not %s" % (type(template))

        r = message(read_tuple, self._id, utils.replaceTS(template, None), getThreadId(), False)
        if r != unblock:
            return utils.replaceTSRef(utils.decode(r))
        else:
            raise SystemError, "Non-blocking primitive received an unblock command"

    def _in(self, template):
        """\brief Destructivly reads a tuple matching the given template

        \param template The template used to match.
        \return The matching tuple
        """
        if not isinstance(template, tuple):
            raise TypeError, "in only takes a tuple, not %s" % (type(template))

        r = message(in_tuple, self._id, utils.replaceTS(template, None), getThreadId(), False)
        if r != unblock:
            return utils.replaceTSRef(utils.decode(r))
        else:
            raise SystemError, "Non-blocking primitive received an unblock command"

    def _rdp(self, template):
        """\brief Reads a tuple matching the given template.

        This follows the Principled Semantics for inp as described by Jacob and Wood
        \param template The template used to match.
        \return The matching tuple
        """
        if not isinstance(template, tuple):
            raise TypeError, "rdp only takes a tuple, not %s" % (type(template))

        r = message(read_tuple, self._id, utils.replaceTS(template, None), getThreadId(), True)
        if r != unblock:
            return utils.replaceTSRef(utils.decode(r))
        else:
            return None

    def _inp(self, template):
        """\brief Destructivly reads a tuple matching the given template.

        This follows the Principled Semantics for inp as described by Jacob and Wood
        \param template The template used to match.
        \return The matching tuple
        """
        if not isinstance(template, tuple):
            raise TypeError, "inp only takes a tuple, not %s" % (type(template))

        r = message(in_tuple, self._id, utils.replaceTS(template, None), getThreadId(), True)
        if r != unblock:
            return utils.replaceTSRef(utils.decode(r))
        else:
            return None

    def collect(self, ts, template):
        """\brief Destructivly moves all matching tuples from this Tuplespace to the given Tuplespace.

        \param ts The tuplespace to move the tuples into.
        \param template The template used to match.
        \return The number of matched tuples
        """
        if not isinstance(ts, TupleSpace):
            raise TypeError, "collect only takes a tuplespace, not %s" % (ts.__class__, )
        if not isinstance(template, tuple):
            raise TypeError, "collect only takes a tuple, not %s" % (type(template), )

        r = message(collect, self._id, ts._id, utils.replaceTS(template, None))
        try:
            return int(r)
        except ValueError:
            raise SystemError, "Unexpected reply to collect message - %s" % r

    def copy_collect(self, ts, template):
        """\brief Copies all matching tuples from this Tuplespace to the given Tuplespace.

        \param ts The tuplespace to move the tuples into.
        \param template The template used to match.
        \return The number of matched tuples
        """
        if not isinstance(ts, TupleSpace):
            raise TypeError, "copy_collect only takes a tuplespace, not %s" % (ts.__class__)
        if not isinstance(template, tuple):
            raise TypeError, "copy_collect only takes a tuple, not %s" % (type(template))

        r = message(copy_collect, self._id, ts._id, utils.replaceTS(template, None))
        try:
            return int(r)
        except ValueError:
            raise SystemError, "Unexpected reply to collect message - %s" % r

    def __str__(self):
        """\brief Get a string representation of the tuplespace
        """
        if self._id == "UTS":
            return "<Universal Tuplespace>"
        else:
            return "<TupleSpace %s>" % (self._id, )
    def __repr__(self):
        """\brief Get a string representation of the tuplespace
        """
        if self._id == "UTS":
            return "<Universal Tuplespace>"
        else:
            return "<TupleSpace %s>" % (self._id, )
ts = TupleSpace

# delay importing the utils module until after we've defined TupleSpace as the utils class indirectly requires it
from linda import utils
utils.TupleSpace = TupleSpace

msg_semaphore = threading.Semaphore()
counter = utils.Counter()

def message(*msg):
    """\internal
    \brief Send the given message to the local server
    \param msg The message to send
    """
    if not _connected:
        raise NotConnected

    if run_as_server:
        msg_semaphore.acquire()
    try:
        s = getSocket()
        try:
            s.send(utils.encode(msg))
        except socket.error, (n, msg):
            if msg == "Broken Pipe":
                print "Broken Pipe"
            else:
                raise
        return s.recv()
    finally:
        if run_as_server:
             msg_semaphore.release()

import sys

def getThreadId():
    if not _connected:
        raise NotConnected
    if run_as_server:
        return "0!0!0"
    obj = threading.currentThread()
    if hasattr(obj, "pylinda_con"):
        return obj.pylinda_con[0]
    else:
        return getSocket()

def getSocket():
    if run_as_server:
        return s
    obj = threading.currentThread()
    if hasattr(obj, "pylinda_con"):
        return obj.pylinda_con[1]
    else:
        soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        soc.connect(("127.0.0.1", port))
        soc = Connection(soc)
        obj.pylinda_con = None, soc
        thread_id = message(register_thread, process_id)
        obj.pylinda_con = thread_id, obj.pylinda_con[1]
        return soc

def connect(cport=2102):
    """\brief Connect to a server running on the local machine.

    This function must be called before any Linda primitives are used otherwise a NotConnected exception will be raised.
    \param cport The port to connect to
    """
    global s, process_id, _connected, port

    port = cport

    try:
        import domain_socket
    except ImportError:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            s.connect(("127.0.0.1", port))
        except socket.error:
            return False
    else:
        s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        try:
            s.connect("/tmp/pylinda")
        except socket.error:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                s.connect(("127.0.0.1", port))
            except socket.error:
                return False

    s = Connection(s)

    obj = threading.currentThread()
    obj.pylinda_con = None, s

    _connected = True

    if not run_as_server:
        process_id = message(register_process)
        thread_id = message(register_thread, process_id)
        obj.pylinda_con = thread_id, s
    else:
        process_id = None

    return True

def disconnect():
    message(unregister_thread)
    obj = threading.currentThread()
    if hasattr(obj, "pylinda_con"):
        obj.pylinda_con[1].close()
        delattr(obj, "pylinda_con")
    #thread_connection[thread.get_ident()][1].close()
    #del thread_connection[thread.get_ident()]

def close():
    thread_connection = {}
    s.close()

process_id = None

## \var Tuplespace universe
## A reference to the universal tuplespace
uts = TupleSpace("UTS")
universe = uts

# These options control whether we want to use Unix Domain sockets or Shared Memory
use_domain = True

