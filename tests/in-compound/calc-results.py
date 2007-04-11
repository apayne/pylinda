#!/usr/bin/python

import os
import math

files = os.listdir("results")

tradf = [x for x in files if x.startswith("traditional")]
regf = [x for x in files if x.startswith("registered")]
reg2f = [x for x in files if x.startswith("reg2")]
unregf = [x for x in files if x.startswith("unreg2")]

trad = [0 for _ in range(51)]
reg = [0 for _ in range(51)]
reg2 = [0 for _ in range(51)]
unreg = [0 for _ in range(51)]
unreg2 = [0 for _ in range(51)]

for f in tradf:
    fp = open("results/"+f)
    for line in fp.readlines():
        trad[int(line.split("\t")[0])] += float(line.split("\t")[1])
for f in regf:
    fp = open("results/"+f)
    for line in fp.readlines():
        reg[int(line.split("\t")[0])] += float(line.split("\t")[1])
for f in reg2f:
    fp = open("results/"+f)
    for line in fp.readlines():
        reg2[int(line.split("\t")[0])] += float(line.split("\t")[1])
for f in unregf:
    fp = open("results/"+f)
    for line in fp.readlines():
        unreg[int(line.split("\t")[0])] += float(line.split("\t")[1])

if len(tradf) > 0:
    trad = [x/len(tradf) for x in trad]
if len(regf) > 0:
    reg = [x/len(regf) for x in reg]
if len(reg2f) > 0:
    reg2 = [x/len(reg2f) for x in reg2]
if len(unregf) > 0:
    unreg = [x/len(unregf) for x in unreg]

fp = open("in-compound.txt", "w")
for i in range(0, 51, 3):
    fp.write("%i\t%f\t%f\t%f\t%f\n" % (int(i), trad[i], reg[i], reg2[i], unreg[i]))
fp.close()
