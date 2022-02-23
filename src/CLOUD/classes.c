/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "classes.h"

extern bool input_locked;

void classes_process_init(classes_data* classes, const size_t classes_count, render_context* rctx) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        classes_data* c = &classes[i];
        lock_init(&c->data.lock);
        if (lock_check_init(&c->data.lock)
            && c->flags & (CLASSES_INIT_AT_STARTUP | CLASSES_SHOW_AT_STARTUP) && c->init) {
            lock_lock(&c->data.lock);
            if (c->init(&c->data, rctx))
                c->data.flags = CLASS_INIT;
            else
                c->data.flags = (class_flags)(CLASS_DISPOSED | CLASS_HIDDEN);
            c->data.imgui_focus = false;

            if (~c->flags & CLASSES_SHOW_AT_STARTUP)
                enum_or(c->data.flags, CLASS_HIDDEN);
            lock_unlock(&c->data.lock);
        }

        if (lock_check_init(&c->data.lock)
            && c->flags & CLASSES_SHOW_AT_STARTUP) {
            lock_lock(&c->data.lock);
            if (c->data.flags & CLASS_INIT && ((c->show && c->show(&c->data)) || !c->show))
                enum_and(c->data.flags, ~(CLASS_HIDE | CLASS_HIDDEN));
            lock_unlock(&c->data.lock);
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
        if (lock_check_init(&c->data.lock)) {
            if (c->data.flags & CLASS_HIDE) {
                lock_lock(&c->data.lock);
                if (~c->data.flags & CLASS_HIDDEN && ((c->hide && c->hide(&c->data)) || !c->hide)) {
                    enum_and(c->data.flags, ~CLASS_HIDE);
                    enum_or(c->data.flags, CLASS_HIDDEN);
                }
                lock_unlock(&c->data.lock);
            }

            if ((c->data.flags & CLASS_DISPOSE || (c->data.flags & CLASS_HIDDEN
                && c->flags & CLASSES_DISPOSE_AT_HIDE))) {
                lock_lock(&c->data.lock);
                if (~c->data.flags & CLASS_HIDDEN && ((c->hide && c->hide(&c->data)) || !c->hide)) {
                    enum_and(c->data.flags, ~CLASS_HIDE);
                    enum_or(c->data.flags, CLASS_HIDDEN);
                }

                if (~c->data.flags & CLASS_DISPOSED && ((c->dispose && c->dispose(&c->data)) || !c->dispose)) {
                    enum_and(c->data.flags, ~CLASS_DISPOSE);
                    enum_or(c->data.flags, CLASS_DISPOSED);
                    c->data.imgui_focus = false;
                }
                lock_unlock(&c->data.lock);
                lock_free(&c->data.lock);
            }

            if (c->data.flags & CLASS_INIT && c->ctrl) {
                lock_lock(&c->data.lock);
                c->ctrl(&c->data);
                lock_unlock(&c->data.lock);
            }
        }
        
        classes_process_ctrl(c->sub_classes, c->sub_classes_count);
    }
}

void classes_process_draw(classes_data* classes, size_t classes_count) {
    for (size_t i = 0; i < classes_count; i++) {
        classes_data* c = &classes[i];
        if (lock_check_init(&c->data.lock) && c->disp) {
            lock_lock(&c->data.lock);
            if (c->data.flags & CLASS_INIT)
                c->disp(&c->data);
            lock_unlock(&c->data.lock);
        }

        if (c->sub_classes && c->sub_classes_count)
            classes_process_draw(c->sub_classes, c->sub_classes_count);
    }
}

void classes_process_drop(classes_data* classes, size_t classes_count, size_t count, char** paths) {
    if (count < 1 || !paths[0])
        return;

    for (size_t i = 0; i < classes_count; i++) {
        classes_data* c = &classes[i];
        if (lock_check_init(&c->data.lock) && c->drop) {
            lock_lock(&c->data.lock);
            if (c->data.flags & CLASS_INIT)
                c->drop(&c->data, count, paths);
            lock_unlock(&c->data.lock);
        }

        if (c->sub_classes && c->sub_classes_count)
            classes_process_drop(c->sub_classes, c->sub_classes_count, count, paths);
    }
}

void classes_process_imgui(classes_data* classes, const size_t classes_count) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        classes_data* c = &classes[i];
        if (lock_check_init(&c->data.lock)) {
            lock_lock(&c->data.lock);
            if (~c->data.flags & CLASS_HIDDEN) {
                input_locked |= c->data.imgui_focus;
                if (c->imgui)
                    c->imgui(&c->data);
            }
            lock_unlock(&c->data.lock);
        }
        
        classes_process_imgui(c->sub_classes, c->sub_classes_count);
    }
}

void classes_process_input(classes_data* classes, const size_t classes_count) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        classes_data* c = &classes[i];
        if (lock_check_init(&c->data.lock)) {
            lock_lock(&c->data.lock);
            if (c->data.flags & CLASS_INIT) {
                if (c->input) {
                    c->input(&c->data);
                }
                input_locked |= c->data.imgui_focus;
            }
            lock_unlock(&c->data.lock);
        }
        
        classes_process_input(c->sub_classes, c->sub_classes_count);
    }
}

void classes_process_sound(classes_data* classes, const size_t classes_count) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        classes_data* c = &classes[i];
        if (lock_check_init(&c->data.lock) && c->sound) {
            lock_lock(&c->data.lock);
            if (c->data.flags & CLASS_INIT)
                c->sound(&c->data);
            lock_unlock(&c->data.lock);
        }
        
        classes_process_sound(c->sub_classes, c->sub_classes_count);
    }
}

void classes_process_dispose(classes_data* classes, const size_t classes_count) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        classes_data* c = &classes[i];
        if (lock_check_init(&c->data.lock)) {
            lock_lock(&c->data.lock);
            if (~c->data.flags & CLASS_HIDDEN && ((c->hide && c->hide(&c->data)) || !c->hide)) {
                enum_and(c->data.flags, ~CLASS_HIDE);
                enum_or(c->data.flags, CLASS_HIDDEN);
            }

            if (~c->data.flags & CLASS_DISPOSED && ((c->dispose && c->dispose(&c->data)) || !c->dispose)) {
                enum_and(c->data.flags, ~CLASS_DISPOSE);
                enum_or(c->data.flags, CLASS_DISPOSED);
                c->data.imgui_focus = false;
            }
            lock_unlock(&c->data.lock);
        }
        lock_free(&c->data.lock);

        classes_process_dispose(c->sub_classes, c->sub_classes_count);
    }
}
