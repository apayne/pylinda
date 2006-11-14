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

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include PYTHON_H

#define LINDA_SERVER
#include "../src/linda.h"
#include "../src/linda_internal.h"
#include "../src/linda_server.h"

#include "linda_python.h"

PyObject* LindaServer_module;

int LindaPython_is_server = 1;

static PyObject* LindaServerPython_serve(PyObject *self, PyObject* args) {
    int use_domain;
    int port = Linda_port;
    if(!PyArg_ParseTuple(args, "i|i", &use_domain, &port)) {
        return NULL;
    }
    int r = Linda_serve(use_domain, port);
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

static PyObject* LindaServerPython_setblocking(PyObject *self, PyObject* args) {
    int sd;
    int val;
    int oval;
    if(!PyArg_ParseTuple(args, "ii", &sd, &val)) {
        return NULL;
    }

    oval = fcntl(sd, F_GETFL, 0);
    if(val) {
        oval &= O_NONBLOCK;
    } else {
        oval = ~(~oval & O_NONBLOCK);
    }
    fcntl(sd, F_GETFL, oval);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* LindaServerPython_getsockname(PyObject *self, PyObject* args) {
    int sd;
    struct sockaddr addr;
    socklen_t len = sizeof(struct sockaddr);
    if(!PyArg_ParseTuple(args, "i", &sd)) {
        return NULL;
    }

    getsockname(sd, &addr, &len);

    return PyString_FromString(addr.sa_data);
}

static PyObject* LindaServerPython_getpeername(PyObject *self, PyObject* args) {
    int sd;
    struct sockaddr addr;
    socklen_t len = sizeof(struct sockaddr);
    if(!PyArg_ParseTuple(args, "i", &sd)) {
        return NULL;
    }

    if(getpeername(sd, &addr, &len) == -1) {
        fprintf(stderr, "GetPeerName Error: %s\n", strerror(errno));
    }

    return PyString_FromFormat("%s:%i", inet_ntoa(((struct sockaddr_in*)&addr)->sin_addr), (int)(((struct sockaddr_in*)&addr)->sin_port));
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
    {"serve",  LindaServerPython_serve, METH_VARARGS, "Create the listening sockets."},
    {"serverSockets",  LindaServerPython_serverSockets, METH_NOARGS, "Return the sockets that are accepting connections"},
    {"accept",  LindaServerPython_accept, METH_VARARGS, "Accept a new connection."},
    {"recv", LindaPython_recv, METH_VARARGS, "Recieve a message from the socket."},
    {"send", LindaPython_send, METH_VARARGS, "Send a message to the socket."},
    {"server_disconnect",  LindaServerPython_disconnect, METH_NOARGS, "Stop serving."},
    {"connect",  LindaServerPython_connect, METH_VARARGS, "Connect to another server."},

    {"setblocking",  LindaServerPython_setblocking, METH_VARARGS, "Set blocking or non-blocking mode of the socket."},
    {"getsockname",  LindaServerPython_getsockname, METH_VARARGS, "Return the socket's own address."},
    {"getpeername",  LindaServerPython_getpeername, METH_VARARGS, "Return the remote address to which the socket is connected."},
    {"socket_disconnect",  LindaServerPython_sddisconnect, METH_VARARGS, "Disconnect a socket."},
    {"socket_close",  LindaServerPython_sddisconnect, METH_VARARGS, "Close a socket."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC init_linda_server(void)
{
    LindaServer_module = Py_InitModule("_linda_server", LindaServerMethods);
    inittsref(LindaServer_module);
    initvalue(LindaServer_module);
}
