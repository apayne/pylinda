import sqlite3

def create_lock_table(c):
    try:
        c.execute('CREATE TABLE lock(id INTEGER PRIMARY KEY)')
    except sqlite3.OperationalError:
        pass

def create_node_table(c):
    try:
        c.execute('CREATE TABLE nodes(addr, type)')
    except sqlite3.OperationalError:
        pass

def create_nodestatus_table(c):
    try:
        c.execute('CREATE TABLE nodestatus(addr, status)')
    except sqlite3.OperationalError:
        pass
