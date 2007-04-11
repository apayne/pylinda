#!/usr/bin/python

import os
import math

files = os.listdir("results")

tradf = [x for x in files if x.startswith("traditional-")]
regf = [x for x in files if x.startswith("registered-")]
reg2f = [x for x in files if x.startswith("registered2-")]
unregf = [x for x in files if x.startswith("unregistered-")]
unreg2f = [x for x in files if x.startswith("unregistered2-")]

trad_memory = [0 for _ in range(51)]
trad_network = [0 for _ in range(51)]
reg_memory = [0 for _ in range(51)]
reg_network = [0 for _ in range(51)]
reg2_memory = [0 for _ in range(51)]
reg2_network = [0 for _ in range(51)]
unreg_memory = [0 for _ in range(51)]
unreg_network = [0 for _ in range(51)]
unreg2_memory = [0 for _ in range(51)]
unreg2_network = [0 for _ in range(51)]


for f in tradf:
    fp = open("results/"+f)
    for line in fp.readlines():
        trad_memory[int(line.split("\t")[0])] += float(line.split("\t")[1])
        trad_network[int(line.split("\t")[0])] += float(line.split("\t")[2])
for f in regf:
    fp = open("results/"+f)
    for line in fp.readlines():
        reg_memory[int(line.split("\t")[0])] += float(line.split("\t")[1])
        reg_network[int(line.split("\t")[0])] += float(line.split("\t")[2])
for f in reg2f:
    fp = open("results/"+f)
    for line in fp.readlines():
        reg2_memory[int(line.split("\t")[0])] += float(line.split("\t")[1])
        reg2_network[int(line.split("\t")[0])] += float(line.split("\t")[2])
for f in unregf:
    fp = open("results/"+f)
    for line in fp.readlines():
        unreg_memory[int(line.split("\t")[0])] += float(line.split("\t")[1])
        unreg_network[int(line.split("\t")[0])] += float(line.split("\t")[2])
for f in unreg2f:
    fp = open("results/"+f)
    for line in fp.readlines():
        unreg2_memory[int(line.split("\t")[0])] += float(line.split("\t")[1])
        unreg2_network[int(line.split("\t")[0])] += float(line.split("\t")[2])

if len(tradf) > 0:
    trad_memory = [x/len(tradf) for x in trad_memory]
if len(tradf) > 0:
    trad_network = [x/len(tradf) for x in trad_network]
if len(regf) > 0:
    reg_memory = [x/len(regf) for x in reg_memory]
if len(regf) > 0:
    reg_network = [x/len(regf) for x in reg_network]
if len(reg2f) > 0:
    reg2_memory = [x/len(reg2f) for x in reg2_memory]
if len(reg2f) > 0:
    reg2_network = [x/len(reg2f) for x in reg2_network]
if len(unregf) > 0:
    unreg_memory = [x/len(unregf) for x in unreg_memory]
if len(unregf) > 0:
    unreg_network = [x/len(unregf) for x in unreg_network]
if len(unreg2f) > 0:
    unreg2_memory = [x/len(unreg2f) for x in unreg2_memory]
if len(unreg2f) > 0:
    unreg2_network = [x/len(unreg2f) for x in unreg2_network]

fp = open("out-network.txt", "w")
for i in range(0, 50, 3):
    fp.write("%i\t%f\t%f\t%f\t%f\t%f\n" % (int(i), trad_network[i]/1024, reg_network[i]/1024, unreg_network[i]/1024, reg2_network[i]/1024, unreg2_network[i]/1024))
fp.close()

fp = open("out-memory.txt", "w")
for i in range(0, 50, 3):
    fp.write("%i\t%f\t%f\t%f\t%f\t%f\n" % (int(i), trad_memory[i], reg_memory[i], unreg_memory[i], reg2_memory[i], unreg2_memory[i]))
fp.close()
