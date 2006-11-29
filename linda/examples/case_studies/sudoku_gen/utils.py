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
        rows.append(cols)
    if len(rows) != 9:
        raise SystemError, "File only has %i rows." % (len(rows), )
    return rows

def tupToGrid(tup):
    assert len(tup) == 81

    grid = []
    for r in range(9):
        grid.append([(x != -1 and x) or (x == -1 and None) for x in tup[r*9:(r+1)*9]])
    return grid

def gridToTup(grid):
    singlerow = []
    for row in grid:
        singlerow.extend([(x is None and -1) or (x is not None and x) for x in row])
    return tuple(singlerow)

def gridString(grid):
    return "\n".join([" ".join([(x is None and "_") or str(x) for x in row]) for row in grid])

