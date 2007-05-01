import sys
import linda

linda.connect()

prime = int(sys.argv[1])

# Let the master know we've found a prime
_, prime_ts = linda.uts._in(("prime_ts", linda.TupleSpace))
prime_ts._out((prime, ))

_, work_ts = linda.uts._in(("work_ts", linda.TupleSpace))

next_prime = 0

count = 0
outcount = 0
while True:
    tup = work_ts._inp((int, prime, count))
    count += 1
    if tup is None:
        break
    # check if the number being tested is divisible by us
    if tup[0] % prime == 0:
        continue
    elif next_prime == 0:
        # there is no prime larger than us, so this must be prime
        work_ts._out((tup[0], 0, 0))
        linda.uts._out(("prime_ts", prime_ts))
        next_prime = tup[0]
    else:
        work_ts._out((tup[0], next_prime, outcount))
        outcount += 1
