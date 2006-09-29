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

import threading
try:
    import readline
except ImportError:
    pass
import time
import os.path
import grammer

from linda import utils
import _linda as linda

keyboard_interrupt = False

class Thread:
    def __init__(self, serverport=2102):
        self.vars = {"TupleSpace": linda.TupleSpace, "ts": linda.TupleSpace, "int": int, "float": float, "str": str}
        self.serverport = serverport

    def run(self):
        start = time.time()
        while time.time() - start < 30:
            sd = linda.connect(self.serverport)
            if sd:
                sd = linda.getSD()
                break
            time.sleep(1)
        if time.time() - start >= 30:
            print "Unable to connect to server. The Monitor is exiting."
            return
        linda.send(sd, ("MONITOR", ))
        linda.recv(sd)
        while True:
            try:
                self.realrun(sd)
                break
            except:
                print "Caught exception"
                raise
        print "Monitor shutting down..."
        linda.disconnect()

    def realrun(self, sd):
        global keyboard_interrupt

        print "Welcome to PyLinda %s." % (linda.version, )
        print "Copyright 2006 Andrew Wilkinson <aw@cs.york.ac.uk>."
        print "Released under the terms of the GNU General Public License."
        print "Type help for instructions."

        while True:
            try:
                text = raw_input("> ")
            except (KeyboardInterrupt, EOFError):
                if self.doCommand(sd, ("quit", )):
                    break
            keyboard_interrupt = False
            try:
                command = grammer.parse("goal", text.strip())
            except SyntaxError, e:
                print "Syntax Error!"
                print e
                continue
            else:
                try:
                    if self.doCommand(sd, command):
                        break
                except SystemError, m:
                    print m
                    pass

    def doCommand(self, sd, command):
        global keyboard_interrupt
        if command is None:
            return
        elif command[0] == "quit":
            print "Shutting down..."
            return True
        elif command[0] == "list":
            linda.send(sd, ("LIST_TS", ))
            r = linda.recv(sd)
            print "TupleSpaces: " + " ".join(eval(r[2]))
        elif command[0] == "inspect":
            ts = self.eval(command[1])
            if not isinstance(ts, linda.TupleSpace):
                print "'%s' is not a tuplespace." % (ts, )
                return
            linda.send(sd, ("INSPECT", ts._id))
            r = linda.recv(sd)
            print r
            if r[1] == "DONT_KNOW":
                print "The local server does not have a partition of %s" % (ts._id, )
                return
            elif r[1] != "RESULT_TUPLE" or not isinstance(r[2], tuple) or len(r[2]) != 5:
                print "Invalid reply: '%s'" % (r, )
                return
            r = r[2]
            print r
            print "References:", [x for x in eval(r[0]) if x != linda.process_id] + ["Monitor" for x in eval(r[0]) if x == linda.process_id]
            print "Threads Blocked:", eval(r[1])
            print "Servers Blocked:", eval(r[4])
            print "Partitions", eval(r[3])
            print "Tuples:", r[2]
            print ""
        elif command[0] == "details":
            print "Node Id:", server.node_id
        elif command[0] == "route":
            print "sending", ("GET_ROUTES", )
            linda.send(sd, ("GET_ROUTES", ))
            cons = linda.recv(sd)
            print cons
            for c in cons[2]:
                if len(c) == 3:
                    print "%s -> %s:%s" % (c[0], c[1], c[2])
                else:
                    print "%s -> %s" % (c[0], c[1])
            else:
                print "No other servers connected"
        elif command[0] == "watch":
            if command[1] is None:
                delay = 10
            else:
                delay = min(0.1, command[1])
            while True:
                self.doCommand(command[2])
                if keyboard_interrupt:
                    keyboard_interrupt = False
                    return
                time.sleep(delay)
        elif command[0] == "out":
            ts = command[1]
            tup = command[2]
            if tup is None and ts[0] != "funccall":
                raise SystemError, "Expected 'out TS TUPLE'."
            elif tup is None:
                tup = ts[2]
                ts = ts[1]
            ts = self.eval(ts)
            tup = self.eval(tup)
            if not isinstance(ts, linda.TupleSpace):
                raise SystemError, "'%s' is not a tuplespace" % (ts, )
            if not isinstance(tup, tuple):
                raise SystemError, "'%s' is not a tuple" % (tup, )
            ts._out(tup)
        elif command[0] == "expr":
            print self.eval(command[1])
        elif command[0] == "help":
            print "Possible commands..."
            print "list - Print list of all tuplespaces on the server"
            print "inspect <tsid> - List the tuples in the given tuplespace"
            print "watch <delay> <command> - Repeat the given command every <delay> seconds.\n    If left out <delay> defaults to 10."
            print "quit - Shut down the server."
        else:
            raise SystemError, "Unknown command %s" % (command, )

    def eval(self, val):
        if val[0] == "val":
            return val[1]
        elif val[0] == "tsid":
            if val[1] == "uts": val = (val[0], "UTS")
            if not utils.isTupleSpaceId(val[1]):
                raise SystemError, "'%s' is not a valid Tuple Space Id" % (val[1], )
            ts = linda.TupleSpace(val[1])
            if val[1] != "UTS":
                ts._addreference(linda.process_id)
            return ts
        elif val[0] == "var":
            if not self.vars.has_key(val[1]):
                raise SystemError, "No variable '%s'." % (val[1], )
            else:
                return self.vars[val[1]]
        elif val[0] == "tuple":
            return tuple(map(self.eval, val[1]))
        elif val[0] == "funccall":
            func = self.eval(val[1])
            args = self.eval(val[2])
            try:
                r = apply(func, args)
            except TypeError:
                raise SystemError, "Unable to call %s%s." % (func, args)
            return r
        elif val[0] == "assign":
            lexpr = val[1]
            rexpr = self.eval(val[2])
            if lexpr[0] == "var":
                self.vars[lexpr[1]] = rexpr
            elif lexpr[0] == "tuple":
                if not isinstance(rexpr, tuple):
                    raise SystemError, "'%s' cannot be unpacked into a tuple." % (rexpr, )
                elif len(lexpr[1]) != len(rexpr):
                    raise SystemError, "Tuple of length %i cannot be unpacked into %i." % (len(rexpr), len(lexpr))
                else:
                    for l, r in zip(lexpr[1], rexpr):
                        if l[0] == "skip":
                            continue
                        elif l[0] == "var":
                            self.vars[l[1]] = r
                        else:
                            raise SystemError, "Cannot unpack %s into %s." % (r, l)
            return rexpr
        elif val[0] in ["in", "inp", "rd", "rdp"]:
            ts = val[1]
            tup = val[2]
            if tup is None and ts[0] != "funccall":
                raise SystemError, "Expected '%s TS TUPLE'." % (val[0], )
            elif tup is None:
                tup = ts[2]
                ts = ts[1]
            ts = self.eval(ts)
            tup = self.eval(tup)
            if not isinstance(ts, linda.TupleSpace):
                raise SystemError, "'%s' is not a tuplespace" % (ts, )
            if not isinstance(tup, tuple):
                raise SystemError, "'%s' is not a tuple" % (tup, )
            if val[0] == "in":
                return ts._in(tup)
            elif val[0] == "inp":
                return ts._inp(tup)
            elif val[0] == "rd":
                return ts._rd(tup)
            elif val[0] == "rdp":
                return ts._rdp(tup)
        elif val[0] == "+":
            a = self.eval(val[1])
            b = self.eval(val[2])
            try:
                return a + b
            except TypeError:
                raise SystemError, "Error. Cannot add %s and %s." % (str(a), str(b))
        elif val[0] == "-":
            a = self.eval(val[1])
            b = self.eval(val[2])
            try:
                return a - b
            except TypeError:
                raise SystemError, "Error. Cannot subt %s and %s." % (str(a), str(b))
        elif val[0] == "*":
            a = self.eval(val[1])
            b = self.eval(val[2])
            try:
                return a * b
            except TypeError:
                raise SystemError, "Error. Cannot mul %s and %s." % (str(a), str(b))
        elif val[0] == "/":
            a = self.eval(val[1])
            b = self.eval(val[2])
            try:
                return a / b
            except TypeError:
                raise SystemError, "Error. Cannot div %s and %s." % (str(a), str(b))
        elif val[0] == "skip":
            raise SystemError, "_ is only allowed as part of a tuple on the left of an assignment."
        else:
            raise SystemError, "Error. Unable to evaluate %s." % (val, )

if __name__ == "__main__":
    Thread().run()
