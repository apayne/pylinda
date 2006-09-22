#ifndef __LINDA_CPP_H__

#define FORCE_C
#include "linda.h"
#undef FORCE_C

#include <string>
#include <vector>

namespace Linda {

class Tuple;
class TupleSpace;

bool connect();
void disconnect();

class Value {
    friend class Tuple;
    friend class TupleSpace;
public:
    Value(bool b);
    Value(const int i);
    Value(float f);
    Value(std::string s);
    Value(char* s);
    Value(char* s, int len);
    Value(const Value& v);

    ~Value();

    template<typename T> T get();
protected:
    void* value;
};


class Tuple {
    friend class TupleSpace;
public:
    Tuple(int size=0);
    Tuple(const Tuple& t);
    Tuple(struct Tuple_t* t);
    ~Tuple();

    void add(Value& v);
    void set(int i, const Value& v);
    Value get(int i);
protected:
    void* values;
};

class TupleSpace {
public:
    TupleSpace();
    TupleSpace(std::string tsid);
    ~TupleSpace();

    void out(Tuple& t);
    Tuple in(Tuple& t);
    Tuple rd(Tuple& t);
    Tuple inp(Tuple& t);
    Tuple rdp(Tuple& t);

    int collect(TupleSpace& ts, Tuple& t);
    int copy_collect(TupleSpace& ts, Tuple& t);

private:
    std::string tsid;
};

extern TupleSpace uts;

}

#endif
