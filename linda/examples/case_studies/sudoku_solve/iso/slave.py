#!/usr/bin/python

import sys

import linda

rowtype = linda.Type("row :: int * int * int * int * int * int * int * int * int;")
gridtype = linda.Type("grid :: row * row * row * row * row * row * row * row * row;")

def slave():
    if not linda.connect():
        print "Please start the Linda server first."
        return

    while True:
        tup = linda.uts._in((int, gridtype))
        empty, grid = tup[0], tupToGrid(tup[1])

        print empty

        if empty == 0:
            linda.uts._out(tup, gridtype)
        else:
            i = 0
            for (x, y, val) in fillInOneSquare(grid):
                grid[x][y] = val
                linda.uts._out((empty-1, gridToTup(grid)))
                i += 1
            print i

def fillInOneSquare(grid):
    for x in range(9):
        for y in range(9):
            i = 0
            for v in getValid(grid, x, y):
                yield (x, y, v)
                i += 1
            if i > 0:
                raise StopIteration

def getValid(grid, x, y):
    valid = [1, 2, 3, 4, 5, 6, 7, 8, 9]
    for i in range(9):
        if grid[x][i] is not None and grid[x][i] in valid:
            valid.remove(grid[x][i])
        if grid[i][y] is not None and grid[x][i] in valid:
            valid.remove(grid[i][y])
        if grid[i%3][i/3] is not None and grid[x][i] in valid:
            valid.remove(grid[i%3][i/3])
    return valid

def tupToGrid(tup):
    grid = []
    for row in tup:
        r = []
        for e in row:
            print e
            if e.isLong():
                r.append(e.int)
            else:
                r.append(None)
        grid.append(r)
    print "tupToGrid", grid
    return grid

def gridToTup(grid):
    print "gridToTup", grid
    v =  linda.Value(tuple([tuple(row) for row in grid]), gridtype)
    print "gridToTup", grid

if __name__ == "__main__":
    slave()
