#include "python2.4/Python.h"

#define LINDA_SERVER
#include "../src/linda.h"
#include "../src/linda_internal.h"
#include "../src/linda_server.h"

#include "linda_python.h"

PyObject* LindaServer_module;

int LindaPython_is_server = 1;

static PyObject* LindaServerPython_serve(PyObject *self, PyObject* args) {
    int r = Linda_serve();
    if(r) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

static PyObject* LindaServerPython_serverSockets(PyObject *self, PyObject* args) {
#ifdef USE_DOMAIN_SOCKETS
    return Py_BuildValue("(ii)", Linda_sd, Linda_udd);
#else
    return Py_BuildValue("(i)", Linda_sd);
#endif
}

static PyObject* LindaServerPython_accept(PyObject *self, PyObject* args) {
    int sd;
    if(!PyArg_ParseTuple(args, "i", &sd)) {
        return NULL;
    }

    return PyInt_FromLong(Linda_accept(sd));
}

static PyObject* LindaServerPython_disconnect(PyObject *self, PyObject* args) {
    Linda_server_disconnect();
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* LindaServerPython_connect(PyObject *self, PyObject* args) {
    int sd;
    char* address;
    if(!PyArg_ParseTuple(args, "s", &address)) {
        return NULL;
    }

    sd = Linda_connect(address);

    if(sd > 0) {
        return PyInt_FromLong(sd);
    } else {
        Py_INCREF(Py_None);
        return Py_None;
    }
}

static PyObject* LindaServerPython_sddisconnect(PyObject *self, PyObject* args) {
    int sd;
    if(!PyArg_ParseTuple(args, "i", &sd)) {
        return NULL;
    }

    Linda_disconnect(sd);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef LindaServerMethods[] = {
    {"serve",  LindaServerPython_serve, METH_NOARGS, "Create the listening sockets."},
    {"serverSockets",  LindaServerPython_serverSockets, METH_NOARGS, "Return the sockets that are accepting connectings"},
    {"accept",  LindaServerPython_accept, METH_VARARGS, "Accept a new connection."},
    {"recv", LindaPython_recv, METH_VARARGS, "Recieve a message from the socket."},
    {"send", LindaPython_send, METH_VARARGS, "Send a message to the socket."},
    {"server_disconnect",  LindaServerPython_disconnect, METH_NOARGS, "Stop serving."},
    {"connect",  LindaServerPython_connect, METH_VARARGS, "Connect to another server."},
    {"disconnect",  LindaServerPython_sddisconnect, METH_VARARGS, "Disconnect a socket."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC init_linda_server(void)
{
    LindaServer_module = Py_InitModule("_linda_server", LindaServerMethods);
    inittsref(LindaServer_module);
}
