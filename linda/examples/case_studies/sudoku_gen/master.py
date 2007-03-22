#!/usr/bin/python

import sys
import time

import linda

class Grid:
    def __init__(self):
        self.grid = []

def master():
    grid = loadFile(sys.argv[1])

    print "Solving..."
    print gridString(grid)

    empty = sum([len([x for x in row if x is None]) for row in grid])

    if not linda.connect():
        print "Please start the Linda server first."
        return

    start = time.time()
    linda.uts._out((empty, ) + gridToTup(grid))

    tup = linda.uts._in((0, ) + (int, ) * (9*9))

    print "Solved Grid in %f seconds." % (time.time() - start)
    print gridString(tupToGrid(tup[1:]))

if __name__ == "__main__":
    master()

