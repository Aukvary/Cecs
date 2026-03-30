#include "EditorApi.h"
#include "scheduler/RuntimeScheduler.h"

DtEFuncTable func_table = (DtEFuncTable) {
    .environment_instance = dt_environment_instance,

    .log = dte_log,
    .warn = dte_warning_log,
    .error = dte_error_log,

    .add_inspector_type = dte_add_inspector_type,
    .add_parser_json_to_type = dt_add_parser_json_to_type,
    .add_serializer_type_to_json = dt_add_serializer_type_to_json,
    .link_parser_json_to_type = dt_link_parser_json_to_type
};
