#!/usr/bin/python

import os
import math

files = os.listdir("results")

tradf = ["trad/" + x for x in os.listdir("results/trad") if not x.startswith(".")]
regf = ["reg/" + x for x in os.listdir("results/reg") if not x.startswith(".")]
reg2f = ["reg2/" + x for x in os.listdir("results/reg2") if not x.startswith(".")]
unregf = ["unreg/" + x for x in os.listdir("results/unreg") if not x.startswith(".")]
unreg2f = ["unreg2/" + x for x in os.listdir("results/unreg2") if not x.startswith(".")]

trad_network = [0 for _ in range(51)]
reg_network = [0 for _ in range(51)]
reg2_network = [0 for _ in range(51)]
unreg_network = [0 for _ in range(51)]
unreg2_network = [0 for _ in range(51)]

for f in tradf:
    fp = open("results/"+f)
    for line in fp.readlines():
        trad_network[int(line.split("\t")[0])] += float(line.split("\t")[1])
for f in regf:
    fp = open("results/"+f)
    for line in fp.readlines():
        reg_network[int(line.split("\t")[0])] += float(line.split("\t")[1])
for f in reg2f:
    fp = open("results/"+f)
    for line in fp.readlines():
        reg2_network[int(line.split("\t")[0])] += float(line.split("\t")[1])
for f in unregf:
    fp = open("results/"+f)
    for line in fp.readlines():
        unreg_network[int(line.split("\t")[0])] += float(line.split("\t")[1])
for f in unreg2f:
    fp = open("results/"+f)
    for line in fp.readlines():
        unreg2_network[int(line.split("\t")[0])] += float(line.split("\t")[1])

if len(tradf) > 0:
    trad_network = [x/len(tradf) for x in trad_network]
if len(regf) > 0:
    reg_network = [x/len(regf) for x in reg_network]
if len(reg2f) > 0:
    reg2_network = [x/len(reg2f) for x in reg2_network]
if len(unregf) > 0:
    unreg_network = [x/len(unregf) for x in unreg_network]
if len(unreg2f) > 0:
    unreg2_network = [x/len(unreg2f) for x in unreg2_network]

fp = open("mem-change.txt", "w")
for i in range(0, 50, 3):
    fp.write("%i\t%f\t%f\t%f\t%f\t%f\n" % (int(i), trad_network[i], reg_network[i], reg2_network[i], unreg_network[i], unreg2_network[i]))
fp.close()
