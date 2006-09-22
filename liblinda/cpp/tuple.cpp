#include "linda.h"

#define FORCE_C
#include "linda.h"

namespace Linda {

Tuple::Tuple(int size) {
    this->values = Tuple_new(size);
}

Tuple::Tuple(const Tuple& t) {
    this->values = Tuple_copy((struct Tuple_t*)t.values);
}

Tuple::Tuple(struct Tuple_t* t) {
    this->values = Tuple_copy(t);
}

Tuple::~Tuple() {
    Tuple_free((struct Tuple_t*)this->values);
}

void Tuple::set(int i, const Value& v) {
    Tuple_set((struct Tuple_t*)this->values, i, *((struct Value_t*)v.value));
}

}

