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

#if (PY_VERSION_HEX < 0x02050000)
typedef int Py_ssize_t;
#define lenfunc inquiry
#define ssizeargfunc intargfunc
#endif

#include "linda_python.h"

static PyObject* linda_Memo_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    linda_MemoObject* self;

    self = (linda_MemoObject*)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->dict = NULL;
    }

    return (PyObject*)self;
}

static int linda_Memo_init(linda_MemoObject* self, PyObject* args, PyObject* kwds) {
    if(!PyArg_ParseTuple(args, ""))
        return -1;

    self->dict = PyDict_New();

    return 0;
}

static void linda_Memo_dealloc(linda_MemoObject* self) {
    if(self->dict != NULL) {
        Py_DECREF(self->dict);
    }
    self->ob_type->tp_free(self);
}

static PyGetSetDef memo_getseters[] = {
    {NULL}  /* Sentinel */
};

static PyObject* linda_Memo_register(linda_MemoObject* self, PyObject* args) {
    PyObject* arg1;
    PyObject* arg2;

    if(!PyArg_ParseTuple(args, "OO", &arg1, &arg2)) {
        return NULL;
    }

    Py_INCREF(arg1);
    Py_INCREF(arg2);

    if(self->convert_to) {
        PyObject* i = PyInt_FromLong((unsigned long)arg1);
        PyDict_SetItem(self->dict, i, arg2);
    } else {
        PyObject* i = PyInt_FromLong((unsigned long)((linda_ValueObject*)arg1)->val);
        PyDict_SetItem(self->dict, i, arg2);
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* linda_Memo_getValue(linda_MemoObject* self, PyObject* args) {
    PyObject* arg;
    PyObject* r;
    PyObject* i;

    if(!self->convert_to) {
        PyErr_SetString(PyExc_TypeError, "This memo is for converting values from real objects.");
        return NULL;
    }

    if(!PyArg_ParseTuple(args, "O", &arg)) {
        return NULL;
    }

    i = PyInt_FromLong((unsigned long)arg);
    r = PyDict_GetItem(self->dict, i);
    if(r == NULL) {
        return NULL;
    } else {
        Py_INCREF(r);
        return r;
    }
}

static PyObject* linda_Memo_getReal(linda_MemoObject* self, PyObject* args) {
    PyObject* arg;
    PyObject* r;
    PyObject* i;

    if(self->convert_to) {
        PyErr_SetString(PyExc_TypeError, "This memo is for converting values from Linda objects.");
        return NULL;
    }

    if(!PyArg_ParseTuple(args, "O", &arg)) {
        return NULL;
    }

    i = PyInt_FromLong((unsigned long)((linda_ValueObject*)arg)->val);
    r = PyDict_GetItem(self->dict, i);
    if(r == NULL) {
        PyErr_SetString(PyExc_KeyError, "Couldn't find value in memo.");
        return NULL;
    } else {
        Py_INCREF(r);
        return r;
    }
}

static PyMethodDef memo_methods[] = {
    {"register", (PyCFunction)linda_Memo_register, METH_VARARGS, ""},
    {"getValue", (PyCFunction)linda_Memo_getValue, METH_VARARGS, ""},
    {"getReal", (PyCFunction)linda_Memo_getReal, METH_VARARGS, ""},
    {NULL}  /* Sentinel */
};

PyTypeObject linda_MemoType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "linda.Memo",        /*tp_name*/
    sizeof(linda_MemoObject), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)linda_Memo_dealloc,  /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,  /*tp_compare*/
    0,/*tp_repr*/
    0,                         /*tp_as_number*/
    0,            /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0, /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "A Linda Memo object",           /* tp_doc */
    0,                         /* tp_traverse; */
    0,                         /* tp_clear; */
    0,                         /* tp_richcompare; */
    0,                         /* tp_weaklistoffset; */
    0,                         /* tp_iter; */
    0,                         /* tp_iternext; */
    memo_methods,             /* tp_methods; */
    0,                         /* tp_members */
    memo_getseters,           /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)linda_Memo_init, /* tp_init */
    0,                         /* tp_alloc */
    linda_Memo_new,      /* tp_new */
};

void initmemo(PyObject* m) {
    linda_TypeMapType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&linda_MemoType) < 0)
        return;
}
