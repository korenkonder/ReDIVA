/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "classes.h"

void classes_process_init(classes_struct* classes, const size_t classes_count, render_context* rctx) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        classes_struct* c = &classes[i];
        lock_init(&c->data.lock);
        if (lock_check_init(&c->data.lock)
            && c->flags & (CLASSES_INIT_AT_STARTUP | CLASSES_SHOW_AT_STARTUP) && c->init) {
            lock_lock(&c->data.lock);
            if (c->init(&c->data, rctx))
                c->data.flags = CLASS_INIT;
            else
                c->data.flags = CLASS_DISPOSED | CLASS_HIDDEN;
            c->data.imgui_focus = false;

            if (~c->flags & CLASSES_SHOW_AT_STARTUP)
                c->data.flags |= CLASS_HIDDEN;
            lock_unlock(&c->data.lock);
        }

        if (lock_check_init(&c->data.lock)
            && c->flags & CLASSES_SHOW_AT_STARTUP) {
            lock_lock(&c->data.lock);
            if (c->data.flags & CLASS_INIT && ((c->show && c->show(&c->data)) || !c->show))
                c->data.flags &= ~(CLASS_HIDE | CLASS_HIDDEN);
            lock_unlock(&c->data.lock);
        }

        if (c->sub_classes && c->sub_classes_count)
            classes_process_init(c->sub_classes, c->sub_classes_count, rctx);
    }
}

void classes_process_draw(classes_struct* classes, size_t classes_count) {
    for (size_t i = 0; i < classes_count; i++) {
        classes_struct* c = &classes[i];
        if (lock_check_init(&c->data.lock) && c->draw) {
            lock_lock(&c->data.lock);
            if (c->data.flags & CLASS_INIT)
                c->draw(&c->data);
            lock_unlock(&c->data.lock);
        }

        if (c->sub_classes && c->sub_classes_count)
            classes_process_draw(c->sub_classes, c->sub_classes_count);
    }
}

void classes_process_drop(classes_struct* classes, size_t classes_count, size_t count, char** paths) {
    if (count < 1 || !paths[0])
        return;

    for (size_t i = 0; i < classes_count; i++) {
        classes_struct* c = &classes[i];
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

void classes_process_imgui(classes_struct* classes, const size_t classes_count) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        classes_struct* c = &classes[i];
        if (lock_check_init(&c->data.lock) && c->imgui) {
            lock_lock(&c->data.lock);
            if (~c->data.flags & CLASS_HIDDEN)
                c->imgui(&c->data);
            lock_unlock(&c->data.lock);
        }
        
        classes_process_imgui(c->sub_classes, c->sub_classes_count);
    }
}

void classes_process_input(classes_struct* classes, const size_t classes_count) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        classes_struct* c = &classes[i];
        if (lock_check_init(&c->data.lock) && c->input) {
            lock_lock(&c->data.lock);
            if (c->data.flags & CLASS_INIT)
                c->input(&c->data);
            lock_unlock(&c->data.lock);
        }
        
        classes_process_input(c->sub_classes, c->sub_classes_count);
    }
}

void classes_process_render(classes_struct* classes, const size_t classes_count) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        classes_struct* c = &classes[i];
        if (lock_check_init(&c->data.lock)) {
            if (c->data.flags & CLASS_HIDE) {
                lock_lock(&c->data.lock);
                if (~c->data.flags & CLASS_HIDDEN && ((c->hide && c->hide(&c->data)) || !c->hide)) {
                    c->data.flags &= ~CLASS_HIDE;
                    c->data.flags |= CLASS_HIDDEN;
                }
                lock_unlock(&c->data.lock);
            }

            if ((c->data.flags & CLASS_DISPOSE || (c->data.flags & CLASS_HIDDEN
                && c->flags & CLASSES_DISPOSE_AT_HIDE))) {
                lock_lock(&c->data.lock);
                if (~c->data.flags & CLASS_HIDDEN && ((c->hide && c->hide(&c->data)) || !c->hide)) {
                    c->data.flags &= ~CLASS_HIDE;
                    c->data.flags |= CLASS_HIDDEN;
                }

                if (~c->data.flags & CLASS_DISPOSED && ((c->dispose && c->dispose(&c->data)) || !c->dispose)) {
                    c->data.flags &= ~CLASS_DISPOSE;
                    c->data.flags |= CLASS_DISPOSED;
                    c->data.imgui_focus = false;
                }
                lock_unlock(&c->data.lock);
                lock_free(&c->data.lock);
            }

            if (c->data.flags & CLASS_INIT && c->render) {
                lock_lock(&c->data.lock);
                c->render(&c->data);
                lock_unlock(&c->data.lock);
            }
        }
        
        classes_process_render(c->sub_classes, c->sub_classes_count);
    }
}

void classes_process_sound(classes_struct* classes, const size_t classes_count) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        classes_struct* c = &classes[i];
        if (lock_check_init(&c->data.lock) && c->sound) {
            lock_lock(&c->data.lock);
            if (c->data.flags & CLASS_INIT)
                c->sound(&c->data);
            lock_unlock(&c->data.lock);
        }
        
        classes_process_sound(c->sub_classes, c->sub_classes_count);
    }
}

void classes_process_dispose(classes_struct* classes, const size_t classes_count) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        classes_struct* c = &classes[i];
        if (lock_check_init(&c->data.lock)) {
            lock_lock(&c->data.lock);
            if (~c->data.flags & CLASS_HIDDEN && ((c->hide && c->hide(&c->data)) || !c->hide)) {
                c->data.flags &= ~CLASS_HIDE;
                c->data.flags |= CLASS_HIDDEN;
            }

            if (~c->data.flags & CLASS_DISPOSED && ((c->dispose && c->dispose(&c->data)) || !c->dispose)) {
                c->data.flags &= ~CLASS_DISPOSE;
                c->data.flags |= CLASS_DISPOSED;
                c->data.imgui_focus = false;
            }
            lock_unlock(&c->data.lock);
        }
        lock_free(&c->data.lock);

        classes_process_dispose(c->sub_classes, c->sub_classes_count);
    }
}
