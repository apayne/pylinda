#!/usr/bin/python

import os
import math

files = os.listdir("results")

d = {}
real = []

for f in files:
    fp = open("results/"+f)
    for line in fp.readlines():
        if line.startswith("!"):
            continue
        if line.endswith(",\n"):
            line = line[:-4] + "ms\n"
        real.append(float(line[:-3]))
        try:
            d[round(float(line[:-3]))]
        except KeyError:
            d[round(float(line[:-3]))] = 1
        else:
            d[round(float(line[:-3]))] = d[round(float(line[:-3]))] + 1

l = d.keys()
l.sort()
fp = open("latency.txt", "w")
for i in l:
    fp.write("%i\t%i\n" % (int(i), d[i]))
fp.close()

print len(real), "data points"
avg = sum(real)/len(real)
print "Average:", avg

print sum([(x - avg)*(x - avg) for x in real])
print sum([(x - avg)*(x - avg) for x in real])/len(real)
print "Std Div:", math.sqrt(sum([(x - avg)*(x - avg) for x in real])/len(real))

print " =  1ms:", len([x for x in real if x <=1]), "%f%%" % (100*len([x for x in real if x <= 1])/len(real), )
print " <  5ms:", len([x for x in real if x < 5]), "%f%%" % (100*len([x for x in real if x < 5])/len(real), )
print " < 10ms:", len([x for x in real if x < 10]), "%f%%" % (100*len([x for x in real if x < 10])/len(real), )
print " < 15ms:", len([x for x in real if x < 15]), "%f%%" % (100*len([x for x in real if x < 15])/len(real), )
print " < 20ms:", len([x for x in real if x < 20]), "%f%%" % (100*len([x for x in real if x < 20])/len(real), )
print " < 25ms:", len([x for x in real if x < 25]), "%f%%" % (100*len([x for x in real if x < 25])/len(real), )
print " > 100ms:", len([x for x in real if x > 100]), "%f%%" % (100*len([x for x in real if x > 100])/len(real), )
