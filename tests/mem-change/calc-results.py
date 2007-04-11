#!/usr/bin/python

import os
import math

files = os.listdir("results")

tradf = [x for x in files if x.startswith("traditional")]
regf = [x for x in files if x.startswith("registered-")]
reg2f = [x for x in files if x.startswith("registered2-")]
unregf = [x for x in files if x.startswith("unregistered")]

trad_network = [0 for _ in range(51)]
reg_network = [0 for _ in range(51)]
reg2_network = [0 for _ in range(51)]
unreg_network = [0 for _ in range(51)]

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

if len(tradf) > 0:
    trad_network = [x/len(tradf) for x in trad_network]
if len(regf) > 0:
    reg_network = [x/len(regf) for x in reg_network]
if len(reg2f) > 0:
    reg2_network = [x/len(reg2f) for x in reg2_network]
if len(unregf) > 0:
    unreg_network = [x/len(unregf) for x in unreg_network]

fp = open("in-network.txt", "w")
for i in range(0, 50, 3):
    fp.write("%i\t%f\t%f\t%f\t%f\n" % (int(i), trad_network[i]/1024, reg_network[i]/1024, reg2_network[i]/1024, unreg_network[i]/1024))
fp.close()
