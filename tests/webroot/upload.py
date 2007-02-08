#!/usr/bin/python

import os
import cgi
import cgitb; cgitb.enable()

import db

form = cgi.FieldStorage()

fp = open("test/uploads/%s" % (form["filename"].value, ), "wb")

fileitem = form["file"]

while 1:
    chunk = fileitem.file.read(100000)
    if not chunk: break
    fp.write(chunk)
fp.close()

print "Content-Type: text/plain"
print
print "Done"
