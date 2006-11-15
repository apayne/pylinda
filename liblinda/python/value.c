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

#include "../../minimal/src/minimal_internal.h"

static PyObject* linda_Value_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    linda_ValueObject* self;

    self = (linda_ValueObject*)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->val = NULL;
    }

    return (PyObject*)self;
}

static int linda_Value_init(linda_ValueObject* self, PyObject* args, PyObject* kwds) {
    int i;
    PyObject* obj = NULL;
    PyObject* type = NULL;

    if(!PyArg_ParseTuple(args, "|OO", &obj, &type))
        return -1;

    if(obj == Py_None) {
        self->val = Linda_nil();
    } else if(PyInt_Check(obj)) {
        self->val = Linda_int(PyInt_AsLong(obj));
    } else if(PyFloat_Check(obj)) {
        self->val = Linda_float(PyFloat_AsDouble(obj));
    } else if(PyString_Check(obj)) {
        self->val = Linda_string(PyString_AsString(obj));
    } else if(PyTuple_Check(obj)) {
        self->val = Linda_tuple(PyTuple_Size(obj));
        for(i = 0; i < PyTuple_Size(obj); i++) {
            PyObject* o = PyTuple_GetItem(obj, i);
            LindaValue nv = PyO2Value(o);
            Py_DECREF(o);
            Linda_tupleSet(self->val, i, nv); /* Steals reference to nv. */
        }
    } else if(PyType_Check(obj)) {
        if(obj == (PyObject*)&PyInt_Type) {
            self->val = Linda_copy(Linda_intType);
        } else if(obj == (PyObject*)&PyFloat_Type) {
            self->val = Linda_copy(Linda_floatType);
        } else if(obj == (PyObject*)&PyString_Type) {
            self->val = Linda_copy(Linda_stringType);
        } else if(obj == (PyObject*)&PyBool_Type) {
            self->val = Linda_copy(Linda_boolType);
        } else if(obj == (PyObject*)&linda_TupleSpaceType) {
            self->val = Linda_copy(Linda_tupleSpaceType);
        } else {
            fprintf(stderr, "PyO2Linda: Invalid type of type.\n");
            return -1;
        }
    } else if(LindaPython_is_server && PyObject_IsInstance(obj, (PyObject*)&linda_TSRefType)) {
        self->val = Linda_copy(((linda_TSRefObject*)obj)->ts);
    } else if(!LindaPython_is_server && PyObject_TypeCheck(obj, &linda_TupleSpaceType)) {
        self->val = Linda_copy(((linda_TupleSpaceObject*)obj)->ts);
    } else {
        fprintf(stderr, "PyO2Linda: Invalid type.\n");
        return -1;
    }

    if(type != NULL) {
        Linda_setType(self->val, ((linda_ValueObject*)type)->val);
    }

    return 0;
}

static void linda_Value_dealloc(linda_ValueObject* self) {
    if(self->val != NULL) {
        Linda_delReference(self->val);
    }
    self->ob_type->tp_free(self);
}

static int linda_Value_cmp(linda_ValueObject* self, linda_ValueObject* other) {
    if(self->val->type != other->val->type) {
        if(self->val->type < other->val->type) {
            return -1;
        } else {
            return 1;
        }
    } else if(self->val->type == NIL) {
        return 0;
    } else if(self->val->type == BOOLEAN) {
        if(self->val->boolean == 0 && other->val->boolean == 0) {
            return 0;
        } else if(self->val->boolean == 0 && other->val->boolean == 1) {
            return -1;
        } else if(self->val->boolean == 1 && other->val->boolean == 0) {
            return 1;
        } else if(self->val->boolean == 1 && other->val->boolean == 1) {
            return 1;
        }
    } else if(self->val->type == INTEGER) {
        if(self->val->integer < other->val->integer) {
            return -1;
        } else if(self->val->integer > other->val->integer) {
            return 1;
        } else {
            return 0;
        }
    } else if(self->val->type == TUPLE) {
        if(self->val->size < other->val->size) {
            return -1;
        } else if(self->val->size > other->val->size) {
            return 1;
        } else {
            int i;
            for(i=0; i<self->val->size; i++) {
                int c;
                PyObject* c1 = Value2PyO(self->val->values[i]);
                PyObject* c2 = Value2PyO(other->val->values[i]);
                c = linda_Value_cmp((linda_ValueObject*)c1, (linda_ValueObject*)c2);
                Py_DECREF(c1); Py_DECREF(c2);
                if(c != 0) {
                    return c;
                }
            }
            return 0;
        }
    } else if(self->val->type == FUNCTION) {
        return strcmp(self->val->func_name, other->val->func_name);
    } else {
        fprintf(stderr, "PyLibLinda: Unknown type (%i) in comparison.\n", self->val->type);
        return 0;
    }
    return 0;
}

static PyObject* linda_Value_str(linda_ValueObject* self) {
    return PyString_FromFormat("%s", Minimal_Value_string(self->val));
}

static PyObject* linda_Value_repr(linda_ValueObject* self) {
    return PyString_FromFormat("%s", Minimal_Value_string(self->val));
}

static PyObject* linda_Value_setSumPos(linda_ValueObject* self, PyObject* args) {
    int i;

    if(!PyArg_ParseTuple(args, "i", &i)) {
        return NULL;
    }

    Linda_setSumPos(self->val, i);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* linda_Value_isType(linda_ValueObject* self) {
    if(Linda_isType(self->val)) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

/* These are all type related functions */

static PyObject* linda_Value_isNil(linda_ValueObject* self) {
    if(!Linda_isType(self->val)) { return NULL; }
    if(self->val->type_spec->type == ST_NIL) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static PyObject* linda_Value_isId(linda_ValueObject* self) {
    if(!Linda_isType(self->val)) { return NULL; }
    if(self->val->type_spec->type == ST_IDENTIFIER) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static PyObject* linda_Value_isProductType(linda_ValueObject* self) {
    if(!Linda_isType(self->val)) { return NULL; }
    if(self->val->type_spec->type == ST_PRODUCT_TYPE) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static PyObject* linda_Value_isSumType(linda_ValueObject* self) {
    if(!Linda_isType(self->val)) { return NULL; }
    if(self->val->type_spec->type == ST_SUM_TYPE) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static PyObject* linda_Value_isPtrType(linda_ValueObject* self) {
    if(!Linda_isType(self->val)) { return NULL; }
    if(self->val->type_spec->type == ST_POINTER) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static PyObject* linda_Value_isFunctionType(linda_ValueObject* self) {
    if(!Linda_isType(self->val)) { return NULL; }
    if(self->val->type_spec->type == ST_TYPE_FUNCTION) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static PyMethodDef value_methods[] = {
    {"setSumPos", (PyCFunction)linda_Value_setSumPos, METH_VARARGS, ""},
    {"isType", (PyCFunction)linda_Value_isType, METH_NOARGS, ""},
    {"isNil", (PyCFunction)linda_Value_isNil, METH_NOARGS, ""},
    {"isId", (PyCFunction)linda_Value_isId, METH_NOARGS, ""},
    {"isProductType", (PyCFunction)linda_Value_isProductType, METH_NOARGS, ""},
    {"isSumType", (PyCFunction)linda_Value_isSumType, METH_NOARGS, ""},
    {"isPtrType", (PyCFunction)linda_Value_isPtrType, METH_NOARGS, ""},
    {"isFunctionType", (PyCFunction)linda_Value_isFunctionType, METH_NOARGS, ""},
    {NULL}  /* Sentinel */
};

static PyObject* linda_Value_gettype(linda_ValueObject *self, void *closure) {
    return Value2PyO(Linda_getType(self->val));
}

static PyObject* linda_Value_getid(linda_ValueObject *self, void *closure) {
    if(!Linda_isType(self->val)) { return NULL; }
    if(self->val->type_spec->type != ST_IDENTIFIER) { return NULL; }
    return PyString_FromString(self->val->type_spec->string);
}

static PyObject* linda_Value_getstring(linda_ValueObject *self, void *closure) {
    if(!Linda_isString(self->val)) { return NULL; }
    return PyString_FromString(Linda_getString(self->val));
}

static PyObject* linda_Value_getarg(linda_ValueObject *self, void *closure) {
    if(!Linda_isType(self->val)) { return NULL; }
    if(self->val->type_spec->type != ST_TYPE_FUNCTION) { return NULL; }
    return Value2PyO(Minimal_typeSpec(self->val->type_name, Minimal_SyntaxTree_copy(self->val->type_spec->branch1)));
}

static PyObject* linda_Value_getresult(linda_ValueObject *self, void *closure) {
    if(!Linda_isType(self->val)) { return NULL; }
    if(self->val->type_spec->type != ST_TYPE_FUNCTION) { return NULL; }
    return Value2PyO(Minimal_typeSpec(self->val->type_name, Minimal_SyntaxTree_copy(self->val->type_spec->branch2)));
}

static PyGetSetDef value_getseters[] = {
    {"id", (getter)linda_Value_getid, (setter)NULL, "", NULL},
    {"type", (getter)linda_Value_gettype, (setter)NULL, "", NULL},
    {"string", (getter)linda_Value_getstring, (setter)NULL, "", NULL},
    {"arg", (getter)linda_Value_getarg, (setter)NULL, "", NULL},
    {"result", (getter)linda_Value_getresult, (setter)NULL, "", NULL},
    {NULL}  /* Sentinel */
};

static Py_ssize_t linda_Value_len(linda_ValueObject *self) {
    if(Linda_isType(self->val)) {
        if(self->val->type_spec->branch2 != NULL) {
            return 2;
        } else {
            return 1;
        }
    } else {
        return Linda_getTupleSize(self->val);
    }
}

static PyObject* linda_Value_item(linda_ValueObject *self, Py_ssize_t index) {
    if(index < 0 || index >= linda_Value_len(self)) {
        PyErr_SetString(PyExc_IndexError, "Index out of range for tuple.");
        return NULL;
    } else if(Linda_isType(self->val)) {
        if(index == 0) {
            return Value2PyO(Minimal_typeSpec(self->val->type_name, Minimal_SyntaxTree_copy(self->val->type_spec->branch1)));
        } else {
            return Value2PyO(Minimal_typeSpec(self->val->type_name, Minimal_SyntaxTree_copy(self->val->type_spec->branch2)));
        }
    } else {
        return Value2PyO(Linda_tupleGet(self->val, index));
    }
}

PySequenceMethods linda_ValueSeq = {
        (lenfunc)linda_Value_len,  /* lenfunc sq_length; */
        0,                /*binaryfunc sq_concat; */
        0,                /*ssizeargfunc sq_repeat; */
        (ssizeargfunc)linda_Value_item, /*ssizeargfunc sq_item; */
        0,                /*ssizessizeargfunc sq_slice; */
        0,                /*ssizeobjargproc sq_ass_item; */
        0,                /*ssizessizeobjargproc sq_ass_slice; */
        0,                /*objobjproc sq_contains; */
        0,                /*binaryfunc sq_inplace_concat; */
        0,                /*ssizeargfunc sq_inplace_repeat; */
};

PyTypeObject linda_ValueType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "linda.Value",        /*tp_name*/
    sizeof(linda_ValueObject), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)linda_Value_dealloc,  /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    (cmpfunc)linda_Value_cmp,  /*tp_compare*/
    (reprfunc)linda_Value_repr,/*tp_repr*/
    0,                         /*tp_as_number*/
    &linda_ValueSeq,            /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    (reprfunc)linda_Value_str, /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "A Linda Value",           /* tp_doc */
    0,                         /* tp_traverse; */
    0,                         /* tp_clear; */
    0,                         /* tp_richcompare; */
    0,                         /* tp_weaklistoffset; */
    0,                         /* tp_iter; */
    0,                         /* tp_iternext; */
    value_methods,             /* tp_methods; */
    0,                         /* tp_members */
    value_getseters,           /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)linda_Value_init, /* tp_init */
    0,                         /* tp_alloc */
    linda_Value_new,      /* tp_new */
};

LindaValue PyO2Value(PyObject* obj) {
    if(PyObject_IsInstance(obj, (PyObject*)&linda_ValueType)) {
        Linda_addReference(((linda_ValueObject*)obj)->val);
        return ((linda_ValueObject*)obj)->val;
    } else {
        PyObject* o = PyObject_CallFunction((PyObject*)&linda_ValueType, "O", obj);
        if(o == NULL) {
            return NULL;
        } else {
            Linda_addReference(((linda_ValueObject*)o)->val);
            Py_DECREF(o);
            return ((linda_ValueObject*)o)->val;
        }
    }
}

PyObject* Value2PyO(LindaValue obj) {
    PyObject* o = PyObject_CallFunction((PyObject*)&linda_ValueType, "i", 0);
    Linda_delReference(((linda_ValueObject*)o)->val);
    Linda_addReference(obj);
    ((linda_ValueObject*)o)->val = obj;
    return o;
}

PyObject* Tuple2PyO(LindaValue t) {
    int i;
    PyObject* pyt = PyTuple_New(Linda_getTupleSize(t));

    for(i=0; i<Linda_getTupleSize(t); i++) {
        PyTuple_SetItem(pyt, i, Value2PyO(Linda_tupleGet(t, i)));
    }

    return pyt;
}

LindaValue PyO2Tuple(PyObject* tup) {
    int i;
    LindaValue t = Linda_tuple(PyTuple_Size(tup));
    for(i=0; i<PyTuple_Size(tup); i++) {
        LindaValue v;
        v = PyO2Value(PyTuple_GetItem(tup, i));
        if(v == NULL) { Linda_delReference(t); return NULL; }
        Linda_tupleSet(t, i, v);
    }
    return t;
}

void initvalue(PyObject* m) {
    linda_ValueType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&linda_ValueType) < 0)
        return;

    Py_INCREF(&linda_ValueType);
    PyModule_AddObject(m, "Value", (PyObject*)&linda_ValueType);
}
