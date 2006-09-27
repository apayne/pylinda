#include "python2.4/Python.h"

#include "linda.h"
#include "linda_python.h"

extern int Linda_sd;

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
    return PyInt_FromLong(Linda_sd);
}

static PyMethodDef LindaMethods[] = {
    {"connect",  LindaPython_connect, METH_VARARGS, "Connect to the local kernel."},
    {"disconnect",  LindaPython_disconnect, METH_NOARGS, "Disconnect from the Linda network."},
    {"recv", LindaPython_recv, METH_VARARGS, "Recieve a message from the socket."},
    {"send", LindaPython_send, METH_VARARGS, "Send a message to the socket."},
    {"getSD", LindaPython_getSD, METH_NOARGS, "Returns the socket number connceced to the serve."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC init_linda(void)
{
    Linda_module = Py_InitModule("_linda", LindaMethods);

    PyModule_AddObject(Linda_module, "version", PyString_FromString(version));

    inittuplespace(Linda_module);
    inittsref(Linda_module);
}
