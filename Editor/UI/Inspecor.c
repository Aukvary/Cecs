#include <limits.h>
#include <nuklear.h>
#include <raylib.h>
#include <string.h>
#include "Collections/Collections.h"
#include "DtAllocators.h"
#include "EditorApi.h"
#include "UI.h"

static DtRbTree field_handlers;

static bool inspector_field_int(const char* name, void* dst);
static bool inspector_field_unsigned_int(const char* field, void* dst);
static bool inspector_field_short(const char* field, void* dst);
static bool inspector_field_unsigned_short(const char* field, void* dst);
static bool inspector_field_long(const char* field, void* dst);
static bool inspector_field_unsigned_long(const char* field, void* dst);
static bool inspector_field_long_long(const char* field, void* dst);
static bool inspector_field_unsigned_long_long(const char* field, void* dst);
static bool inspector_field_float(const char* field, void* dst);
static bool inspector_field_double(const char* field, void* dst);
static bool inspector_field_char_ptr(const char* field, void* dst);
static bool inspector_field_vector2(const char* field, void* dst);
static bool inspector_field_rectangle(const char* field, void* dst);
static bool inspector_field_bool(const char* field, void* dst);
static bool inspector_field_color(const char* field, void* dst);

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

    dte_add_inspector_type("int", inspector_field_int);
    dte_add_inspector_type("unsigned int", inspector_field_unsigned_int);
    dte_add_inspector_type("short", inspector_field_short);
    dte_add_inspector_type("unsigned short", inspector_field_unsigned_short);
    dte_add_inspector_type("long", inspector_field_long);
    dte_add_inspector_type("unsigned long", inspector_field_unsigned_long);
    dte_add_inspector_type("long long", inspector_field_long_long);
    dte_add_inspector_type("unsigned long long", inspector_field_unsigned_long_long);
    dte_add_inspector_type("float", inspector_field_float);
    dte_add_inspector_type("double", inspector_field_double);
    dte_add_inspector_type("char*", inspector_field_char_ptr);
    dte_add_inspector_type("Vector2", inspector_field_vector2);
    dte_add_inspector_type("Rectangle", inspector_field_rectangle);
    dte_add_inspector_type("bool", inspector_field_bool);
    dte_add_inspector_type("Color", inspector_field_color);
}

void dte_add_inspector_type(const char* type, bool (*handle)(const char* name, void* data)) {
    dt_rb_tree_add(&field_handlers, handle, get_hash(type));
}

bool dte_inspector_field_draw(const char* type, const char* name, void* data) {
    bool (*handle)(const char*, void*) = dt_rb_tree_get(&field_handlers, get_hash(type));
    if (handle)
        return handle(name, data);

    return false;
}

static bool inspector_field_int(const char* name, void* dst) {
    int* val = dst;
    nk_layout_row_dynamic(nk_ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", name);
    nk_label(nk_ctx, label, NK_TEXT_LEFT);

    int old = *val;
    nk_property_int(nk_ctx, "#", INT_MIN, val, INT_MAX, 1, 1.0f);
    return *val != old;
}

static bool inspector_field_unsigned_int(const char* field, void* dst) {
    unsigned int* val = dst;
    int temp = (int) *val;
    nk_layout_row_dynamic(nk_ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(nk_ctx, label, NK_TEXT_LEFT);

    nk_property_int(nk_ctx, "#", 0, &temp, INT_MAX, 1, 1.0f);
    bool changed = (unsigned int) temp != *val;
    *val = (unsigned int) temp;
    return changed;
}

static bool inspector_field_short(const char* field, void* dst) {
    short* val = dst;
    int temp = *val;
    nk_layout_row_dynamic(nk_ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(nk_ctx, label, NK_TEXT_LEFT);

    nk_property_int(nk_ctx, "#", SHRT_MIN, &temp, SHRT_MAX, 1, 1.0f);
    bool changed = (short) temp != *val;
    *val = (short) temp;
    return changed;
}

static bool inspector_field_unsigned_short(const char* field, void* dst) {
    unsigned short* val = dst;
    int temp = *val;
    nk_layout_row_dynamic(nk_ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(nk_ctx, label, NK_TEXT_LEFT);

    nk_property_int(nk_ctx, "#", 0, &temp, USHRT_MAX, 1, 1.0f);
    bool changed = (unsigned short) temp != *val;
    *val = (unsigned short) temp;
    return changed;
}

static bool inspector_field_long(const char* field, void* dst) {
    long* val = dst;
    double temp = (double) *val;
    nk_layout_row_dynamic(nk_ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(nk_ctx, label, NK_TEXT_LEFT);

    nk_property_double(nk_ctx, "#", (double) LONG_MIN, &temp, (double) LONG_MAX, 1.0, 1.0f);
    bool changed = (long) temp != *val;
    *val = (long) temp;
    return changed;
}

static bool inspector_field_unsigned_long(const char* field, void* dst) {
    unsigned long* val = dst;
    double temp = (double) *val;
    nk_layout_row_dynamic(nk_ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(nk_ctx, label, NK_TEXT_LEFT);

    nk_property_double(nk_ctx, "#", 0.0, &temp, (double) ULONG_MAX, 1.0, 1.0f);
    bool changed = (unsigned long) temp != *val;
    *val = (unsigned long) temp;
    return changed;
}

static bool inspector_field_long_long(const char* field, void* dst) {
    long long* val = dst;
    double temp = (double) *val;
    nk_layout_row_dynamic(nk_ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(nk_ctx, label, NK_TEXT_LEFT);

    nk_property_double(nk_ctx, "#", -1e18, &temp, 1e18, 1.0, 1.0f);
    bool changed = (long long) temp != *val;
    *val = (long long) temp;
    return changed;
}

static bool inspector_field_unsigned_long_long(const char* field, void* dst) {
    unsigned long long* val = dst;
    double temp = (double) *val;
    nk_layout_row_dynamic(nk_ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(nk_ctx, label, NK_TEXT_LEFT);

    nk_property_double(nk_ctx, "#", 0, &temp, (double) ULLONG_MAX, 1.0, 1.0f);
    bool changed = (unsigned long long) temp != *val;
    *val = (unsigned long long) temp;
    return changed;
}

static bool inspector_field_float(const char* field, void* dst) {
    float* val = dst;
    double temp = *val;
    nk_layout_row_dynamic(nk_ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(nk_ctx, label, NK_TEXT_LEFT);

    nk_layout_row_dynamic(nk_ctx, 20, 1);
    nk_property_double(nk_ctx, "#", -1e18, &temp, 1e18, 1.0, 1.0f);
    bool changed = (float) temp != *val;
    *val = (float) temp;
    return changed;
}

static bool inspector_field_double(const char* field, void* dst) {
    double* val = dst;
    double old = *val;
    nk_layout_row_dynamic(nk_ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(nk_ctx, label, NK_TEXT_LEFT);

    nk_layout_row_dynamic(nk_ctx, 20, 1);
    nk_property_double(nk_ctx, "#", -1e18, &old, 1e18, 1, 1.0f);
    return *val != old;
}

static bool inspector_field_char_ptr(const char* field, void* dst) {
    nk_layout_row_dynamic(nk_ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(nk_ctx, label, NK_TEXT_LEFT);

    char** str = dst;
    if (!*str) {
        *str = DT_MALLOC(256);
        (*str)[0] = '\0';
    }

    int len = (int) strlen(*str);

    nk_flags state = nk_edit_string(nk_ctx, NK_EDIT_FIELD | NK_EDIT_SIG_ENTER, *str, &len, 255,
                                    nk_filter_default);
    (*str)[len] = '\0';

    if (state & NK_EDIT_COMMITED) {
        nk_edit_unfocus(nk_ctx);
        return true;
    }

    return false;
}

static bool inspector_field_vector2(const char* field, void* dst) {
    Vector2* v = dst;
    Vector2 old = *v;

    nk_layout_row_dynamic(nk_ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(nk_ctx, label, NK_TEXT_LEFT);

    nk_layout_row_dynamic(nk_ctx, 25, 2);
    nk_property_float(nk_ctx, "#X:", -10000.0f, &v->x, 10000.0f, 0.1f, 0.5f);
    nk_property_float(nk_ctx, "#Y:", -10000.0f, &v->y, 10000.0f, 0.1f, 0.5f);

    return v->x != old.x || v->y != old.y;
}

static bool inspector_field_rectangle(const char* field, void* dst) {
    Rectangle* rect = dst;
    Rectangle old = *rect;

    nk_layout_row_dynamic(nk_ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);
    nk_label(nk_ctx, label, NK_TEXT_LEFT);

    nk_layout_row_dynamic(nk_ctx, 25, 2);
    nk_property_float(nk_ctx, "#X:", -10000.0f, &rect->x, 10000.0f, 0.1f, 0.5f);
    nk_property_float(nk_ctx, "#Y:", -10000.0f, &rect->y, 10000.0f, 0.1f, 0.5f);
    nk_property_float(nk_ctx, "#W:", -10000.0f, &rect->width, 10000.0f, 0.1f, 0.5f);
    nk_property_float(nk_ctx, "#H:", -10000.0f, &rect->height, 10000.0f, 0.1f, 0.5f);

    return rect->x != old.x || rect->y != old.y || rect->width != old.width ||
           rect->height != old.height;
}

static bool inspector_field_bool(const char* field, void* dst) {
    nk_bool* enable = dst;
    bool changed = false;

    nk_layout_row_dynamic(nk_ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);

    if (nk_checkbox_label(nk_ctx, label, enable)) {
        changed = true;
    }

    return changed;
}

static bool inspector_field_color(const char* field, void* dst) {
    nk_layout_row_dynamic(nk_ctx, 20, 1);
    char label[64];
    snprintf(label, sizeof(label), "%s:", field);

    nk_layout_row_dynamic(nk_ctx, 25, 1);

    Color* color = dst;
    struct nk_colorf nk_color = (struct nk_colorf) {
        .r = (float) color->r / 255.0f,
        .g = (float) color->g / 255.0f,
        .b = (float) color->b / 255.0f,
        .a = (float) color->a / 255.0f,
    };
    struct nk_colorf old = nk_color;

    if (nk_combo_begin_color(nk_ctx, nk_rgba_cf(nk_color), nk_vec2(nk_widget_width(nk_ctx), 400))) {
        nk_layout_row_dynamic(nk_ctx, 120, 1);
        nk_color = nk_color_picker(nk_ctx, nk_color, NK_RGBA);
        nk_layout_row_dynamic(nk_ctx, 25, 1);

        nk_color.r = nk_propertyf(nk_ctx, "#R:", 0, nk_color.r, 1.0f, 0.01f, 0.005f);
        nk_color.g = nk_propertyf(nk_ctx, "#G:", 0, nk_color.g, 1.0f, 0.01f, 0.005f);
        nk_color.b = nk_propertyf(nk_ctx, "#B:", 0, nk_color.b, 1.0f, 0.01f, 0.005f);
        nk_color.a = nk_propertyf(nk_ctx, "#A:", 0, nk_color.a, 1.0f, 0.01f, 0.005f);

        color->r = (u8) (nk_color.r * 255);
        color->g = (u8) (nk_color.g * 255);
        color->b = (u8) (nk_color.b * 255);
        color->a = (u8) (nk_color.a * 255);
        nk_combo_end(nk_ctx);
    }

    return nk_color.r != old.r || nk_color.g != old.g || nk_color.b != old.b || nk_color.a != old.a;
}
