#!/usr/bin/python

import os
import math

files = os.listdir("results")

prodf = [x for x in files if x.startswith("product-")]
sumf = [x for x in files if x.startswith("sum-")]

prod = ["" for _ in range(51)]
sum = ["" for _ in range(51)]

for f in prodf:
    fp = open("results/"+f)
    for line in fp.readlines():
        if prod[int(line.split("\t")[0])] == "":
            prod[int(line.split("\t")[0])] = float(line.split("\t")[1])
        else:
            prod[int(line.split("\t")[0])] += float(line.split("\t")[1])
for f in sumf:
    fp = open("results/"+f)
    for line in fp.readlines():
        if sum[int(line.split("\t")[0])] == "":
            sum[int(line.split("\t")[0])] = float(line.split("\t")[1])
        else:
            sum[int(line.split("\t")[0])] += float(line.split("\t")[1])

if len(prodf) > 0:
    prod = ["" if x == "" else x/len(prodf) or ""  for x in prod]
if len(sumf) > 0:
    sum = ["" if x == "" else x/len(sumf) for x in sum]

fp = open("many-types.txt", "w")
for i in range(0, 51, 3):
    fp.write("%i\t%s\t%s\n" % (int(i), prod[i], sum[i]))
fp.close()
