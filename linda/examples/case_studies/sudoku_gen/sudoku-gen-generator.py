#!/usr/bin/python

import sys
import time

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

def 


if __name__ == "__main__":
    generator()
