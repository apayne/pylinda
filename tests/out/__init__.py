import os
import sys
import time

import db as dbsqlite

root = "/home/andrew/python/pylinda/tests/webroot/"

def init_test():
    pass

def start_test(db):
    global start_time

    print "Nodes ready:",
    while len(dbsqlite.get_ready_nodes(db)) < int(sys.argv[1]):
        print len(dbsqlite.get_ready_nodes(db)),
        sys.stdout.flush()
        time.sleep(5)
    print

    start_time = time.time()

def test_running(db):
    files = [x for x in os.listdir(root + "test/uploads") if not x.startswith(".")]
    print "Test still running after %i seconds (%s)" % (int(time.time() - start_time), files)
    if len(files) > int(sys.argv[1]):
        print "Error, too many files uploaded", files
        return False
    elif len(files) < int(sys.argv[1]):
        return True
    else:
        return False

def end_test(db):
    files = os.listdir(root + "test/uploads")
    if len(files) > int(sys.argv[1]):
        print "Error, too many files uploaded", files
        return
    if len(files) < int(sys.argv[1]):
        print "Error, too few files uploaded", files
        return
