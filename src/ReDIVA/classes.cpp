/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "classes.hpp"

extern bool input_locked;

void classes_process_init(classes_data* classes, const size_t classes_count, render_context* rctx) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        classes_data* c = &classes[i];
        if (c->flags & (CLASSES_INIT_AT_STARTUP | CLASSES_SHOW_AT_STARTUP) && c->init) {
            if (c->init(&c->data, rctx))
                c->data.flags = CLASS_INIT;
            else
                c->data.flags = (class_flags)(CLASS_DISPOSED | CLASS_HIDDEN);
            c->data.imgui_focus = false;

            if (!(c->flags & CLASSES_SHOW_AT_STARTUP))
                enum_or(c->data.flags, CLASS_HIDDEN);
        }

        if (c->flags & CLASSES_SHOW_AT_STARTUP) {
            if (c->data.flags & CLASS_INIT)
                enum_and(c->data.flags, ~(CLASS_HIDE | CLASS_HIDDEN));
        }

        if (c->sub_classes && c->sub_classes_count)
            classes_process_init(c->sub_classes, c->sub_classes_count, rctx);
    }
}

void classes_process_ctrl(classes_data* classes, const size_t classes_count) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        classes_data* c = &classes[i];
        if (c->data.flags & CLASS_HIDE) {
            if (!(c->data.flags & CLASS_HIDDEN)) {
                enum_and(c->data.flags, ~CLASS_HIDE);
                enum_or(c->data.flags, CLASS_HIDDEN);
                enum_and(c->data.flags, ~CLASS_HAS_PARENT);
            }
        }

        if ((c->data.flags & CLASS_DISPOSE || (c->data.flags & CLASS_HIDDEN
            && c->flags & CLASSES_DISPOSE_AT_HIDE))) {
            if (!(c->data.flags & CLASS_HIDDEN)) {
                enum_and(c->data.flags, ~CLASS_HIDE);
                enum_or(c->data.flags, CLASS_HIDDEN);
                enum_and(c->data.flags, ~CLASS_HAS_PARENT);
            }

            if (!(c->data.flags & CLASS_DISPOSED) && ((c->dispose && c->dispose(&c->data)) || !c->dispose)) {
                enum_and(c->data.flags, ~CLASS_DISPOSE);
                enum_or(c->data.flags, CLASS_DISPOSED);
                c->data.imgui_focus = false;
            }
        }

        classes_process_ctrl(c->sub_classes, c->sub_classes_count);
    }
}

void classes_process_imgui(classes_data* classes, const size_t classes_count) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        classes_data* c = &classes[i];
        if (!(c->data.flags & CLASS_HIDDEN) && c->imgui) {
            c->imgui(&c->data);
            input_locked |= c->data.imgui_focus;
        }

        classes_process_imgui(c->sub_classes, c->sub_classes_count);
    }
}
void classes_process_dispose(classes_data* classes, const size_t classes_count) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        classes_data* c = &classes[i];
        if (!(c->data.flags & CLASS_HIDDEN)) {
            enum_and(c->data.flags, ~CLASS_HIDE);
            enum_or(c->data.flags, CLASS_HIDDEN);
            enum_and(c->data.flags, ~CLASS_HAS_PARENT);
        }

        if (!(c->data.flags & CLASS_DISPOSED) && ((c->dispose && c->dispose(&c->data)) || !c->dispose)) {
            enum_and(c->data.flags, ~CLASS_DISPOSE);
            enum_or(c->data.flags, CLASS_DISPOSED);
            c->data.imgui_focus = false;
        }

        classes_process_dispose(c->sub_classes, c->sub_classes_count);
    }
}
