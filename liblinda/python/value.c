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

#include <stdio.h>

#include PYTHON_H

#include "linda.h"
#include "linda_python.h"

static PyObject* linda_Value_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    linda_ValueObject* self;

    self = (linda_ValueObject*)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->val = NULL;
    }

    return (PyObject*)self;
}

static int linda_Value_init(linda_ValueObject* self, PyObject* args, PyObject* kwds) {
    int i;
    PyObject* obj = NULL;
#ifdef TYPES
    PyObject* type = NULL;

    if(!PyArg_ParseTuple(args, "|OO", &obj, &type))
        return -1;
#else
    if(!PyArg_ParseTuple(args, "|O", &obj))
        return -1;
#endif

    if(PyObject_IsInstance(obj, (PyObject*)&linda_ValueType)) {
        self->val = Linda_copy(((linda_ValueObject*)obj)->val);
    } else if(obj == Py_None) {
        self->val = Linda_nil();
    } else if(PyInt_Check(obj)) {
        self->val = Linda_long(PyInt_AsLong(obj));
    } else if(PyFloat_Check(obj)) {
        self->val = Linda_float(PyFloat_AsDouble(obj));
    } else if(PyString_Check(obj)) {
        self->val = Linda_string(PyString_AsString(obj));
    } else if(PyTuple_Check(obj)) {
        self->val = Linda_tuple(PyTuple_Size(obj));
        for(i = 0; i < PyTuple_Size(obj); i++) {
            PyObject* o = PyTuple_GetItem(obj, i);
            LindaValue nv = PyO2Value(o);
            if(nv == NULL) {
                Linda_delReference(self->val);
                PyErr_SetString(PyExc_TypeError, "PyO2Value: Failed to convert value when converting tuple.\n");
                return -1;
            }
            Linda_tupleSet(self->val, i, nv); /* Steals reference to nv. */
        }
    } else if(PyType_Check(obj)) {
        if(obj == (PyObject*)&PyInt_Type) {
            self->val = Linda_copy(Linda_longType);
        } else if(obj == (PyObject*)&PyFloat_Type) {
            self->val = Linda_copy(Linda_floatType);
        } else if(obj == (PyObject*)&PyString_Type) {
            self->val = Linda_copy(Linda_stringType);
        } else if(obj == (PyObject*)&PyBool_Type) {
            self->val = Linda_copy(Linda_boolType);
        } else if(obj == (PyObject*)&linda_TupleSpaceType) {
            self->val = Linda_copy(Linda_tupleSpaceType);
        } else if(obj == (PyObject*)&linda_ValueType) {
            self->val = Linda_copy(((linda_ValueObject*)obj)->val);
        } else {
            PyErr_SetString(PyExc_TypeError, "PyO2Value: Invalid type of type.\n");
            return -1;
        }
    } else if(Linda_is_server && PyObject_IsInstance(obj, (PyObject*)&linda_TSRefType)) {
        self->val = Linda_copy(((linda_TSRefObject*)obj)->ts);
    } else if(!Linda_is_server && PyObject_TypeCheck(obj, &linda_TupleSpaceType)) {
        self->val = Linda_copy(((linda_TupleSpaceObject*)obj)->ts);
    } else {
        PyErr_SetString(PyExc_TypeError, "PyO2Value: Invalid type.\n");
        return -1;
    }

#ifdef TYPES
    if(type != NULL && PyObject_IsInstance(type, (PyObject*)&linda_ValueType)) {
        Linda_setType(self->val, ((linda_ValueObject*)type)->val);
    } else if(type != NULL) {
        PyErr_SetString(PyExc_TypeError, "PyO2Value: Invalid type object.\n");
        return -1;
    }
#endif

    return 0;
}

static void linda_Value_dealloc(linda_ValueObject* self) {
    if(self->val != NULL) {
        Linda_delReference(self->val);
    }
    self->ob_type->tp_free(self);
}

static int linda_Value_cmp(linda_ValueObject* self, linda_ValueObject* other) {
    if(PyErr_Occurred()) {
        fprintf(stderr, "Warning, exception set when entering linda_Value_cmp. Clearing.\n");
        PyErr_Print();
    }
    if(self->val->type != other->val->type) {
        if(self->val->type < other->val->type) {
            return -1;
        } else {
            return 1;
        }
    } else if(self->val->type == NIL) {
        return 0;
    } else if(self->val->type == BOOLEAN) {
        if(self->val->boolean == 0 && other->val->boolean == 0) {
            return 0;
        } else if(self->val->boolean == 0 && other->val->boolean == 1) {
            return -1;
        } else if(self->val->boolean == 1 && other->val->boolean == 0) {
            return 1;
        } else {
            return 0;
        }
    } else if(self->val->type == INTEGER || self->val->type == LONG) {
        if(self->val->integer < other->val->integer) {
            return -1;
        } else if(self->val->integer > other->val->integer) {
            return 1;
        } else {
            return 0;
        }
    } else if(self->val->type == STRING) {
        return strcmp(self->val->string, other->val->string);
    } else if(self->val->type == TUPLE) {
        if(self->val->size < other->val->size) {
            return -1;
        } else if(self->val->size > other->val->size) {
            return 1;
        } else {
            int i;
            for(i=0; i<self->val->size; i++) {
                int c;
                PyObject* c1 = Value2PyO(self->val->values[i]);
                PyObject* c2 = Value2PyO(other->val->values[i]);
                c = linda_Value_cmp((linda_ValueObject*)c1, (linda_ValueObject*)c2);
                Py_DECREF(c1); Py_DECREF(c2);
                if(c != 0) {
                    return c;
                }
            }
            return 0;
        }
    } else if(self->val->type == FUNCTION) {
        return strcmp(self->val->func_name, other->val->func_name);
    } else if(self->val->type == TYPE) {
        if(strcmp(self->val->type_name, other->val->type_name)) {
            return strcmp(self->val->type_name, other->val->type_name);
        } else {
            return Minimal_SyntaxTree_cmp(self->val->type_spec, other->val->type_spec);
        }
    } else {
        PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("PyLibLinda: Unknown type (%i) in comparison.\n", self->val->type));
        return -1;
    }
}

static PyObject* linda_Value_str(linda_ValueObject* self) {
    char* s = Minimal_Value_string(self->val);
    PyObject* o = PyString_FromFormat("%s", s);
    free(s);
    return o;
}

static PyObject* linda_Value_repr(linda_ValueObject* self) {
    char* s = Minimal_Value_string(self->val);
    PyObject* o = PyString_FromFormat("%s", s);
    free(s);
    return o;
}

static PyObject* linda_Value_setSumPos(linda_ValueObject* self, PyObject* args) {
    int i;

    if(!PyArg_ParseTuple(args, "i", &i)) {
        return NULL;
    }

    Linda_setSumPos(self->val, i);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* linda_Value_isType(linda_ValueObject* self) {
    if(Linda_isType(self->val)) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static PyObject* linda_Value_isLong(linda_ValueObject* self) {
    if(Linda_isLong(self->val)) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static PyObject* linda_Value_isString(linda_ValueObject* self) {
    if(Linda_isString(self->val)) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

/* These are all type related functions */

static PyObject* linda_Value_isNil(linda_ValueObject* self) {
    if(!Linda_isType(self->val)) {
         PyErr_SetString(PyExc_TypeError, "Value is not a type.\n");
        return NULL;
    }
    if(self->val->type_spec == NULL) {
        Py_INCREF(Py_False);
        return Py_False;
    } else if(self->val->type_spec->type == ST_NIL) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static PyObject* linda_Value_isId(linda_ValueObject* self) {
    if(!Linda_isType(self->val)) {
        PyErr_SetString(PyExc_TypeError, "Value is not a type.\n");
        return NULL;
    }
    if(self->val->type_spec == NULL) {
        Py_INCREF(Py_False);
        return Py_False;
    } else if(self->val->type_spec->type == ST_IDENTIFIER) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static PyObject* linda_Value_isProductType(linda_ValueObject* self) {
    if(!Linda_isType(self->val)) {
        PyErr_SetString(PyExc_TypeError, "Value is not a type.\n");
        return NULL;
    }
    if(self->val->type_spec == NULL) {
        Py_INCREF(Py_False);
        return Py_False;
    } else if(self->val->type_spec->type == ST_PRODUCT_TYPE) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static PyObject* linda_Value_isSumType(linda_ValueObject* self) {
    if(!Linda_isType(self->val)) {
        PyErr_SetString(PyExc_TypeError, "Value is not a type.\n");
        return NULL;
    }
    if(self->val->type_spec == NULL) {
        Py_INCREF(Py_False);
        return Py_False;
    } else if(self->val->type_spec->type == ST_SUM_TYPE) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static PyObject* linda_Value_isPtrType(linda_ValueObject* self) {
    if(!Linda_isType(self->val)) {
        PyErr_SetString(PyExc_TypeError, "Value is not a type.\n");
        return NULL;
    }
    if(self->val->type_spec == NULL) {
        Py_INCREF(Py_False);
        return Py_False;
    } else if(self->val->type_spec->type == ST_POINTER) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static PyObject* linda_Value_isFunctionType(linda_ValueObject* self) {
    if(!Linda_isType(self->val)) {
        PyErr_SetString(PyExc_TypeError, "Value is not a type.\n");
        return NULL;
    }
    if(self->val->type_spec == NULL) {
        Py_INCREF(Py_False);
        return Py_False;
    } else if(self->val->type_spec->type == ST_TYPE_FUNCTION) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static PyMethodDef value_methods[] = {
    {"setSumPos", (PyCFunction)linda_Value_setSumPos, METH_VARARGS, ""},
    {"isType", (PyCFunction)linda_Value_isType, METH_NOARGS, ""},
    {"isNil", (PyCFunction)linda_Value_isNil, METH_NOARGS, ""},
    {"isLong", (PyCFunction)linda_Value_isLong, METH_NOARGS, ""},
    {"isId", (PyCFunction)linda_Value_isId, METH_NOARGS, ""},
    {"isProductType", (PyCFunction)linda_Value_isProductType, METH_NOARGS, ""},
    {"isSumType", (PyCFunction)linda_Value_isSumType, METH_NOARGS, ""},
    {"isPtrType", (PyCFunction)linda_Value_isPtrType, METH_NOARGS, ""},
    {"isFunctionType", (PyCFunction)linda_Value_isFunctionType, METH_NOARGS, ""},
    {"isString", (PyCFunction)linda_Value_isString, METH_NOARGS, ""},
    {NULL}  /* Sentinel */
};

static PyObject* linda_Value_gettype(linda_ValueObject *self, void *closure) {
    LindaValue v = Linda_getType(self->val);
    if(v == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    } else {
        return Value2PyO(v);
    }
}

static PyObject* linda_Value_getid(linda_ValueObject *self, void *closure) {
    if(!Linda_isType(self->val)) { PyErr_SetString(PyExc_TypeError, "getID - Not a type."); return NULL; }
    if(self->val->type_spec->type != ST_IDENTIFIER) { PyErr_SetString(PyExc_TypeError, "getID - Not an identifier."); return NULL; }
    PyObject* string = PyString_FromString(self->val->type_spec->string);
    if(string == NULL) {
        PyErr_SetString(PyExc_TypeError, "Failed to get id string.");
        return NULL;
    } else {
        return string;
    }
}

static PyObject* linda_Value_getint(linda_ValueObject *self, void *closure) {
    if(!Linda_isLong(self->val)) { PyErr_SetString(PyExc_TypeError, "getInt - Not an integer."); return NULL; }
    return PyInt_FromLong(self->val->integer);
}

static PyObject* linda_Value_getstring(linda_ValueObject *self, void *closure) {
    if(!Linda_isString(self->val)) { PyErr_SetString(PyExc_TypeError, "getString - Not a string."); return NULL; }
    return PyString_FromString(Linda_getString(self->val));
}

static PyObject* linda_Value_getarg(linda_ValueObject *self, void *closure) {
    if(!Linda_isType(self->val)) { PyErr_SetString(PyExc_TypeError, "getArg - Not a type."); return NULL; }
    if(self->val->type_spec->type != ST_TYPE_FUNCTION) { PyErr_SetString(PyExc_TypeError, "getArg - Not a function type."); return NULL; }
    LindaValue val = Minimal_typeSpec(self->val->type_name, Minimal_SyntaxTree_copy(self->val->type_spec->branches[0]));
    Linda_addReference((void*)(self->val->typemap));
    val->typemap = self->val->typemap;
    return Value2PyO(val);
}

static PyObject* linda_Value_getresult(linda_ValueObject *self, void *closure) {
    if(!Linda_isType(self->val)) { PyErr_SetString(PyExc_TypeError, "getResult - Not a type."); return NULL; }
    if(self->val->type_spec->type != ST_TYPE_FUNCTION) { PyErr_SetString(PyExc_TypeError, "getResult - Not a function type."); return NULL; }
    LindaValue val = Minimal_typeSpec(self->val->type_name, Minimal_SyntaxTree_copy(self->val->type_spec->branches[1]));
    Linda_addReference((void*)(self->val->typemap));
    val->typemap = self->val->typemap;
    return Value2PyO(val);
}

#ifdef TYPES
static PyObject* linda_Value_gettypemap(linda_ValueObject *self, void *closure) {
    if(!Linda_isType(self->val)) { PyErr_SetString(PyExc_TypeError, "getTypeMap - Not a type."); return NULL; }

    linda_TypeMapObject* map = (linda_TypeMapObject*)PyObject_CallFunction((PyObject*)&linda_TypeMapType, "", 0);

    if(Linda_isType(self->val)) {
        Linda_addReference((void*)(self->val->typemap));
        map->map = self->val->typemap;
    } else {
        Linda_addReference((void*)(self->val->typeobj->typemap));
        map->map = self->val->typeobj->typemap;
    }

    return (PyObject*)map;
}

static PyObject* linda_Value_gettypeid(linda_ValueObject* self, void* closure) {
    if(!Linda_isType(self->val)) {
        PyErr_SetString(PyExc_TypeError, "getTypeID - Not a type.");
        return NULL;
    }

    return PyInt_FromLong(self->val->type_id);
}

static int linda_Value_settypeid(linda_ValueObject* self, PyObject* value, void* closure) {
    if(!Linda_isType(self->val)) { PyErr_SetString(PyExc_TypeError, "setTypeID - Not a type."); return -1; }

    if(!PyInt_Check(value)) { PyErr_SetString(PyExc_TypeError, "setTypeID - Setting to not an integer."); return -1; }

    self->val->type_id = PyInt_AsLong(value);
    return 0;
}

static PyObject* linda_Value_getsumpos(linda_ValueObject* self, void* closure) {
    return PyInt_FromLong(Linda_getSumPos(self->val));
}

static int linda_Value_setsumpos(linda_ValueObject* self, PyObject* value, void* closure) {
    if(!PyInt_Check(value)) { PyErr_SetString(PyExc_TypeError, "setSumPos - Setting to not an integer."); return -1; }

    Linda_setSumPos(self->val, PyInt_AsLong(value));
    return 0;
}
#endif

static PyGetSetDef value_getseters[] = {
    {"id", (getter)linda_Value_getid, (setter)NULL, "", NULL},
    {"int", (getter)linda_Value_getint, (setter)NULL, "", NULL},
    {"type", (getter)linda_Value_gettype, (setter)NULL, "", NULL},
    {"string", (getter)linda_Value_getstring, (setter)NULL, "", NULL},
    {"arg", (getter)linda_Value_getarg, (setter)NULL, "", NULL},
    {"result", (getter)linda_Value_getresult, (setter)NULL, "", NULL},
#ifdef TYPES
    {"typemap", (getter)linda_Value_gettypemap, (setter)NULL, "", NULL},
    {"type_id", (getter)linda_Value_gettypeid, (setter)linda_Value_settypeid, "", NULL},
    {"sum_pos", (getter)linda_Value_getsumpos, (setter)linda_Value_setsumpos, "", NULL},
#endif
    {NULL}  /* Sentinel */
};

static Py_ssize_t linda_Value_len(linda_ValueObject *self) {
    if(Linda_isType(self->val)) {
        return self->val->type_spec->length;
    } else if(Linda_isString(self->val)) {
        return strlen(self->val->string);
    } else if(Linda_isTuple(self->val)) {
        return Linda_getTupleSize(self->val);
    } else {
        PyErr_SetString(PyExc_TypeError, "Taking length of a type without a length.");
        return -1;
    }
}

static PyObject* linda_Value_item(linda_ValueObject *self, Py_ssize_t index) {
    if(Linda_isType(self->val)) {
        Minimal_SyntaxTree* tree = Minimal_SyntaxTree_copy(self->val->type_spec->branches[index]);
        LindaValue val = Minimal_typeSpec(self->val->type_name, tree);
        Minimal_SyntaxTree_free(tree);
        Linda_addReference((void*)(self->val->typemap));
        val->typemap = self->val->typemap;
        return Value2PyO(val);
    } else if(Linda_isTuple(self->val)) {
        if(index < 0 || index >= linda_Value_len(self)) {
            PyErr_SetString(PyExc_IndexError, "Index out of range for tuple.");
            return NULL;
        } else {
            return Value2PyO(Linda_tupleGet(self->val, index));
        } 
    } else if(Linda_isString(self->val)) {
        if(index < 0 || index >= strlen(self->val->string)) {
            PyErr_SetString(PyExc_IndexError, "Index out of range for string.");
            return NULL;
        } else {
            return Py_BuildValue("c", self->val->string[index]);
        }
    } else {
        PyErr_SetString(PyExc_TypeError, "Getting item from a type with is a single element.");
        return NULL;
    }
}

PySequenceMethods linda_ValueSeq = {
        (lenfunc)linda_Value_len,  /* lenfunc sq_length; */
        0,                /*binaryfunc sq_concat; */
        0,                /*ssizeargfunc sq_repeat; */
        (ssizeargfunc)linda_Value_item, /*ssizeargfunc sq_item; */
        0,                /*ssizessizeargfunc sq_slice; */
        0,                /*ssizeobjargproc sq_ass_item; */
        0,                /*ssizessizeobjargproc sq_ass_slice; */
        0,                /*objobjproc sq_contains; */
        0,                /*binaryfunc sq_inplace_concat; */
        0,                /*ssizeargfunc sq_inplace_repeat; */
};

static PyObject* linda_Value_add(linda_ValueObject* self, linda_ValueObject* other) {
    if(Linda_isLong(self->val) && Linda_isLong(other->val)) {
        return PyInt_FromLong(Linda_getLong(self->val) + Linda_getLong(other->val));
    } else {
        PyErr_SetString(PyExc_TypeError, "Addition to value by invalid type.");
        return NULL;
    }
}

static PyObject* linda_Value_sub(linda_ValueObject* self, linda_ValueObject* other) {
    if(Linda_isLong(self->val) && Linda_isLong(other->val)) {
        return PyInt_FromLong(Linda_getLong(self->val) - Linda_getLong(other->val));
    } else {
        PyErr_SetString(PyExc_TypeError, "Subtraction from value by invalid type.");
        return NULL;
    }
}

static int linda_Value_nonzero(linda_ValueObject* self) {
    if(self->val->type == NIL) {
        return 0;
    } else if(self->val->type == BOOLEAN) {
        if(self->val->boolean == 0) {
            return 0;
        } else {
            return 1;
        }
    } else if(self->val->type == INTEGER || self->val->type == LONG) {
        if(self->val->integer == 0) {
            return 0;
        } else {
            return 1;
        }
    } else if(self->val->type == STRING) {
        if(strlen(self->val->string) == 0) {
            return 0;
        } else {
            return 1;
        }
    } else if(self->val->type == TUPLE) {
        if(self->val->size == 0) {
            return 0;
        } else {
            return 1;
        }
    } else if(self->val->type == FUNCTION) {
        return 1;
    } else if(self->val->type == TYPE) {
        return 1;
    } else {
        PyErr_SetObject(PyExc_TypeError, PyString_FromFormat("PyLibLinda: Unknown type (%i) in nb_nonzero.\n", self->val->type));
        return -1;
    }
}

static PyObject* linda_Value_nbint(linda_ValueObject* self) {
    if(Linda_isLong(self->val)) {
        return PyInt_FromLong(Linda_getLong(self->val));
    } else {
        PyErr_SetString(PyExc_TypeError, "Intification of value of an invalid type.");
        return NULL;
    }
}

static int linda_Value_coerce(PyObject** self, PyObject** other) {
    if(PyInt_Check(*self)) {
        *other = Value2PyO(Linda_long(PyInt_AsLong(*other)));
        Py_INCREF(*self);
        return 0;
    } else if(PyInt_Check(*other)) {
        *other = Value2PyO(Linda_long(PyInt_AsLong(*other)));
        Py_INCREF(*self);
        return 0;
    } else {
        return 1;
    }
}

PyNumberMethods linda_ValueNum = {
        (binaryfunc)linda_Value_add,  /* binaryfunc nb_add; */
        (binaryfunc)linda_Value_sub,  /* binaryfunc nb_subtract; */
        0,  /* binaryfunc nb_multiply; */
        0,  /* binaryfunc nb_divide; */
        0,  /* binaryfunc nb_remainder; */
        0,  /* binaryfunc nb_divmod; */
        0,  /* ternaryfunc nb_power; */
        0,  /* unaryfunc nb_negative; */
        0,  /* unaryfunc nb_positive; */
        0,  /* unaryfunc nb_absolute; */
        (inquiry)linda_Value_nonzero,  /* inquiry nb_nonzero; */
        0,  /* unaryfunc nb_invert; */
        0,  /* binaryfunc nb_lshift; */
        0,  /* binaryfunc nb_rshift; */
        0,  /* binaryfunc nb_and; */
        0,  /* binaryfunc nb_xor; */
        0,  /* binaryfunc nb_or; */
        (coercion)linda_Value_coerce,  /* coercion nb_coerce; */
        (unaryfunc)linda_Value_nbint,  /* unaryfunc nb_int; */
        0,  /* unaryfunc nb_long; */
        0,  /* unaryfunc nb_float; */
        0,  /* unaryfunc nb_oct; */
        0,  /* unaryfunc nb_hex; */
        /* Added in release 2.0 */
        0,  /* binaryfunc nb_inplace_add; */
        0,  /* binaryfunc nb_inplace_subtract; */
        0,  /* binaryfunc nb_inplace_multiply; */
        0,  /* binaryfunc nb_inplace_divide; */
        0,  /* binaryfunc nb_inplace_remainder; */
        0,  /* ternaryfunc nb_inplace_power; */
        0,  /* binaryfunc nb_inplace_lshift; */
        0,  /* binaryfunc nb_inplace_rshift; */
        0,  /* binaryfunc nb_inplace_and; */
        0,  /* binaryfunc nb_inplace_xor; */
        0,  /* binaryfunc nb_inplace_or; */
        /* Added in release 2.2 */
        /* The following require the Py_TPFLAGS_HAVE_CLASS flag */
        0,  /* binaryfunc nb_floor_divide; */
        0,  /* binaryfunc nb_true_divide; */
        0,  /* binaryfunc nb_inplace_floor_divide; */
        0,  /* binaryfunc nb_inplace_true_divide; */
        /* Added in release 2.5 */
        0,  /* unaryfunc nb_index; */
};

PyTypeObject linda_ValueType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "linda.Value",        /*tp_name*/
    sizeof(linda_ValueObject), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)linda_Value_dealloc,  /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    (cmpfunc)linda_Value_cmp,  /*tp_compare*/
    (reprfunc)linda_Value_repr,/*tp_repr*/
    &linda_ValueNum,           /*tp_as_number*/
    &linda_ValueSeq,           /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    (reprfunc)linda_Value_str, /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "A Linda Value",           /* tp_doc */
    0,                         /* tp_traverse; */
    0,                         /* tp_clear; */
    0,                         /* tp_richcompare; */
    0,                         /* tp_weaklistoffset; */
    0,                         /* tp_iter; */
    0,                         /* tp_iternext; */
    value_methods,             /* tp_methods; */
    0,                         /* tp_members */
    value_getseters,           /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)linda_Value_init, /* tp_init */
    0,                         /* tp_alloc */
    linda_Value_new,      /* tp_new */
};

LindaValue PyO2Value(PyObject* obj) {
    if(PyObject_IsInstance(obj, (PyObject*)&linda_ValueType)) {
        Linda_addReference(((linda_ValueObject*)obj)->val);
        return ((linda_ValueObject*)obj)->val;
    } else {
        PyObject* o = PyObject_CallFunctionObjArgs((PyObject*)&linda_ValueType, obj, NULL);
        if(o == NULL) {
            PyErr_SetString(PyExc_SystemError, "Failed to create Value from Python Object."); 
            return NULL;
        } else {
            Linda_addReference(((linda_ValueObject*)o)->val);
            LindaValue v = ((linda_ValueObject*)o)->val;
            Py_DECREF(o);
            return v;
        }
    }
}

PyObject* Value2PyO(LindaValue obj) {
    PyObject* o = PyObject_CallFunction((PyObject*)&linda_ValueType, "i", 0);
    if(o == NULL) {
        PyErr_SetString(PyExc_SystemError, "Failed to create Python Object from Value."); 
        return NULL;
    }
    Linda_delReference(((linda_ValueObject*)o)->val);
    Linda_addReference(obj);
    ((linda_ValueObject*)o)->val = obj;
    return o;
}

PyObject* Tuple2PyO(LindaValue t) {
    int i;
    PyObject* pyt = PyTuple_New(Linda_getTupleSize(t));

    for(i=0; i<Linda_getTupleSize(t); i++) {
        PyObject* v = Value2PyO(Linda_tupleGet(t, i));
        if(v == NULL) {
            Py_DECREF(pyt);
            return NULL;
        }
        PyTuple_SetItem(pyt, i, v);
    }

    return pyt;
}

LindaValue PyO2Tuple(PyObject* tup) {
    int i;
    LindaValue t = Linda_tuple(PyTuple_Size(tup));
    for(i=0; i<PyTuple_Size(tup); i++) {
        LindaValue v;
        v = PyO2Value(PyTuple_GetItem(tup, i));
        if(v == NULL) { Linda_delReference(t); return NULL; }
        Linda_tupleSet(t, i, v);
    }
    return t;
}

void initvalue(PyObject* m) {
    linda_ValueType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&linda_ValueType) < 0)
        return;

    Py_INCREF(&linda_ValueType);
    PyModule_AddObject(m, "Value", (PyObject*)&linda_ValueType);
}
