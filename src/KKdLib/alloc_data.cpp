/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "alloc_data.hpp"

alloc_data::alloc_data() : next() {
    size = 4000;
}

alloc_data::~alloc_data() {
    deallocate();
}

void* alloc_data::allocate(size_t size) {
    alloc_node* node = next;
    size = max_def(size, 1);
    size = (size + 7) / 8 * 8;

    bool allocate = true;
    if (node) {
        alloc_node* last_node = 0;
        while (node) {
            if ((size_t)(node->capacity - node->size) >= size)
                last_node = node;
            node = node->next;
        }

        if (last_node) {
            node = last_node;
            allocate = false;
        }
        else
            node = next;
    }

    if (allocate) {
        size_t data_size = this->size;
        size_t _size = size + sizeof(alloc_node);
        size_t mults = 0;
        while (data_size < _size) {
            data_size *= 2;
            if (data_size >= _size)
                break;

            mults++;
            data_size *= 2;

            if (mults >= 16) {
                if (data_size < _size)
                    return 0;
                break;
            }
        }

        alloc_node* new_node = (alloc_node*)malloc(data_size);
        if (!new_node)
            return 0;

        next = new_node;

        new_node->next = node;
        new_node->size = 0;
        new_node->capacity = data_size - sizeof(alloc_node);
        node = new_node;
    }

    uint8_t* data = node->data + node->size;
    memset(data, 0, size);
    node->size += size;
    return data;
}

void alloc_data::deallocate() {
    alloc_node* node = next;
    while (node) {
        alloc_node* next_node = node->next;
        free(node);
        node = next_node;
    }

    next = 0;
}
