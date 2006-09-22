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

import linda
import sys
import random
import os

#
# Process command line options
#
from optparse import OptionParser

parser = OptionParser(version="%prog 1.0")
parser.add_option("-p", "--connect-port", type="int", dest="connectport", default=2102,
                  help="The port to connect to.")

(options, args) = parser.parse_args()

linda.connect(options.connectport)
#
#
#

# Lists of men's and women's names
men = ["AIDAN", "JADEN", "CADEN", "ETHAN", "CALEB", "DYLAN", "JACOB", "JORDAN", "LOGAN", "HAYDEN", "CONNOR", "RYAN", "MORGAN", "CAMERON", "ANDREW", "JOSHUA", "NOAH", "MATTHEW", "ADDISON", "ASHTON"]
women = ["MADISON", "EMMA", "ABIGAIL", "RILEY", "CHLOE", "HANNAH", "ALEXIS", "ISABELLA", "MACKENZIE", "TAYLOR", "OLIVIA", "HAILEY", "PAIGE", "EMILY", "GRACE", "AVA", "AALIYAH", "ALYSSA", "FAITH", "BRIANNA"]

ts = None

# Function that represents a Man
def Man(name):
    # Get the tuplespace we're working in
    ts = linda.uts._in(("ts", linda.TupleSpace))[1]

    # Randomize the order we want to propose to people in
    order = women[:]
    random.shuffle(order)

    # propose to each woman in order
    for w in order:
        print name + " proposing to " + w
        ts._out(("propose", name, w))

        # wait to see if we get rejected. If a deadlock is reached we've finished and should exit
        r = ts._inp(("reject", name, w))
        if r is None:
            break

    print "%s ended with %s (%i)" % (name, w, order.index(w) + 1)

# Function that represents a Woman
def Woman(name):
    # Returns the person we'd rather marry
    def BestOf(fiance, suitor):
        if fiance is None:
            return suitor
        elif order.index(fiance) < order.index(suitor):
            return fiance
        else:
            return suitor
    # Returns the person we'd rather not marry
    def WorstOf(fiance, suitor):
        if BestOf(fiance, suitor) == fiance:
            return suitor
        else:
            return fiance

    # Get the tuplespace we're working in
    ts = linda.uts._in(("ts", linda.TupleSpace))[1]

    # Randomize the order we want to marry people in
    order = men[:]
    random.shuffle(order)

    fiance = None
    while 1:
        # wait to be proposed to. If this returns None then a deadlock was reached
        p = ts._inp(("propose", str, name))
        if p is None:
            break
        else:
            suitor = p[1]

        # Choose who to reject and who to keep
        reject = WorstOf(fiance, suitor)
        fiance = BestOf(fiance, suitor)

        if reject is not None:
            print "%s rejecting %s for %s" % (name, reject, fiance)
            ts._out(("reject", reject, name))
        else:
            print "%s accepting %s" % (name, fiance)

    print "%s ended with %s (%i)" % (name, fiance, order.index(fiance) + 1)

# Create a tuplespace for us to work in
if args[0] == "man" and args[1] == "0":
    print "create tuplespace"
    ts = linda.TupleSpace()
    for i in range(len(men) + len(women)):
        linda.uts._out(("ts", ts))
    del ts

if args[0] == "man":
    Man(men[int(sys.argv[2])])

elif args[0] == "men":
    for i in range(0,len(men)):
        print "spawn", i
        os.spawnl(os.P_NOWAIT, "marriage.py", "marriage.py", "man", str(i))
        #if os.fork() == 0:
        #    os.execlp("./marriage.py", "marriage.py", "man", str(i))

elif args[0] == "woman":
    Woman(women[int(sys.argv[2])])

elif args[0] == "women":
    for i in range(0,len(women)):
        print "spawn", i
        os.spawnl(os.P_NOWAIT, "marriage.py", "marriage.py", "woman", str(i))
        #if os.fork() == 0:
        #    os.execlp("./marriage.py", "marriage.py","woman", str(i))
