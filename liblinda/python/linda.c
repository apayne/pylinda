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
#include "linda_internal.h"
#include "linda_python.h"

PyObject* Linda_module;

int LindaPython_is_server = 0;

static PyObject* LindaPython_connect(PyObject* self, PyObject* args) {
    int port = Linda_port;
    if(!PyArg_ParseTuple(args, "|i", &port)) {
        return NULL;
    }
    int r = Linda_connect(port);
    if(r) {
        PyModule_AddStringConstant(Linda_module, "process_id", process_id);

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

static PyMethodDef LindaMethods[] = {
    {"connect",  LindaPython_connect, METH_VARARGS, "Connect to the local kernel."},
    {"disconnect",  LindaPython_disconnect, METH_NOARGS, "Disconnect from the Linda network."},
    {"recv", LindaPython_recv, METH_VARARGS, "Recieve a message from the socket."},
    {"send", LindaPython_send, METH_VARARGS, "Send a message to the socket."},
    {"getSD", LindaPython_getSD, METH_NOARGS, "Returns the socket number connceced to the server."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC init_linda(void)
{
    Linda_init();

    Linda_module = Py_InitModule("_linda", LindaMethods);

    PyModule_AddObject(Linda_module, "version", PyString_FromString(version));

    inittuplespace(Linda_module);
    inittsref(Linda_module);
}
