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
    rows = []
    for row in grid.grid:
        cols = []
        for col in row:
            if col is None:
                v = linda.Nil()
                v.sum_pos = 1
            else:
                v = linda.Value(col)
                v.sum_pos = 0
            cols.append(v)
        rows.append(tuple(cols))
    return linda.Value(tuple(rows))

def valueToGrid(val, memo):
    g = Grid()
    for x in range(9):
        for y in range(9):
            if val[x][y].sum_pos == 1:
                v = None
            else:
                v = int(val[x][y])
            g.grid[x][y] = v
    return g

gridtype = linda.Type("grid :: ((int + Nil) * 9) * 9;", Grid, gridToValue, valueToGrid)

def master():
    grid = Grid() # Create an empty grid

    work_ts = linda.TupleSpace()
    linda.uts._out(("work_ts", work_ts)

    func = linda.loadCodeFromFile("difficulty_func.min")
    linda.uts._out(("difficulty_func", func))

    start = time.time()
    linda.uts._out((empty, grid))

    tup = linda.uts._in((gridtype, ))

    assert tup[0] == 0, tup[0]

    print "Generated Grid in %f seconds." % (time.time() - start)
    print gridString(tup[1])

def gridString(grid):
    return "\n".join([" ".join([(x is None and "_") or str(x) for x in row]) for row in grid.grid])

if __name__ == "__main__":
    master()
