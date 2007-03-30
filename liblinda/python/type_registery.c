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

#include "config.h"

#include PYTHON_H

#include "linda_python.h"

PyObject* Linda_module;

PyObject* dict_to;
PyObject* dict_from;

void LindaPython_registerType(PyObject* type, PyObject* _class, PyObject* _to, PyObject* _from) {
    PyObject* t = PyTuple_New(2);
    PyTuple_SetItem(t, 0, _to);
    PyTuple_SetItem(t, 1, _from);
    PyDict_SetItem(dict_to, _class, t);
    PyDict_SetItem(dict_from, type, _from);
}

PyObject* LindaPython_lookupConvertTo(PyObject* type) {
    PyObject* t = PyDict_GetItem(dict_to, type);
    if(t == NULL) {
        return t;
    }
    t = PyTuple_GetItem(t, 0);
    if(t == NULL) {
        printf("failed to get from tuple\n");
    }
    return t;
}

LindaValue LindaPython_lookupType(PyObject* type) {
    PyObject* t = PyDict_GetItem(dict_to, type);
    if(t == NULL) {
        return NULL;
    }
    return PyO2Value(PyTuple_GetItem(t, 1));
}

void initregistery(PyObject* m) {
    dict_to = PyDict_New();
    dict_from = PyDict_New();
}
