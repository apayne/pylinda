#!/usr/bin/python

import os
import cgi
import cgitb; cgitb.enable()

import db

con = db.connect()

db.lock(con)
try:
    type = db.get_type(con, os.environ['REMOTE_ADDR'])

    master = db.get_master(con)

    if type is None:
        if master:
            con.execute('INSERT INTO nodes (addr, type) VALUES (?,?)', (os.environ['REMOTE_ADDR'], "slave"))
            con.commit()
            type = "slave"
        else:
            con.execute('INSERT INTO nodes (addr, type) VALUES (?,?)', (os.environ['REMOTE_ADDR'], "master"))
            con.commit()
            type = "master"
            master = os.environ['REMOTE_ADDR']
finally:
    db.unlock(con)

print "Content-Type: text/plain"
print

if type == "master":
    fp = open("test/master")
elif type == "slave":
    fp = open("test/slave")
else:
    raise SystemError, type

print fp.read() % {"master_addr": master, "this_addr": os.environ['REMOTE_ADDR'], "test_dir": "http://scoobies.dyndns.org:2223/pylinda/test", "main_dir": "http://scoobies.dyndns.org:2223/pylinda"}

