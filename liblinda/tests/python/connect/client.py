#!/usr/bin/python

import sys

import _linda as linda

if __name__ == "__main__":
    if not linda.connect():
        sys.stderr.write("Error. Cannot connect to server.\n")
        sys.exit(-1)

    linda.uts._out((100, ))

    linda.disconnect()
