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

#define LINDA_SERVER
#define HACKY_MAGIC
#include "linda_python.h"
#ifdef WIN32
#define LSEXPORT
#define LSIMPORT __declspec(dllimport)
#endif
#include "linda_server.h"

#ifdef USE_DOMAIN_SOCKETS
#include <sys/un.h>
#endif
#include <sys/types.h> 
#ifdef USE_DOMAIN_SOCKETS
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif

PyObject* LindaServer_module;

int LindaPython_is_server = 1;

static PyObject* LindaServerPython_init(PyObject *self, PyObject* args) {
    Linda_init();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* LindaServerPython_serve(PyObject *self, PyObject* args) {
    int use_domain;
    int r;
    int port = Linda_port;
    if(!PyArg_ParseTuple(args, "i|i", &use_domain, &port)) {
        return NULL;
    }
    r = Linda_serve(use_domain, port);
    if(r) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

static PyObject* LindaServerPython_finalise(PyObject *self, PyObject* args) {
    Linda_finalise();

    Py_INCREF(Py_None);
    return Py_None;
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

#ifndef WIN32
    oval = fcntl(sd, F_GETFL, 0);
#endif
    if(val) {
#ifdef WIN32
        ioctlsocket(sd, FIONBIO, &val);
#else
        oval &= O_NONBLOCK;
#endif
    } else {
#ifdef WIN32
        ioctlsocket(sd, FIONBIO, &val);
#else
        oval = ~(~oval & O_NONBLOCK);
#endif
    }
#ifndef WIN32
    fcntl(sd, F_GETFL, oval);
#endif

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

static PyObject* LindaServerPython_setnodeid(PyObject *self, PyObject* args) {
    char* nid;

    if(!PyArg_ParseTuple(args, "s", &nid)) {
        return NULL;
    }

    Linda_setNodeID(nid);

    Py_INCREF(Py_None);
    return Py_None;
}

#ifdef TYPES
static PyObject* linda_TypeFromId(PyObject* self, PyObject* args) {
    char* typeid;
    PyObject* o;
    LindaValue l;

    if(!PyArg_ParseTuple(args, "s", &typeid)) {
        return NULL;
    }

    l = Minimal_typeFromId(typeid);
    o = Value2PyO(l);
    Linda_delReference(l);
    return o;
}

static PyObject* LindaServerPython_Ptr(PyObject* self, PyObject* args) {
    PyObject* obj;
    LindaValue v;
    PyObject* o;
    LindaValue l;

    if(!PyArg_ParseTuple(args, "O", &obj)) {
        return NULL;
    }

    v = PyO2Value(obj);
    if(v == NULL) {
        return NULL;
    }

    l = Linda_ptr(v);
    o = Value2PyO(l);
    Linda_delReference(l);
    return o;
}
#endif

static PyMethodDef LindaServerMethods[] = {
    {"init",  LindaServerPython_init, METH_NOARGS, ""},
    {"serve",  LindaServerPython_serve, METH_VARARGS, "Create the listening sockets."},
    {"finalise",  LindaServerPython_finalise, METH_NOARGS, ""},
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
    {"setnodeid",  LindaServerPython_setnodeid, METH_VARARGS, "."},
#ifdef TYPES
    {"Ptr",  LindaServerPython_Ptr, METH_VARARGS, "Set the id of this node."},
    {"TypeFromId", linda_TypeFromId, METH_VARARGS, ""},
#endif
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC init_linda_server(void)
{
    LindaServer_module = Py_InitModule("_linda_server", LindaServerMethods);

#ifdef TYPES
    PyModule_AddObject(LindaServer_module, "use_types", Py_True);
    if(Linda_register_types) {
        Py_INCREF(Py_True);
        PyModule_AddObject(LindaServer_module, "register_types", Py_True);
    } else {
        Py_INCREF(Py_False);
        PyModule_AddObject(LindaServer_module, "register_types", Py_False);
    }
#else
    PyModule_AddObject(LindaServer_module, "use_types", Py_False);
#endif

    PyModule_AddObject(LindaServer_module, "version", PyString_FromString(Linda_version));

    inittsref(LindaServer_module);
    initvalue(LindaServer_module);
    inittypemap(LindaServer_module);
}
