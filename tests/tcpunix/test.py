#!/usr/bin/python

import socket
import os
import sys
import time

if len(sys.argv) == 1:
     print "Please run ./client.py unix or ./client.py tcp"
     sys.exit(0)

def client():
    if sys.argv[1] == "unix":
       s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
       s.connect("/tmp/unixtcp_test")
    elif sys.argv[1] == "tcp":
       s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
       s.connect(("127.0.0.1", 10000))

    data = "a" * 1024 * 1024 * 10

    start = time.time()
    for i in range(100):
        s.send(data)
    end = time.time()

    print "Sent 1000MB in %.3f seconds. %.3f MB/s" % (end - start, 1024 * 1024 * 10 / (end - start))

def server():
    try:
        os.unlink("/tmp/unixtcp_test")
    except OSError:
        pass
    if sys.argv[1] == "unix":
       s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
       s.bind("/tmp/unixtcp_test")
    elif sys.argv[1] == "tcp":
       s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
       s.bind(("127.0.0.1", 10000))

    s.listen(1)
    s, addr = s.accept()
    size = 0
    expect = 100 * 10 * 1024 * 1024
    start = time.time()
    while size < expect:
         size += len(s.recv(expect - size))
    end = time.time()

    print "Recv 1000MB in %.3f seconds. %.3f MB/s" % (end - start, 1024 * 1024 * 10 / (end - start))

    try:
        os.unlink("/tmp/unixtcp_test")
    except OSError:
        pass

if os.fork() == 0:
    time.sleep(1)
    client()
else:
    server()
