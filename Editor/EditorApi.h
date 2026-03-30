#ifndef EDITOR_API_H
#define EDITOR_API_H

#include "scheduler/FileHandle.h"

typedef enum {
    DTE_NORMAL,
    DTE_WARNING,
    DTE_ERROR
} DtEMessageType;

typedef struct {
    char text[256];
    DtEMessageType type;
} DtEMessage;

typedef struct {
    void(*log)(const char*, ...);
    void(*warn)(const char*, ...);
    void(*error)(const char*, ...);
} DtELogFuncTable;

DT_EXPORT
void dte_log(const char* format, ...);

DT_EXPORT
void dte_warning_log(const char* format, ...);

DT_EXPORT
void dte_error_log(const char* format, ...);

DT_EXPORT
void dte_add_inspector_type(const char* type,
                                  void (*handle)(const char* name, void* data));

void dte_inspector_field_draw(const char* type, const char* name, void* data);

#endif /*EDITOR_API_H*/
