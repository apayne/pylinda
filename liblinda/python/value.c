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

PyObject* Value2PyO(Value* v) {
    if(Value_is_nil(v)) {
        Py_INCREF(Py_None);
        return Py_None;
    } else if(Value_is_int(v)) {
        return Py_BuildValue("i", Value_get_int(v));
    } else if(Value_is_float(v)) {
        return Py_BuildValue("f", Value_get_float(v));
    } else if(Value_is_string(v)) {
        return PyString_FromStringAndSize(Value_get_string(v), Value_get_string_len(v));
    } else if(Value_is_type(v)) {
        if(strcmp(v->typespec, "int") == 0) {
            Py_INCREF(&PyInt_Type);
            return (PyObject*)&PyInt_Type;
        } else if(strcmp(v->typespec, "float") == 0) {
            Py_INCREF(&PyFloat_Type);
            return (PyObject*)&PyFloat_Type;
        } else if(strcmp(v->typespec, "bool") == 0) {
            Py_INCREF(&PyBool_Type);
            return (PyObject*)&PyBool_Type;
        } else if(strcmp(v->typespec, "string") == 0) {
            Py_INCREF(&PyString_Type);
            return (PyObject*)&PyString_Type;
        } else if(strcmp(v->typespec, "tuple") == 0) {
            Py_INCREF(&PyTuple_Type);
            return (PyObject*)&PyTuple_Type;
        } else if(strcmp(v->typespec, "tuplespace") == 0) {
            if(LindaPython_is_server) {
                Py_INCREF(&linda_TSRefType);
                return (PyObject*)&linda_TSRefType;
            } else {
                Py_INCREF(&linda_TupleSpaceType);
                return (PyObject*)&linda_TupleSpaceType;
            }
        } else {
            fprintf(stderr, "Value2PyO: Invalid type specification (%s).\n", v->typespec);
            Py_INCREF(Py_None);
            return Py_None;
        }
    } else if(Value_is_tsref(v)) {
        if(LindaPython_is_server) {
            linda_TSRefObject* ts;
            ts = (linda_TSRefObject*)linda_TSRefType.tp_alloc(&linda_TSRefType, 0);
            ts->ts = (char*)malloc(strlen(Value_get_tsref(v))+1);
            strcpy(ts->ts, Value_get_tsref(v));
            return (PyObject*)ts;
        } else {
            linda_TupleSpaceObject* ts;
            ts = (linda_TupleSpaceObject*)linda_TupleSpaceType.tp_alloc(&linda_TupleSpaceType, 0);
            ts->ts = (char*)malloc(strlen(Value_get_tsref(v))+1);
            strcpy(ts->ts, Value_get_tsref(v));
            return (PyObject*)ts;
        }
    } else if(Value_is_tuple(v)) {
        int i;
        Tuple t = Value_get_tuple(v);
        PyObject* pyt = PyTuple_New(Tuple_get_size(t));
        for(i=0; i<Tuple_get_size(t); i++) {
            PyTuple_SetItem(pyt, i, Value2PyO(Tuple_get(t, i)));
        }
        return pyt;
    } else {
        fprintf(stderr, "Value2PyO: Invalid type (%i).\n", v->type);
        Py_INCREF(Py_None);
        return Py_None;
    }
}

Value* PyO2Value(PyObject* obj) {
    int i;
    Value v;
    Value* r;
    if(obj == Py_None) {
        v = Value_nil();
    } else if(PyInt_Check(obj)) {
        v = Value_int(PyInt_AsLong(obj));
    } else if(PyFloat_Check(obj)) {
        v = Value_float(PyFloat_AsDouble(obj));
    } else if(PyString_Check(obj)) {
        v = Value_string(PyString_AsString(obj));
    } else if(PyTuple_Check(obj)) {
        Tuple t = Tuple_new(PyTuple_Size(obj));
        for(i = 0; i < PyTuple_Size(obj); i++) {
            Value* nv = PyO2Value(PyTuple_GetItem(obj, i));
            Tuple_set(t, i, *nv);
            free(nv);
        }
        v = Value_tuple(t);
    } else if(PyType_Check(obj)) {
        if(obj == (PyObject*)&PyInt_Type) {
            v = Value_copy(&Value_intType);
        } else if(obj == (PyObject*)&PyFloat_Type) {
            v = Value_copy(&Value_floatType);
        } else if(obj == (PyObject*)&PyString_Type) {
            v = Value_copy(&Value_stringType);
        } else if(obj == (PyObject*)&PyBool_Type) {
            v = Value_copy(&Value_boolType);
        } else if(obj == (PyObject*)&PyTuple_Type) {
            v = Value_copy(&Value_tupleType);
        } else if(obj == (PyObject*)&linda_TupleSpaceType) {
            v = Value_copy(&Value_tuplespaceType);
        } else {
            fprintf(stderr, "PyO2Value: Invalid type of type.\n");
            return NULL;
        }
    } else if(LindaPython_is_server && PyObject_IsInstance(obj, (PyObject*)&linda_TSRefType)) {
        v = Value_tsref(((linda_TSRefObject*)obj)->ts);
    } else if(!LindaPython_is_server && PyObject_TypeCheck(obj, &linda_TupleSpaceType)) {
        v = Value_tsref(((linda_TupleSpaceObject*)obj)->ts);
    } else {
        fprintf(stderr, "PyO2Value: Invalid type.\n");
        return NULL;
    }

    r = Value_copyptr(&v);
    Value_clear(&v);
    return r;
}

PyObject* Tuple2PyO(Tuple t) {
    int i;
    PyObject* pyt = PyTuple_New(Tuple_get_size(t));

    for(i=0; i<Tuple_get_size(t); i++) {
        PyTuple_SetItem(pyt, i, Value2PyO(Tuple_get(t, i)));
    }

    return pyt;
}

Tuple PyO2Tuple(PyObject* tup) {
    int i;
    Tuple t = Tuple_new(PyTuple_Size(tup));
    for(i=0; i<PyTuple_Size(tup); i++) {
        Value* v;
        v = PyO2Value(PyTuple_GetItem(tup, i));
        if(v == NULL) { Tuple_free(t); return NULL; }
        Tuple_set(t, i, *v);
        free(v);
    }
    return t;
}
