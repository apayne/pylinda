#include "python2.4/Python.h"

#include "linda.h"
#include "../src/linda_internal.h"
#include "linda_python.h"

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
        case DONE:
            t = Py_BuildValue("(Os)", msgid, "DONE");
            break;
        case DONT_KNOW:
            t = Py_BuildValue("(Os)", msgid, "DONT_KNOW");
            break;
        case RESULT_STRING:
            t = Py_BuildValue("(Oss)", msgid, "RESULT_STRING", m->string);
            break;
        case RESULT_INT:
            t = Py_BuildValue("(Osi)", msgid, "RESULT_INT", m->i);
            break;
        case RESULT_TUPLE:
            t = Py_BuildValue("(OsO)", msgid, "RESULT_TUPLE", Tuple2PyO(m->tuple));
            break;
        case OUT:
            t = Py_BuildValue("(OssO)", msgid, "OUT", m->out.ts, Tuple2PyO(m->out.t));
            break;
        case IN:
            t = Py_BuildValue("(OssOs)", msgid, "IN", m->in.ts, Tuple2PyO(m->in.t), m->in.tid);
            break;
        case RD:
            t = Py_BuildValue("(OssOs)", msgid, "RD", m->rd.ts, Tuple2PyO(m->rd.t), m->in.tid);
            break;
        case INP:
            t = Py_BuildValue("(OssOs)", msgid, "INP", m->in.ts, Tuple2PyO(m->in.t), m->rd.tid);
            break;
        case RDP:
            t = Py_BuildValue("(OssOs)", msgid, "RDP", m->rd.ts, Tuple2PyO(m->rd.t), m->rd.tid);
            break;
        case CREATE_TUPLESPACE:
            t = Py_BuildValue("(Oss)", msgid, "CREATE_TUPLESPACE", m->string);
            break;
        case ADD_REFERENCE:
            t = Py_BuildValue("(Osss)", msgid, "ADD_REFERENCE", m->ref.ts, m->ref.tid);
            break;
        case DELETE_REFERENCE:
            t = Py_BuildValue("(Osss)", msgid, "DELETE_REFERENCE", m->ref.ts, m->ref.tid);
            break;
        case MONITOR:
            t = Py_BuildValue("(Os)", msgid, "MONITOR");
            break;
        case LIST_TS:
            t = Py_BuildValue("(Os)", msgid, "LIST_TS");
            break;
        case INSPECT:
            t = Py_BuildValue("(Oss)", msgid, "INSPECT", m->string);
            break;
        case REGISTER_PROCESS:
            t = Py_BuildValue("(Os)", msgid, "REGISTER_PROCESS");
            break;
        case REGISTER_THREAD:
            t = Py_BuildValue("(Oss)", msgid, "REGISTER_THREAD", m->ts);
            break;
        case GET_NODE_ID:
            t = Py_BuildValue("(Os)", msgid, "GET_NODE_ID");
            break;
        case MY_NAME_IS:
            t = Py_BuildValue("(Oss)", msgid, "MY_NAME_IS", m->string);
            break;
        case REGISTER_PARTITION:
            t = Py_BuildValue("(Osss)", msgid, "REGISTER_PARTITION", m->ref.ts, m->ref.tid);
            break;
        case GET_PARTITIONS:
            printf("%s\n", m->ts);
            t = Py_BuildValue("(Osss)", msgid, "GET_PARTITIONS", m->ts);
            break;
        case DELETED_PARTITION:
            t = Py_BuildValue("(Osss)", msgid, "DELETED_PARTITION", m->ref.ts, m->ref.tid);
            break;
        case GET_REQUESTS:
            t = Py_BuildValue("(Oss)", msgid, "GET_REQUESTS", m->ts);
            break;
        case GET_NEIGHBOURS:
            t = Py_BuildValue("(Os)", msgid, "GET_NEIGHBOURS");
            break;
        case GET_CONNECTION_DETAILS:
            t = Py_BuildValue("(Oss)", msgid, "GET_CONNECTION_DETAILS", m->string);
            break;
        case TUPLE_REQUEST:
            t = Py_BuildValue("(OssO)", msgid, "TUPLE_REQUEST", m->tuple_request.ts, Tuple2PyO(m->tuple_request.t));
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
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of argument.\n", action)); \
            return NULL; \
        }

#define PYTHON_TO_MSG_STRING(name, func) \
    } else if(strcmp(action, name) == 0) { \
        if(PyTuple_Size(tuple) != (offset+2)) { \
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of argument (%i not %i).\n", action, PyTuple_Size(tuple), offset+2)); \
            return NULL; \
        } else if(!PyString_Check(PyTuple_GetItem(tuple, offset+1))) { \
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("'%s' is not a string.\n", PyString_AsString(PyObject_Repr(PyTuple_GetItem(tuple, offset+1))))); \
            return NULL; \
        } else { \
            m = Message_##func(PyString_AsString(PyTuple_GetItem(tuple, offset+1))); \
            Message_send(sd, msgid, m); \
            Message_free(m); \
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
        PyErr_SetString(PyExc_TypeError, "send takes a tuple as the second argument");
        return NULL;
    }
    if(PyTuple_Check(PyTuple_GetItem(tuple, 0))) {
        msgidobj = PyTuple_GetItem(tuple, 0);
        if(PyTuple_Size(msgidobj) != 3) {
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("The MsgID should be a three tuple. %s.", PyObject_Repr(msgidobj)));
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
    if(action == NULL) { return NULL; }

    if(0) {
    PYTHON_TO_MSG_NONE("DONE", done)
    PYTHON_TO_MSG_NONE("DONT_KNOW", dont_know)
    PYTHON_TO_MSG_STRING("RESULT_STRING", result_string)
    } else if(strcmp(action, "RESULT_INT") == 0) {
        if(PyTuple_Size(tuple) == (offset+1) && PyInt_Check(PyTuple_GetItem(tuple, offset+1))) {
            m = Message_result_int(PyInt_AsLong(PyTuple_GetItem(tuple, offset+1)));
            Message_send(sd, msgid, m);
            Message_free(m);
        } else {
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of argument.\n", action));
            return NULL;
        }
    } else if(strcmp(action, "RESULT_TUPLE") == 0) {
        if(PyTuple_Size(tuple) == (offset+2) && PyTuple_Check(PyTuple_GetItem(tuple, offset+1))) {
            Tuple t = PyO2Tuple(PyTuple_GetItem(tuple, offset+1));
            m = Message_result_tuple(t);
            Message_send(sd, msgid, m);
            Message_free(m);
            Tuple_free(t);
        } else {
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of argument.\n", action));
            return NULL;
        }
    PYTHON_TO_MSG_NONE("CREATE_TUPLESPACE", createTuplespace)
    PYTHON_TO_MSG_STRING("ADD_REFERENCE", addReference)
    PYTHON_TO_MSG_STRING("DELETE_REFERENCE", deleteReference)
    PYTHON_TO_MSG_NONE("MONITOR", monitor)
    PYTHON_TO_MSG_NONE("LIST_TS", list_ts)
    PYTHON_TO_MSG_STRING("INSPECT", inspect)
    PYTHON_TO_MSG_STRING("MY_NAME_IS", my_name_is)
    PYTHON_TO_MSG_NONE("GET_NODE_ID", get_node_id)
    } else if(strcmp(action, "REGISTER_PARTITION") == 0) {
        if(PyTuple_Size(tuple) == (offset+3) && PyString_Check(PyTuple_GetItem(tuple, offset+1)) && PyString_Check(PyTuple_GetItem(tuple, offset+2))) {
            m = Message_register_partition(PyString_AsString(PyTuple_GetItem(tuple, offset+1)), PyString_AsString(PyTuple_GetItem(tuple, offset+2)));
            Message_send(sd, msgid, m);
            Message_free(m);
        } else {
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of argument.\n", action));
            return NULL;
        }
    PYTHON_TO_MSG_STRING("GET_PARTITIONS", get_partitions)
    } else if(strcmp(action, "DELETED_PARTITION") == 0) {
        if(PyTuple_Size(tuple) == (offset+3) && PyString_Check(PyTuple_GetItem(tuple, offset+1)) && PyString_Check(PyTuple_GetItem(tuple, offset+2))) {
            m = Message_deleted_partition(PyString_AsString(PyTuple_GetItem(tuple, offset+1)), PyString_AsString(PyTuple_GetItem(tuple, offset+2)));
            Message_send(sd, msgid, m);
            Message_free(m);
        } else {
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of argument.\n", action));
            return NULL;
        }
    PYTHON_TO_MSG_STRING("GET_REQUESTS", get_requests)
    PYTHON_TO_MSG_NONE("GET_NEIGHBOURS", get_neighbours)
    PYTHON_TO_MSG_STRING("GET_CONNECTION_DETAILS", get_connection_details)
    } else if(strcmp(action, "TUPLE_REQUEST") == 0) {
        if(PyTuple_Size(tuple) == (offset+3) && PyString_Check(PyTuple_GetItem(tuple, offset+1)) && PyTuple_Check(PyTuple_GetItem(tuple, offset+2))) {
            Tuple t = PyO2Tuple(PyTuple_GetItem(tuple, offset+2));
            m = Message_tuple_request(PyString_AsString(PyTuple_GetItem(tuple, offset+1)), t);
            Message_send(sd, msgid, m);
            Message_free(m);
            Tuple_free(t);
        } else {
            PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("%s has wrong number of argument.\n", action));
            return NULL;
        }
    } else {
        PyErr_SetObject(PyExc_SystemError, PyString_FromFormat("%s is an unknown message. Not sent.\n", action));
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
