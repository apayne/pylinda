#!/usr/bin/python

import sys

import linda

def slave():
    if not linda.connect():
        print "Please start the Linda server first."
        return

    processed, waiting = 0, 1

    while True:
        tup = linda.uts._in((int, ) + (int, ) * (9*9))
        empty, grid = tup[0], tupleToGrid(tup[1:])

        print processed, waiting, empty,
        processed += 1
        waiting -= 1

        if empty == 0:
            linda.uts._out(tup)
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
                    linda.uts._out((empty-1, ) + gridToTuple(grid))
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
    assert len(tup) == 81

    grid = []
    for r in range(9):
        grid.append([(x != -1 and x) or (x == -1 and None) for x in tup[r*9:(r+1)*9]])
    return grid

def gridToTuple(grid):
    singlerow = []
    for row in grid:
        singlerow.extend([(x is None and -1) or (x is not None and x) for x in row])
    return tuple(singlerow)

if __name__ == "__main__":
    slave()
