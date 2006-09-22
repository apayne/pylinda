#!/usr/bin/python

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

import linda
linda.connect()

import sys

MaxIters = 256*256-1

if len(sys.argv) > 1 and sys.argv[1] == "master":
    out = linda.TupleSpace()
    pix = linda.TupleSpace()

    linda.universe._out(("out", out))
    linda.universe._out(("pix", pix))

    width, height = 80, 80
    left, right, top, bottom = -2.0, 1.0, -1.0, 1.0

    rls = (right - left) / width
    bts = (bottom - top) / height
    
    ylist = range(height) * width
    xlist = range(width) * height
    
    xlist.sort()
        
    map(lambda (x,y): out._out((x,y, left + x * rls, top + y * bts)), zip(xlist, ylist))
    
    import Image
    im = Image.new("RGB", (width, height))

    for i in range(height * width):
        x, y, r, g, b = pix._in((int, int, int, int, int))
        print "%s\r" % i,
        im.putpixel((x, y), (r, g, b))
#    ylist = range(height) * width
#    xlist = range(width) * height
    
#    xlist.sort()
#    for (x, y) in zip(xlist, ylist):
#            x, y, r, g, b = pix._in((x, y, int, int, int))
#            print "%i,%i" % (x,y)
#            im.putpixel((x, y), (r, g, b))

    im.save("fractal.png")

    out = linda.universe._in(("out", linda.TupleSpace))[1]
    pix = linda.universe._in(("pix", linda.TupleSpace))[1]

else:
    out = linda.universe._rd(("out", linda.TupleSpace))[1]
    pix = linda.universe._rd(("pix", linda.TupleSpace))[1]

    col_mul = (2**24-1)/float(MaxIters)

    while True:
        t = out._inp((int, int, float, float))
        if t is None:
            break

        (x, y, cr, ci) = t
        print "in", x, y

        #Z = 0 + 0j
        #C = cr + ci*1j

        #count = -1
        #while abs(Z) <= 2.0 and count < MaxIters:
        #     count += 1

             #Z = Z * Z + C

        zr, zi = 0.0, 0.0
        rsquared = zr * zr
        isquared = zi * zi
        count = -1
        while rsquared + isquared <= 4.0 and count < MaxIters:
            count += 1

            zi = zr * zi * 2 + ci
            zr = rsquared - isquared + cr

            rsquared = zr * zr
            isquared = zi * zi

        count = int(count * col_mul)

        pix._out((x, y, count%256, (count>>8)%256, (count>>16)%256))
