import sqlite3
import time

from tables import *

def connect():
    con = sqlite3.connect('/home/andrew/python/pylinda/tests/webroot/db.sqlite')
    con.row_factory = sqlite3.Row
    return con

lock_id = None
def lock(c):
    global lock_id
    try:
        c.execute('SELECT * FROM lock ORDER BY id ASC')
    except sqlite3.OperationalError:
        create_lock_table(c)

    r = c.execute('INSERT INTO lock (id) VALUES (NULL)')
    lock_id = c.execute('select last_insert_rowid()').fetchone()[0]
    c.commit()

    count = 0
    while c.execute('SELECT * FROM lock ORDER BY id ASC').fetchone()['id'] != lock_id:
        time.sleep(1)
        count += 1
        if count > 60:
            c.execute('DELETE FROM lock WHERE id=?', (lock_id, ))
            raise SystemError, "Timeout"

def unlock(c):
    global lock_id
    c.execute('DELETE FROM lock WHERE id=?', (lock_id, ))
    c.commit()
    lock_id = None

def list_nodes(c):
    try:
        r = c.execute('SELECT * FROM nodes')
    except sqlite3.OperationalError:
        return []

    rows = r.fetchall()
    return [(n["addr"], n["type"]) for n in rows]

def get_type(c, addr):
    try:
        r = c.execute('SELECT * FROM nodes WHERE addr=?', (addr, ))
    except sqlite3.OperationalError:
        create_node_table(c)
        return None

    row = r.fetchone()
    if row is None:
        return None
    else:
        return row["type"]

def get_master(c):
    try:
        r = c.execute('SELECT * FROM nodes WHERE type="master"')
    except sqlite3.OperationalError:
        create_node_table(c)
        return None

    row = r.fetchone()
    if row is None:
        return None
    else:
        return row["addr"]

def make_node_ready(c, addr):
    try:
        r = c.execute('SELECT * FROM nodestatus WHERE addr=?', (addr, ))
    except sqlite3.OperationalError:
        create_nodestatus_table(c)
        r = c.execute('SELECT * FROM nodestatus WHERE addr=?', (addr, ))

    if len(r.fetchall()) == 0:
        c.execute('INSERT INTO nodestatus (addr, status) VALUES (?,"ready")', (addr, ))
    else:
        c.execute('UPDATE nodestatus SET status = "ready" WHERE addr=?', (addr, ))

def get_ready_nodes(c):
    try:
        r = c.execute('SELECT * FROM nodestatus')
    except sqlite3.OperationalError:
        return []

    rows = r.fetchall()
    return [(n["addr"], n["status"]) for n in rows]
