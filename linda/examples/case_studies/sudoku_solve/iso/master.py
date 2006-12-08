#!/usr/bin/python

import sys
import time

import linda

rowtype = linda.Type("row :: int * int * int * int * int * int * int * int * int;")
gridtype = linda.Type("grid :: row * row * row * row * row * row * row * row * row;")

def master():
    grid = loadFile(sys.argv[1])

    print "Solving..."
    print gridString(grid)

    empty = sum([len([x for x in row if x is None]) for row in grid])

    if not linda.connect():
        print "Please start the Linda server first."
        return

    start = time.time()
    linda.uts._out((empty, gridToTup(grid)))

    tup = linda.uts._in((0, gridtype))

    assert tup[0] == 0, tup[0]

    print "Solved Grid in %f seconds." % (time.time() - start)
    print gridString(tupleToGrid(tup[1]))

def loadFile(filename):
    fp = open(filename, "r")
    rows = []
    for line in fp:
        if len(rows) == 9:
            break
        cols = line.strip().split(" ")
        if len(cols) != 9:
            raise SystemError, "Row (%s) only has %i columns." % (line, len(cols))
        cols = [(x != "_" and int(x)) or (x == "_" and None) for x in cols]
        rows.append(tuple(cols))
    if len(rows) != 9:
        raise SystemError, "File only has %i rows." % (len(rows), )
    return tuple(rows)

def tupleToGrid(tup):
    grid = []
    for row in tup:
        r = []
        for e in row:
            if e.isLong():
                r.append(int(e))
            else:
                r.append(None)
        grid.append(r)
    return grid

def gridToTup(grid):
    return linda.Value(grid, gridtype)

def gridString(grid):
    return "\n".join([" ".join([(x is None and "_") or str(x) for x in row]) for row in grid])

if __name__ == "__main__":
    master()
