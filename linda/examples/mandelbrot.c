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

#include "Python.h"

#include <stdio.h>

#define MaxIters 256*256

#define CheckNULL(x) if(x==NULL) { PyErr_Print(); exit(-1); }

int main(int argc, char* argv[]) {
   PyObject* linda_module;
   PyObject* out; PyObject* pix;

   PyObject* r;

   double col_mul;

   col_mul = (pow(2, 24)-1)/((double)MaxIters);

   Py_Initialize();
   PySys_SetArgv(argc, argv);

   linda_module = PyImport_Import(Py_BuildValue("s", "linda"));
   CheckNULL(linda_module);

   r = PyObject_CallFunction(PyObject_GetAttrString(linda_module, "connect"), "");
   CheckNULL(r);
   Py_XDECREF(r);

   out = PyObject_CallFunction(PyObject_GetAttrString(PyObject_GetAttrString(linda_module, "universe"), "_rd"),
                             "((s,O))", "out", PyObject_GetAttrString(linda_module, "TupleSpace"));
   CheckNULL(out);
   out = PyTuple_GetItem(out, 1);

   pix = PyObject_CallFunction(PyObject_GetAttrString(PyObject_GetAttrString(linda_module, "universe"), "_rd"),
                             "((s,O))", "pix", PyObject_GetAttrString(linda_module, "TupleSpace"));
   CheckNULL(pix);
   pix = PyTuple_GetItem(pix, 1);
   
   int i = 0;
   while(1) {
       PyObject* t;
       int x; int y;
       double cr; double ci;
       double zr; double zi;
       double rsquared; double isquared;
       int count;

       Py_INCREF(&PyInt_Type); 
       Py_INCREF(&PyInt_Type);
       Py_INCREF(&PyFloat_Type); 
       Py_INCREF(&PyFloat_Type);
       t = PyObject_CallMethod(out, "_inp", "((OOOO))", (PyObject*)&PyInt_Type, (PyObject*)&PyInt_Type, (PyObject*)&PyFloat_Type, (PyObject*)&PyFloat_Type);
       CheckNULL(t);

       if(t == Py_None) {
           break;
       }

       /*PyObject* intcoords = PyTuple_GetItem(t, 0);
       PyObject* floatcoords = PyTuple_GetItem(t, 1);*/
       x = PyInt_AsLong(PyTuple_GetItem(t, 0));
       y = PyInt_AsLong(PyTuple_GetItem(t, 1));
       printf("%i\r", i);
       i++;
       
       cr = PyFloat_AsDouble(PyTuple_GetItem(t, 2));
       ci = PyFloat_AsDouble(PyTuple_GetItem(t, 3));

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

       CheckNULL(PyObject_CallMethod(pix, "_out", "((iiiii))", x, y, (count)%256, (count>>8)%256, (count>>16)%256));
   }

   Py_Finalize();
   return 0;
}
