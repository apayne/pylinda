#!/usr/bin/python

import os
import sys
import linda
import subprocess

linda.connect()

_, work_ts = linda.uts._in(("work_ts", linda.TupleSpace))

while True:
    # Get a new prime
    tup = work_ts._inp((int, 0, int))
    if tup is None:
        break
    # Spawn a sieve process
    linda.uts._out(("work_ts", work_ts))

    subprocess.Popen(["python", "prime_sieve.py", str(tup[0])])
