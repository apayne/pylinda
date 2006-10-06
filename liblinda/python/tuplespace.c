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

#include "linda.h"
#include "linda_python.h"

static PyObject* linda_TupleSpace_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    linda_TupleSpaceObject* self;

    self = (linda_TupleSpaceObject*)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->ts = NULL;
    }

    return (PyObject*)self;
}

static int linda_TupleSpace_init(linda_TupleSpaceObject* self, PyObject* args, PyObject* kwds)
{
    char* id = NULL;

    if(!PyArg_ParseTuple(args, "|s", &id))
        return -1;

    if(id == NULL) {
        self->ts = Linda_createTuplespace();
        if(self->ts == NULL) {
            return -1;
        } else {
            return 0;
        }
    } else {
        self->ts = malloc(strlen(id)+1);
        strcpy(self->ts, id);
        Linda_addReference(id);
        return 0; 
    }
}

static PyObject* linda_TupleSpace_out(linda_TupleSpaceObject* self, PyObject* args) {
    PyObject* tuple;
    Tuple t;

    if(!PyArg_ParseTuple(args, "O", &tuple)) {
        return NULL;
    }

    if(!PyTuple_Check(tuple)) {
        PyErr_SetString(PyExc_TypeError, "_out takes a tuple.");
        return NULL;
    }

    t = PyO2Tuple(tuple);
    if(t == NULL) {
        fprintf(stderr, "Error converting tuple.\n");
        Py_INCREF(Py_None);
        return Py_None;
    }

    Linda_out(self->ts, t);

    Tuple_free(t);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* linda_TupleSpace_in(linda_TupleSpaceObject* self, PyObject* args) {
    PyObject* tuple;
    Tuple t1;
    Tuple t2;

    if(!PyArg_ParseTuple(args, "O", &tuple)) {
        return NULL;
    }

    if(!PyTuple_Check(tuple)) {
        PyErr_SetString(PyExc_TypeError, "_in takes a tuple.");
        return NULL;
    }

    t1 = PyO2Tuple(tuple);
    if(t1 == NULL) {
        fprintf(stderr, "Error converting tuple.\n");
        Py_INCREF(Py_None);
        return Py_None;
    }

    t2 = Linda_in(self->ts, t1);

    Tuple_free(t1);

    if(t2 == NULL) {
        PyErr_SetString(PyExc_KeyboardInterrupt, "Linda message interupted");
        return NULL;
    } else {
        tuple = Tuple2PyO(t2);
        Tuple_free(t2);

        return tuple;
    }
}

static PyObject* linda_TupleSpace_rd(linda_TupleSpaceObject* self, PyObject* args) {
    PyObject* tuple;
    Tuple t1;
    Tuple t2;

    if(!PyArg_ParseTuple(args, "O", &tuple)) {
        return NULL;
    }

    if(!PyTuple_Check(tuple)) {
        PyErr_SetString(PyExc_TypeError, "_rd takes a tuple.");
        return NULL;
    }

    t1 = PyO2Tuple(tuple);
    if(t1 == NULL) {
        fprintf(stderr, "Error converting tuple.\n");
        Py_INCREF(Py_None);
        return Py_None;
    }

    t2 = Linda_rd(self->ts, t1);

    Tuple_free(t1);

    if(t2 == NULL) {
        PyErr_SetString(PyExc_KeyboardInterrupt, "Linda message interupted");
        return NULL;
    } else {
        tuple = Tuple2PyO(t2);
        Tuple_free(t2);

        return tuple;
    }
}

static PyObject* linda_TupleSpace_inp(linda_TupleSpaceObject* self, PyObject* args) {
    PyObject* tuple;
    Tuple t1;
    Tuple t2;

    if(!PyArg_ParseTuple(args, "O", &tuple)) {
        return NULL;
    }

    if(!PyTuple_Check(tuple)) {
        PyErr_SetString(PyExc_TypeError, "_in takes a tuple.");
        return NULL;
    }

    t1 = PyO2Tuple(tuple);
    if(t1 == NULL) {
        fprintf(stderr, "Error converting tuple.\n");
        Py_INCREF(Py_None);
        return Py_None;
    }

    t2 = Linda_inp(self->ts, t1);
    Tuple_free(t1);

    if(t2 == NULL) {
        Py_IncRef(Py_None);
        return Py_None;
    } else {
        tuple = Tuple2PyO(t2);
        Tuple_free(t2);

        return tuple;
    }
}

static PyObject* linda_TupleSpace_rdp(linda_TupleSpaceObject* self, PyObject* args) {
    PyObject* tuple;
    Tuple t1;
    Tuple t2;

    if(!PyArg_ParseTuple(args, "O", &tuple)) {
        return NULL;
    }

    if(!PyTuple_Check(tuple)) {
        PyErr_SetString(PyExc_TypeError, "_rdp takes a tuple.");
        return NULL;
    }

    t1 = PyO2Tuple(tuple);
    if(t1 == NULL) {
        fprintf(stderr, "Error converting tuple.\n");
        Py_INCREF(Py_None);
        return Py_None;
    }

    t2 = Linda_rdp(self->ts, t1);
    Tuple_free(t1);

    if(t2 == NULL) {
        Py_IncRef(Py_None);
        return Py_None;
    } else {
        tuple = Tuple2PyO(t2);
        Tuple_free(t2);

        return tuple;
    }
}

static PyObject* linda_TupleSpace_collect(linda_TupleSpaceObject* self, PyObject* args) {
    long r;
    PyObject* tuple;
    PyObject* ts;
    Tuple t;

    if(!PyArg_ParseTuple(args, "OO", &ts, &tuple)) {
        return NULL;
    }

    if(!PyObject_IsInstance(ts, (PyObject*)&linda_TupleSpaceType)) {
        PyErr_SetString(PyExc_TypeError, "collect takes a tuplespace as the first parameter.");
        return NULL;
    }
    if(!PyTuple_Check(tuple)) {
        PyErr_SetString(PyExc_TypeError, "collect takes a tuple as the second parameter.");
        return NULL;
    }

    t = PyO2Tuple(tuple);
    if(t == NULL) {
        fprintf(stderr, "Error converting tuple.\n");
        Py_INCREF(Py_None);
        return Py_None;
    }

    r = Linda_collect(self->ts, ((linda_TupleSpaceObject*)ts)->ts, t);
    Tuple_free(t);
    if(r < 0) {
        PyErr_SetString(PyExc_KeyboardInterrupt, "Linda message interupted");
        return NULL;
    } else {
        return PyInt_FromLong(r);
    }
}

static PyObject* linda_TupleSpace_copy_collect(linda_TupleSpaceObject* self, PyObject* args) {
    long r;
    PyObject* tuple;
    PyObject* ts;
    Tuple t;

    if(!PyArg_ParseTuple(args, "OO", &ts, &tuple)) {
        return NULL;
    }

    if(!PyObject_IsInstance(ts, (PyObject*)&linda_TupleSpaceType)) {
        PyErr_SetString(PyExc_TypeError, "copy_collect takes a tuplespace as the first parameter.");
        return NULL;
    }
    if(!PyTuple_Check(tuple)) {
        PyErr_SetString(PyExc_TypeError, "copy_collect takes a tuple as the second parameter.");
        return NULL;
    }

    t = PyO2Tuple(tuple);
    if(t == NULL) {
        fprintf(stderr, "Error converting tuple.\n");
        Py_INCREF(Py_None);
        return Py_None;
    }

    r = Linda_copy_collect(self->ts, ((linda_TupleSpaceObject*)ts)->ts, t);
    Tuple_free(t);

    if(r < 0) {
        PyErr_SetString(PyExc_KeyboardInterrupt, "Linda message interupted");
        return NULL;
    } else {
        return PyInt_FromLong(r);
    }
}

static void linda_TupleSpace_dealloc(linda_TupleSpaceObject* self)
{
    Linda_deleteReference(self->ts);
    self->ob_type->tp_free(self);
}

static PyObject* linda_TupleSpace_str(linda_TupleSpaceObject* self) {
    return PyString_FromFormat("<TupleSpace %s>", self->ts);
}

static PyMethodDef tuplespace_methods[] = {
    {"_out", (PyCFunction)linda_TupleSpace_out, METH_VARARGS, "Places the given tuple into the Tuplespace."},
    {"_in", (PyCFunction)linda_TupleSpace_in, METH_VARARGS, "Removes a matching tuple from the Tuplespace."},
    {"_rd", (PyCFunction)linda_TupleSpace_rd, METH_VARARGS, "Gets a matching tuple from the Tuplespace."},
    {"_inp", (PyCFunction)linda_TupleSpace_inp, METH_VARARGS, "Removes a matching tuple from the Tuplespace. Returns None if deadlocked."},
    {"_rdp", (PyCFunction)linda_TupleSpace_rdp, METH_VARARGS, "Gets a matching tuple from the Tuplespace. Returns None if deadlocked."},
    {"collect", (PyCFunction)linda_TupleSpace_collect, METH_VARARGS, "Moves tuples from the other tuplespace to this."},
    {"copy_collect", (PyCFunction)linda_TupleSpace_copy_collect, METH_VARARGS, "Copies tuples from the other tuplespace to this."},
    {NULL}  /* Sentinel */
};

static PyObject* linda_TupleSpace_getid(linda_TupleSpaceObject *self, void *closure)
{
    return PyString_FromString(self->ts);
}

static PyGetSetDef tuplespace_getseters[] = {
    {"_id", (getter)linda_TupleSpace_getid, (setter)NULL, "The Tuplespace id.", NULL},
    {NULL}  /* Sentinel */
};

PyTypeObject linda_TupleSpaceType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "linda.TupleSpace",        /*tp_name*/
    sizeof(linda_TupleSpaceObject), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)linda_TupleSpace_dealloc,  /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    (reprfunc)linda_TupleSpace_str, /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "A TupleSpace",            /* tp_doc */
    0,                         /* tp_traverse; */
    0,                         /* tp_clear; */
    0,                         /* tp_richcompare; */
    0,                         /* tp_weaklistoffset; */
    0,                         /* tp_iter; */
    0,                         /* tp_iternext; */
    tuplespace_methods,        /* tp_methods; */
    0,                         /* tp_members */
    tuplespace_getseters,      /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)linda_TupleSpace_init, /* tp_init */
    0,                         /* tp_alloc */
    linda_TupleSpace_new,      /* tp_new */
};

void inittuplespace(PyObject* m) {
    linda_TupleSpaceObject* uts;

    linda_TupleSpaceType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&linda_TupleSpaceType) < 0)
        return;

    Py_INCREF(&linda_TupleSpaceType);
    PyModule_AddObject(m, "TupleSpace", (PyObject*)&linda_TupleSpaceType);

    uts = (linda_TupleSpaceObject*)linda_TupleSpaceType.tp_alloc(&linda_TupleSpaceType, 0);
    uts->ts = (char*)malloc(4);
    strcpy(uts->ts, "UTS");
    PyModule_AddObject(m, "uts", (PyObject*)uts);
}
