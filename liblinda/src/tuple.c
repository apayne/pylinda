#include "linda.h"

Tuple Tuple_new(size) {
    int i;

    Tuple t = (Tuple)malloc(sizeof(struct Tuple_t));
    t->size = size;
    if(size == 0) {
        t->values = NULL;
    } else {
        t->values = (Value*)malloc(sizeof(Value) * size);
        for(i=0; i<size; i++) {
            t->values[i] = Value_nil();
        }
    }
    return t;
}

void Tuple_delete(Tuple t) {
    Tuple_clear(t);
    free(t->values);
    free(t);
}

void Tuple_add(Tuple t, Value val) {
    Tuple_set(t, t->size, val);
}

void Tuple_set(Tuple t, int pos, Value val) {
    if(pos >= t->size) {
        Value* tmp = t->values;
        t->values = (Value*)malloc(sizeof(Value) * (pos + 1));
        memcpy(t->values, tmp, sizeof(Value) * t->size);
        free(tmp);
        t->size = pos + 1;
    }
    t->values[pos] = val;
}

Value* Tuple_get(Tuple t, int pos) {
    return &(t->values[pos]);
}

int Tuple_get_size(Tuple t) {
    return t->size;
}

Tuple Tuple_copy(Tuple t) {
    int i;

    if(t == NULL) { return NULL; }
    Tuple nt = (Tuple)malloc(sizeof(struct Tuple_t));
    nt->size = t->size;
    if(t->size == 0) {
        nt->values = NULL;
    } else {
        nt->values = (Value*)malloc(sizeof(Value) * t->size);
        for(i=0; i<t->size; i++) {
            Tuple_set(nt, i, Value_copy(&(t->values[i])));
        }
    }
    return nt;
}

void Tuple_clear(Tuple t) {
    if(t == NULL) { return; }
    int i;
    for(i = 0; i < t->size; i++) {
        Value_clear(&(t->values[i]));
    }
    free(t->values);
    t->values = NULL;
}

void Tuple_free(Tuple t) {
    Tuple_clear(t);
    free(t);
}
