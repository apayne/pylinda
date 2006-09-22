#!/usr/bin/python

import sys

import _linda_server

if __name__ == "__main__":
    if not _linda_server.serve():
        sys.exit(-1)

    sd = _linda_server.accept()

    m = _linda_server.recv(sd)
    print m
    if m[0] == "OUT":
        _linda_server.send(sd, ("DONE", ))

    _linda_server.server_disconnect()
