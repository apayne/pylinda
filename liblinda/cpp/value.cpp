#include "linda.h"

#define FORCE_C
#include "linda.h"
#include "../src/linda_internal.h"

namespace Linda {

Value::Value(int i) {
    this->value = malloc(sizeof(struct Value_t));
    *((struct Value_t*)this->value) = Value_int(i);
}

Value::Value(const Value& v) {
    this->value = Value_copyptr((struct Value_t*)v.value);
}

Value::~Value() {
    Value_free((struct Value_t*)this->value);
}

}

