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

from threading import Semaphore

cache_lock = Semaphore()

__cache = {}
getTypeId = Counter(start=1)

def registerType(type):
    assert type.isType()

    cache_lock.acquire()
    try:
        for tid in __cache:
            t = __cache[tid]
            if t == type:
                return tid

        id = getTypeId()
        __cache[id] = type
        return id
    finally:
        cache_lock.release()

def emptyTypeCache():
    __cache = None

def lookupType(id):
    v = __cache[id]
    return v
