#include <raylib.h>
#include <string.h>
#include "RuntimeScheduler.h"

static DtRbTree parsers_json_to_type;
static DtRbTree serializers_type_to_json;

static void parse_json_to_int(cJSON* src, void* dst);
static void parse_json_to_unsigned_int(cJSON* src, void* dst);
static void parse_json_to_short(cJSON* src, void* dst);
static void parse_json_to_unsigned_short(cJSON* src, void* dst);
static void parse_json_to_long(cJSON* src, void* dst);
static void parse_json_unsigned_long(cJSON* src, void* dst);
static void parse_json_to_long_long(cJSON* src, void* dst);
static void parse_json_to_unsigned_long_long(cJSON* src, void* dst);
static void parse_json_to_float(cJSON* src, void* dst);
static void parse_json_to_double(cJSON* src, void* dst);
static void parse_json_to_char_ptr(cJSON* src, void* dst);
static void parse_json_to_vector2(cJSON* src, void* dst);

static cJSON* serialize_int_to_json(const void* src);
static cJSON* serialize_unsigned_int_to_json(const void* src);
static cJSON* serialize_short_to_json(const void* src);
static cJSON* serialize_unsigned_short_to_json(const void* src);
static cJSON* serialize_long_to_json(const void* src);
static cJSON* serialize_unsigned_long_to_json(const void* src);
static cJSON* serialize_long_long_to_json(const void* src);
static cJSON* serialize_unsigned_long_long_to_json(const void* src);
static cJSON* serialize_float_to_json(const void* src);
static cJSON* serialize_double_to_json(const void* src);
static cJSON* serialize_char_ptr_to_json(const void* src);
static cJSON* serialize_vector2_to_json(const void* src);

static u64 get_hash(const char* name) {
    int hash = 2147483647;
    while (*name) {
        hash ^= *name++;
        hash *= 314159;
    }
    return hash;
}

__attribute__((constructor)) static void dt_parser_initialize(void) {
    parsers_json_to_type = dt_rb_tree_new();
    dt_add_parser_json_to_type("int", parse_json_to_int);
    dt_add_parser_json_to_type("unsigned int", parse_json_to_unsigned_int);
    dt_add_parser_json_to_type("short", parse_json_to_short);
    dt_add_parser_json_to_type("unsigned short", parse_json_to_unsigned_short);
    dt_add_parser_json_to_type("long", parse_json_to_long);
    dt_add_parser_json_to_type("unsigned long", parse_json_unsigned_long);
    dt_add_parser_json_to_type("long long", parse_json_to_long_long);
    dt_add_parser_json_to_type("unsigned long long", parse_json_to_unsigned_long_long);
    dt_add_parser_json_to_type("float", parse_json_to_float);
    dt_add_parser_json_to_type("double", parse_json_to_double);
    dt_add_parser_json_to_type("char*", parse_json_to_char_ptr);
    dt_add_parser_json_to_type("Vector2", parse_json_to_vector2);

    serializers_type_to_json = dt_rb_tree_new();

    dt_add_serializer_type_to_json("int", serialize_int_to_json);
    dt_add_serializer_type_to_json("unsigned int", serialize_unsigned_int_to_json);
    dt_add_serializer_type_to_json("short", serialize_short_to_json);
    dt_add_serializer_type_to_json("unsigned short", serialize_unsigned_short_to_json);
    dt_add_serializer_type_to_json("long", serialize_long_to_json);
    dt_add_serializer_type_to_json("unsigned long", serialize_unsigned_long_to_json);
    dt_add_serializer_type_to_json("long long", serialize_long_long_to_json);
    dt_add_serializer_type_to_json("unsigned long long", serialize_unsigned_long_long_to_json);
    dt_add_serializer_type_to_json("float", serialize_float_to_json);
    dt_add_serializer_type_to_json("double", serialize_double_to_json);
    dt_add_serializer_type_to_json("char*", serialize_char_ptr_to_json);
    dt_add_serializer_type_to_json("Vector2", serialize_vector2_to_json);
}

void dt_add_parser_json_to_type(const char* type, const TypeParser parser) {
    const u64 hash = get_hash(type);
    dt_rb_tree_add(&parsers_json_to_type, parser, hash);
}

void dt_link_parser_json_to_type(const char* type, const char* base_type) {
    TypeParser parser;
    if ((parser = dt_rb_tree_get(&parsers_json_to_type, get_hash(base_type)))) {
        dt_rb_tree_add(&parsers_json_to_type, parser, get_hash(type));
    }
}

void dt_parse_json_to_type(const char* type, cJSON* src, void* dst) {
    const TypeParser parser = dt_rb_tree_get(&parsers_json_to_type, get_hash(type));
    if (parser) {
        parser(src, dst);
        return;
    }

    FOREACH(ModuleInfo*, module, &dt_environment_instance()->modules.iterator, {
        TypeParser parser = *(TypeParser*) DT_LIB_GET(module->handle, "dt_parse_json_to_type");
        if (parser) {
            parser(src, dst);
        }
    });
}

static void parse_json_to_int(cJSON* src, void* dst) {
    if (cJSON_IsNumber(src)) {
        int num = (int) cJSON_GetNumberValue(src);
        memcpy(dst, &num, sizeof(int));
    }
}

static void parse_json_to_unsigned_int(cJSON* src, void* dst) {
    if (cJSON_IsNumber(src)) {
        unsigned int num = (unsigned int) cJSON_GetNumberValue(src);
        memcpy(dst, &num, sizeof(unsigned int));
    }
}

static void parse_json_to_short(cJSON* src, void* dst) {
    if (cJSON_IsNumber(src)) {
        short num = (short) cJSON_GetNumberValue(src);
        memcpy(dst, &num, sizeof(short));
    }
}

static void parse_json_to_unsigned_short(cJSON* src, void* dst) {
    if (cJSON_IsNumber(src)) {
        unsigned short num = (unsigned short) cJSON_GetNumberValue(src);
        memcpy(dst, &num, sizeof(unsigned short));
    }
}

static void parse_json_to_long(cJSON* src, void* dst) {
    if (cJSON_IsNumber(src)) {
        long num = (long) cJSON_GetNumberValue(src);
        memcpy(dst, &num, sizeof(long));
    }
}

static void parse_json_unsigned_long(cJSON* src, void* dst) {
    if (cJSON_IsNumber(src)) {
        unsigned long num = (unsigned long) cJSON_GetNumberValue(src);
        memcpy(dst, &num, sizeof(unsigned long));
    }
}

static void parse_json_to_long_long(cJSON* src, void* dst) {
    if (cJSON_IsNumber(src)) {
        long long num = (long long) cJSON_GetNumberValue(src);
        memcpy(dst, &num, sizeof(long long));
    }
}

static void parse_json_to_unsigned_long_long(cJSON* src, void* dst) {
    if (cJSON_IsNumber(src)) {
        unsigned long long num = (unsigned long long) cJSON_GetNumberValue(src);
        memcpy(dst, &num, sizeof(unsigned long long));
    }
}

static void parse_json_to_float(cJSON* src, void* dst) {
    if (cJSON_IsNumber(src)) {
        float num = (float) cJSON_GetNumberValue(src);
        memcpy(dst, &num, sizeof(float));
    }
}

static void parse_json_to_double(cJSON* src, void* dst) {
    if (cJSON_IsNumber(src)) {
        double num = cJSON_GetNumberValue(src);
        memcpy(dst, &num, sizeof(double));
    }
}

static void parse_json_to_char_ptr(cJSON* src, void* dst) {
    if (cJSON_IsString(src)) {
        const char* str = cJSON_GetStringValue(src);
        char* str_copy = strdup(str);
        *(char**) dst = str_copy;
    }
}

static void parse_json_to_vector2(cJSON* src, void* dst) {
    if (cJSON_IsArray(src)) {
        cJSON* x = cJSON_GetArrayItem(src, 0);
        if (!cJSON_IsNumber(x))
            return;

        cJSON* y = cJSON_GetArrayItem(src, 1);
        if (!cJSON_IsNumber(y))
            return;

        ((Vector2*) dst)->x = (float) cJSON_GetNumberValue(x);
        ((Vector2*) dst)->y = (float) cJSON_GetNumberValue(y);
    }
}

void dt_add_serializer_type_to_json(const char* type, const TypeSerializer serializer) {
    dt_rb_tree_add(&serializers_type_to_json, serializer, get_hash(type));
}

cJSON* dt_serialize_type_to_json(const char* type, const void* src) {
    const TypeSerializer serializer = dt_rb_tree_get(&serializers_type_to_json, get_hash(type));
    if (serializer) {
        return serializer(src);
    }

    FOREACH(ModuleInfo*, module, &dt_environment_instance()->modules.iterator, {
        TypeSerializer serializer =
            *(TypeSerializer*) DT_LIB_GET(module->handle, "dt_serialize_type_to_json");

        if (serializer) {
            return serializer(src);
        }
    });

    return NULL;
}

static cJSON* serialize_int_to_json(const void* src) {
    return cJSON_CreateNumber((double) (*(int*) src));
}

static cJSON* serialize_unsigned_int_to_json(const void* src) {
    return cJSON_CreateNumber((double) (*(unsigned int*) src));
}

static cJSON* serialize_short_to_json(const void* src) {
    return cJSON_CreateNumber((double) (*(short*) src));
}

static cJSON* serialize_unsigned_short_to_json(const void* src) {
    return cJSON_CreateNumber((double) (*(unsigned short*) src));
}

static cJSON* serialize_long_to_json(const void* src) {
    return cJSON_CreateNumber((double) (*(long*) src));
}

static cJSON* serialize_unsigned_long_to_json(const void* src) {
    return cJSON_CreateNumber((double) (*(unsigned long*) src));
}

static cJSON* serialize_long_long_to_json(const void* src) {
    return cJSON_CreateNumber((double) (*(long long*) src));
}

static cJSON* serialize_unsigned_long_long_to_json(const void* src) {
    return cJSON_CreateNumber((double) (*(unsigned long long*) src));
}

static cJSON* serialize_float_to_json(const void* src) {
    return cJSON_CreateNumber((double) (*(float*) src));
}

static cJSON* serialize_double_to_json(const void* src) {
    return cJSON_CreateNumber(*(double*) src);
}

static cJSON* serialize_char_ptr_to_json(const void* src) {
    const char** str = (const char**) src;
    return cJSON_CreateString(*str ? *str : "");
}

static cJSON* serialize_vector2_to_json(const void* src) {
    const Vector2* v = (const Vector2*) src;
    cJSON* arr = cJSON_CreateArray();
    cJSON_AddItemToArray(arr, cJSON_CreateNumber(v->x));
    cJSON_AddItemToArray(arr, cJSON_CreateNumber(v->y));
    return arr;
}
