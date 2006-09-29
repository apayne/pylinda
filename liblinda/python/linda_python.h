/*
*    Copyright 2006 Andrew Wilkinson <aw@cs.york.ac.uk>.
*
*    This file is part of LibLinda (http://www-users.cs.york.ac.uk/~aw/pylinda)
*
*    LibLinda is free software; you can redistribute it and/or modify
*    it under the terms of the GNU Lesser General Public License as published by
*    the Free Software Foundation; either version 2.1 of the License, or
*    (at your option) any later version.
*
*    LibLinda is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with LibLinda; if not, write to the Free Software
*    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef LINDA_PYTHON

void inittuplespace(PyObject* m);

Value* PyO2Value(PyObject* obj);
Tuple PyO2Tuple(PyObject* tup);
PyObject* Tuple2PyO(Tuple t);

PyObject* LindaPython_send(PyObject *self, PyObject* args);
PyObject* LindaPython_recv(PyObject *self, PyObject* args);

extern int LindaPython_is_server;

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    Linda_tuplespace ts;
} linda_TupleSpaceObject;
PyTypeObject linda_TupleSpaceType;

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    Linda_tuplespace ts;
} linda_TSRefObject;
PyTypeObject linda_TSRefType;

#endif
