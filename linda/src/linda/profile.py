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

import thread
import threading
import time

class Profiler:
    def __init__(self):
        self.count = {}
        self.current = {}
        self.total = 0

        self.semaphore = threading.Semaphore()

    def __del__(self):
        print "Profiler data for PyLinda..."
        print self.total
        print "Point\t\t\t\tCount\tTotal\tAvg"
        l = self.count.keys()
        l.sort()
        for i in l:
            point = i
            point = point + "\t" * (4 - len(point)/8)
            print "%s%i\t%.3f\t%.4f" % (point, self.count[i][0], self.count[i][1], self.count[i][1]/self.count[i][0])

    def enter(self, point):
        self.semaphore.acquire()
        if self.current.has_key(thread.get_ident()):
            self.current[thread.get_ident()].append((point, time.time()))
        else:
            self.current[thread.get_ident()] = [(point, time.time())]
        self.semaphore.release()

    def _exit(self):
        self.semaphore.acquire()
        point, t = self.current[thread.get_ident()].pop()
        t = time.time() - t

        if self.count.has_key(point):
            count, ttime = self.count[point]
            self.count[point] = count + 1, ttime + t
        else:
            self.count[point] = 1, t
        self.semaphore.release()

#P = Profiler()
