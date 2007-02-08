#!/usr/bin/python

import os
import cgi
import cgitb; cgitb.enable()

import db

file("master", "w").write("\n")

print "Content-Type: text/plain"
print
print "Done."
