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
#define HACKY_MAGIC
#include "linda_internal.h"

PyObject* LindaPython_recv(PyObject *self, PyObject* args) {
    int sd;
    Message* m;

    if(!PyArg_ParseTuple(args, "i", &sd)) {
        return NULL;
    }

    m = Message_recv(sd);
    if(m == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    } else {
        PyObject* t;
        PyObject* msgid;
        if(m->msgid == NULL) {
            Py_INCREF(Py_None);
            msgid = Py_None;
        } else {
            msgid = Py_BuildValue("(ssi)", m->msgid->dest, m->msgid->source, m->msgid->count);
        }
        switch(m->type) {
        case L_DONE:
            t = Py_BuildValue("(Os)", msgid, "DONE");
            break;
        case L_DONT_KNOW:
            t = Py_BuildValue("(Os)", msgid, "DONT_KNOW");
            break;
        case L_RESULT_STRING:
            t = Py_BuildValue("(Oss)", msgid, "RESULT_STRING", m->string);
            break;
        case L_RESULT_INT:
            t = Py_BuildValue("(Osi)", msgid, "RESULT_INT", m->i);
            break;
        case L_RESULT_TUPLE:
            t = Py_BuildValue("(OsO)", msgid, "RESULT_TUPLE", Tuple2PyO(m->tuple));
            break;
        case L_OUT:
            t = Py_BuildValue("(OssO)", msgid, "OUT", Minimal_getTupleSpace(m->out.ts), Tuple2PyO(m->out.t));
            break;
        case L_IN:
            t = Py_BuildValue("(OssOs)", msgid, "IN", Minimal_getTupleSpace(m->in.ts), Tuple2PyO(m->in.t), m->in.tid);
            break;
        case L_RD:
            t = Py_BuildValue("(OssOs)", msgid, "RD", Minimal_getTupleSpace(m->rd.ts), Tuple2PyO(m->rd.t), m->in.tid);
            break;
        case L_INP:
            t = Py_BuildValue("(OssOs)", msgid, "INP", Minimal_getTupleSpace(m->in.ts), Tuple2PyO(m->in.t), m->rd.tid);
            break;
        case L_RDP:
            t = Py_BuildValue("(OssOs)", msgid, "RDP", Minimal_getTupleSpace(m->rd.ts), Tuple2PyO(m->rd.t), m->rd.tid);
            break;
        case L_COLLECT:
            t = Py_BuildValue("(OsssO)", msgid, "COLLECT", Minimal_getTupleSpace(m->collect.ts1), Minimal_getTupleSpace(m->collect.ts2), Tuple2PyO(m->collect.t));
            break;
        case L_COPY_COLLECT:
            t = Py_BuildValue("(OsssO)", msgid, "COPY_COLLECT", Minimal_getTupleSpace(m->collect.ts1), Minimal_getTupleSpace(m->collect.ts2), Tuple2PyO(m->collect.t));
            break;
        case L_UNBLOCK:
            t = Py_BuildValue("(Os)", msgid, "UNBLOCK");
            break;
        case L_CREATE_TUPLESPACE:
            t = Py_BuildValue("(Oss)", msgid, "CREATE_TUPLESPACE", m->string);
            break;
        case L_ADD_REFERENCE:
            t = Py_BuildValue("(Osss)", msgid, "ADD_REFERENCE", m->ref.ts, m->ref.tid);
            break;
        case L_DELETE_REFERENCE:
            t = Py_BuildValue("(Osss)", msgid, "DELETE_REFERENCE", m->ref.ts, m->ref.tid);
            break;
        case L_MONITOR:
            t = Py_BuildValue("(Os)", msgid, "MONITOR");
            break;
        case L_LIST_TS:
            t = Py_BuildValue("(Os)", msgid, "LIST_TS");
            break;
        case L_INSPECT:
            t = Py_BuildValue("(Oss)", msgid, "INSPECT", Minimal_getTupleSpace(m->ts));
            break;
        case L_GET_ROUTES:
            t = Py_BuildValue("(Os)", msgid, "GET_ROUTES");
            break;
        case L_REGISTER_PROCESS:
            t = Py_BuildValue("(Os)", msgid, "REGISTER_PROCESS");
            break;
        case L_REGISTER_THREAD:
            t = Py_BuildValue("(Oss)", msgid, "REGISTER_THREAD", m->ts);
            break;
        case L_REGISTER_TYPE:
            t = Py_BuildValue("(OsOs)", msgid, "REGISTER_TYPE", Value2PyO(m->typestruct.typeobj), m->typestruct.pid);
            break;
        case L_UPDATE_TYPE:
            t = Py_BuildValue("(OsiOs)", msgid, "UPDATE_TYPE", m->typestruct.type_id, Value2PyO(m->typestruct.typeobj), m->typestruct.pid);
            break;
        case L_GET_NODE_ID:
            t = Py_BuildValue("(Os)", msgid, "GET_NODE_ID");
            break;
        case L_MY_NAME_IS:
            t = Py_BuildValue("(Oss)", msgid, "MY_NAME_IS", m->string);
            break;
        case L_REGISTER_PARTITION:
            t = Py_BuildValue("(Osss)", msgid, "REGISTER_PARTITION", m->ref.ts, m->ref.tid);
            break;
        case L_GET_PARTITIONS:
            t = Py_BuildValue("(Oss)", msgid, "GET_PARTITIONS", Minimal_getTupleSpace(m->ts));
            break;
        case L_DELETED_PARTITION:
            t = Py_BuildValue("(Osss)", msgid, "DELETED_PARTITION", m->ref.ts, m->ref.tid);
            break;
        case L_GET_REQUESTS:
            t = Py_BuildValue("(Oss)", msgid, "GET_REQUESTS", Minimal_getTupleSpace(m->ts));
            break;
        case L_GET_NEIGHBOURS:
            t = Py_BuildValue("(Os)", msgid, "GET_NEIGHBOURS");
            break;
        case L_GET_CONNECTION_DETAILS:
            t = Py_BuildValue("(Oss)", msgid, "GET_CONNECTION_DETAILS", m->string);
            break;
        case L_TUPLE_REQUEST:
            t = Py_BuildValue("(OssO)", msgid, "TUPLE_REQUEST", Minimal_getTupleSpace(m->tuple_request.ts), Tuple2PyO(m->tuple_request.t));
            break;
        case L_CANCEL_REQUEST:
            t = Py_BuildValue("(OssO)", msgid, "CANCEL_REQUEST", Minimal_getTupleSpace(m->tuple_request.ts), Tuple2PyO(m->tuple_request.t));
            break;
        case L_MULTIPLE_IN:
            t = Py_BuildValue("(OssO)", msgid, "MULTIPLE_IN", Minimal_getTupleSpace(m->tuple_request.ts), Tuple2PyO(m->tuple_request.t));
            break;
        default:
            PyErr_SetObject(PyExc_SystemError, PyString_FromFormat("Received invalid message (%i).", m->type));
            Message_free(m);
            return NULL;
        }
        Message_free(m);
        return t;
    }
}

#define PYTHON_TO_MSG_NONE(name, func) \
    } else if(strcmp(action, name) == 0) { \
        if(PyTuple_Size(tuple) == (offset+1)) { \
            m = Message_##func(); \
            Message_send(sd, msgid, m); \
            Message_free(m); \
        } else { \
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of arguments.\n", action)); \
            return NULL; \
        }

#define PYTHON_TO_MSG_STRING(name, func) \
    } else if(strcmp(action, name) == 0) { \
        if(PyTuple_Size(tuple) != (offset+2)) { \
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of arguments (%i not %i).\n", action, (int)PyTuple_Size(tuple), offset+2)); \
            return NULL; \
        } else if(!PyString_Check(PyTuple_GetItem(tuple, offset+1))) { \
            PyObject* repr = PyObject_Repr(PyTuple_GetItem(tuple, offset+1)); \
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("'%s' is not a string.\n", PyString_AsString(repr))); \
            Py_DecRef(repr); \
            return NULL; \
        } else { \
            m = Message_##func(PyString_AsString(PyTuple_GetItem(tuple, offset+1))); \
            Message_send(sd, msgid, m); \
            Message_free(m); \
        }

#define PYTHON_TO_MSG_TS(name, func) \
    } else if(strcmp(action, name) == 0) { \
        if(PyTuple_Size(tuple) != (offset+2)) { \
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of arguments (%i not %i).\n", action, (int)PyTuple_Size(tuple), offset+2)); \
            return NULL; \
        } else if(!PyString_Check(PyTuple_GetItem(tuple, offset+1))) { \
            PyObject* repr = PyObject_Repr(PyTuple_GetItem(tuple, offset+1)); \
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("'%s' is not a string.\n", PyString_AsString(repr))); \
            Py_DecRef(repr); \
            return NULL; \
        } else { \
            LindaValue ts = Minimal_tupleSpace(PyString_AsString(PyTuple_GetItem(tuple, offset+1))); \
            m = Message_##func(ts); \
            Message_send(sd, msgid, m); \
            Message_free(m); \
            Linda_delReference(ts); \
        }

PyObject* LindaPython_send(PyObject *self, PyObject* args) {
    int sd;
    int offset;
    char* action;
    MsgID* msgid = NULL;
    PyObject* msgidobj;
    PyObject* tuple;
    Message* m;

    if(!PyArg_ParseTuple(args, "iO", &sd, &tuple)) {
        return NULL;
    }
    if(!PyTuple_Check(tuple)) {
        PyErr_SetString(PyExc_TypeError, "send takes a tuple as the second arguments");
        return NULL;
    }
    if(PyTuple_Check(PyTuple_GetItem(tuple, 0))) {
        msgidobj = PyTuple_GetItem(tuple, 0);
        if(PyTuple_Size(msgidobj) != 3) {
            PyObject* repr = PyObject_Repr(msgidobj);
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("The MsgID should be a three-tuple. %s.", PyString_AsString(repr)));
            Py_DecRef(repr);
            return NULL;
        }
        msgid = (MsgID*)malloc(sizeof(MsgID));
        msgid->dest = (char*)malloc(strlen(PyString_AsString(PyTuple_GetItem(msgidobj, 0)))+1);
        strcpy(msgid->dest, PyString_AsString(PyTuple_GetItem(msgidobj, 0)));
        msgid->source = (char*)malloc(strlen(PyString_AsString(PyTuple_GetItem(msgidobj, 1)))+1);
        strcpy(msgid->source, PyString_AsString(PyTuple_GetItem(msgidobj, 1)));
        msgid->count = PyInt_AsLong(PyTuple_GetItem(msgidobj, 2));
        action = PyString_AsString(PyTuple_GetItem(tuple, 1));
        offset = 1;
    } else {
        action = PyString_AsString(PyTuple_GetItem(tuple, 0));
        offset = 0;
    }
    if(action == NULL) { PyErr_SetString(PyExc_SystemError, "NULL action!"); return NULL; }

    if(0) {
    PYTHON_TO_MSG_NONE("DONE", done)
    PYTHON_TO_MSG_NONE("DONT_KNOW", dont_know)
    PYTHON_TO_MSG_STRING("RESULT_STRING", result_string)
    } else if(strcmp(action, "RESULT_INT") == 0) {
        if(PyTuple_Size(tuple) == (offset+2) && PyInt_Check(PyTuple_GetItem(tuple, offset+1))) {
            m = Message_result_int(PyInt_AsLong(PyTuple_GetItem(tuple, offset+1)));
            Message_send(sd, msgid, m);
            Message_free(m);
        } else {
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of arguments.\n", action));
            return NULL;
        }
    } else if(strcmp(action, "RESULT_TUPLE") == 0) {
        if(PyTuple_Size(tuple) == (offset+2) && PyTuple_Check(PyTuple_GetItem(tuple, offset+1))) {
            LindaValue t = PyO2Tuple(PyTuple_GetItem(tuple, offset+1));
            m = Message_result_tuple(t);
            Message_send(sd, msgid, m);
            Message_free(m);
            Linda_delReference(t);
        } else if(PyTuple_Size(tuple) == (offset+2)
               && PyObject_IsInstance(PyTuple_GetItem(tuple, offset+1), (PyObject*)&linda_ValueType)
               && Linda_isTuple(((linda_ValueObject*)PyTuple_GetItem(tuple, offset+1))->val)) {
            LindaValue t = ((linda_ValueObject*)PyTuple_GetItem(tuple, offset+1))->val;
            Linda_addReference(t);
            m = Message_result_tuple(t);
            Message_send(sd, msgid, m);
            Message_free(m);
            Linda_delReference(t);
        } else {
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of arguments.\n", action));
            return NULL;
        }
    PYTHON_TO_MSG_NONE("CREATE_TUPLESPACE", createTuplespace)
    PYTHON_TO_MSG_TS("ADD_REFERENCE", addReference)
    PYTHON_TO_MSG_TS("DELETE_REFERENCE", deleteReference)
    PYTHON_TO_MSG_NONE("MONITOR", monitor)
    PYTHON_TO_MSG_NONE("LIST_TS", list_ts)
    PYTHON_TO_MSG_TS("INSPECT", inspect)
    PYTHON_TO_MSG_NONE("GET_ROUTES", get_routes)
    PYTHON_TO_MSG_STRING("MY_NAME_IS", my_name_is)
    PYTHON_TO_MSG_NONE("GET_NODE_ID", get_node_id)
    } else if(strcmp(action, "REGISTER_PARTITION") == 0) {
        if(PyTuple_Size(tuple) == (offset+3) && PyString_Check(PyTuple_GetItem(tuple, offset+1)) && PyString_Check(PyTuple_GetItem(tuple, offset+2))) {
            m = Message_register_partition(Minimal_tupleSpace(PyString_AsString(PyTuple_GetItem(tuple, offset+1))),
                                            PyString_AsString(PyTuple_GetItem(tuple, offset+2)));
            Message_send(sd, msgid, m);
            Message_free(m);
        } else {
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of arguments.\n", action));
            return NULL;
        }
    PYTHON_TO_MSG_TS("GET_PARTITIONS", get_partitions)
    } else if(strcmp(action, "DELETED_PARTITION") == 0) {
        if(PyTuple_Size(tuple) == (offset+3) && PyString_Check(PyTuple_GetItem(tuple, offset+1)) && PyString_Check(PyTuple_GetItem(tuple, offset+2))) {
            m = Message_deleted_partition(Minimal_tupleSpace(PyString_AsString(PyTuple_GetItem(tuple, offset+1))), 
                                            PyString_AsString(PyTuple_GetItem(tuple, offset+2)));
            Message_send(sd, msgid, m);
            Message_free(m);
        } else {
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of arguments.\n", action));
            return NULL;
        }
    PYTHON_TO_MSG_TS("GET_REQUESTS", get_requests)
    PYTHON_TO_MSG_NONE("GET_NEIGHBOURS", get_neighbours)
    PYTHON_TO_MSG_STRING("GET_CONNECTION_DETAILS", get_connection_details)
    } else if(strcmp(action, "COLLECT") == 0) {
        if(PyTuple_Size(tuple) == (offset+4) && PyString_Check(PyTuple_GetItem(tuple, offset+1)) && PyString_Check(PyTuple_GetItem(tuple, offset+2)) && PyTuple_Check(PyTuple_GetItem(tuple, offset+3))) {
            LindaValue t = PyO2Tuple(PyTuple_GetItem(tuple, offset+3));
            m = Message_collect(Minimal_tupleSpace(PyString_AsString(PyTuple_GetItem(tuple, offset+1))),
                                Minimal_tupleSpace(PyString_AsString(PyTuple_GetItem(tuple, offset+2))), t);
            Message_send(sd, msgid, m);
            Message_free(m);
            Linda_delReference(t);
        } else {
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of arguments.\n", action));
            return NULL;
        }
    } else if(strcmp(action, "COPY_COLLECT") == 0) {
        if(PyTuple_Size(tuple) == (offset+4) && PyString_Check(PyTuple_GetItem(tuple, offset+1)) && PyString_Check(PyTuple_GetItem(tuple, offset+2)) && PyTuple_Check(PyTuple_GetItem(tuple, offset+3))) {
            LindaValue t = PyO2Tuple(PyTuple_GetItem(tuple, offset+3));
            m = Message_copy_collect(Minimal_tupleSpace(PyString_AsString(PyTuple_GetItem(tuple, offset+1))), 
                                    Minimal_tupleSpace(PyString_AsString(PyTuple_GetItem(tuple, offset+2))), t);
            Message_send(sd, msgid, m);
            Message_free(m);
            Linda_delReference(t);
        } else {
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of arguments.\n", action));
            return NULL;
        }
    PYTHON_TO_MSG_NONE("UNBLOCK", unblock)
    } else if(strcmp(action, "TUPLE_REQUEST") == 0) {
        if(PyTuple_Size(tuple) == (offset+3) && PyString_Check(PyTuple_GetItem(tuple, offset+1)) && PyTuple_Check(PyTuple_GetItem(tuple, offset+2))) {
            LindaValue t = PyO2Tuple(PyTuple_GetItem(tuple, offset+2));
            m = Message_tuple_request(Minimal_tupleSpace(PyString_AsString(PyTuple_GetItem(tuple, offset+1))), t);
            Message_send(sd, msgid, m);
            Message_free(m);
            Linda_delReference(t);
        } else {
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of arguments.\n", action));
            return NULL;
        }
    } else if(strcmp(action, "CANCEL_REQUEST") == 0) {
        if(PyTuple_Size(tuple) == (offset+3) && PyString_Check(PyTuple_GetItem(tuple, offset+1)) && PyTuple_Check(PyTuple_GetItem(tuple, offset+2))) {
            LindaValue t = PyO2Tuple(PyTuple_GetItem(tuple, offset+2));
            m = Message_cancel_request(Minimal_tupleSpace(PyString_AsString(PyTuple_GetItem(tuple, offset+1))), t);
            Message_send(sd, msgid, m);
            Message_free(m);
            Linda_delReference(t);
        } else {
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of arguments.\n", action));
            return NULL;
        }
    } else if(strcmp(action, "MULTIPLE_IN") == 0) {
        if(PyTuple_Size(tuple) == (offset+3) && PyString_Check(PyTuple_GetItem(tuple, offset+1)) && PyTuple_Check(PyTuple_GetItem(tuple, offset+2))) {
            LindaValue t = PyO2Tuple(PyTuple_GetItem(tuple, offset+2));
            m = Message_multiple_in(Minimal_tupleSpace(PyString_AsString(PyTuple_GetItem(tuple, offset+1))), t);
            Message_send(sd, msgid, m);
            Message_free(m);
            Linda_delReference(t);
        } else {
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of arguments.\n", action));
            return NULL;
        }
    } else if(strcmp(action, "REGISTER_TYPE") == 0) {
        if(PyTuple_Size(tuple) == (offset+2)) {
            LindaValue t = PyO2Value(PyTuple_GetItem(tuple, offset+1));
            m = Message_register_type(t);
            Message_send(sd, msgid, m);
            Message_free(m);
            Linda_delReference(t);
        } else {
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of arguments.\n", action));
            return NULL;
        }
    } else if(strcmp(action, "UPDATE_TYPE") == 0) {
        if(PyTuple_Size(tuple) == (offset+3)) {
            LindaValue id = PyO2Value(PyTuple_GetItem(tuple, offset+1));
            LindaValue t = PyO2Value(PyTuple_GetItem(tuple, offset+2));
            m = Message_update_type(Linda_getInt(id), t, -1);
            Message_send(sd, msgid, m);
            Message_free(m);
            Linda_delReference(id);
            Linda_delReference(t);
        } else if(PyTuple_Size(tuple) == (offset+4)) {
            LindaValue id = PyO2Value(PyTuple_GetItem(tuple, offset+1));
            LindaValue t = PyO2Value(PyTuple_GetItem(tuple, offset+2));
            LindaValue p = PyO2Value(PyTuple_GetItem(tuple, offset+3));
            m = Message_update_type(Linda_getInt(id), t, Linda_getInt(p));
            Message_send(sd, msgid, m);
            Message_free(m);
            Linda_delReference(id);
            Linda_delReference(t);
            Linda_delReference(p);
        } else {
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of arguments.\n", action));
            return NULL;
        }
    } else {
        PyErr_SetObject(PyExc_SystemError, PyString_FromFormat("%s is an unknown message. Not sent.\n", action));
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
