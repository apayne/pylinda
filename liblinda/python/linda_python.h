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

#ifndef LINDA_PYTHON_H
#define LINDA_PYTHON_H

#include "linda.h"

void inittuplespace(PyObject* m);
void inittsref(PyObject* m);
void initvalue(PyObject* m);
void inittypemap(PyObject* m);

LindaValue PyO2Value(PyObject* obj);
PyObject* Value2PyO(LindaValue obj);
LindaValue PyO2Tuple(PyObject* tup);
PyObject* Tuple2PyO(LindaValue t);

PyObject* LindaPython_send(PyObject *self, PyObject* args);
PyObject* LindaPython_recv(PyObject *self, PyObject* args);

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    LindaValue ts;
} linda_TupleSpaceObject;
PyTypeObject linda_TupleSpaceType;

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    LindaValue ts;
} linda_TSRefObject;
PyTypeObject linda_TSRefType;

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    LindaValue val;
} linda_ValueObject;
PyTypeObject linda_ValueType;

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    MinimalLayer map;
} linda_TypeMapObject;
PyTypeObject linda_TypeMapType;

#endif
