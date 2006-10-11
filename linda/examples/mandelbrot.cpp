/*
    The file is copyright Andrew Wilkinson <aw@cs.york.ac.uk> 2004.

    This file is part of PyLinda (http://www-users.cs.york.ac.uk/~aw/pylinda)

    PyLinda is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    PyLinda is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <math.h>
#include <linda.h>

#define MaxIters 256*256

int main(int argc, char* argv[]) {
    double col_mul;
    col_mul = (pow(2, 24)-1)/((double)MaxIters);
    Linda::Tuple templat;
    Linda::Tuple t;
    Linda::Tuple* tp;
    Linda::TupleSpace out;
    Linda::TupleSpace pix;

    Linda::connect(Linda_port);

    templat = Linda::Tuple(2);
    templat.set(0, "out");
    templat.set(0, Linda::tuplespaceType);
    t = Linda::uts.rd(templat);

    out = t.get(1).get<Linda::TupleSpace>();

    templat = Linda::Tuple(2);
    templat.set(0, "pix");
    templat.set(0, Linda::tuplespaceType);
    t = Linda::uts.rd(templat);

    pix = t.get(1).get<Linda::TupleSpace>();

    templat = Linda::Tuple(4);
    templat.set(0, Linda::intType);
    templat.set(1, Linda::intType);
    templat.set(2, Linda::intType);
    templat.set(3, Linda::intType);

    int i = 0;
    while(true) {
        int x; int y;
        double cr; double ci;
        double zr; double zi;
        double rsquared; double isquared;
        int count;

        tp = out.inp(templat);
        if(tp == NULL) {
            break;
        }

        x = tp->get(0).get<int>();
        y = tp->get(1).get<int>();
        printf("%i\r", i);
        i++;

        cr = tp->get(2).get<double>();
        ci = tp->get(3).get<double>();

        free(tp);

        zr = 0.0; zi = 0.0;
        rsquared = zr * zr;
        isquared = zi * zi;
        count = -1;
        while(rsquared + isquared <= 4.0 && count < MaxIters) {
            count += 1;

            zi = zr * zi * 2 + ci;
            zr = rsquared - isquared + cr;

            rsquared = zr * zr;
            isquared = zi * zi;
        }

        count = (int)(count * col_mul);

        t = Linda::Tuple(5);
        t.set(0, x);
        t.set(1, y);
        t.set(2, (count)%256);
        t.set(3, (count>>8)%256);
        t.set(4, (count>>16)%256);

        pix.out(t);
    }

    Linda_disconnect();

    return 0;
}
