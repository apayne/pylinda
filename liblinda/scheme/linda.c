#include "linda.h"
#include <libguile.h>

void LindaSCM_init();

struct SCM_LindaValue {
    LindaValue v;
};

static scm_t_bits SCM_LindaValue_tag;

SCM SCM_LindaValue_mark(SCM value_smob) {
    //struct SCM_LindaValue* value = (struct SCM_LindaValue*) SCM_SMOB_DATA(value_smob);

    return SCM_BOOL_F;
}

size_t SCM_LindaValue_free(SCM value_smob) {
    struct SCM_LindaValue* value = (struct SCM_LindaValue*) SCM_SMOB_DATA(value_smob);

    Linda_delReference(value->v);

    return 0;
}

LindaValue SCM_to_LindaValue(SCM v) {
    LindaValue r = NULL;
    if(scm_is_integer(v)) {
        r = Linda_int(scm_to_int32(v));
    } else if(scm_is_true(scm_list_p(v))) {
        long i = scm_ilength(v);
        r = Linda_tuple(i);
        i--;
        while(i >= 0) {
            Linda_tupleSet(r, i, SCM_to_LindaValue(scm_list_ref(v, scm_from_uint32(i))));

            i--;
        }
    } else {
        scm_assert_smob_type(SCM_LindaValue_tag, v);

        {
        struct SCM_LindaValue* value = (struct SCM_LindaValue*) SCM_SMOB_DATA(v);
        Linda_addReference(value->v);

        r = value->v;
        }
    }
    return r;
}

SCM Wrap_LindaValue(LindaValue v) {
   SCM smob;
   struct SCM_LindaValue* value = (struct SCM_LindaValue*)scm_gc_malloc(sizeof(struct SCM_LindaValue), "LindaValue");

   Linda_addReference(v);
   value->v = v;

   SCM_NEWSMOB(smob, SCM_LindaValue_tag, value);

   return smob;
}

SCM LindaValue_to_SCM(LindaValue v) {
    if(Linda_isInt(v)) {
       return scm_from_int32(Linda_getInt(v));
    } else if(Linda_isTuple(v)) {
       int i;
       SCM list = scm_make_list(scm_from_int32(Linda_getTupleSize(v)), NULL);

       for(i = 0; i < Linda_getTupleSize(v); i++) {
           scm_list_set_x(list, scm_from_int32(i), LindaValue_to_SCM(Linda_tupleGet(v, i)));
       }

       return list;
    } else {
       return Wrap_LindaValue(v);
    }
}

struct SCM_LindaType {
    LindaValue type;

    SCM _from;
    SCM _to;
};

static scm_t_bits SCM_LindaType_tag;

SCM SCM_LindaType_mark(SCM value_smob) {
    struct SCM_LindaType* value = (struct SCM_LindaType*) SCM_SMOB_DATA(value_smob);

    scm_gc_mark(value->_from);
    scm_gc_mark(value->_to);

    return SCM_BOOL_F;
}

size_t SCM_LindaType_free(SCM value_smob) {
    struct SCM_LindaType* value = (struct SCM_LindaType*) SCM_SMOB_DATA(value_smob);

    Linda_delReference(value->type);

    return 0;
}

static SCM _wrap_Linda_connect(SCM s_0) {
    int arg1 ;
    SCM result;

    arg1 = scm_to_int32(s_0);

    Linda_connect(arg1);
    result = SCM_UNSPECIFIED;

    return result;
}

static SCM _wrap_Linda_out(SCM s_0, SCM s_1) {
    LindaValue arg1;
    LindaValue arg2;
    SCM result;

    arg1 = SCM_to_LindaValue(s_0);
    arg2 = SCM_to_LindaValue(s_1);

    Linda_out(arg1, arg2);

    Linda_delReference(arg1);
    Linda_delReference(arg2);

    result = SCM_UNSPECIFIED;

    return result;
}

static SCM _wrap_Linda_in(SCM s_0, SCM s_1) {
    LindaValue arg1;
    LindaValue arg2;
    LindaValue r;
    SCM result;

    arg1 = SCM_to_LindaValue(s_0);
    arg2 = SCM_to_LindaValue(s_1);

    r = Linda_in(arg1, arg2);

    Linda_delReference(arg1);
    Linda_delReference(arg2);

    result = Wrap_LindaValue(r);

    return result;
}

static SCM _wrap_Linda_make_type(SCM s_0, SCM s_1, SCM s_2) {
    char* typespec = scm_to_locale_string(s_0);
    LindaValue type = Linda_type(typespec);

    SCM smob;
    struct SCM_LindaType* value = (struct SCM_LindaType*)scm_gc_malloc(sizeof(struct SCM_LindaType), "LindaType");

    value->type = type;
    value->_from = s_1;
    value->_to = s_2;

    SCM_NEWSMOB(smob, SCM_LindaValue_tag, value);

    free(typespec);

    return func;
}

static SCM _wrap_Linda_call_type(SCM s_0, SCM s_1) {
}

static SCM _wrap_Linda_port(SCM s_0) {
    SCM result;

    if(s_0 != SCM_UNDEFINED) {
       Linda_port = scm_to_int32(s_0);
    }

    result = scm_from_int32(Linda_port);

    return result;
}

static SCM _wrap_Linda_uts(SCM s_0) {
    if (s_0 != SCM_UNDEFINED) {
        fprintf(stderr, "Error, can't set uts.");
    }

    return LindaValue_to_SCM(Linda_uts);
}

static SCM _wrap_Linda_int_type() {
    return LindaValue_to_SCM(Linda_intType);
}

static SCM _wrap_Linda_string_type() {
    return LindaValue_to_SCM(Linda_stringType);
}

static SCM toString(SCM v) {
    return LindaValue_to_SCM(SCM_to_LindaValue(v));
}

static SCM toInt(SCM v) {
    return LindaValue_to_SCM(SCM_to_LindaValue(v));
}

SCM scm_init_linda_module() {
    SCM_LindaValue_tag = scm_make_smob_type("LindaValue", sizeof(struct SCM_LindaValue));
    scm_set_smob_mark(SCM_LindaValue_tag, SCM_LindaValue_mark);
    scm_set_smob_free(SCM_LindaValue_tag, SCM_LindaValue_free);
    /*scm_set_smob_print(SCM_LindaValue_tag, SCM_LindaValue_print);*/
    /*scm_set_smob_equalp(SCM_LindaValue_tag, SCM_LindaValue_equalp);*/

    SCM_LindaType_tag = scm_make_smob_type("LindaType", sizeof(struct SCM_LindaType));
    scm_set_smob_mark(SCM_LindaType_tag, SCM_LindaType_mark);
    scm_set_smob_free(SCM_LindaType_tag, SCM_LindaType_free);

    scm_c_define_gsubr("Linda-connect", 1, 0, 0, _wrap_Linda_connect);
    scm_c_define_gsubr("Linda-out", 2, 0, 0, _wrap_Linda_out);
    scm_c_define_gsubr("Linda-in", 2, 0, 0, _wrap_Linda_in);
    scm_c_define_gsubr("Linda-port", 0, 1, 0, _wrap_Linda_port);
    scm_c_define_gsubr("Linda-uts", 0, 1, 0, _wrap_Linda_uts);

    scm_c_define_gsubr("Linda-int-type", 0, 0, 0, _wrap_Linda_int_type);
    scm_c_define_gsubr("Linda-string-type", 0, 0, 0, _wrap_Linda_string_type);

    scm_c_define_gsubr("Linda-make-type", 3, 0, 0, _wrap_Linda_make_type);
    scm_c_define_gsubr("Linda-call-type", 2, 0, 0, _wrap_Linda_call_type);

    scm_c_define_gsubr("Linda-toString", 1, 0, 0, toString);
    scm_c_define_gsubr("Linda-toInt", 1, 0, 0, toInt);

    return SCM_UNSPECIFIED;
}

