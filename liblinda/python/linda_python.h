#ifndef LINDA_PYTHON

void inittuplespace(PyObject* m);

Value* PyO2Value(PyObject* obj);
Tuple PyO2Tuple(PyObject* tup);
PyObject* Tuple2PyO(Tuple t);

PyObject* LindaPython_send(PyObject *self, PyObject* args);
PyObject* LindaPython_recv(PyObject *self, PyObject* args);

extern int LindaPython_is_server;

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    Linda_tuplespace ts;
} linda_TupleSpaceObject;
PyTypeObject linda_TupleSpaceType;

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    Linda_tuplespace ts;
} linda_TSRefObject;
PyTypeObject linda_TSRefType;

#endif
