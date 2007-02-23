#!/usr/bin/python

import os
import math

files = os.listdir("results")

tradf = [x for x in files if x.startswith("traditional-")]
regf = [x for x in files if x.startswith("registered-")]
reg2f = [x for x in files if x.startswith("registered2-")]
unregf = [x for x in files if x.startswith("unregistered-")]

trad = ["" for _ in range(51)]
reg = ["" for _ in range(51)]
reg2 = ["" for _ in range(51)]
unreg = ["" for _ in range(51)]

for f in tradf:
    fp = open("results/"+f)
    for line in fp.readlines():
        if trad[int(line.split("\t")[0])] == "":
            trad[int(line.split("\t")[0])] = float(line.split("\t")[1])
        else:
            trad[int(line.split("\t")[0])] += float(line.split("\t")[1])
for f in regf:
    fp = open("results/"+f)
    for line in fp.readlines():
        if reg[int(line.split("\t")[0])] == "":
            reg[int(line.split("\t")[0])] = float(line.split("\t")[1])
        else:
            reg[int(line.split("\t")[0])] += float(line.split("\t")[1])
for f in reg2f:
    fp = open("results/"+f)
    for line in fp.readlines():
        if reg2[int(line.split("\t")[0])] == "":
            reg2[int(line.split("\t")[0])] = float(line.split("\t")[1])
        else:
            reg2[int(line.split("\t")[0])] += float(line.split("\t")[1])
for f in unregf:
    fp = open("results/"+f)
    for line in fp.readlines():
        if unreg[int(line.split("\t")[0])] == "":
            unreg[int(line.split("\t")[0])] = float(line.split("\t")[1])
        else:
            unreg[int(line.split("\t")[0])] += float(line.split("\t")[1])

if len(tradf) > 0:
    trad = ["" if x == "" else x/len(tradf) or ""  for x in trad]
if len(regf) > 0:
    reg = ["" if x == "" else x/len(regf) for x in reg]
if len(reg2f) > 0:
    reg2 = ["" if x == "" else x/len(reg2f) for x in reg2]
if len(unregf) > 0:
    unreg = ["" if x == "" else x/len(unregf) for x in unreg]

fp = open("in-single.txt", "w")
for i in range(0, 51, 3):
    fp.write("%i\t%s\t%s\t%s\t%s\n" % (int(i), trad[i], reg[i], reg2[i], unreg[i]))
fp.close()
