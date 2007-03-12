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

from utils import Counter

from threading import RLock

import _linda_server

cache_lock = RLock()

__cache = {}

def saveIso(t1, t2, iso):
    if isinstance(t1, _linda_server.Value):
        t1 = t1.type_id
    if isinstance(t2, _linda_server.Value):
        t2 = t2.type_id
    cache_lock.acquire()
    print "save iso for", t1, t2
    try:
        __cache[(t1, t2)] = iso
    finally:
        cache_lock.release()

def emptyIsoCache():
    __cache = None

def lookupIso(t1, t2):
    if isinstance(t1, _linda_server.Value):
        t1 = t1.type_id
    if isinstance(t2, _linda_server.Value):
        t2 = t2.type_id
    try:
        v = __cache[(t1, t2)]
    except KeyError:
        v = compare(type_cache.lookupType(t1), type_cache.lookupType(t2))
        saveIso(t1, t2, v)
    return v

def clearIsos(t):
    if isinstance(t, _linda_server.Value):
        t = t.type_id
    cache_lock.acquire()
    try:
        for t1, t2 in __cache.keys():
            if t1 == t or t2 == t:
                del __cache[(t1, t2)]
    finally:
        cache_lock.release()

from match import compare
import type_cache
