#!/usr/bin/python

import sys
import time

import linda

if not linda.connect():
    print "Please start the Linda server first."
    sys.exit(1)

class Grid:
    def __init__(self):
        self.grid = tuple([tuple([None for _ in range(9)]) for _ in range(9)])

def gridToValue(grid, memo):
    return linda.Value(grid.grid)

def valueToGrid(val, memo):
    print "convert grid back"
    g = Grid()
    for x in range(9):
        for y in range(9):
            g.grid[x][y] = val[x][y]
    return g

gridtype = linda.Type("grid :: ((int + Nil) * 9) * 9;", Grid, gridToValue, valueToGrid)

def master():
    grid = loadFile(sys.argv[1])

    print "Solving..."
    print gridString(grid)

    empty = sum([len([x for x in row if x is None]) for row in grid.grid])

    start = time.time()
    linda.uts._out((empty, grid))

    tup = linda.uts._in((0, gridtype))

    assert tup[0] == 0, tup[0]

    print "Solved Grid in %f seconds." % (time.time() - start)
    print gridString(tup[1])

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
    g = Grid()
    g.grid = tuple(rows)
    return g

def gridString(grid):
    return "\n".join([" ".join([(x is None and "_") or str(x) for x in row]) for row in grid.grid])

if __name__ == "__main__":
    master()
