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

#include <stdio.h>

#include PYTHON_H

#include "linda.h"
#include "linda_python.h"

PyObject* Linda2PyO(LindaValue v) {
    if(Linda_isNil(v)) {
        Py_INCREF(Py_None);
        return Py_None;
    } else if(Linda_isInt(v)) {
        return Py_BuildValue("i", Linda_getInt(v));
    } else if(Linda_isFloat(v)) {
        return Py_BuildValue("f", Linda_getFloat(v));
    } else if(Linda_isString(v)) {
        return PyString_FromStringAndSize(Linda_getString(v), Linda_getStringLen(v));
    } else if(Linda_isTypeSpec(v)) {
        if(v->type_spec->type == ST_IDENTIFIER && strcmp(v->type_spec->string, "int") == 0) {
            Py_INCREF(&PyInt_Type);
            return (PyObject*)&PyInt_Type;
        } else if(v->type_spec->type == ST_IDENTIFIER && strcmp(v->type_spec->string, "float") == 0) {
            Py_INCREF(&PyFloat_Type);
            return (PyObject*)&PyFloat_Type;
        } else if((v->type_spec->type == ST_IDENTIFIER && strcmp(v->type_spec->string, "bool") == 0) == 0) {
            Py_INCREF(&PyBool_Type);
            return (PyObject*)&PyBool_Type;
        } else if((v->type_spec->type == ST_IDENTIFIER && strcmp(v->type_spec->string, "string") == 0) == 0) {
            Py_INCREF(&PyString_Type);
            return (PyObject*)&PyString_Type;
        } else if((v->type_spec->type == ST_IDENTIFIER && strcmp(v->type_spec->string, "tuplespace") == 0) == 0) {
            if(LindaPython_is_server) {
                Py_INCREF(&linda_TSRefType);
                return (PyObject*)&linda_TSRefType;
            } else {
                Py_INCREF(&linda_TupleSpaceType);
                return (PyObject*)&linda_TupleSpaceType;
            }
        } else {
            fprintf(stderr, "Linda2PyO: Invalid type specification (%s).\n", v->type_name);
            Py_INCREF(Py_None);
            return Py_None;
        }
    } else if(Linda_isTupleSpace(v)) {
        if(LindaPython_is_server) {
            linda_TSRefObject* ts;
            ts = (linda_TSRefObject*)linda_TSRefType.tp_alloc(&linda_TSRefType, 0);
            ts->ts = Linda_copy(v);
            Py_INCREF(ts);
            return (PyObject*)ts;
        } else {
            linda_TupleSpaceObject* ts;
            ts = (linda_TupleSpaceObject*)linda_TupleSpaceType.tp_alloc(&linda_TupleSpaceType, 0);
            ts->ts = Linda_copy(v);
            Py_INCREF(ts);
            return (PyObject*)ts;
        }
    } else if(Linda_isTuple(v)) {
        int i;
        PyObject* pyt = PyTuple_New(Linda_getTupleSize(v));
        for(i=0; i<Linda_getTupleSize(v); i++) {
            PyTuple_SetItem(pyt, i, Linda2PyO(Linda_tupleGet(v, i)));
        }
        return pyt;
    } else {
        fprintf(stderr, "Linda2PyO: Invalid type (%i).\n", v->type);
        Py_INCREF(Py_None);
        return Py_None;
    }
}

LindaValue PyO2Linda(PyObject* obj) {
    int i;
    LindaValue v;
    if(obj == Py_None) {
        v = Linda_nil();
    } else if(PyInt_Check(obj)) {
        v = Linda_int(PyInt_AsLong(obj));
    } else if(PyFloat_Check(obj)) {
        v = Linda_float(PyFloat_AsDouble(obj));
    } else if(PyString_Check(obj)) {
        v = Linda_string(PyString_AsString(obj));
    } else if(PyTuple_Check(obj)) {
        v = Linda_tuple(PyTuple_Size(obj));
        for(i = 0; i < PyTuple_Size(obj); i++) {
            LindaValue nv = PyO2Value(PyTuple_GetItem(obj, i));
            Linda_tupleSet(v, i, nv);
            Minimal_delReference(nv);
        }
    } else if(PyType_Check(obj)) {
        if(obj == (PyObject*)&PyInt_Type) {
            v = Linda_copy(Linda_intType);
        } else if(obj == (PyObject*)&PyFloat_Type) {
            v = Linda_copy(Linda_floatType);
        } else if(obj == (PyObject*)&PyString_Type) {
            v = Linda_copy(Linda_stringType);
        } else if(obj == (PyObject*)&PyBool_Type) {
            v = Linda_copy(Linda_boolType);
        } else if(obj == (PyObject*)&linda_TupleSpaceType) {
            v = Linda_copy(Linda_tupleSpaceType);
        } else {
            fprintf(stderr, "PyO2Linda: Invalid type of type.\n");
            return NULL;
        }
    } else if(LindaPython_is_server && PyObject_IsInstance(obj, (PyObject*)&linda_TSRefType)) {
        v = Linda_copy(((linda_TSRefObject*)obj)->ts);
    } else if(!LindaPython_is_server && PyObject_TypeCheck(obj, &linda_TupleSpaceType)) {
        v = Linda_copy(((linda_TupleSpaceObject*)obj)->ts);
    } else {
        fprintf(stderr, "PyO2Linda: Invalid type.\n");
        return NULL;
    }

    return v;
}

PyObject* Tuple2PyO(LindaValue t) {
    int i;
    PyObject* pyt = PyTuple_New(Linda_getTupleSize(t));

    for(i=0; i<Linda_getTupleSize(t); i++) {
        PyTuple_SetItem(pyt, i, Linda2PyO(Linda_tupleGet(t, i)));
    }

    return pyt;
}

LindaValue PyO2Tuple(PyObject* tup) {
    int i;
    LindaValue t = Linda_tuple(PyTuple_Size(tup));
    for(i=0; i<PyTuple_Size(tup); i++) {
        LindaValue v;
        v = PyO2Linda(PyTuple_GetItem(tup, i));
        if(v == NULL) { Linda_delReference(t); return NULL; }
        Linda_tupleSet(t, i, v);
        Linda_delReference(v);
    }
    return t;
}
