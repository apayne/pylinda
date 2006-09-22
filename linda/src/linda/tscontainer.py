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

## \namespace tscontainer
## \brief This module contains the class that contains various tuplespaces
## \internal
##
## \author Andrew Wilkinson <aw@cs.york.ac.uk>
##

import threading
from tuplespace import TupleSpace
import sys
import gc

## \class TupleSpaceContainer
## \internal
## \brief This class wraps a dictionary containing all the tuplespaces local to the server, indexed by their tuplespace id
##
## It exposes the most useful methods from a dictionary in addition to a couple of convient functions for dealing with
## tuplespaces.
##
## \author Andrew Wilkinson <aw@cs.york.ac.uk>
##
class TupleSpaceContainer:
    def __init__(self):
        self.ts = {}

        self.semaphore = threading.Semaphore()

    ## \brief Returns the number of tuplespaces contained within this container object
    ## \internal
    def __len__(self):
        self.semaphore.acquire()
        try:
            return len(self.ts.keys())
        finally:
            self.semaphore.release()

    ## \brief Returns an iterator that iterates through the tuplespace ids
    ## \internal
    def __iter__(self):
        def iter(self):
            keys = self.ts.keys()
            for i in keys:
                if i not in self.ts.keys():
                    continue
                yield i
        return iter(self)

    ## \brief Returns a tuplespace with the given id
    ## \internal
    def __getitem__(self, item):
        return self.ts[item]

    ## \brief Returns true if this container object has a tuplespace with the given id
    ## \internal
    def has_key(self, item):
        return self.ts.has_key(item)

    ## \brief Return a list of all tuplespace ids
    ## \internal
    def keys(self):
        return self.ts.keys()

    ## \brief Create a new Tuplespace with the given id
    ## \internal
    def newTupleSpace(self, id):
        self.semaphore.acquire()
        try:
            self.ts[id] = TupleSpace(id)
        finally:
            self.semaphore.release()

    ## \brief Add a new reference to the given tuplespace
    ## \internal
    ## \param ts The tuplespace id
    ## \param ref The object creating the new reference
    def addReference(self, ts, ref):
        self.semaphore.acquire()
        try:
            ts_obj = self.ts[ts]
        finally:
            self.semaphore.release()
        ts_obj.addreference(ref)

    ## \brief Remove a reference from the given object to the tuplespace
    ## \internal
    ## This function may cause the tuplespace to be completly removed if this is the last reference.
    ## If the given object has more than one reference to this tuplespace then only the first reference is deleted.
    ## \param ts The tuplespace id
    ## \param ref The object deleting the reference
    def deleteReference(self, ts, ref):
        self.semaphore.acquire()
        try:
            try:
                ts_obj = self.ts[ts]
            except KeyError:
                return
        finally:
            self.semaphore.release()
        ts_obj.removereference(ref)
        self.garbage(ts_obj)

    ## \brief Remove all references from the given object to the tuplespace
    ## \internal
    ## This function may cause the tuplespace to be completly removed this removes the last reference.
    ## \param ts The tuplespace id
    ## \param ref The object deleting their references
    def deleteAllReferences(self, ts, ref):
        self.semaphore.acquire()
        try:
            try:
                ts_obj = self.ts[ts]
            except KeyError:
                return
        finally:
            self.semaphore.release()
        ts_obj.removeanyreferences(ref)
        self.garbage(ts_obj)

    def forceGarbage(self):
        self.semaphore.acquire()
        try:
            for ts in self.ts.keys():
                self.ts[ts].refs = []
                self.semaphore.release()
                self.garbage(self.ts[ts])
                self.semaphore.acquire()
        finally:
            self.semaphore.release()

    ## \brief Test if the given tuplespace should be deleted.
    ## \internal
    def garbage(self, ts):
        if len(ts.refs) == 0:
            # The function returns the number of reference remaining, if it returns 0 then there are no references
            # and we can delete the tuplespace
            self.semaphore.acquire()
            try:
                try:
                    del self.ts[ts._id]
                except KeyError:
                    # another thread deleted this tuplespace while we were blocked on the semaphore
                    return
            finally:
                self.semaphore.release()

            del ts
            gc.collect() # Garbage collection isn't always run, so force a collection now to remove the tuplespace fully
