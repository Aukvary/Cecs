#include <limits.h>
#include <raylib.h>
#include <string.h>
#include "Collections/Collections.h"
#include "EditorApi.h"
#include "UI.h"

static DtRbTree field_handlers;

static void inspector_field_int(const char* name, void* dst);
static void inspector_field_unsigned_int(const char* field, void* dst);
static void inspector_field_short(const char* field, void* dst);
static void inspector_field_unsigned_short(const char* field, void* dst);
static void inspector_field_long(const char* field, void* dst);
static void inspector_field_unsigned_long(const char* field, void* dst);
static void inspector_field_long_long(const char* field, void* dst);
static void inspector_field_unsigned_long_long(const char* field, void* dst);
static void inspector_field_float(const char* field, void* dst);
static void inspector_field_double(const char* field, void* dst);
static void inspector_field_char_ptr(const char* field, void* dst);
static void inspector_field_vector2(const char* field, void* dst);

static u64 get_hash(const char* name) {
    int hash = 2147483647;
    while (*name) {
        hash ^= *name++;
        hash *= 314159;
    }
    return hash;
}

__attribute__((constructor)) void initialize_field_handlers() {
    field_handlers = dt_rb_tree_new();

    dt_editor_add_inspector_type("int", inspector_field_int);
    dt_editor_add_inspector_type("unsigned int", inspector_field_unsigned_int);
    dt_editor_add_inspector_type("short", inspector_field_short);
    dt_editor_add_inspector_type("unsigned short", inspector_field_unsigned_short);
    dt_editor_add_inspector_type("long", inspector_field_long);
    dt_editor_add_inspector_type("unsigned long", inspector_field_unsigned_long);
    dt_editor_add_inspector_type("long long", inspector_field_long_long);
    dt_editor_add_inspector_type("unsigned long long", inspector_field_unsigned_long_long);
    dt_editor_add_inspector_type("float", inspector_field_float);
    dt_editor_add_inspector_type("double", inspector_field_double);
    dt_editor_add_inspector_type("char*", inspector_field_char_ptr);
    dt_editor_add_inspector_type("Vector2", inspector_field_vector2);
}

void dt_editor_add_inspector_type(const char* type,
                                  void (*handle)(const char* name, void* data)) {
    dt_rb_tree_add(&field_handlers, handle, get_hash(type));
}

void dt_editor_inspector_field_draw(const char* type, const char* name, void* data) {
    void (*handle)(const char*, void*) = dt_rb_tree_get(&field_handlers, get_hash(type));
    if (handle)
        handle(name, data);
}

static void inspector_field_int(const char* name, void* dst) {
    int* val = dst;

    nk_layout_row_dynamic(ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", name);
    nk_label(ctx, label, NK_TEXT_LEFT);

    nk_property_int(ctx, "", INT_MIN, val, INT_MAX, 1, 1.0f);
}

static void inspector_field_unsigned_int(const char* field, void* dst) {
    unsigned int* val = dst;
    int temp = (int) *val;

    nk_layout_row_dynamic(ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(ctx, label, NK_TEXT_LEFT);

    nk_property_int(ctx, "", 0, &temp, INT_MAX, 1, 1.0f);
    *val = (unsigned int) temp;
}

static void inspector_field_short(const char* field, void* dst) {
    short* val = dst;
    int temp = *val;

    nk_layout_row_dynamic(ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(ctx, label, NK_TEXT_LEFT);

    nk_property_int(ctx, "", SHRT_MIN, &temp, SHRT_MAX, 1, 1.0f);
    *val = (short) temp;
}

static void inspector_field_unsigned_short(const char* field, void* dst) {
    unsigned short* val = dst;
    int temp = *val;

    nk_layout_row_dynamic(ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(ctx, label, NK_TEXT_LEFT);

    nk_property_int(ctx, "", 0, &temp, USHRT_MAX, 1, 1.0f);
    *val = (unsigned short) temp;
}

static void inspector_field_long(const char* field, void* dst) {
    long* val = dst;
    double temp = (double) *val;

    nk_layout_row_dynamic(ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(ctx, label, NK_TEXT_LEFT);

    nk_property_double(ctx, "", (double) LONG_MIN, &temp, (double) LONG_MAX, 1.0, 1.0f);
    *val = (long) temp;
}

static void inspector_field_unsigned_long(const char* field, void* dst) {
    unsigned long* val = dst;
    double temp = (double) *val;

    nk_layout_row_dynamic(ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(ctx, label, NK_TEXT_LEFT);

    nk_property_double(ctx, "", 0.0, &temp, (double) ULONG_MAX, 1.0, 1.0f);
    *val = (unsigned long) temp;
}

static void inspector_field_long_long(const char* field, void* dst) {
    long long* val = dst;
    double temp = (double) *val;

    nk_layout_row_dynamic(ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(ctx, label, NK_TEXT_LEFT);

    nk_property_double(ctx, "", -1e18, &temp, 1e18, 1.0, 1.0f);
    *val = (long long) temp;
}

static void inspector_field_unsigned_long_long(const char* field, void* dst) {
    long* val = dst;
    double temp = (double) *val;

    nk_layout_row_dynamic(ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(ctx, label, NK_TEXT_LEFT);

    nk_property_double(ctx, "", 0, &temp, (double) ULLONG_MAX, 1.0, 1.0f);
    *val = (long) temp;
}

static void inspector_field_float(const char* field, void* dst) {
    float* val = dst;
    double temp = *val;

    nk_layout_row_dynamic(ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(ctx, label, NK_TEXT_LEFT);

    nk_layout_row_dynamic(ctx, 20, 1);
    nk_property_double(ctx, "", -1e18, &temp, 1e18, 1.0, 1.0f);
    *val = (float) temp;
}

static void inspector_field_double(const char* field, void* dst) {
    double* val = dst;
    double temp = *val;

    nk_layout_row_dynamic(ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(ctx, label, NK_TEXT_LEFT);

    nk_layout_row_dynamic(ctx, 20, 1);
    nk_property_double(ctx, "", -1e18, &temp, 1e18, 1, 1.0f);
    *val = temp;
}

static void inspector_field_char_ptr(const char* field, void* dst) {
    char* str = dst;
    int len = (int) strlen(str);

    nk_layout_row_dynamic(ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(ctx, label, NK_TEXT_LEFT);

    nk_edit_string(ctx, NK_EDIT_FIELD, str, &len, 255, nk_filter_default);
    str[len] = '\0';
}

static void inspector_field_vector2(const char* field, void* dst) {
    Vector2* v = dst;

    nk_layout_row_dynamic(ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(ctx, label, NK_TEXT_LEFT);

    nk_layout_row_dynamic(ctx, 25, 2);
    nk_property_float(ctx, "X:", -10000.0f, &v->x, 10000.0f, 0.1f, 0.5f);
    nk_property_float(ctx, "Y:", -10000.0f, &v->y, 10000.0f, 0.1f, 0.5f);
}
