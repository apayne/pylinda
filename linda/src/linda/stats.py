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

import os
import threading

import connections
import utils

def _in(tup):
    if tup[0] == "mem":
        return utils.encode((tup[0], ) + getMemSize())
    else:
        return utils.encode((tup[0], getstat(tup[0])))

stats = {}

def getstat(stat):
    try:
        return stats[stat]
    except KeyError:
        return 0

def inc_stat(stat):
    try:
        stats[stat] += 1
    except KeyError:
        stats[stat] = 1

def dec_stat(stat):
    try:
        stats[stat] -= 1
    except KeyError:
        stats[stat] = 0

def getMemSize():
    print "get mem"
    data = open("/proc/%i/stat" % (os.getpid(), ), "r").readline()
    return int(data.split(" ")[21]), int(data.split(" ")[20])