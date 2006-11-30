#!/usr/bin/python

import sys

import linda

rowtype = linda.Type("row :: int * int * int * int * int * int * int * int * int;")
gridtype = linda.Type("grid :: row * row * row * row * row * row * row * row * row;")

def slave():
    if not linda.connect():
        print "Please start the Linda server first."
        return

    processed, waiting = 0, 1

    while True:
        tup = linda.uts._in((int, gridtype))
        empty, grid = tup[0], tupleToGrid(tup[1])

        print processed, waiting, empty,
        processed += 1
        waiting -= 1

        if empty == 0:
            linda.uts._out(tup, gridtype)
        else:
            i = 0
            try:
                x, y = getBlankSquare(grid)
            except TypeError:
                pass
            else:
                for val in getValid(grid, x, y):
                    print (x, y, val),
                    grid[x][y] = val
                    linda.uts._out((empty-1, gridToTuple(grid)))
                    i += 1
                print
                waiting += i

def getBlankSquare(grid):
    for x in range(9):
        for y in range(9):
            if grid[x][y] == None:
                return x, y

def getValid(grid, x, y):
    valid = [1, 2, 3, 4, 5, 6, 7, 8, 9]
    for i in range(9):
        if grid[x][i] is not None and grid[x][i] in valid:
            valid.remove(grid[x][i])
        if grid[i][y] is not None and grid[i][y] in valid:
            valid.remove(grid[i][y])
        if grid[(x/3)*3+i%3][(y/3)*3+i/3] is not None and grid[(x/3)*3+i%3][(y/3)*3+i/3] in valid:
            valid.remove(grid[(x/3)*3+i%3][(y/3)*3+i/3])
    return valid

def tupleToGrid(tup):
    grid = []
    for row in tup:
        r = []
        for e in row:
            if e.isLong():
                r.append(e.int)
            else:
                r.append(None)
        grid.append(r)
    return grid

def gridToTuple(grid):
    return linda.Value(tuple([tuple(row) for row in grid]), gridtype)

if __name__ == "__main__":
    slave()
