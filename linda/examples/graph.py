#!/usr/bin/env python

#    Copyright 2004 Andrew Wilkinson <aw@cs.york.ac.uk>.
#
#    This file is part of PyLinda (http://www-users.cs.york.ac.uk/~aw/pylinda)
#
#    PyLinda is free software; you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation; either version 2.1 of the License, or
#    (at your option) any later version.
#
#    PyLinda is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with PyLinda; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

import sys
import time

import linda.kernel as linda

edges = [(0, 1, 1), (0, 5, 2), (1, 2, 3), (2, 2, 3), (1, 4, 4), (2, 3, 4), (3, 1, 4)]

if len(sys.argv) > 1 and sys.argv[1] == "master":
    ts = linda.TupleSpace()
    linda.universe._out(("ts", ts))

    # output the edges
    for e in edges:
        ts._out(("edge", e[0], e[1], e[2]))
        # put them in both directions as they're undirected
        ts._out(("edge", e[2], e[1], e[0]))

    # output the start node
    ts._out(("node", 0, 0, 0))
    ts._out(("node", 0))

    ts._out(("ticket", ))

    while 1:
        # count the number of edges left in the graph
        newts = linda.TupleSpace()
        edges = ts.copy_collect(newts, "('edge', ?, ?, ?)")

        if edges > 0:
            time.sleep(5)
        else:
            while 1:
                node = ts._in("('node', ?, ?, 1)")
                print "node %i = %i" % (node[1], node[2])
else:
    # get our working tuple space
    ts = linda.universe._rd("('ts', ?)")[1]

    while 1:
        # get the ticket
        print "getting ticket"
        ts._in("('ticket')")
        print "got ticket"
        
        # choose a node to start from
        node1 = ts._in("('node', ?, ?, 0)")

        # count the number of edges leaving the node
        newts = linda.TupleSpace()
        edge_count = ts.collect(newts, "('edge', %i, ?, ?)" % (node1[1], ))

        # if there aren't any then mark the node as complete (i.e. 4th value is 1)
        if edge_count == 0:
            ts._out(("node", node1[1], node1[2], 1))
            ts._out(("ticket", ))
            continue

        # get one of the edges
        edge = newts._in("('edge', %i, ?, ?)" % (node1[1], ))
        # copy the rest back to the working space
        newts.collect(ts, "('edge', ?, ?, ?)")

        # see if the destination node exists
        newts = linda.TupleSpace()
        node_exists = ts.copy_collect(newts, "('node', %i)" % (edge[3], ))
        if node_exists == 0:
            # if it doesn't then out an existance tuple and create the inital values
            ts._out(("node", edge[3]))
            node2 = ("node", edge[3], -1, 0)
        else:
            # if it does then grab the node
            node2 = ts._in("('node', %i, ?, ?)" % (edge[3], ))

        # return the ticket
        ts._out(("ticket", ))
        print "returning ticket"

        # check to see if this edge results in a shorter path
        if node1[2] + edge[2] < node2[2] or node2[2] == -1:
            node2 = node2[0], node2[1], node1[2] + edge[2], node2[3]

        # return our two nodes to the pot
        ts._out(node1)
        ts._out(node2)
