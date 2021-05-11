/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "classes.h"

void classes_process_dispose(classes_struct* classes, const size_t classes_count) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        if (classes[i].dispose) {
            lock_lock(classes[i].lock);
            classes[i].dispose();
            lock_unlock(classes[i].lock);
        }
        lock_dispose(classes[i].lock);

        if (classes[i].sub_classes && classes[i].sub_classes_count)
            classes_process_dispose(classes[i].sub_classes, classes[i].sub_classes_count);
    }
}

void classes_process_init(classes_struct* classes, const size_t classes_count) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        lock_init(classes[i].lock);
        if (classes[i].init && classes[i].flags & CLASSES_INIT_AT_STARTUP) {
            lock_lock(classes[i].lock);
            classes[i].init();
            lock_unlock(classes[i].lock);
        }

        if (classes[i].sub_classes && classes[i].sub_classes_count)
            classes_process_init(classes[i].sub_classes, classes[i].sub_classes_count);
    }
}

void classes_process_control(classes_struct* classes, const size_t classes_count) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        if (classes[i].lock_init && classes[i].control) {
            lock_lock(classes[i].lock);
            classes[i].control();
            lock_unlock(classes[i].lock);
        }

        if (classes[i].sub_classes && classes[i].sub_classes_count)
            classes_process_control(classes[i].sub_classes, classes[i].sub_classes_count);
    }
}

void classes_process_drop(classes_struct* classes, size_t classes_count, size_t count, wchar_t** paths) {
    for (size_t i = 0; i < classes_count; i++) {
        if (classes[i].lock_init && classes[i].drop) {
            lock_lock(classes[i].lock);
            classes[i].drop(count, paths);
            lock_unlock(classes[i].lock);
        }

        if (classes[i].sub_classes && classes[i].sub_classes_count)
            classes_process_drop(classes[i].sub_classes, classes[i].sub_classes_count, count, paths);
    }
}

void classes_process_imgui(classes_struct* classes, const size_t classes_count) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        if (classes[i].lock_init && classes[i].imgui) {
            lock_lock(classes[i].lock);
            classes[i].imgui();
            lock_unlock(classes[i].lock);
        }

        if (classes[i].sub_classes && classes[i].sub_classes_count)
            classes_process_imgui(classes[i].sub_classes, classes[i].sub_classes_count);
    }
}

void classes_process_input(classes_struct* classes, const size_t classes_count) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        if (classes[i].lock_init && classes[i].input) {
            lock_lock(classes[i].lock);
            classes[i].input();
            lock_unlock(classes[i].lock);
        }

        if (classes[i].sub_classes && classes[i].sub_classes_count)
            classes_process_input(classes[i].sub_classes, classes[i].sub_classes_count);
    }
}

void classes_process_render(classes_struct* classes, const size_t classes_count) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        if (classes[i].lock_init && classes[i].render) {
            lock_lock(classes[i].lock);
            classes[i].render();
            lock_unlock(classes[i].lock);
        }

        if (classes[i].sub_classes && classes[i].sub_classes_count)
            classes_process_render(classes[i].sub_classes, classes[i].sub_classes_count);
    }
}

void classes_process_sound(classes_struct* classes, const size_t classes_count) {
    if (!classes || !classes_count)
        return;

    for (size_t i = 0; i < classes_count; i++) {
        if (classes[i].lock_init && classes[i].sound) {
            lock_lock(classes[i].lock);
            classes[i].sound();
            lock_unlock(classes[i].lock);
        }

        if (classes[i].sub_classes && classes[i].sub_classes_count)
            classes_process_sound(classes[i].sub_classes, classes[i].sub_classes_count);
    }
}
