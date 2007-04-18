#!/usr/bin/python

import sys

import linda

if not linda.connect():
    print "Please start the Linda server first."
    sys.exit(1)

class Square:
    def __init__(self, val):
        self.val = val

def squareToValue(sq, memo):
    if sq.val is None:
        v = linda.Nil()
        v.sum_pos = 0
    else:
        v = linda.Value(sq.val)
        v.sum_pos = 1
    return v

def valueToSquare(val, memo):
    if val.isNil():
        return Square(None)
    else:
        return Square(int(val))

sqtype = linda.Type("square :: Nil + int;", Square, squareToValue, valueToSquare)

class Row:
    def __init__(self, row):
        self.row = row

def rowToValue(row, memo):
    return linda.Value(row.row)

def valueToRow(val, memo):
    return Row(list(val))

rowtype = linda.Type("row :: square * 9;", Row, rowToValue, valueToRow)

class Grid:
    def __init__(self, rows):
        self.rows = rows

def gridToValue(grid, memo):
    return linda.Value(grid.rows)

def valueToGrid(val, memo):
    return Grid([row for row in val])

gridtype = linda.Type("grid :: row * 9;", Grid, gridToValue, valueToGrid)

def slave():
    processed, waiting = 0, 1

    while True:
        tup = linda.uts._in((int, gridtype))
        empty, grid = tup[0], tup[1]

        print processed, waiting, empty,
        processed += 1
        waiting -= 1

        if empty == 0:
            linda.uts._out(tup, gridtype)
        else:
            i = 0
            try:
                x, y = getBlankSquare(grid)
            except TypeError: # if getBlankSquare returns none we get a TypeError
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

if __name__ == "__main__":
    slave()
