#include "DtAllocators.h"
#include "Ecs/DtEcs.h"
#include "EditorApi.h"
#include "UI.h"

#define MAX_MESSAGES 1000

static DtEMessage messages[MAX_MESSAGES];
static int message_write_ptr = 0;
static int message_count = 0;

static DrawSystem* message_panel_new();
static void message_panel_draw(void* _);

DT_REGISTER_DRAW(MessagePanel, message_panel_new);

static DrawSystem* message_panel_new() {
    DrawSystem* draw = DT_MALLOC(sizeof(DrawSystem));

    *draw = (DrawSystem) {
        .init = NULL,
        .draw = message_panel_draw,
        .destroy = NULL,
    };

    return draw;
}

static void message_panel_draw(void* _) {
    float width = (float) GetScreenWidth();
    float height = (float) GetScreenHeight();

    if (nk_begin(nk_ctx, "MSG Panel",
                 nk_rect(0, height - height / 4 + height / 30, width - width / 5,
                         height / 4 - height / 30),
                 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE)) {
        nk_layout_row_dynamic(nk_ctx, 20, 1);
        for (int i = 0; i < message_count; i++) {
            int real_idx =
                (message_count < MAX_MESSAGES) ? i : (message_write_ptr + i) % MAX_MESSAGES;

            DtEMessage* msg = &messages[real_idx];
            nk_label(nk_ctx, msg->text, NK_TEXT_ALIGN_LEFT);
        }
    }
    nk_end(nk_ctx);
}

static void dt_editor_add_message(DtEMessageType type, const char* format, va_list args) {
    messages[message_write_ptr].type = type;
    vsnprintf(messages[message_write_ptr].text, sizeof(messages[message_write_ptr].text), format,
              args);

    message_write_ptr = (message_write_ptr + 1) % MAX_MESSAGES;

    if (message_count < MAX_MESSAGES) {
        message_count++;
    }
}

void dte_log(const char* format, ...) {
    va_list args;
    va_start(args, format);
    dt_editor_add_message(DTE_NORMAL, format, args);
    va_end(args);
}

void dte_warning_log(const char* format, ...) {
    va_list args;
    va_start(args, format);
    dt_editor_add_message(DTE_WARNING, format, args);
    va_end(args);
}

void dte_error_log(const char* format, ...) {
    va_list args;
    va_start(args, format);
    dt_editor_add_message(DTE_ERROR, format, args);
    va_end(args);
}
