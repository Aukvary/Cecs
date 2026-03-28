#ifndef EDITOR_API_H
#define EDITOR_API_H

#include "scheduler/FileHandle.h"

DT_EXPORT
void dt_editor_log(const char* format, ...);

DT_EXPORT
void dt_editor_warning_log(const char* format, ...);

DT_EXPORT
void dt_editor_error_log(const char* format, ...);

DT_EXPORT
void dt_editor_add_inspector_type(const char* type,
                                  void (*handle)(const char* name, void* data));

void dt_editor_inspector_field_draw(const char* type, const char* name, void* data);

#endif /*EDITOR_API_H*/
