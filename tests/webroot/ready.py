#!/usr/bin/python

import os
import cgi
import cgitb; cgitb.enable()

import db

con = db.connect()

db.lock(con)
try:
    db.make_node_ready(con, os.environ['REMOTE_ADDR'])
finally:
    db.unlock(con)

print "Content-Type: text/plain"
print
print "Done."
