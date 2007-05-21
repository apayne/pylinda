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

static PyObject* LindaPython_connect(PyObject* self, PyObject* args) {
    int port = Linda_port;
    int r;
    if(!PyArg_ParseTuple(args, "|i", &port)) {
        return NULL;
    }
    r = Linda_connect(port);
    if(r) {
        PyModule_AddStringConstant(Linda_module, "process_id", Linda_process_id);

        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

static PyObject* LindaPython_disconnect(PyObject* self, PyObject* args) {
    Linda_disconnect();
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* LindaPython_getSD(PyObject* self, PyObject* args) {
    Linda_thread_data* tdata = Linda_get_thread_data();
    return PyInt_FromLong(tdata->sd);
}

#ifdef TYPES
static PyObject* linda_Type(PyObject* self, PyObject* args) {
    char* typespec;
    PyObject* o;
    PyObject* _class = NULL;
    PyObject* _from = NULL;
    PyObject* _to = NULL;
    LindaValue l;

    if(!PyArg_ParseTuple(args, "s|OOO", &typespec, &_class, &_from, &_to)) {
        return NULL;
    }
    if(_class != NULL && !(_from != NULL && _to != NULL)) {
        PyErr_SetString(PyExc_TypeError, "Type takes either 1 or 4 parameters.\n");
        return NULL;
    }

    l = Linda_type(typespec);

    o = Value2PyO(l);

    if(_class != NULL) {
        LindaPython_registerType(o, _class, _from, _to);
    }

    Linda_delReference(l);
    return o;
}
#endif

static PyObject* linda_Sum(PyObject* self, PyObject* args) {
    PyObject* o;
    PyObject* value = NULL;
    int sumpos;
    LindaValue v;
    PyObject* type = NULL;

    if(!PyArg_ParseTuple(args, "Oi|O", &value, &sumpos, &type)) {
        return NULL;
    }

    v = Linda_sum(PyO2Value(value), sumpos);

#ifdef TYPES
    if(type != NULL && PyObject_IsInstance(type, (PyObject*)&linda_ValueType)) {
        Linda_setType(v, ((linda_ValueObject*)type)->val);
    } else if(type != NULL) {
        PyErr_SetString(PyExc_TypeError, "PyO2Value: Invalid type object.\n");
        Linda_delReference(v);
        return NULL;
    }
#endif

    o = Value2PyO(v);

    Linda_delReference(v);
    return o;
}

static PyObject* linda_Function(PyObject* self, PyObject* args) {
    char* code;
    PyObject* o;
    LindaValue l;

    if(!PyArg_ParseTuple(args, "s", &code)) {
        return NULL;
    }

    l = Linda_function(code);
    o = Value2PyO(l);
    Linda_delReference(l);
    return o;
}

static PyObject* linda_Ptr(PyObject* self, PyObject* args) {
    PyObject* obj;
    LindaValue v;
    PyObject* o;
    LindaValue l;
    PyObject* type = NULL;

    if(!PyArg_ParseTuple(args, "O|O", &obj, &type)) {
        return NULL;
    }

    v = PyO2Value(obj);
    if(v == NULL) {
        return NULL;
    }

    l = Linda_ptr(v);

#ifdef TYPES
    if(type != NULL && PyObject_IsInstance(type, (PyObject*)&linda_ValueType)) {
        Linda_setType(l, ((linda_ValueObject*)type)->val);
    } else if(type != NULL) {
        PyErr_SetString(PyExc_TypeError, "PyO2Value: Invalid type object.\n");
        Linda_delReference(v); Linda_delReference(l);
        return NULL;
    }
#endif

    o = Value2PyO(l);
    Linda_delReference(l);
    return o;
}

static PyMethodDef LindaMethods[] = {
    {"connect",  LindaPython_connect, METH_VARARGS, "Connect to the local kernel."},
    {"disconnect",  LindaPython_disconnect, METH_NOARGS, "Disconnect from the Linda network."},
    {"recv", LindaPython_recv, METH_VARARGS, "Recieve a message from the socket."},
    {"send", LindaPython_send, METH_VARARGS, "Send a message to the socket."},
    {"getSD", LindaPython_getSD, METH_NOARGS, "Returns the socket number connceced to the server."},
#ifdef TYPES
    {"Type", linda_Type, METH_VARARGS, ""},
#endif
    {"Sum", linda_Sum, METH_VARARGS, ""},
    {"Function", linda_Function, METH_VARARGS, ""},
    {"Ptr", linda_Ptr, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC init_linda(void)
{
    Linda_init();

    Linda_module = Py_InitModule("_linda", LindaMethods);

    PyModule_AddObject(Linda_module, "version", PyString_FromString(Linda_version));

#ifdef TYPES
    PyModule_AddObject(Linda_module, "use_types", Py_True);
    if(Linda_register_types) {
        Py_INCREF(Py_True);
        PyModule_AddObject(Linda_module, "register_types", Py_True);
    } else {
        Py_INCREF(Py_False);
        PyModule_AddObject(Linda_module, "register_types", Py_False);
    }
#else
    PyModule_AddObject(Linda_module, "use_types", Py_False);
#endif

    inittuplespace(Linda_module);
    inittsref(Linda_module);
    initvalue(Linda_module);
    inittypemap(Linda_module);
    initregistery(Linda_module);
    initmemo(Linda_module);
}
