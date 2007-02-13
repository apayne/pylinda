import os
import sys
import time

import db as dbsqlite

root = "/home/andrew/python/pylinda/tests/webroot/"

def init_test():
    try:
        os.unlink(root + "block")
    except OSError:
        pass

def start_test(db):
    global start_time

    print "Nodes ready:",
    while len(dbsqlite.get_ready_nodes(db)) < int(sys.argv[1]):
        print len(dbsqlite.get_ready_nodes(db)),
        sys.stdout.flush()
        time.sleep(5)
    print

    open(root + "block", "w").write("\n")
    start_time = time.time()

def test_running(db):
    files = [x for x in os.listdir(root + "test/uploads") if not x.startswith(".")]
    print "Test still running after %i seconds (%s)" % (int(time.time() - start_time), files)
    if len(files) > 1:
        print "Error, too many files uploaded", files
        return False
    elif len(files) == 0:
        return True
    else:
        return False

def end_test(db):
    try:
        os.unlink(root + "block")
    except OSError:
        pass

    files = os.listdir(root + "test/uploads")
    if len(files) > 1:
        print "Error, too many files uploaded", files
        return
    if len(files) < 1:
        print "Error, too few files uploaded", files
        return

    file(root + "test/results/%i" % (int(sys.argv[1]), ), "w").write(file(root + "test/uploads/"+files[0], "r").read())

    os.unlink(root + "test/uploads/"+files[0])
